#include "gestion_donnees.h"
#include "../common/structures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/file.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>

#define DATA_DIR "/home/kali/reservation_vols/data_files/"
#define LOCK_TIMEOUT 5

Vol vols[MAX_VOLS];
Historique historiques[MAX_HISTORIQUES];
Facture factures[MAX_AGENCES];
int nb_vols = 0;
int nb_historiques = 0;

void creer_repertoire_data() {
    struct stat st;
    if (stat(DATA_DIR, &st) == -1) {
        // Crée récursivement le répertoire
        char cmd[256];
        snprintf(cmd, sizeof(cmd), "mkdir -p %s", DATA_DIR);
        system(cmd);

        // Vérifie que le répertoire a bien été créé
        if (stat(DATA_DIR, &st) == -1) {
            perror("Erreur création répertoire data");
            exit(EXIT_FAILURE);
        }
    }
}


void charger_vols() {
    creer_repertoire_data();

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%svols.txt", DATA_DIR);

    FILE *f = fopen(filepath, "r");
    if (!f) {
        printf("Création du fichier vols.txt...\n");
        f = fopen(filepath, "w");
        if (!f) {
            perror("Erreur création fichier vols");
            exit(EXIT_FAILURE);
        }
        fprintf(f, "Référence Vol Destination Nombre Places Prix Place\n");
        fprintf(f, "1000 Paris 20 500\n");
        fprintf(f, "2000 Medina 10 2500\n");
        fprintf(f, "3000 Montréal 40 3500\n");
        fprintf(f, "4000 Dubai 15 3000\n");
        fclose(f);
        return;
    }

    char ligne[256];
    fgets(ligne, sizeof(ligne), f); // En-tête

    nb_vols = 0;
    while(fgets(ligne, sizeof(ligne), f) && nb_vols < MAX_VOLS) {
        if(sscanf(ligne, "%d %49s %d %d",
              &vols[nb_vols].ref_vol,
              vols[nb_vols].destination,
              &vols[nb_vols].places_dispo,
              &vols[nb_vols].prix_place) == 4) {
            nb_vols++;
        }
    }
    fclose(f);
}

void charger_historique() {
    creer_repertoire_data();

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%shisto.txt", DATA_DIR);

    FILE *f = fopen(filepath, "r");
    if (!f) {
        printf("Création du fichier histo.txt...\n");
        f = fopen(filepath, "w");
        if (!f) {
            perror("Erreur création fichier histo");
            exit(EXIT_FAILURE);
        }
        fprintf(f, "Référence Vol Agence Transaction Valeur Résultat\n");
        fclose(f);
        return;
    }

    char ligne[256];
    fgets(ligne, sizeof(ligne), f); // En-tête

    nb_historiques = 0;
    while(fgets(ligne, sizeof(ligne), f) && nb_historiques < MAX_HISTORIQUES) {
        if(sscanf(ligne, "%d %d %19s %d %19s",
              &historiques[nb_historiques].ref_vol,
              &historiques[nb_historiques].agence,
              historiques[nb_historiques].transaction,
              &historiques[nb_historiques].valeur,
              historiques[nb_historiques].resultat) == 5) {
            nb_historiques++;
        }
    }
    fclose(f);
}

void charger_factures() {
    creer_repertoire_data();

    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%sfacture.txt", DATA_DIR);

    FILE *f = fopen(filepath, "r");
    if (!f) {
        printf("Création du fichier facture.txt...\n");
        f = fopen(filepath, "w");
        if (!f) {
            perror("Erreur création fichier facture");
            exit(EXIT_FAILURE);
        }
        fprintf(f, "Référence Agence Somme à payer\n");
        fclose(f);
        return;
    }

    char ligne[256];
    fgets(ligne, sizeof(ligne), f); // En-tête

    for(int i = 0; i < MAX_AGENCES; i++) {
        factures[i].agence = i+1;
        factures[i].somme = 0;
    }

    while(fgets(ligne, sizeof(ligne), f)) {
        int agence;
        float somme;
        if(sscanf(ligne, "%d %f", &agence, &somme) == 2) {
            if(agence > 0 && agence <= MAX_AGENCES) {
                factures[agence-1].somme = somme;
            }
        }
    }
    fclose(f);
}

int verrouiller_fichier(int fd) {
    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_pid = getpid();

    int attempts = 0;
    while(fcntl(fd, F_SETLK, &fl) == -1) {
        if (errno != EAGAIN || attempts >= LOCK_TIMEOUT) {
            perror("Erreur verrouillage fichier");
            return -1;
        }
        sleep(1);
        attempts++;
    }
    return 0;
}

void deverrouiller_fichier(int fd) {
    struct flock fl;
    fl.l_type = F_UNLCK;
    fl.l_whence = SEEK_SET;
    fl.l_start = 0;
    fl.l_len = 0;
    fl.l_pid = getpid();

    if (fcntl(fd, F_SETLK, &fl) == -1) {
        perror("Erreur deverrouillage fichier");
    }
}

void sauvegarder_vols() {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%svols.txt", DATA_DIR);

    // Méthode atomique : écriture dans un fichier temporaire d'abord
    char temp_path[256];
    snprintf(temp_path, sizeof(temp_path), "%svols.tmp", DATA_DIR);

    int fd = open(temp_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Erreur création fichier temporaire vols");
        return;
    }

    if (verrouiller_fichier(fd) < 0) {
        close(fd);
        return;
    }

    FILE *f = fdopen(fd, "w");
    if (!f) {
        perror("Erreur fdopen fichier temporaire vols");
        deverrouiller_fichier(fd);
        close(fd);
        return;
    }

    fprintf(f, "Référence Vol Destination Nombre Places Prix Place\n");
    for(int i = 0; i < nb_vols; i++) {
        fprintf(f, "%d %s %d %d\n",
               vols[i].ref_vol,
               vols[i].destination,
               vols[i].places_dispo,
               vols[i].prix_place);
    }

    fflush(f);
    deverrouiller_fichier(fd);
    fclose(f);

    // Renomme atomiquement le fichier temporaire
    if(rename(temp_path, filepath) != 0) {
        perror("Erreur renommage fichier vols");
        unlink(temp_path); // Nettoie le fichier temporaire
    }
}

void sauvegarder_historique() {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%shisto.txt", DATA_DIR);

    char temp_path[256];
    snprintf(temp_path, sizeof(temp_path), "%shisto.tmp", DATA_DIR);

    int fd = open(temp_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Erreur création fichier temporaire histo");
        return;
    }

    if (verrouiller_fichier(fd) < 0) {
        close(fd);
        return;
    }

    FILE *f = fdopen(fd, "w");
    if (!f) {
        perror("Erreur fdopen fichier temporaire histo");
        deverrouiller_fichier(fd);
        close(fd);
        return;
    }

    fprintf(f, "Référence Vol Agence Transaction Valeur Résultat\n");
    for(int i = 0; i < nb_historiques; i++) {
        fprintf(f, "%d %d %s %d %s\n",
               historiques[i].ref_vol,
               historiques[i].agence,
               historiques[i].transaction,
               historiques[i].valeur,
               historiques[i].resultat);
    }

    fflush(f);
    deverrouiller_fichier(fd);
    fclose(f);

    if(rename(temp_path, filepath) != 0) {
        perror("Erreur renommage fichier histo");
        unlink(temp_path);
    }
}

void sauvegarder_factures() {
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%sfacture.txt", DATA_DIR);

    char temp_path[256];
    snprintf(temp_path, sizeof(temp_path), "%sfacture.tmp", DATA_DIR);

    int fd = open(temp_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("Erreur création fichier temporaire facture");
        return;
    }

    if (verrouiller_fichier(fd) < 0) {
        close(fd);
        return;
    }

    FILE *f = fdopen(fd, "w");
    if (!f) {
        perror("Erreur fdopen fichier temporaire facture");
        deverrouiller_fichier(fd);
        close(fd);
        return;
    }

    fprintf(f, "Référence Agence Somme à payer\n");
    for(int i = 0; i < MAX_AGENCES; i++) {
        if(factures[i].somme != 0) {
            fprintf(f, "%d %.2f\n", factures[i].agence, factures[i].somme);
        }
    }

    fflush(f);
    deverrouiller_fichier(fd);
    fclose(f);

    if(rename(temp_path, filepath) != 0) {
        perror("Erreur renommage fichier facture");
        unlink(temp_path);
    }
}

void sauvegarder_donnees() {
    printf("Sauvegarde des données en cours...\n");
    sauvegarder_vols();
    sauvegarder_historique();
    sauvegarder_factures();
    printf("Sauvegarde des données terminée\n");
}
