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
typedef struct enseignant
{
    /* data */
};

int main (){
    
    int  j  = 0 ; 
    int exo = 0 ; 
    int * buffer ;
    key_t sem_key = ftok("main.c" ,2);
    int sem_id = semget(sem_key , 11 , IPC_CREAT | 0666);
    //attacher
    key_t seg_key = ftok("main.c",3);
    int seg_id = shmget(seg_key , M*sizeof(int), IPC_CREAT | 0666);

    buffer = (int *)shmat(seg_id , 0 , 0);
    int cpt = 0;
    do{
        sleep(2);
        P(sem_id , 10 ); 
        buffer[j] = exo ;
        printf("L\'enseignant pose l\'exercice %d\n " , buffer[j]);
        exo ++ ;
        j = (j+1) % M ;
        cpt++ ;
        for (int i = 0; i < N ; i++)
        {
            V(sem_id , i ); 
        }
        
    }while(cpt < 10) ;

    shmdt(buffer);


}