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

void P(int sem_id, int sem_num) {
    struct sembuf operation = {sem_num, -1, 0};
    semop(sem_id, &operation, 1);
}

void V(int sem_id, int sem_num) {
    struct sembuf operation = {sem_num, 1, 0};
    semop(sem_id, &operation, 1);
}

int main(int argc, char const *argv[]) {
    printf("Le producteur démarre...\n");

    int mp = 0;
    int cpt = 0;

    // Mémoire partagée
    key_t cle = ftok("main.c", 3); // utiliser la même que celle dans le main
    int t_id = shmget(cle, sizeof(int), IPC_CREAT | 0666); // il est cree dans le main
    int *buffer = (int *)shmat(t_id, 0, 0);

    // Sémaphores
    key_t cle2 = ftok("main.c", 2);
    int sem = semget(cle2, 2, IPC_CREAT | 0666);

    do {
        P(sem, 1); // Wait (consommateur non actif)

        buffer[0] = mp++; // Déposer un objet
        printf("Le producteur vient de déposer un objet : %d\n", buffer[0]);
        sleep(2);
        V(sem, 0); // Signal (objet disponible)
        cpt++;
    } while (cpt < 10);

    shmdt(buffer); // Détacher la mémoire partagée
    return 0;
}
