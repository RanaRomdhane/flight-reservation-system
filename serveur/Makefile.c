CC = gcc
CFLAGS = -Wall -Wextra -pthread -I../common
LDFLAGS = -pthread

SRV_SRCS = serveur.c gestion_donnees.c protocole.c
SRV_OBJS = $(SRV_SRCS:.c=.o)
SRV_EXEC = serveur

all: $(SRV_EXEC)

$(SRV_EXEC): $(SRV_OBJS)
	@mkdir -p data
	$(CC) $(LDFLAGS) -o $@ $^
	@echo "Compilation réussie. Exécutable créé: ./serveur"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRV_OBJS) $(SRV_EXEC) data/*.txt

install:
	@mkdir -p data
	@[ -d ../data_files ] && cp -n ../data_files/*.txt data/ 2>/dev/null || echo "Aucun fichier à copier"
	@echo "Fichiers de données installés dans data/"

.PHONY: all clean install
