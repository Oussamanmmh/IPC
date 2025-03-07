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
#define nc 2 

int main(){
    int * mem_partage ;
    char nageur_num[10] ;
    key_t cle_sem = ftok("main.c",2);
    int sem = semget(cle_sem , 3 ,IPC_CREAT | 0666);
    if(sem == -1){
        perror("Erreur : impossible de créer l'ensemble des sémaphores");
        exit(1);
    }
    //initialisation des semaphores ,
    semctl(sem , 0, SETVAL , nc ) ;
    semctl(sem ,1 ,SETVAL , 1 ) ;
    semctl(sem ,2,SETVAL , 0 ) ;
    //creation de segment partage 
    key_t cle_seg = ftok("main.c" , 3) ;
    int seg_id = shmget(cle_seg , 2*sizeof(int) , IPC_CREAT | 0666);
    if (seg_id == -1)
    {
       perror("Erreur : impossible de creer la memoire partage");
       exit(1);
    }
    mem_partage =(int*) shmat(seg_id , NULL , 0 ) ;
    mem_partage[0]= 0 ; // pour le nombre de panier demander
    mem_partage[1]= 0 ; //pour le nombre de panier occuper

    //creation des processus nageurs 
    int p ;
    for (int i =0 ; i<10 ;i++){
        sprintf(nageur_num, "%d", i); //pour avoir le numero du nageur

        p = fork();
        if (p==0){
             if(execl("pgme_nageur" , "pgme_nageur" , nageur_num , NULL)==-1){
                perror("Erreur : impossible de creer le processus nageur");
                exit(1);
            };
        }
        

    }
    while(wait(NULL) != -1);
    shmdet(mem_partage);
    shmctl(seg_id , IPC_RMID , NULL);
    semctl(sem , 0 , IPC_RMID );
    return 0 ;
}