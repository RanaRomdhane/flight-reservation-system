#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stddef.h>

#include "../common/structures.h"
#include "interface.h"

#define PORT 8080

int main(int argc, char const *argv[]) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    Requete req;
    Reponse rep;
    int agence_id;
    char server_ip[16];

    if(argc != 3) {
        printf("Usage: %s <id_agence> <ip_serveur>\n", argv[0]);
        return -1;
    }

    agence_id = atoi(argv[1]);
    strncpy(server_ip, argv[2], sizeof(server_ip));

    if((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nErreur création socket\n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET, server_ip, &serv_addr.sin_addr) <= 0) {
        printf("\nAdresse IP invalide: %s\n", server_ip);
        return -1;
    }

    printf("Tentative de connexion au serveur %s...\n", server_ip);

    if(connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnexion échouée\n");
        return -1;
    }

    printf("Connecté au serveur %s en tant qu'agence %d\n", server_ip, agence_id);

    int choix;
    do {
        afficher_menu();
        scanf("%d", &choix);
        getchar(); // Pour consommer le '\n'

        memset(&req, 0, sizeof(Requete));
        memset(&rep, 0, sizeof(Reponse));

        switch(choix) {
            case 1: // Consulter vols
                strcpy(req.type, "CONSULT");
                if(send(sock, &req, sizeof(Requete), 0) < 0) {
                    perror("Erreur envoi");
                    close(sock);
                    return -1;
                }
                if(recv(sock, &rep, sizeof(Reponse), 0) <= 0) {
                    perror("Erreur réception");
                    close(sock);
                    return -1;
                }
                afficher_vols(rep.vols, rep.nb_vols);
                break;

            case 2: // Réservation
                printf("Référence vol: ");
                scanf("%d", &req.ref_vol);
                printf("Nombre de places: ");
                scanf("%d", &req.places);
                strcpy(req.type, "RESERVE");
                req.agence = agence_id;
                if(send(sock, &req, sizeof(Requete), 0) < 0) {
                    perror("Erreur envoi");
                    close(sock);
                    return -1;
                }
                if(recv(sock, &rep, sizeof(Reponse), 0) <= 0) {
                    perror("Erreur réception");
                    close(sock);
                    return -1;
                }
                printf("%s: %s\n", rep.statut, rep.message);
                break;

            case 3: // Annulation
                printf("Référence vol: ");
                scanf("%d", &req.ref_vol);
                printf("Nombre de places à annuler: ");
                scanf("%d", &req.places);
                strcpy(req.type, "CANCEL");
                req.agence = agence_id;
                if(send(sock, &req, sizeof(Requete), 0) < 0) {
                    perror("Erreur envoi");
                    close(sock);
                    return -1;
                }
                if(recv(sock, &rep, sizeof(Reponse), 0) <= 0) {
                    perror("Erreur réception");
                    close(sock);
                    return -1;
                }
                printf("%s: %s\n", rep.statut, rep.message);
                break;

            case 4: // Facture
                strcpy(req.type, "FACTURE");
                req.agence = agence_id;
                if(send(sock, &req, sizeof(Requete), 0) < 0) {
                    perror("Erreur envoi");
                    close(sock);
                    return -1;
                }
                if(recv(sock, &rep, sizeof(Reponse), 0) <= 0) {
                    perror("Erreur réception");
                    close(sock);
                    return -1;
                }
                afficher_facture(rep.montant);
                break;

            case 5: // Historique
                strcpy(req.type, "HISTORY");
                req.agence = agence_id;
                if(send(sock, &req, sizeof(Requete), 0) < 0) {
                    perror("Erreur envoi");
                    close(sock);
                    return -1;
                }
                if(recv(sock, &rep, sizeof(Reponse), 0) <= 0) {
                    perror("Erreur réception");
                    close(sock);
                    return -1;
                }
                afficher_historique(rep.historiques, rep.nb_historiques);
                break;

            case 6:
                printf("Déconnexion...\n");
                break;

            default:
                printf("Choix invalide\n");
        }
    } while(choix != 6);

    close(sock);
    return 0;
}
