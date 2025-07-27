#ifndef INTERFACE_H
#define INTERFACE_H

#include "../common/structures.h"

void afficher_menu();
void afficher_vols(Vol vols[], int nb_vols);
void afficher_facture(float montant);
void afficher_historique(Historique historiques[], int nb_historiques);

#endif
