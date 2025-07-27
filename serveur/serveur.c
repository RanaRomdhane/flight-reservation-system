#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/file.h>
#include <time.h>
#include <arpa/inet.h> // Ajouté pour inet_ntoa

#include "../common/structures.h"
#include "protocole.h"
#include "gestion_donnees.h"

#define PORT 8080
#define MAX_CLIENTS 10
#define SAUVEGARDE_INTERVAL 300 // 5 minutes en secondes


pthread_mutex_t mutex_vols = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_histo = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_factures = PTHREAD_MUTEX_INITIALIZER;

time_t last_save = 0;

void *gestion_client(void *socket_desc) {
    int sock = *(int*)socket_desc;
    Requete req;
    Reponse rep;

    // Récupérer l'adresse IP du client
    struct sockaddr_in addr;
    socklen_t addr_size = sizeof(addr);
    getpeername(sock, (struct sockaddr*)&addr, &addr_size);
    char client_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, client_ip, INET_ADDRSTRLEN);

    printf("Nouvelle connexion de %s\n", client_ip);

    while(1) {
        if(recv(sock, &req, sizeof(Requete), 0) <= 0) {
            printf("Connexion perdue avec %s\n", client_ip);
            break;
        }

        printf("Requête reçue de %s: %s\n", client_ip, req.type);

        if(strcmp(req.type, "CONSULT") == 0) {
            pthread_mutex_lock(&mutex_vols);
            rep.nb_vols = nb_vols;
            for(int i = 0; i < nb_vols && i < MAX_VOLS; i++) {
                rep.vols[i] = vols[i];
            }
            pthread_mutex_unlock(&mutex_vols);
            strncpy(rep.statut, "SUCCES", MAX_STATUT-1);
            send(sock, &rep, sizeof(Reponse), 0);
        }
        else if(strcmp(req.type, "RESERVE") == 0) {
            traiter_reservation(sock, req);
            sauvegarder_donnees();
        }
        else if(strcmp(req.type, "CANCEL") == 0) {
            traiter_annulation(sock, req);
            sauvegarder_donnees();
        }
        else if(strcmp(req.type, "FACTURE") == 0) {
            consulter_facture(sock, req);
        }
        else if(strcmp(req.type, "HISTORY") == 0) {
            consulter_historique(sock, req);
        }
    }

    close(sock);
    free(socket_desc);
    return NULL;
}

int main() {
    creer_repertoire_data();
    int server_fd, new_socket, *new_sock;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    charger_vols();
    charger_historique();
    charger_factures();
    last_save = time(NULL);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Échec de création du socket");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Échec des options du socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;// Écoute sur toutes les interfaces
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Échec du bind");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Échec de l'écoute");
        exit(EXIT_FAILURE);
    }

    printf("Serveur démarré. En attente de connexions sur le port %d...\n", PORT);

    while(1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Échec de l'acceptation");
            continue;
        }

        if(time(NULL) - last_save > SAUVEGARDE_INTERVAL) {
            sauvegarder_donnees();
            last_save = time(NULL);
            printf("Sauvegarde automatique effectuée\n");
        }

        pthread_t thread;
        new_sock = malloc(sizeof(int));
        *new_sock = new_socket;

        if (pthread_create(&thread, NULL, gestion_client, (void*)new_sock) < 0) {
            perror("Échec de création du thread");
            close(new_socket);
            free(new_sock);
        }

        pthread_detach(thread);
    }

    return 0;
}
