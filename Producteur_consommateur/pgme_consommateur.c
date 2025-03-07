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

void P(int sem_id, int sem_num)
{
  struct sembuf operation = {sem_num, -1, 0};
  semop(sem_id, &operation, 1);
}

void V(int sem_id, int sem_num)
{
  struct sembuf operation = {sem_num, 1, 0};
  semop(sem_id, &operation, 1);
}

int main(int argc, char const *argv[])
{
  printf("Le consommateur démarre...\n");

  int mc = 0;
  int cpt = 0;

  // Mémoire partagée
  key_t cle = ftok("main.c", 3);
  int tampon_id = shmget(cle, sizeof(int), IPC_CREAT | 0666);
  int *buffer = (int *)shmat(tampon_id, NULL, 0);

  // Sémaphores
  key_t cle2 = ftok("main.c", 2);
  int sem = semget(cle2, 2, IPC_CREAT | 0666);

  do
  {
    P(sem, 0); // Wait (objet disponible)

    mc = buffer[0]; // Retirer un objet
    printf("Le consommateur vient de retirer un objet : %d\n", mc);
    sleep(2);

    V(sem, 1); // Signal (place disponible)
    cpt++;
  } while (cpt < 10);

  shmdt(buffer); // Détacher la mémoire partagée
  return 0;
}
