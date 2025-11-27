#include <stdio.h>
#include <string.h>

#define MAX_RES 100
#define MAX_SALLE 10
//tache1
typedef struct {
    int id;
    char nom_client[50];
    char salle[50];
    char date[20];   // YYYY-MM-DD
    int heure_debut;  // ex: 9 pour 09:00
    int heure_fin;    // ex: 11 pour 11:00
    int nombre_personnes;
    float tarif;
    char statut[20];
} Reservation;

typedef struct {
    char nom[50];
    int capacite;
    float tarif_horaire;
    char equipements[200];
} Salle;



// Vérifie si la salle est disponible
//int n :c’est combien de réservations sont déjà dans ce tableau.
int Disponibilite(char *nomSalle, char *date, int hDebut, int hFin, Reservation reservations[], int n) {
    // Fonction pour vérifier si une salle est disponible
    for(int i = 0; i < n; i++) {
        // Vérifie si c'est la même salle et même date
        if(strcmp(reservations[i].salle, nomSalle) == 0 && strcmp(reservations[i].date, date) == 0) {
            // Vérifie chevauchement des horaires
            if(hDebut < reservations[i].heure_fin && reservations[i].heure_debut < hFin) {
                return 0; // NON disponible
            }
        }
    }
    return 1; // Disponible
}

int verifCapacite(int nombre_personnes, Salle salles[], int nb_salles, char *nomSalle) {
    for(int i = 0; i < nb_salles; i++) {
        if(strcmp(salles[i].nom, nomSalle) == 0) {
            if(nombre_personnes <= salles[i].capacite) {
                return 1; // Capacité suffisante
            } else {
                return 0; // Capacité insuffisante
            }
        }
    }
    return 0; // Salle non trouvée
}

int creerReservation(Reservation r, Reservation reservations[], int *nb_res, Salle salles[], int nb_salles) {
    // 1Vérifier la capacité
    if(!verifCapacite(r.nombre_personnes, salles, nb_salles, r.salle)) {
        printf("Erreur : Capacité insuffisante pour la salle %s\n", r.salle);
        return 0;
    }
    //Vérifier la disponibilité
    if(!Disponibilite(r.salle, r.date, r.heure_debut, r.heure_fin, reservations, *nb_res)) {
        printf("Erreur : La salle %s est déjà réservée à cette horaire\n", r.salle);
        return 0;
    }
    //Calcul automatique du tarif
    float tarif_horaire = 0;
    for(int i = 0; i < nb_salles; i++) {
        if(strcmp(salles[i].nom, r.salle) == 0) {
            tarif_horaire = salles[i].tarif_horaire;
            break;
        }
    }
    r.tarif = tarif_horaire * (r.heure_fin - r.heure_debut);
    //Ajouter la réservation dans le tableau
    reservations[*nb_res] = r;
    (*nb_res)++;

    printf("Réservation créée avec succès ! Tarif = %.2f DT\n", r.tarif);
    return 1;
    }


//tache3



//Salle salles[] → le tableau où on va stocker les informations des salles.
//int *nb_salles → pointeur vers le nombre de salles lues, on va le remplir.

//%.2f\n  : 2 chiffres apres virgule

float calculerTarifTotal(Reservation *r, Salle salles[], int nb_salles) {
    float tarif_horaire = 0;

    
    for(int i = 0; i < nb_salles; i++) {
        if(strcmp(salles[i].nom, r->salle) == 0) {
            tarif_horaire = salles[i].tarif_horaire;
            break;
        }
    }
    // Calcul du tarif total = tarif horaire * durée
    r->tarif = tarif_horaire * (r->heure_fin - r->heure_debut);
    return r->tarif;
}

void sauvegardetarifs_salle(Salle salles[], int nb_salles, const char *nomFichier) {
    FILE *f = fopen(nomFichier, "w"); // ouvrir en mode écriture
    if(!f) {
        printf("Erreur : impossible d'écrire dans %s\n", nomFichier);
        return;
    }

    for(int i = 0; i < nb_salles; i++) {
        fprintf(f, "%s %.2f\n", salles[i].nom, salles[i].tarif_horaire);
    }

    fclose(f); // fermer le fichier
}
//APPEL: sauvegarderTarifs(salles, nb_salles, "tarifs.txt");  // <-- nom du fichier 
void sauvegardetarifs_reservation(Reservation reservations[], int nb_reservations, Salle salles[], int nb_salles, const char *nomFichier){
    FILE *f = fopen(nomFichier, "w");
    if(!f) {
        printf("Erreur : impossible d'écrire dans %s\n", nomFichier);
        return;
    }

    for(int i = 0; i < nb_reservations; i++) {
        // Calculer le tarif total automatiquement
        calculerTarifTotal(&reservations[i], salles, nb_salles);

        // Écrire la réservation dans le fichier
        fprintf(f, "%d %s %s %s %d %d %d %.2f\n",
                reservations[i].id,
                reservations[i].nom_client,
                reservations[i].salle,
                reservations[i].date,
                reservations[i].heure_debut,
                reservations[i].heure_fin,
                reservations[i].nombre_personnes,
                reservations[i].tarif);
    }

    fclose(f);
}

// tache 4:Sauvegarder les factures dans des fichiers dédiés


//prend une réservation r en paramètre.
void genererFacture(Reservation r, int id) {
    FILE *f = fopen(nomFichier, "w");
    if(!f) {
        printf("Erreur : impossible de créer %s\n", nomFichier);
        return;
    }
    int duree = r.heure_fin - r.heure_debut;
    fprintf(f, "Facture ID : %d\n", r.id);
    fprintf(f, "Client : %s\n", r.nom_client);
    fprintf(f, "Salle : %s\n", r.salle);
    fprintf(f, "Date : %s\n", r.date);
    fprintf(f, "Durée : %d heures\n", duree);
    fprintf(f, "Montant : %.2f DT\n", r.tarif);
    fclose(f);
}

//tache5
//Calculer le chiffre d'affaires par salle
// Compter le nombre de réservations par mois
//Afficher les salles les plus populaires
