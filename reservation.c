#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RES 200
#define MAX_SALLE 10

typedef enum {PENDING, CONFIRMED, CANCELLED} Status;

typedef struct Client {
    int id;
    char nom[50];
    char email[50];
    char telephone[20];
    int total_reservations;
    float total_spent;
    struct Client *suivant;
} Client;

typedef struct {
    int id;
    char nom[50];
    int capacite;
    float tarif_horaire;
    char equipements[200];
} Salle;

typedef struct {
    int id;
    int client_id;
    char client_name[50];
    char client_email[50];
    int salle_id;
    char salle_name[50];
    char date[20];
    int heure_debut;
    int heure_fin;
    int nombre_personnes;
    float tarif;
    Status statut;
} Reservation;

static Salle salles[MAX_SALLE];
static Reservation reservations[MAX_RES];
static int nb_res = 0;
static Client *clients = NULL;

int saisieInt(const char *msg, int min, int max) {
    int val;
    while (1) {
        printf("%s", msg);
        if (scanf("%d", &val) != 1) {
            while (getchar() != '\n');
            continue;
        }
        if (val >= min && val <= max) return val;
    }
}

void saisieString(const char *msg, char *out, int maxlen) {
    printf("%s", msg);
    scanf("%49s", out);
}

int dateValide(const char *date) {
    if (!date || strlen(date) != 10) return 0;
    if (date[4] != '-' || date[7] != '-') return 0;
    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if (date[i] < '0' || date[i] > '9') return 0;
    }
    int y, m, d;
    if (sscanf(date, "%d-%d-%d", &y, &m, &d) != 3) return 0;
    if (m < 1 || m > 12) return 0;
    if (d < 1 || d > 31) return 0;
    return 1;
}

Client *trouverClientParNomEmail(const char *nom, const char *email) {
    Client *c = clients;
    while (c) {
        if (strcmp(c->nom, nom) == 0 && strcmp(c->email, email) == 0) return c;
        c = c->suivant;
    }
    return NULL;
}

Client *trouverClientParId(int id) {
    Client *c = clients;
    while (c) {
        if (c->id == id) return c;
        c = c->suivant;
    }
    return NULL;
}

int prochainIdClient() {
    int max = 0;
    Client *c = clients;
    while (c) {
        if (c->id > max) max = c->id;
        c = c->suivant;
    }
    return max + 1;
}

Client *ajouterClientSiInexistant_wrapper(const char *nom, const char *email) {
    Client *ex = trouverClientParNomEmail(nom, email);
    if (ex) return ex;
    char tel[20];
    printf("Téléphone pour le nouveau client : ");
    scanf("%19s", tel);
    Client *n = malloc(sizeof(Client));
    if (!n) return NULL;
    n->id = prochainIdClient();
    strncpy(n->nom, nom, sizeof(n->nom)-1); n->nom[sizeof(n->nom)-1]='\0';
    strncpy(n->email, email, sizeof(n->email)-1); n->email[sizeof(n->email)-1]='\0';
    strncpy(n->telephone, tel, sizeof(n->telephone)-1); n->telephone[sizeof(n->telephone)-1]='\0';
    n->total_reservations = 0; n->total_spent = 0.0f; n->suivant = clients; clients = n;
    return n;
}

float prixAvecFidelite(Client *c, float prix_base) {
    if (!c) return prix_base;
    float red = 0.0f;
    if (c->total_reservations > 10) red = 0.20f;
    else if (c->total_reservations > 5) red = 0.10f;
    return prix_base * (1.0f - red);
}

void initSalles() {
    strncpy(salles[0].nom, "SalleA", sizeof(salles[0].nom)-1); salles[0].capacite = 10; salles[0].tarif_horaire = 50.0f;
    strncpy(salles[1].nom, "SalleB", sizeof(salles[1].nom)-1); salles[1].capacite = 20; salles[1].tarif_horaire = 80.0f;
    strncpy(salles[2].nom, "SalleC", sizeof(salles[2].nom)-1); salles[2].capacite = 5;  salles[2].tarif_horaire = 30.0f;
    strncpy(salles[3].nom, "SalleD", sizeof(salles[3].nom)-1); salles[3].capacite = 15; salles[3].tarif_horaire = 60.0f;
    strncpy(salles[4].nom, "SalleE", sizeof(salles[4].nom)-1); salles[4].capacite = 12; salles[4].tarif_horaire = 55.0f;
    strncpy(salles[5].nom, "SalleF", sizeof(salles[5].nom)-1); salles[5].capacite = 8;  salles[5].tarif_horaire = 40.0f;
    strncpy(salles[6].nom, "SalleG", sizeof(salles[6].nom)-1); salles[6].capacite = 25; salles[6].tarif_horaire = 100.0f;
    strncpy(salles[7].nom, "SalleH", sizeof(salles[7].nom)-1); salles[7].capacite = 6;  salles[7].tarif_horaire = 35.0f;
    strncpy(salles[8].nom, "SalleI", sizeof(salles[8].nom)-1); salles[8].capacite = 18; salles[8].tarif_horaire = 70.0f;
    strncpy(salles[9].nom, "SalleJ", sizeof(salles[9].nom)-1); salles[9].capacite = 30; salles[9].tarif_horaire = 120.0f;
    for (int i=0;i<MAX_SALLE;i++) salles[i].id = i+1;
}

int DisponibiliteSalle(int salle_id, const char *date, int hd, int hf) {
    for (int i=0;i<nb_res;i++) {
        if (reservations[i].salle_id == salle_id && strcmp(reservations[i].date, date) == 0) {
            if (!(hf <= reservations[i].heure_debut || hd >= reservations[i].heure_fin))
                return 0;
        }
    }
    return 1;
}

float calculPrixBase(int salle_id, int hd, int hf) {
    if (salle_id < 1 || salle_id > MAX_SALLE) return 0.0f;
    float taux = salles[salle_id-1].tarif_horaire;
    int duree = hf - hd;
    if (duree < 0) duree = 0;
    return taux * duree;
}

void sauvegarderClients() {
    FILE *f = fopen("clients.txt","w");
    if (!f) return;
    Client *c = clients;
    while (c) {
        fprintf(f,"%d %s %s %s %d %.2f\n",c->id,c->nom,c->email,c->telephone,c->total_reservations,c->total_spent);
        c = c->suivant;
    }
    fclose(f);
}

void chargerClients() {
    FILE *f = fopen("clients.txt","r");
    if (!f) return;
    while (!feof(f)) {
        Client *n = malloc(sizeof(Client));
        if (!n) break;
        if (fscanf(f,"%d %49s %49s %19s %d %f",&n->id,n->nom,n->email,n->telephone,&n->total_reservations,&n->total_spent)!=6) { free(n); break; }
        n->suivant = clients; clients = n;
    }
    fclose(f);
}

void sauvegarderReservations() {
    FILE *f = fopen("reservations.txt","w");
    if (!f) return;
    for (int i=0;i<nb_res;i++)
        fprintf(f,"%d %d %s %s %d %s %s %d %d %d %.2f %d\n",
                reservations[i].id,reservations[i].client_id,reservations[i].client_name,reservations[i].client_email,
                reservations[i].salle_id,reservations[i].salle_name,reservations[i].date,reservations[i].heure_debut,
                reservations[i].heure_fin,reservations[i].nombre_personnes,reservations[i].tarif,reservations[i].statut);
    fclose(f);
}

void chargerReservations() {
    FILE *f = fopen("reservations.txt","r");
    if (!f) return;
    while (!feof(f) && nb_res<MAX_RES) {
        Reservation r;
        if (fscanf(f,"%d %d %49s %49s %d %49s %19s %d %d %d %f %d",
                   &r.id,&r.client_id,r.client_name,r.client_email,&r.salle_id,r.salle_name,r.date,&r.heure_debut,
                   &r.heure_fin,&r.nombre_personnes,&r.tarif,(int*)&r.statut)!=12) break;
        reservations[nb_res++] = r;
    }
    fclose(f);
}

int effectuerReservationInteractive() {
    if (nb_res >= MAX_RES) return 1;
    char nom[50], email[50];
    saisieString("Nom du client : ", nom, sizeof(nom));
    saisieString("Email du client : ", email, sizeof(email));
    Client *c = trouverClientParNomEmail(nom, email);
    if (!c) c = ajouterClientSiInexistant_wrapper(nom,email);
    char date[20];
    do { saisieString("Date (YYYY-MM-DD) : ", date, sizeof(date)); } while(!dateValide(date));
    int hd = saisieInt("Heure début (0-23) : ",0,23);
    int hf = saisieInt("Heure fin (1-24) : ",hd+1,24);
    int nb_pers,salles_dispo[MAX_SALLE],nb_disp;
    while(1) {
        nb_pers = saisieInt("Nombre de personnes : ",1,1000); nb_disp=0;
        for(int i=0;i<MAX_SALLE;i++)
            if(salles[i].capacite>=nb_pers && DisponibiliteSalle(salles[i].id,date,hd,hf))
                salles_dispo[nb_disp++]=i;
        if(nb_disp==0) { if(!saisieInt("Aucune salle disponible. Réessayer? (1=oui 0=non): ",0,1)) return 1; continue;}
        for(int i=0;i<nb_disp;i++) printf("%d) %s (cap %d, %.2f/h)\n",i+1,salles[salles_dispo[i]].nom,salles[salles_dispo[i]].capacite,salles[salles_dispo[i]].tarif_horaire);
        int choix = saisieInt("Choisissez la salle : ",1,nb_disp);
        int idxSalle = salles_dispo[choix-1];
        Reservation r;
        r.id = nb_res+1;
        r.client_id = c->id;
        strncpy(r.client_name,c->nom,sizeof(r.client_name)-1); r.client_name[sizeof(r.client_name)-1]='\0';
        strncpy(r.client_email,c->email,sizeof(r.client_email)-1); r.client_email[sizeof(r.client_email)-1]='\0';
        r.salle_id = salles[idxSalle].id;
        strncpy(r.salle_name,salles[idxSalle].nom,sizeof(r.salle_name)-1); r.salle_name[sizeof(r.salle_name)-1]='\0';
        strncpy(r.date,date,sizeof(r.date)-1); r.date[sizeof(r.date)-1]='\0';
        r.heure_debut=hd; r.heure_fin=hf;
        r.nombre_personnes=nb_pers; r.statut=CONFIRMED;
        r.tarif=prixAvecFidelite(c,calculPrixBase(r.salle_id,hd,hf));
        reservations[nb_res++]=r;
        c->total_reservations++; c->total_spent+=r.tarif;
        sauvegarderClients(); sauvegarderReservations();
        return 0;
    }
}

void genererFactureParId(int id) {
    if(id<1 || id>nb_res) return;
    Reservation *r = &reservations[id-1];
    char nomFichier[64]; snprintf(nomFichier,sizeof(nomFichier),"facture_%d.txt",r->id);
    FILE *f = fopen(nomFichier,"w");
    if(!f) return;
    fprintf(f,"FACTURE ID: %d\nClient: %s (%s)\nSalle: %s\nDate: %s\nHeure: %d-%d\nNombre personnes: %d\nMontant: %.2f\n",
            r->id,r->client_name,r->client_email,r->salle_name,r->date,r->heure_debut,r->heure_fin,r->nombre_personnes,r->tarif);
    fclose(f);
}

void listerReservations() {
    for(int i=0;i<nb_res;i++) {
        Reservation *r=&reservations[i];
        printf("ID %d | Client: %s | Salle: %s | Date: %s | %d-%d | %d pers | %.2f | Statut %d\n",
               r->id,r->client_name,r->salle_name,r->date,r->heure_debut,r->heure_fin,r->nombre_personnes,r->tarif,r->statut);
    }
}

void chiffreAffairesParSalle() {
    float ca[MAX_SALLE]={0};
    for(int i=0;i<nb_res;i++) ca[reservations[i].salle_id-1]+=reservations[i].tarif;
    for(int i=0;i<MAX_SALLE;i++) printf("%s : %.2f\n",salles[i].nom,ca[i]);
}

void reservationsParMois_affiche() {
    int mois_count[12]={0};
    for(int i=0;i<nb_res;i++) {
        int y,m,d;
        if(sscanf(reservations[i].date,"%d-%d-%d",&y,&m,&d)==3 && m>=1 && m<=12) mois_count[m-1]++;
    }
    for(int i=0;i<12;i++) printf("Mois %02d : %d\n",i+1,mois_count[i]);
}

void sallesPopulaires_affiche() {
    int compte[MAX_SALLE]={0};
    for(int i=0;i<nb_res;i++) compte[reservations[i].salle_id-1]++;
    int maxi=0; for(int i=0;i<MAX_SALLE;i++) if(compte[i]>maxi) maxi=compte[i];
    for(int i=0;i<MAX_SALLE;i++) if(compte[i]==maxi) printf("%s : %d réservations\n",salles[i].nom,compte[i]);
}

void listerClients() {
    Client *c=clients;
    while(c) {
        printf("ID %d | %s | %s | tel: %s | réservations: %d | dépensé: %.2f\n",
               c->id,c->nom,c->email,c->telephone,c->total_reservations,c->total_spent);
        c=c->suivant;
    }
}

int main() {
    initSalles();
    chargerClients();
    chargerReservations();
    int choix;
    do {
        printf("\n--- Menu ---\n1) Nouvelle réservation\n2) Lister réservations\n3) Générer facture (par ID)\n4) Chiffre d'affaires par salle\n5) Réservations par mois\n6) Salles populaires\n7) Lister clients\n0) Quitter\n");
        choix = saisieInt("Choix : ",0,7);
        switch(choix){
            case 1: effectuerReservationInteractive(); break;
            case 2: listerReservations(); break;
            case 3: if(nb_res) genererFactureParId(saisieInt("ID réservation : ",1,nb_res)); break;
            case 4: chiffreAffairesParSalle(); break;
            case 5: reservationsParMois_affiche(); break;
            case 6: sallesPopulaires_affiche(); break;
            case 7: listerClients(); break;
        }
    } while(choix!=0);
    while(clients) { Client *tmp=clients->suivant; free(clients); clients=tmp; }
    return 0;
}
