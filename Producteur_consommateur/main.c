#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>

int sem, tampon;

int main(int argc, char const *argv[]) { 
    // Création des sémaphores
    key_t cle = ftok("main.c", 2);
    sem = semget(cle, 2, IPC_CREAT | 0666);
    if (sem == -1) {
        perror("Erreur : impossible de créer l'ensemble des sémaphores");
        exit(1);
    }

    // Initialiser les sémaphores
    semctl(sem, 0, SETVAL, 0); // Sémaphore 0 initialisé à 0
    semctl(sem, 1, SETVAL, 1); // Sémaphore 1 initialisé à 1

    // Création des segments partagés
    cle = ftok("main.c", 3);
    tampon = shmget(cle, sizeof(int), IPC_CREAT | 0666);
    if (tampon == -1) {
        perror("Erreur : impossible de créer la mémoire partagée");
        exit(2);
    }

    // Création des processus enfants
    int p = fork();
    if (p == 0) {
        printf("Processus producteur créé\n");
        if (execl("./pgme_producteur", "./pgme_producteur", NULL) == -1) {
            perror("Erreur lors de l'exécution de pgme_producteur");
            exit(1);
        }
    } else {
        int p2 = fork();
        if (p2 == 0) {
            printf("Processus consommateur créé\n");
            if (execl("./pgme_consommateur", "./pgme_consommateur", NULL) == -1) {
                perror("Erreur lors de l'exécution de pgme_consommateur");
                exit(1);
            }
        }
    }

    // Attente de la fin des processus enfants
    while (wait(NULL) != -1);

    // Suppression des ressources IPC
    shmctl(tampon, IPC_RMID, NULL); // Suppression du segment partagé
    semctl(sem, 0, IPC_RMID);       // Suppression des sémaphores
    printf("Ressources IPC libérées\n");

    return 0;
}
