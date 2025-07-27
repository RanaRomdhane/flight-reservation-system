#ifndef GESTION_DONNEES_H
#define GESTION_DONNEES_H

#include "../common/structures.h"

#define MAX_VOLS 100
#define MAX_HISTORIQUES 1000
#define MAX_AGENCES 10
#define DATA_DIR "/home/kali/reservation_vols/data_files/"

extern Vol vols[MAX_VOLS];
extern Historique historiques[MAX_HISTORIQUES];
extern Facture factures[MAX_AGENCES];
extern int nb_vols;
extern int nb_historiques;

void creer_repertoire_data();
void charger_vols();
void charger_historique();
void charger_factures();
void sauvegarder_vols();
void sauvegarder_historique();
void sauvegarder_factures();
void sauvegarder_donnees();

#endif
