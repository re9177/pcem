#ifndef _PIIX4_H_
#define _PIIX4_H_

void piix4_init(int card, int pci_a, int pci_b, int pci_c, int pci_d, void (*nb_reset)());

#endif /* _PIIX4_H_ */
