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
#include "cpu/cpu.h"

#include "piix4.h"

#define DEVSTS_TRP_STS_DEV13 (1 << 29)
#define DEVCTL_TRP_EN_DEV13 (1 << 25)
#define GLBSTS_APM_STS (1 << 5)

typedef struct piix4_t {
	uint8_t card[256];
	uint8_t card_ide[256];
	uint8_t card_pm[256];
	sff_busmaster_t busmaster[2];

	uint8_t rc;
	uint8_t port_92;

	uint16_t pmba;
	struct {
		uint8_t apmc, apms;
		uint64_t timer_offset;

		uint16_t pmsts;
		uint16_t pmen;
		uint16_t pmcntrl;
		uint32_t devctl;
		uint32_t devsts;
		uint16_t glbsts;
		uint16_t gpen;
		uint16_t gpsts;
		uint32_t pcntrl;
		uint16_t glben;
		uint32_t glbctl;
		uint32_t gporeg;

		int dev13_enable;
		uint16_t dev13_base, dev13_size;
	} pm;

	uint16_t smbba;
	struct {
		uint8_t host_ctrl;
	} smbus;

	void (*nb_reset)();
} piix4_t;

static piix4_t piix4;

static uint8_t piix4_dev13_read(uint16_t addr, void* p) {
	piix4_t* dev = (piix4_t*)p;
	if(dev->pm.devctl & DEVCTL_TRP_EN_DEV13) {
		dev->pm.devsts |= DEVSTS_TRP_STS_DEV13;
		x86_smi_trigger();
	}
	return 0xff;
}
static void piix4_dev13_write(uint16_t addr, uint8_t val, void* p) {
	piix4_t* dev = (piix4_t*)p;
	if(dev->pm.devctl & DEVCTL_TRP_EN_DEV13) {
		dev->pm.devsts |= DEVSTS_TRP_STS_DEV13;
		x86_smi_trigger();
	}
}

static void piix4_dev13_recalc(piix4_t* dev) {
	if(dev->pm.dev13_enable)
		io_removehandler(dev->pm.dev13_base, dev->pm.dev13_size, piix4_dev13_read, NULL, NULL, piix4_dev13_write, NULL, NULL, dev);
	dev->pm.dev13_base = dev->card_pm[0x70] | (dev->card_pm[0x71] << 8);
	dev->pm.dev13_size = (dev->card_pm[0x72] & 0xf) + 1;
	dev->pm.dev13_enable = dev->card_pm[0x72] & 0x10;
	if(dev->pm.dev13_enable)
		io_sethandler(dev->pm.dev13_base, dev->pm.dev13_size, piix4_dev13_read, NULL, NULL, piix4_dev13_write, NULL, NULL, dev);
}

static void piix4_pm_write(uint16_t port, uint8_t val, void* p) {
	piix4_t* dev = (piix4_t*)p;
	switch(port & 0x3f) {
		case 0x00:
			dev->pm.pmsts &= ~(val & 0x31);
			break;
		case 0x01:
			dev->pm.pmsts &= ~((val & 0x8d) << 8);
			break;
		case 0x02:
			dev->pm.pmen = (dev->pm.pmen & ~0xff) | (val & 0x21);
			break;
		case 0x03:
			dev->pm.pmen = (dev->pm.pmen & ~0xff00) | ((val & 0x05) << 8);
			break;
		case 0x04:
			dev->pm.pmcntrl = (dev->pm.pmcntrl & ~0xff) | (val & 0x03);
			break;
		case 0x05:
			if(val & 0x20) {
				if(val == 0x20)
					stop_emulation_now();
			}
			dev->pm.pmcntrl = (dev->pm.pmcntrl & ~0xff00) | ((val & 0x1c) << 8);
			break;
		case 0x0c:
		case 0x0d:
			dev->pm.gpsts &= ~(val << ((port & 1) * 8));
			break;
		case 0x0e:
			dev->pm.pmen = (dev->pm.pmen & ~0xff) | (val & 0x01);
			break;
		case 0x0f:
			dev->pm.pmen = (dev->pm.pmen & ~0xff00) | ((val & 0x0f) << 8);
			break;
		case 0x10:
			dev->pm.pcntrl = (dev->pm.pcntrl & ~0xff) | (val & 0x1e);
			break;
		case 0x11:
			dev->pm.pcntrl = (dev->pm.pcntrl & ~0xff00) | ((val & 0x3e) << 8);
			break;
		case 0x18:
			dev->pm.glbsts &= ~(val & 0x25);
			break;
		case 0x19:
			dev->pm.glbsts &= ~((val & 0x0d) << 8);
			break;
		case 0x1c:
		case 0x1d:
		case 0x1e:
		case 0x1f:
			dev->pm.devsts &= ~(val << ((port & 3) * 8));
			break;
		case 0x20:
			dev->pm.glben = (dev->pm.glben & ~0xff) | (val & 0x01);
			break;
		case 0x21:
			dev->pm.glben = (dev->pm.glben & ~0xff00) | ((val & 0x0f) << 8);
			break;
		case 0x28:
			dev->pm.glbctl = (dev->pm.glbctl & ~0xff) | (val & 0x05);
			break;
		case 0x29:
			dev->pm.glbctl = (dev->pm.glbctl & ~0xff00) | ((val & 0xff) << 8);
			break;
		case 0x2a:
			dev->pm.glbctl = (dev->pm.glbctl & ~0xff0000) | ((val & 0x01) << 16);
			break;
		case 0x2b:
			dev->pm.glbctl = (dev->pm.glbctl & ~0xff000000) | ((val & 0x07) << 24);
			break;
		case 0x2c:
			dev->pm.devctl = (dev->pm.devctl & ~0xff) | val;
			break;
		case 0x2d:
			dev->pm.devctl = (dev->pm.devctl & ~0xff00) | (val << 8);
			break;
		case 0x2e:
			dev->pm.devctl = (dev->pm.devctl & ~0xff0000) | (val << 16);
			break;
		case 0x2f:
			dev->pm.devctl = (dev->pm.devctl & ~0xff000000) | ((val & 0x0f) << 24);
			break;
		case 0x34:
			dev->pm.gporeg = (dev->pm.gporeg & ~0xff) | val;
			break;
		case 0x35:
			dev->pm.gporeg = (dev->pm.gporeg & ~0xff00) | (val << 8);
			break;
		case 0x36:
			dev->pm.gporeg = (dev->pm.gporeg & ~0xff0000) | (val << 16);
			break;
		case 0x37:
			dev->pm.gporeg = (dev->pm.gporeg & ~0xff000000) | (val << 24);
			break;
	}
}

static uint8_t piix4_pm_read(uint16_t port, void* p) {
	piix4_t* dev = (piix4_t*)p;
	uint8_t ret = 0xff;
	switch(port & 0x3f) {
		case 0x00:
		case 0x01:
			ret = dev->pm.pmsts >> ((port & 1) * 8);
			break;
		case 0x02:
		case 0x03:
			ret = dev->pm.pmen >> ((port & 1) * 8);
			break;
		case 0x04:
		case 0x05:
			ret = dev->pm.pmcntrl >> ((port & 1) * 8);
			break;
		case 0x08:
		case 0x09:
		case 0x0a:
		case 0x0b: {
			uint64_t timer = tsc - dev->pm.timer_offset;
			uint32_t pm_timer = (uint32_t)((timer * 3579545ULL) / cpu_get_speed());
			ret = pm_timer >> ((port & 3) * 8);
			break;
		}
		case 0x0c:
		case 0x0d:
			ret = dev->pm.gpsts >> ((port & 1) * 8);
			break;
		case 0x0e:
		case 0x0f:
			ret = dev->pm.pmen >> ((port & 1) * 8);
			break;
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
			ret = dev->pm.pcntrl >> ((port & 3) * 8);
			break;
		case 0x14:
		case 0x15:
			ret = 0;
			break;
		case 0x18:
		case 0x19:
			ret = dev->pm.glbsts >> ((port & 1) * 8);
			break;
		case 0x1c:
		case 0x1d:
		case 0x1e:
		case 0x1f:
			ret = dev->pm.devsts >> ((port & 3) * 8);
			break;
		case 0x20:
		case 0x21:
			ret = dev->pm.glben >> ((port & 1) * 8);
			break;
		case 0x28:
		case 0x29:
		case 0x2a:
		case 0x2b:
			ret = dev->pm.glbctl >> ((port & 3) * 8);
			break;
		case 0x2c:
		case 0x2d:
		case 0x2e:
		case 0x2f:
			ret = dev->pm.devctl >> ((port & 3) * 8);
			break;
		case 0x30:
		case 0x31:
		case 0x32:
			ret = 0;
			break;
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
			ret = dev->pm.gporeg >> ((port & 3) * 8);
			break;
	}
	return ret;
}

static void piix4_smbus_write(uint16_t port, uint8_t val, void* p) {
	piix4_t* dev = (piix4_t*)p;
	switch(port & 0xf) {
		case 0x2:
			dev->smbus.host_ctrl = (val & 0x5f) | (dev->smbus.host_ctrl & 0x40);
			break;
	}
}

static uint8_t piix4_smbus_read(uint16_t port, void* p) {
	piix4_t* dev = (piix4_t*)p;
	uint8_t ret = 0xff;
	switch(port & 0x0f) {
		case 0x0:
			ret = 0;
			break;
		case 0x2:
			ret = dev->smbus.host_ctrl;
			break;
		case 0x5:
			ret = 0;
			break;
	}
	return ret;
}

static void piix4_pm_clear_io(piix4_t* dev) {
	io_removehandler(dev->pmba, 0x0040, piix4_pm_read, NULL, NULL, piix4_pm_write, NULL, NULL, dev);
	io_removehandler(dev->smbba, 0x0010, piix4_smbus_read, NULL, NULL, piix4_smbus_write, NULL, NULL, dev);
}

static void piix4_pm_set_io(piix4_t* dev) {
	if((dev->card_pm[0x04] & 0x01) && dev->pmba)
		io_sethandler(dev->pmba, 0x0040, piix4_pm_read, NULL, NULL, piix4_pm_write, NULL, NULL, dev);
	if((dev->card_pm[0x04] & 0x01) && dev->smbba)
		io_sethandler(dev->smbba, 0x0010, piix4_smbus_read, NULL, NULL, piix4_smbus_write, NULL, NULL, dev);
}

static void piix4_pm_pci_write(int addr, uint8_t val, void* p) {
	piix4_t* dev = (piix4_t*)p;
	switch(addr) {
		case 0x04:
			piix4_pm_clear_io(dev);
			dev->card_pm[0x04] = val & 1;
			piix4_pm_set_io(dev);
			break;
		case 0x40:
			piix4_pm_clear_io(dev);
			dev->pmba = (dev->pmba & 0xff00) | (val & 0xc0);
			piix4_pm_set_io(dev);
			break;
		case 0x41:
			piix4_pm_clear_io(dev);
			dev->pmba = (dev->pmba & 0xc0) | (val << 8);
			piix4_pm_set_io(dev);
			break;
		case 0x58:
		case 0x59:
		case 0x5b:
		case 0x5c:
			dev->card_pm[addr] = val;
			break;
		case 0x70:
		case 0x71:
		case 0x72:
			dev->card_pm[addr] = val;
			piix4_dev13_recalc(dev);
			break;
		case 0x90:
			piix4_pm_clear_io(dev);
			dev->smbba = (dev->smbba & 0xff00) | (val & 0xf0);
			piix4_pm_set_io(dev);
			break;
		case 0x91:
			piix4_pm_clear_io(dev);
			dev->smbba = (dev->smbba & 0xf0) | (val << 8);
			piix4_pm_set_io(dev);
			break;
	}
}

static uint8_t piix4_pm_pci_read(int addr, void* p) {
	piix4_t* dev = (piix4_t*)p;
	return dev->card_pm[addr];
}

static void piix4_write(int func, int addr, uint8_t val, void* priv) {
	if(func > 3)
		return;

	if(func == 1) {
		switch(addr) {
			case 0x04:
				piix4.card_ide[0x04] = (piix4.card_ide[0x04] & ~5) | (val & 5);
				break;
			case 0x07:
				piix4.card_ide[0x07] = (piix4.card_ide[0x07] & ~0x38) | (val & 0x38);
				break;
			case 0x0d:
				piix4.card_ide[0x0d] = val;
				break;
			case 0x20:
				piix4.card_ide[0x20] = (val & ~0x0f) | 1;
				break;
			case 0x21:
				piix4.card_ide[0x21] = val;
				break;
			case 0x40:
				piix4.card_ide[0x40] = val;
				break;
			case 0x41:
				piix4.card_ide[0x41] = val;
				break;
			case 0x42:
				piix4.card_ide[0x42] = val;
				break;
			case 0x43:
				piix4.card_ide[0x43] = val;
				break;
		}
		if(addr == 4 || (addr & ~3) == 0x20) {
			uint16_t base = (piix4.card_ide[0x20] & 0xf0) | (piix4.card_ide[0x21] << 8);
			io_removehandler(0, 0x10000, sff_bus_master_read, NULL, NULL, sff_bus_master_write, NULL, NULL, piix4.busmaster);
			if(piix4.card_ide[0x04] & 1)
				io_sethandler(base, 0x10, sff_bus_master_read, NULL, NULL, sff_bus_master_write, NULL, NULL, piix4.busmaster);
		}
		if(addr == 4 || addr == 0x41 || addr == 0x43) {
			ide_pri_disable();
			ide_sec_disable();
			if(piix4.card_ide[0x04] & 1) {
				if(piix4.card_ide[0x41] & 0x80)
					ide_pri_enable();
				if(piix4.card_ide[0x43] & 0x80)
					ide_sec_enable();
			}
		}
		return;
	} else if(func == 2) {
		return;
	} else if(func == 3) {
		piix4_pm_pci_write(addr, val, &piix4);
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
	}
	piix4.card[addr] = val;
}

static uint8_t piix4_read(int func, int addr, void* priv) {
	if(func > 3)
		return 0xff;
	if(func == 1)
		return piix4.card_ide[addr];
	if(func == 2)
		return 0xff;
	if(func == 3)
		return piix4_pm_pci_read(addr, &piix4);
	return piix4.card[addr];
}

static uint8_t piix4_rc_read(uint16_t port, void* p) {
	return piix4.rc;
}
static void piix4_rc_write(uint16_t port, uint8_t val, void* p) {
	if(val & 4) {
		if(val & 2) {
			if(piix4.nb_reset)
				piix4.nb_reset();
			keyboard_at_reset();
			ide_reset_devices();
			piix4.pm.timer_offset = tsc;
			resetx86();
			piix4.pm.apmc = piix4.pm.apms = 0;
		} else
			softresetx86();
	}
	piix4.rc = val & ~4;
}

static uint8_t piix4_92_read(uint16_t port, void* p) {
	return piix4.port_92;
}
static void piix4_92_write(uint16_t port, uint8_t val, void* p) {
	if((mem_a20_alt ^ val) & 2) {
		mem_a20_alt = val & 2;
		mem_a20_recalc();
	}
	if((~piix4.port_92 & val) & 1) {
		softresetx86();
		cpu_set_edx();
	}
	piix4.port_92 = val;
}

static uint8_t piix4_apm_read(uint16_t port, void* p) {
	switch(port) {
		case 0xb2:
			return piix4.pm.apmc;
		case 0xb3:
			return piix4.pm.apms;
	}
	return 0xff;
}

static void piix4_apm_write(uint16_t port, uint8_t val, void* p) {
	switch(port) {
		case 0xb2:
			piix4.pm.apmc = val;
			if(piix4.card_pm[0x5b] & (1 << 1)) {
				piix4.pm.glbsts |= GLBSTS_APM_STS;
				x86_smi_trigger();
			}
			break;
		case 0xb3:
			piix4.pm.apms = val;
			break;
	}
}

void piix4_init(int card, int pci_a, int pci_b, int pci_c, int pci_d, void (*nb_reset)()) {
	memset(&piix4, 0, sizeof(piix4_t));
	piix4.nb_reset = nb_reset;

	pci_add_specific(card, piix4_read, piix4_write, NULL);

	piix4.card[0x00] = 0x86;
	piix4.card[0x01] = 0x80; /*Intel*/
	piix4.card[0x02] = 0x10;
	piix4.card[0x03] = 0x71; /*82371EB PIIX4*/
	piix4.card[0x04] = 0x07;
	piix4.card[0x05] = 0x00;
	piix4.card[0x06] = 0x80;
	piix4.card[0x07] = 0x02;
	piix4.card[0x08] = 0x00;
	piix4.card[0x09] = 0x00;
	piix4.card[0x0a] = 0x01;
	piix4.card[0x0b] = 0x06;
	piix4.card[0x0e] = 0x80;
	piix4.card[0x4c] = 0x4d;
	piix4.card[0x4e] = 0x03;
	piix4.card[0x60] = piix4.card[0x61] = piix4.card[0x62] = piix4.card[0x63] = 0x80;
	piix4.card[0x69] = 0x02;
	piix4.card[0x70] = piix4.card[0x71] = 0x80;
	piix4.card[0x76] = piix4.card[0x77] = 0x0c;
	piix4.card[0x78] = 0x02;
	piix4.card[0xa0] = 0x08;

	piix4.card_ide[0x00] = 0x86;
	piix4.card_ide[0x01] = 0x80;
	piix4.card_ide[0x02] = 0x11;
	piix4.card_ide[0x03] = 0x71; /*82371EB PIIX4 IDE*/
	piix4.card_ide[0x04] = 0x02;
	piix4.card_ide[0x05] = 0x00;
	piix4.card_ide[0x06] = 0x80;
	piix4.card_ide[0x07] = 0x02;
	piix4.card_ide[0x09] = 0x80;
	piix4.card_ide[0x0a] = 0x01;
	piix4.card_ide[0x0b] = 0x01;
	piix4.card_ide[0x20] = 0x01;
	piix4.card_ide[0x40] = piix4.card_ide[0x41] = 0x00;
	piix4.card_ide[0x42] = piix4.card_ide[0x43] = 0x00;

	piix4.card_pm[0x00] = 0x86;
	piix4.card_pm[0x01] = 0x80;
	piix4.card_pm[0x02] = 0x13;
	piix4.card_pm[0x03] = 0x71;
	piix4.card_pm[0x06] = 0x80;
	piix4.card_pm[0x07] = 0x02;
	piix4.card_pm[0x09] = 0x00;
	piix4.card_pm[0x0a] = 0x80;
	piix4.card_pm[0x0b] = 0x06;
	piix4.card_pm[0x3d] = 0x01;
	piix4.card_pm[0x40] = 0x01;
	piix4.card_pm[0x90] = 0x01;
	piix4.pm.timer_offset = 0;
	piix4.pm.gporeg = 0x7fffbfff;

	sff_bus_master_init(piix4.busmaster);

	pci_set_card_routing(pci_a, PCI_INTA);
	pci_set_card_routing(pci_b, PCI_INTB);
	pci_set_card_routing(pci_c, PCI_INTC);
	pci_set_card_routing(pci_d, PCI_INTD);

	ide_pri_disable();
	ide_sec_disable();
	pic_init_elcrx();

	io_sethandler(0x0cf9, 0x0001, piix4_rc_read, NULL, NULL, piix4_rc_write, NULL, NULL, NULL);
	io_sethandler(0x0092, 0x0001, piix4_92_read, NULL, NULL, piix4_92_write, NULL, NULL, NULL);
	io_sethandler(0x00b2, 0x0002, piix4_apm_read, NULL, NULL, piix4_apm_write, NULL, NULL, NULL);
}
