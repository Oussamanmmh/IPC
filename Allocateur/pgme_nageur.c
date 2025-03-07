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
#define np 5

// la fonction p pour tester
void P(int sem_id, int sem_num)
{
  struct sembuf operation = {sem_num, -1, 0};
  semop(sem_id, &operation, 1);
}

//la fonction v pour faire reveiller des processus
void V(int sem_id, int sem_num)
{
  struct sembuf operation = {sem_num, 1, 0};
  semop(sem_id, &operation, 1);
}

//les procedures 
void demander_panier(int *nump , int sem_id , int * mem_partage){
   
    P(sem_id , 1) ;
    mem_partage[0]++ ;
    if(mem_partage[1] == np){

        V(sem_id , 1) ;
        P(sem_id , 2) ;

    }
    mem_partage[0]-- ;
    mem_partage[1]++ ;
    *nump = mem_partage[1] ;
    V(sem_id ,1) ;
    
}

void liberer_panier(int i , int *nump , int sem_id ,int * mem_partage ){
    
    P(sem_id ,1) ;
    mem_partage[1]-- ;
    *nump = mem_partage[1];
    if(mem_partage[0]>0){
        printf("LE NAGEUR %i VA LIBERER UN PANIER ET IL YA A:%i PANIER DEMANDER EN ATTENTE\n",i,mem_partage[0]);
        V(sem_id , 2) ;
    }
    else{
        V(sem_id , 1) ;
    }

}
void demander_cabine(int sem_id){
   
    P(sem_id , 0) ;
}
void liberer_cabine(int sem_id){
   
    V(sem_id , 0) ;
}

int main(int argc, char const *argv[])
{   
    int nageur_num = atoi(argv[1]);
    int nump ;
    int * mem_partage ;
    key_t sem_key = ftok("main.c" ,2) ;
    //creation de l'ensemble des semaphores
    int sem_id = semget(sem_key , 3 ,IPC_CREAT | 0666);
    if(sem_id ==-1){
        perror("Erreur : impossible de créer l'ensemble des sémaphores");
        exit(1);
    }

    //attament de la memoire partage 
    key_t mem_key = ftok("main.c" ,3);
    int mem_id = shmget(mem_key , 2*sizeof(int), IPC_CREAT|0666);
    if(mem_id == -1){
        perror("Erreur : impossible de creer la memoire partage");
        exit(1);
    }
    
    mem_partage = (int*)shmat(mem_id , NULL , 0);
    //debut de la simulation
        demander_panier(&nump , sem_id , mem_partage);
        demander_cabine(sem_id);
        sleep(4);
        liberer_cabine(sem_id);
        sleep(7);
        printf("JE SUI LE NAGEUR NUMERO %d , j occupe le panier %d \n",nageur_num,nump);
        demander_cabine(sem_id);
        sleep(4);
        liberer_cabine(sem_id);
        liberer_panier(nageur_num , &nump , sem_id , mem_partage);
        printf("je suis le nageur %d j'ai liberer le panier, il rest :%d panier libres " , nageur_num , np-nump );   
    return 0;

    shmdet(mem_partage);//detachement de la memoire partage
}
