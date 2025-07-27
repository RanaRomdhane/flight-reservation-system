#include "interface.h"
#include <stdio.h>

void afficher_menu() {
    printf("\n=== MENU AGENCE ===\n");
    printf("1. Consulter les vols disponibles\n");
    printf("2. Effectuer une réservation\n");
    printf("3. Annuler une réservation\n");
    printf("4. Consulter ma facture\n");
    printf("5. Consulter mon historique\n");
    printf("6. Quitter\n");
    printf("Choix: ");
}

void afficher_vols(Vol vols[], int nb_vols) {
    if(nb_vols == 0) {
        printf("\nAucun vol disponible\n");
        return;
    }

    printf("\n=== VOLS DISPONIBLES ===\n");
    printf("Ref\tDestination\tPlaces\tPrix\n");
    printf("--------------------------------\n");
    for(int i = 0; i < nb_vols; i++) {
        printf("%d\t%-10s\t%d\t%d\n",
              vols[i].ref_vol,
              vols[i].destination,
              vols[i].places_dispo,
              vols[i].prix_place);
    }
}

void afficher_facture(float montant) {
    printf("\n=== FACTURE ===\n");
    printf("Montant total à payer: %.2f\n", montant);
}

void afficher_historique(Historique historiques[], int nb_historiques) {
    if(nb_historiques == 0) {
        printf("\nAucune transaction enregistrée\n");
        return;
    }

    printf("\n=== HISTORIQUE DES TRANSACTIONS ===\n");
    printf("Vol\tType\t\tPlaces\tRésultat\n");
    printf("----------------------------------------\n");
    for(int i = 0; i < nb_historiques; i++) {
        printf("%d\t%-10s\t%d\t%s\n",
              historiques[i].ref_vol,
              historiques[i].transaction,
              historiques[i].valeur,
              historiques[i].resultat);
    }
}
