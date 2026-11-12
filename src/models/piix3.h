#ifndef _PIIX3_H_
#define _PIIX3_H_

void piix_init(int card, int pci_a, int pci_b, int pci_c, int pci_d, void (*nb_reset)());
void piix3_init(int card, int pci_a, int pci_b, int pci_c, int pci_d, void (*nb_reset)());

#endif /* _PIIX3_H_ */
