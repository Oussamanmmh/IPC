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
#define M 4 
#define N 6 
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
    int i = atoi(argv[1]);
     int j = 0 ;
     int exo ;
     int k = 0 ;
     int * cpt;
     int * exos ;
     
    key_t seg_key = ftok("main.c" , 3) ;//la cle 
    int seg_id = shmget(seg_key , M*sizeof(int), IPC_CREAT|0666);
    exos = (int*) shmat(seg_id , 0 , 0);

    key_t seg_cpt_key = ftok("main.c" , 4) ;//la cle 
    int seg_cpt_id = shmget(seg_cpt_key , M*sizeof(int), IPC_CREAT|0666);
    cpt = (int*) shmat(seg_cpt_id , 0 , 0);
     do{
        
        P(sem_id , i ) ;

        exo = exos[j];
        printf("L\'etudiant %d fait l\'exercice %d\n" , i , exos[j]);
        P(sem_id , j+N) ;
        cpt[j] = (cpt[j]+1)% N ;
        if(cpt[j]==0){
            V(sem_id , 10) ;
        }
        V(sem_id , j+N) ;
        j=(j+1)% M ; 
        k++;
     }while(k<10);
    shmdt(cpt) ;
    shmdt(exos);
     
    return 0;
}
