# reservation-system fichiers principaux
Software in C for managing room reservations
Ce projet permet de gérer les réservations de salles pour une entreprise ou un espace de coworking. Il fournit des fonctionnalités complètes telles que :
    Création et gestion des réservations
    Vérification de la capacité et de la disponibilité des salles
    Calcul automatique des tarifs
    Génération de factures pour les clients
    Statistiques sur les réservations et le chiffre d’affaires
    Identification des salles les plus populaires

Le projet est développé en C et utilise des structures (Reservation et Salle) pour gérer les informations.

Fichiers principaux: 
main.c : fichier principal avec le menu et les appels de fonctions.

Fonctions principales:
        1)Fonction : Disponibilite
Cette fonction vérifie si une salle donnée est disponible pour un créneau horaire spécifique à une date donnée.
Valeur de retour :
0 → La salle est disponible pour le créneau demandé
1 → La salle n’est pas disponible (chevauchement avec une réservation existante)

        2)Fonction : verifCapacite
Cette fonction vérifie si une salle peut accueillir le nombre de personnes demandé pour une réservation.
Valeur de retour :
0 → La capacité de la salle est suffisante pour le nombre de personnes demandé
1 → La capacité de la salle est insuffisante

        3)creerReservation
Cette fonction permet de créer une nouvelle réservation et de l’ajouter au tableau des réservations si :
   La salle peut accueillir le nombre de personnes demandé
   La salle est disponible pour le créneau horaire souhaité
Valeur de retour :
0 → La réservation a été créée avec succès
1 → La réservation n’a pas pu être créée (salle non disponible ou capacité insuffisante)

    4)calculerTarifTotal
Cette fonction calcule le tarif total d’une réservation en fonction de la salle choisie et de la durée de la réservation.
Valeur de retour :
Retourne le tarif total calculé pour la réservation (float).

    5)sauvegardetarifs_salle
Cette fonction permet de sauvegarder les tarifs horaires de toutes les salles dans un fichier texte.
Valeur de retour :
Aucun (fonction void).
En cas d’erreur d’ouverture du fichier, affiche un message d’erreur.

    6)sauvegardetarifs_reservation
Cette fonction permet de sauvegarder toutes les réservations dans un fichier texte, avec le tarif calculé pour chacune.
Valeur de retour :
Aucun (void).
En cas d’erreur d’ouverture du fichier, affiche un message d’erreur.

    7)genererFacture
Cette fonction génère une facture individuelle pour une réservation sous forme de fichier texte.
Valeur de retour :
Aucun (void).
Affiche un message d’erreur si le fichier ne peut pas être créé.
    8)chiffredaffairesparsalle
Cette fonction calcule et affiche le chiffre d’affaires total pour chaque salle en fonction des réservations effectuées.

    9)sallesPopulaires
Cette fonction identifie et affiche les salles les plus populaires, c’est-à-dire celles avec le plus grand nombre de réservations.
