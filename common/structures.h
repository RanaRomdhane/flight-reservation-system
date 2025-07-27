#ifndef STRUCTURES_H
#define STRUCTURES_H

#define MAX_VOLS 100
#define MAX_HISTORIQUES 1000
#define MAX_AGENCES 10
#define MAX_TRANSACTION 20
#define MAX_RESULTAT 20
#define MAX_STATUT 20
#define MAX_MESSAGE 100
#define MAX_DESTINATION 50

typedef struct {
    int ref_vol;
    char destination[MAX_DESTINATION];
    int places_dispo;
    int prix_place;
} Vol;

typedef struct {
    int ref_vol;
    int agence;
    char transaction[MAX_TRANSACTION];
    int valeur;
    char resultat[MAX_RESULTAT];
} Historique;

typedef struct {
    int agence;
    float somme;
} Facture;

typedef struct {
    char type[20]; // "CONSULT", "RESERVE", "CANCEL", "FACTURE", "HISTORY"
    int ref_vol;
    int agence;
    int places;
} Requete;

typedef struct {
    char statut[MAX_STATUT]; // "SUCCES", "ECHEC"
    char message[MAX_MESSAGE];
    Vol vols[MAX_VOLS];
    Historique historiques[100];
    int nb_vols;
    int nb_historiques;
    float montant;
} Reponse;

#endif
