#ifndef PROTOCOLE_H
#define PROTOCOLE_H

#include "../common/structures.h"
#include <stddef.h> // Pour offsetof

#define MAX_HISTO_TO_SEND 50

void traiter_reservation(int sock, Requete req);
void traiter_annulation(int sock, Requete req);
void consulter_facture(int sock, Requete req);
void consulter_historique(int sock, Requete req);

#endif
