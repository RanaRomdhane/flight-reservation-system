#include "protocole.h"
#include "../common/structures.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <stddef.h> // Pour offsetof

extern Vol vols[MAX_VOLS];
extern Historique historiques[MAX_HISTORIQUES];
extern Facture factures[MAX_AGENCES];
extern int nb_vols;
extern int nb_historiques;
extern pthread_mutex_t mutex_vols, mutex_histo, mutex_factures;

void traiter_annulation(int sock, Requete req) {
    Reponse rep;
    memset(&rep, 0, sizeof(Reponse));
    int vol_trouve = 0;

    pthread_mutex_lock(&mutex_vols);
    pthread_mutex_lock(&mutex_histo);
    pthread_mutex_lock(&mutex_factures);

    for(int i = 0; i < nb_vols; i++) {
        if(vols[i].ref_vol == req.ref_vol) {
            vol_trouve = 1;

            vols[i].places_dispo += req.places;

            if(nb_historiques < MAX_HISTORIQUES) {
                historiques[nb_historiques].ref_vol = req.ref_vol;
                historiques[nb_historiques].agence = req.agence;
                strncpy(historiques[nb_historiques].transaction, "Annulation", MAX_TRANSACTION-1);
                historiques[nb_historiques].valeur = req.places;
                strncpy(historiques[nb_historiques].resultat, "succès", MAX_RESULTAT-1);
                nb_historiques++;
            }

            float penalite = 0.1 * req.places * vols[i].prix_place;
            factures[req.agence-1].agence = req.agence;
            factures[req.agence-1].somme -= req.places * vols[i].prix_place - penalite;

            strncpy(rep.statut, "SUCCES", MAX_STATUT-1);
            snprintf(rep.message, MAX_MESSAGE, "Annulation effectuée. Pénalité: %.2f", penalite);
            break;
        }
    }

    if(!vol_trouve) {
        strncpy(rep.statut, "ECHEC", MAX_STATUT-1);
        strncpy(rep.message, "Vol non trouvé", MAX_MESSAGE-1);
    }

    pthread_mutex_unlock(&mutex_factures);
    pthread_mutex_unlock(&mutex_histo);
    pthread_mutex_unlock(&mutex_vols);

    send(sock, &rep, sizeof(Reponse), 0);
}

void consulter_facture(int sock, Requete req) {
    Reponse rep;
    memset(&rep, 0, sizeof(Reponse));

    pthread_mutex_lock(&mutex_factures);

    if(req.agence > 0 && req.agence <= MAX_AGENCES) {
        strncpy(rep.statut, "SUCCES", MAX_STATUT-1);
        rep.montant = factures[req.agence-1].somme;
        snprintf(rep.message, MAX_MESSAGE, "Facture agence %d: %.2f", req.agence, rep.montant);
    } else {
        strncpy(rep.statut, "ECHEC", MAX_STATUT-1);
        strncpy(rep.message, "Agence invalide", MAX_MESSAGE-1);
    }

    pthread_mutex_unlock(&mutex_factures);

    send(sock, &rep, sizeof(Reponse), 0);
}

void traiter_reservation(int sock, Requete req) {
    Reponse rep;
    memset(&rep, 0, sizeof(Reponse));
    int vol_trouve = 0;

    pthread_mutex_lock(&mutex_vols);
    pthread_mutex_lock(&mutex_histo);
    pthread_mutex_lock(&mutex_factures);

    for(int i = 0; i < nb_vols; i++) {
        if(vols[i].ref_vol == req.ref_vol) {
            vol_trouve = 1;

            if(vols[i].places_dispo >= req.places) {
                vols[i].places_dispo -= req.places;

                if(nb_historiques < MAX_HISTORIQUES) {
                    historiques[nb_historiques].ref_vol = req.ref_vol;
                    historiques[nb_historiques].agence = req.agence;
                    strncpy(historiques[nb_historiques].transaction, "Demande", MAX_TRANSACTION-1);
                    historiques[nb_historiques].valeur = req.places;
                    strncpy(historiques[nb_historiques].resultat, "succès", MAX_RESULTAT-1);
                    nb_historiques++;
                }

                factures[req.agence-1].agence = req.agence;
                factures[req.agence-1].somme += req.places * vols[i].prix_place;

                strncpy(rep.statut, "SUCCES", MAX_STATUT-1);
                strncpy(rep.message, "Réservation effectuée", MAX_MESSAGE-1);
            } else {
                if(nb_historiques < MAX_HISTORIQUES) {
                    historiques[nb_historiques].ref_vol = req.ref_vol;
                    historiques[nb_historiques].agence = req.agence;
                    strncpy(historiques[nb_historiques].transaction, "Demande", MAX_TRANSACTION-1);
                    historiques[nb_historiques].valeur = req.places;
                    strncpy(historiques[nb_historiques].resultat, "impossible", MAX_RESULTAT-1);
                    nb_historiques++;
                }

                strncpy(rep.statut, "ECHEC", MAX_STATUT-1);
                snprintf(rep.message, MAX_MESSAGE, "Places insuffisantes. Disponible: %d", vols[i].places_dispo);
            }
            break;
        }
    }

    if(!vol_trouve) {
        strncpy(rep.statut, "ECHEC", MAX_STATUT-1);
        strncpy(rep.message, "Vol non trouvé", MAX_MESSAGE-1);
    }

    pthread_mutex_unlock(&mutex_factures);
    pthread_mutex_unlock(&mutex_histo);
    pthread_mutex_unlock(&mutex_vols);

    send(sock, &rep, sizeof(Reponse), 0);
}

void consulter_historique(int sock, Requete req) {
    Reponse rep;
    memset(&rep, 0, sizeof(Reponse));
    int count = 0;

    pthread_mutex_lock(&mutex_histo);

    for(int i = 0; i < nb_historiques && count < MAX_HISTO_TO_SEND; i++) {
        if(historiques[i].agence == req.agence) {
            rep.historiques[count] = historiques[i];
            count++;
        }
    }

    rep.nb_historiques = count;
    strncpy(rep.statut, "SUCCES", MAX_STATUT-1);
    snprintf(rep.message, MAX_MESSAGE, "Historique de l'agence %d (%d transactions)", req.agence, count);

    pthread_mutex_unlock(&mutex_histo);

    send(sock, &rep, sizeof(Reponse), 0);
}
