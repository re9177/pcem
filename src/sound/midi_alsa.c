#include <alsa/asoundlib.h>
#include <stdbool.h>
#include "ibm.h"
#include "config.h"
#include "plat-midi.h"

#define MAX_MIDI_DEVICES 128
#define MAX_RAWMIDI_SUBDEVICES 32
#define MAX_CTL_NAME_LEN 32
#define MAX_DEVICE_NAME_LEN 50
#define MIDI_BUFFER_SIZE 256

typedef struct sequencer_env_t
{
	snd_seq_t *seq;
	snd_midi_event_t *midi_codec;
	snd_seq_addr_t src;
	snd_seq_addr_t dest;
} sequencer_env_t;

typedef struct rawmidi_env_t
{
	snd_rawmidi_t *midiout;
} rawmidi_env_t;

typedef struct rawmidi_output_addr_t {
        int card;
        int device;
        int sub;
        char name[50];
} rawmidi_output_addr_t;

typedef struct midi_output_t
{
	char name[MAX_DEVICE_NAME_LEN];

	void (*open)(struct midi_output_t*);
	void (*close)(struct midi_output_t*);
	void (*write)(struct midi_output_t*, uint8_t val);

	union
	{
		struct sequencer_device_t
		{
			snd_seq_addr_t address;
		} seq;
		struct rawmidi_device_t
		{
			rawmidi_output_addr_t address;
		} rawmidi;
	} info;
} midi_output_t;

static sequencer_env_t sequencer_env = {0};
static rawmidi_env_t rawmidi_env = {0};

static bool initialized = false;
static size_t midi_output_count;
static midi_output_t midi_outputs[MAX_MIDI_DEVICES * 2];
static midi_output_t *current_output = NULL;

static snd_seq_t *open_seq() {
        int status;
        snd_seq_t *seq;

        if ((status = snd_seq_open(&seq, "default", SND_SEQ_OPEN_OUTPUT, 0)) < 0)
	{
		pclog("MIDI: Could not open the ALSA sequencer.\n", snd_strerror(status));
		return NULL;
	}

        return seq;
}

static void midi_open_alsa_seq(midi_output_t *output)
{
	int status;
	sequencer_env_t *env = &sequencer_env;

	if (env->seq != NULL || (env->seq = open_seq()) == NULL)
	{
		return;
	}

	const unsigned int caps = SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ;
	const unsigned int type = SND_SEQ_PORT_TYPE_APPLICATION | SND_SEQ_PORT_TYPE_MIDI_GENERIC;

	snd_seq_set_client_name(env->seq, "PCem");
	env->src.port = snd_seq_create_simple_port(env->seq, config_name, caps, type);

	if (env->src.port < 0)
	{
		snd_seq_close(env->seq);
		env->seq = NULL;
		pclog("MIDI: Could not create ALSA sequencer port: %s.\n", snd_strerror(env->src.port));
		return;
	}

	env->dest = output->info.seq.address;

	bool dst_is_src = env->dest.client == env->src.client && env->dest.port == env->src.port;

	if (env->dest.client == 0 || dst_is_src)
	{
		env->dest.client = SND_SEQ_ADDRESS_SUBSCRIBERS;
		env->dest.port = SND_SEQ_ADDRESS_UNKNOWN;
	}

	if (env->dest.client != SND_SEQ_ADDRESS_SUBSCRIBERS)
	{
		if ((status = snd_seq_connect_to(env->seq, env->src.port, env->dest.client, env->dest.port)) < 0)
		{
			/*
			 * Failing to connect to another client is harmless. The user may
			 * connect PCem's ports manually to something else using aconnect(1) at
			 * any point during runtime.
			 */
			pclog("MIDI: Could not connect to ALSA sequencer client %d:%d: %s.\n",
					env->dest.client,
					env->dest.port,
					snd_strerror(status));
		}
	}

	if ((status = snd_midi_event_new(MIDI_BUFFER_SIZE, &env->midi_codec)) < 0)
	{
		snd_seq_close(env->seq);
		env->seq = NULL;
		pclog("MIDI: Could not create a MIDI event encoder/decoder: %s.\n", snd_strerror(status));
		return;
	}

	snd_midi_event_init(env->midi_codec);
}

static void midi_close_alsa_seq(midi_output_t *output)
{
	sequencer_env_t *env = &sequencer_env;

	if (env->seq != NULL)
	{
		snd_seq_drain_output(env->seq);
		snd_seq_close(env->seq);
		env->seq = NULL;

		if (env->midi_codec != NULL)
		{
			snd_midi_event_free(env->midi_codec);
			env->midi_codec = NULL;
		}
	}
}

static void midi_write_alsa_seq(midi_output_t *output, uint8_t val)
{
	sequencer_env_t *env = &sequencer_env;

	if (env->seq == NULL)
	{
		return;
	}

	snd_seq_event_t ev;
	int result = snd_midi_event_encode_byte(env->midi_codec, val, &ev);

	if (result == 1)
	{
		bool flush;
		switch (ev.type)
		{
			case SND_SEQ_EVENT_PGMCHANGE:
			case SND_SEQ_EVENT_CHANPRESS:
				flush = false;
				break;
			default:
				flush = true;
		}

		snd_seq_ev_set_direct(&ev);
		snd_seq_ev_set_source(&ev, env->src.port);
		snd_seq_ev_set_subs(&ev);
		snd_seq_event_output(env->seq, &ev);

		if (flush)
		{
			snd_seq_drain_output(env->seq);
		}
	}
}

static ssize_t midi_query_alsa_seq(midi_output_t *list, size_t maxlen) {
	snd_seq_t *seq = open_seq();

	if (seq == NULL)
	{
		return -1;
	}

	int count = 0;

	snd_seq_client_info_t *client_info;
	snd_seq_port_info_t *port_info;

	snd_seq_client_info_alloca(&client_info);
	snd_seq_port_info_alloca(&port_info);

	const unsigned int caps_mask = SND_SEQ_PORT_CAP_WRITE | SND_SEQ_PORT_CAP_SUBS_WRITE;

	snd_seq_client_info_set_client(client_info, -1);

	while (snd_seq_query_next_client(seq, client_info) >= 0 && count < maxlen)
	{
		int client = snd_seq_client_info_get_client(client_info);

		snd_seq_port_info_set_client(port_info, client);
		snd_seq_port_info_set_port(port_info, -1);

		while (snd_seq_query_next_port(seq, port_info) >= 0 && count < maxlen)
		{
			unsigned int port_type = snd_seq_port_info_get_type(port_info);

			if (port_type & SND_SEQ_PORT_TYPE_MIDI_GENERIC)
			{
				unsigned int caps = snd_seq_port_info_get_capability(port_info);

				if ((caps & caps_mask) == caps_mask)
				{
					int port = snd_seq_port_info_get_port(port_info);

					list[count] = (midi_output_t)
					{
						.open = midi_open_alsa_seq,
						.close = midi_close_alsa_seq,
						.write = midi_write_alsa_seq,
						.info.seq =
						{
							.address =
							{
								.client = client,
								.port = port
							}
						},
					};

					char *name = list[count].name;
					const char *queried_name = snd_seq_client_info_get_name(client_info);
					snprintf(name, MAX_DEVICE_NAME_LEN, "alsa_seq(%d:%d): %s",
							client,
							port,
							queried_name);

					count++;
				}
			}
		}
	}

	return count;
}

static void midi_open_rawmidi(midi_output_t *output)
{
	int status;
	rawmidi_env_t *env = &rawmidi_env;

	char ctl_name[MAX_CTL_NAME_LEN];

	int ctl_name_len = snprintf(ctl_name, MAX_CTL_NAME_LEN, "hw:%i,%i,%i",
			output->info.rawmidi.address.card,
			output->info.rawmidi.address.device,
			output->info.rawmidi.address.sub);

	if (ctl_name_len >= MAX_CTL_NAME_LEN)
	{
		pclog("MIDI: Failed to open ALSA RawMIDI port %s: Name is too long.\n", ctl_name);
		return;
	}

	if ((status = snd_rawmidi_open(NULL, &env->midiout, ctl_name, 0)) < 0)
	{
		pclog("MIDI: Failed to open ALSA RawMIDI port %s: %s.\n",
				ctl_name,
				snd_strerror(status));
		return;
	}
}

static void midi_close_rawmidi(midi_output_t *output)
{
	if (rawmidi_env.midiout != NULL)
	{
		snd_rawmidi_drain(rawmidi_env.midiout);
		snd_rawmidi_close(rawmidi_env.midiout);
		rawmidi_env.midiout = NULL;
	}
}

static void midi_write_rawmidi(midi_output_t *output, uint8_t val)
{
	rawmidi_env_t *env = &rawmidi_env;

	if (env->midiout == NULL)
	{
		return;
	}

	snd_rawmidi_write(env->midiout, &val, 1);
}

static size_t rawmidi_get_subdevices(snd_ctl_t *ctl, int device, rawmidi_output_addr_t *list, size_t maxlen)
{
	size_t count = 0;
	int card;

	snd_rawmidi_info_t *info;
	int sub_nr, sub;

	snd_rawmidi_info_alloca(&info);

	snd_rawmidi_info_set_device(info, device);
	snd_rawmidi_info_set_stream(info, SND_RAWMIDI_STREAM_OUTPUT);
	snd_ctl_rawmidi_info(ctl, info);

	card = snd_rawmidi_info_get_card(info);
	sub_nr = snd_rawmidi_info_get_subdevices_count(info);

	for (sub = 0; sub < sub_nr && count < maxlen; sub++)
	{
		snd_rawmidi_info_set_subdevice(info, sub);

		if (snd_ctl_rawmidi_info(ctl, info) == 0)
		{
			list[count] = (rawmidi_output_addr_t)
			{
				.card = card,
				.device = device,
				.sub = sub,
			};

			count++;
		}
	}

	return count;
}

static ssize_t midi_query_rawmidi(midi_output_t *list, size_t maxlen)
{
	int status;
	size_t count = 0;
	int card = -1;

	if ((status = snd_card_next(&card)) < 0)
	{
		pclog("MIDI: Could not determine ALSA card number: %s\n", snd_strerror(status));
		return -1;
	}
 
        if (card < 0)
        {
                return 0; /*No cards*/
        }

        while (card >= 0) {
                char *shortname;

                if (snd_card_get_name(card, &shortname) >= 0) {
                        snd_ctl_t *ctl;
			char ctl_name[MAX_CTL_NAME_LEN];
			int ctl_name_len = snprintf(ctl_name, MAX_CTL_NAME_LEN, "hw:%d", card);

                        if (ctl_name_len < MAX_CTL_NAME_LEN && snd_ctl_open(&ctl, ctl_name, 0) >= 0)
                        {
                                int device = -1;

                                do {
                                        size_t sub_count = 0;
					rawmidi_output_addr_t subdevices[MAX_RAWMIDI_SUBDEVICES];
                                        
                                        status = snd_ctl_rawmidi_next_device(ctl, &device);
                                        
                                        if (status >= 0 && device != -1) {
                                                sub_count = rawmidi_get_subdevices(ctl, device, subdevices, MAX_RAWMIDI_SUBDEVICES);

                                                for(int sub_i = 0; sub_i < sub_count && count < maxlen; sub_i++, count++) 
                                                {
                                                        rawmidi_output_addr_t *address = &subdevices[sub_i];

                                                        list[count] = (midi_output_t) 
                                                        {
                                                        .open = midi_open_rawmidi,
							.close = midi_close_rawmidi,
							.write = midi_write_rawmidi,
							.info.rawmidi =
							{
								.address =
								{
								        .card = address->card,
								        .device = address->device,
								        .sub = address-> sub
								}
								}
							};

                                                        char *name = list[count].name;
							snprintf(name, MAX_DEVICE_NAME_LEN, "rawmidi(%d:%d:%d): %s",
									address->card,
									address->device,
									address->sub,
									shortname);
                                                }
                                        }
                                } while (device >= 0 && count < maxlen);
                        }
                }

                free(shortname);

                if (count >= maxlen || snd_card_next(&card) < 0)
                {
                        break;
                }
        }
        return count;
}

static bool midi_query()
{
        if (initialized)
        {
                return true;
        }

        ssize_t rawmidi_count = midi_query_rawmidi(midi_outputs, MAX_MIDI_DEVICES);

        if (rawmidi_count > 0)
	{
		midi_output_count += rawmidi_count;
	}

        ssize_t alsa_seq_count = midi_query_alsa_seq(&midi_outputs[midi_output_count], MAX_MIDI_DEVICES);

        if (alsa_seq_count < 0) 
        {
                return false;
        }
        midi_output_count += alsa_seq_count;

	initialized = true;

	return true;
}

void midi_init()
{
	if (!midi_query())
	{
		return;
	}

	int index = config_get_int(CFG_MACHINE, NULL, "midi", 0);

	if (index >= midi_output_count)
	{
		pclog("MIDI: The configured MIDI device is missing.\n");
 		return;
	}

	current_output = &midi_outputs[index];

	current_output->open(current_output);
}

void midi_close() {
        if (current_output == NULL) 
        {
                return;
        }
}

void midi_write(uint8_t val) {
        if (current_output == NULL)
	{
		return;
	}

	current_output->write(current_output, val);
}

int midi_get_num_devs() {
        if (!midi_query())
	{
		return 0;
	}

        return midi_output_count;
}

void midi_get_dev_name(int num, char *s) 
{ 
        if (!midi_query())
	{
		return;
	}

	strcpy(s, midi_outputs[num].name);
}
