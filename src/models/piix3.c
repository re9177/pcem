/*PRD format :
		word 0 - base address
		word 1 - bits 1 - 15 = byte count, bit 31 = end of transfer
*/
#include <string.h>

#include "ibm.h"
#include "ide/ide.h"
#include "ide/ide_sff8038i.h"
#include "keyboard/keyboard_at.h"
#include "io.h"
#include "memory/mem.h"
#include "bus/pci.h"
#include "models/pic.h"
#include "cpu/x86.h"

#include "piix3.h"

#define REG_SMICNTL 0xa0
#define REG_SMIEN 0xa2
#define REG_SMIREQ 0xaa

#define SMICNTL_CSMIGATE (1 << 0)
#define SMIEN_APMC (1 << 7)
#define SMIREQ_RAPMC (1 << 7)

typedef struct piix3_t {
	uint8_t card[256];
	uint8_t card_ide[256];
	sff_busmaster_t busmaster[2];

	uint8_t rc;
	uint8_t apmc, apms;

	void (*nb_reset)();
} piix3_t;

static piix3_t piix3;

static void piix3_write(int func, int addr, uint8_t val, void* priv) {
	if(func > 1)
		return;

	if(func == 1) {
		switch(addr) {
			case 0x04:
				piix3.card_ide[0x04] = (piix3.card_ide[0x04] & ~5) | (val & 5);
				break;
			case 0x07:
				piix3.card_ide[0x07] = (piix3.card_ide[0x07] & ~0x38) | (val & 0x38);
				break;
			case 0x0d:
				piix3.card_ide[0x0d] = val;
				break;

			case 0x20:
				piix3.card_ide[0x20] = (val & ~0x0f) | 1;
				break;
			case 0x21:
				piix3.card_ide[0x21] = val;
				break;

			case 0x40:
				piix3.card_ide[0x40] = val;
				break;
			case 0x41:
				piix3.card_ide[0x41] = val;
				break;
			case 0x42:
				piix3.card_ide[0x42] = val;
				break;
			case 0x43:
				piix3.card_ide[0x43] = val;
				break;
		}
		if(addr == 4 || (addr & ~3) == 0x20) {
			uint16_t base = (piix3.card_ide[0x20] & 0xf0) | (piix3.card_ide[0x21] << 8);
			io_removehandler(0, 0x10000, sff_bus_master_read, NULL, NULL, sff_bus_master_write, NULL, NULL, piix3.busmaster);
			if(piix3.card_ide[0x04] & 1)
				io_sethandler(base, 0x10, sff_bus_master_read, NULL, NULL, sff_bus_master_write, NULL, NULL, piix3.busmaster);
		}
		if(addr == 4 || addr == 0x41 || addr == 0x43) {
			ide_pri_disable();
			ide_sec_disable();
			if(piix3.card_ide[0x04] & 1) {
				if(piix3.card_ide[0x41] & 0x80)
					ide_pri_enable();
				if(piix3.card_ide[0x43] & 0x80)
					ide_sec_enable();
			}
		}
		return;
	}

	if(addr >= 0x0f && addr < 0x4c)
		return;

	switch(addr) {
		case 0x00:
		case 0x01:
		case 0x02:
		case 0x03:
		case 0x08:
		case 0x09:
		case 0x0a:
		case 0x0b:
		case 0x0e:
			return;

		case 0x60:
			if(val & 0x80)
				pci_set_irq_routing(PCI_INTA, PCI_IRQ_DISABLED);
			else
				pci_set_irq_routing(PCI_INTA, val & 0xf);
			break;
		case 0x61:
			if(val & 0x80)
				pci_set_irq_routing(PCI_INTB, PCI_IRQ_DISABLED);
			else
				pci_set_irq_routing(PCI_INTB, val & 0xf);
			break;
		case 0x62:
			if(val & 0x80)
				pci_set_irq_routing(PCI_INTC, PCI_IRQ_DISABLED);
			else
				pci_set_irq_routing(PCI_INTC, val & 0xf);
			break;
		case 0x63:
			if(val & 0x80)
				pci_set_irq_routing(PCI_INTD, PCI_IRQ_DISABLED);
			else
				pci_set_irq_routing(PCI_INTD, val & 0xf);
			break;
		case REG_SMIREQ:
		case REG_SMIREQ + 1:
			piix3.card[addr] &= val;
			return;
	}
	piix3.card[addr] = val;
}

static uint8_t piix3_read(int func, int addr, void* priv) {
	if(func > 1)
		return 0xff;
	if(func == 1)
		return piix3.card_ide[addr];
	return piix3.card[addr];
}

static uint8_t piix3_rc_read(uint16_t port, void* p) {
	return piix3.rc;
}

static void piix3_rc_write(uint16_t port, uint8_t val, void* p) {
	if(val & 4) {
		if(val & 2) {
			if(piix3.nb_reset)
				piix3.nb_reset();
			keyboard_at_reset();
			ide_reset_devices();
			resetx86();
			piix3.apmc = piix3.apms = 0;
		} else
			softresetx86();
	}
	piix3.rc = val & ~4;
}

static uint8_t piix3_apm_read(uint16_t port, void* p) {
	switch(port) {
		case 0xb2:
			return piix3.apmc;
		case 0xb3:
			return piix3.apms;
	}
	return 0xff;
}

static void piix3_apm_write(uint16_t port, uint8_t val, void* p) {
	switch(port) {
		case 0xb2:
			piix3.apmc = val;
			if(piix3.card[REG_SMIEN] & SMIEN_APMC) {
				piix3.card[REG_SMIREQ] |= SMIREQ_RAPMC;
				if(piix3.card[REG_SMICNTL] & SMICNTL_CSMIGATE)
					x86_smi_trigger();
			}
			break;
		case 0xb3:
			piix3.apms = val;
			break;
	}
}

static void piix3_common_init(int card, int pci_a, int pci_b, int pci_c, int pci_d, void (*nb_reset)(),
							  uint8_t isa_id_lo, uint8_t isa_id_hi, uint8_t ide_id_lo, uint8_t ide_id_hi) {
	memset(&piix3, 0, sizeof(piix3_t));
	piix3.nb_reset = nb_reset;

	pci_add_specific(card, piix3_read, piix3_write, NULL);

	piix3.card[0x00] = 0x86;
	piix3.card[0x01] = 0x80; /*Intel*/
	piix3.card[0x02] = isa_id_lo;
	piix3.card[0x03] = isa_id_hi;
	piix3.card[0x04] = 0x07;
	piix3.card[0x05] = 0x00;
	piix3.card[0x06] = 0x80;
	piix3.card[0x07] = 0x02;
	piix3.card[0x08] = 0x00;
	piix3.card[0x09] = 0x00;
	piix3.card[0x0a] = 0x01;
	piix3.card[0x0b] = 0x06;
	piix3.card[0x0e] = 0x80;
	piix3.card[0x4c] = 0x4d;
	piix3.card[0x4e] = 0x03;
	piix3.card[0x60] = piix3.card[0x61] = piix3.card[0x62] = piix3.card[0x63] = 0x80;
	piix3.card[0x69] = 0x02;
	piix3.card[0x70] = piix3.card[0x71] = 0x80;
	piix3.card[0x76] = piix3.card[0x77] = 0x0c;
	piix3.card[0x78] = 0x02;
	piix3.card[0x79] = 0x00;
	piix3.card[0xa0] = 0x08;
	piix3.card[0xa2] = piix3.card[0xa3] = 0x00;
	piix3.card[0xa4] = piix3.card[0xa5] = piix3.card[0xa6] = piix3.card[0xa7] = 0x00;
	piix3.card[0xa8] = 0x0f;
	piix3.card[0xaa] = piix3.card[0xab] = 0x00;
	piix3.card[0xac] = 0x00;
	piix3.card[0xae] = 0x00;

	piix3.card_ide[0x00] = 0x86;
	piix3.card_ide[0x01] = 0x80;
	piix3.card_ide[0x02] = ide_id_lo;
	piix3.card_ide[0x03] = ide_id_hi;
	piix3.card_ide[0x04] = 0x02;
	piix3.card_ide[0x05] = 0x00;
	piix3.card_ide[0x06] = 0x80;
	piix3.card_ide[0x07] = 0x02;
	piix3.card_ide[0x08] = 0x00;
	piix3.card_ide[0x09] = 0x80;
	piix3.card_ide[0x0a] = 0x01;
	piix3.card_ide[0x0b] = 0x01;
	piix3.card_ide[0x0d] = 0x00;
	piix3.card_ide[0x0e] = 0x00;
	piix3.card_ide[0x20] = 0x01;
	piix3.card_ide[0x21] = piix3.card_ide[0x22] = piix3.card_ide[0x23] = 0x00;
	piix3.card_ide[0x40] = piix3.card_ide[0x41] = 0x00;
	piix3.card_ide[0x42] = piix3.card_ide[0x43] = 0x00;

	sff_bus_master_init(piix3.busmaster);

	pci_set_card_routing(pci_a, PCI_INTA);
	pci_set_card_routing(pci_b, PCI_INTB);
	pci_set_card_routing(pci_c, PCI_INTC);
	pci_set_card_routing(pci_d, PCI_INTD);

	ide_pri_disable();
	ide_sec_disable();

	pic_init_elcrx();

	io_sethandler(0x0cf9, 0x0001, piix3_rc_read, NULL, NULL, piix3_rc_write, NULL, NULL, NULL);
	io_sethandler(0x00b2, 0x0002, piix3_apm_read, NULL, NULL, piix3_apm_write, NULL, NULL, NULL);
}

void piix_init(int card, int pci_a, int pci_b, int pci_c, int pci_d, void (*nb_reset)()) {
	/* this is temporary i hope i'll get to this sooner than later but for now it's ok */
	piix3_common_init(card, pci_a, pci_b, pci_c, pci_d, nb_reset, 0x2e, 0x12, 0x30, 0x12);
}

void piix3_init(int card, int pci_a, int pci_b, int pci_c, int pci_d, void (*nb_reset)()) {
	piix3_common_init(card, pci_a, pci_b, pci_c, pci_d, nb_reset, 0x00, 0x70, 0x10, 0x70);
}
