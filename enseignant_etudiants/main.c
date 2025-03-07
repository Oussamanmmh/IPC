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
int main(int argc, char const *argv[])
{   
    char etudiant_num[10] ;
    key_t sem_key = ftok("main.c" , 2) ;//la cle 
     
    //initialisation des semaphores ,
    for (int i = 0; i < 6; i++)// plein
    {
        semctl(sem_id , i , SETVAL , 0);
    }
    for (int i = 6 ; i < 10; i++)//Mutex
    {
         semctl(sem_id , i , SETVAL , 1);
    }
     semctl(sem_id , 10 , SETVAL , M) ;//Vide
    key_t seg_key = ftok("main.c" , 3) ;//la cle 
    int seg_id = shmget(seg_key , M*sizeof(int), IPC_CREAT|0666);
  key_t seg_cpt_key = ftok("main.c" , 4) ;//la cle 
    int seg_cpt_id = shmget(seg_cpt_key , M*sizeof(int), IPC_CREAT|0666);

    int p = fork();
    if (p==0)
    {
       if( execl("./enseignant" ,"./enseignant", NULL)==-1){
            perror("error: creation de processus enseignant\n");
            exit(1);
       }
    }
    for (int i = 0; i <N; i++)
    {
        sprintf(etudiant_num , "%d" , i);
        int p1 = fork() ;
                if(p1 == 0 ){
                    if( execl("./etudiant" ,"./etudiant", etudiant_num , NULL)==-1){
                    perror("error: creation de processus etudiant\n");
                    exit(1);
            }
        }
    }
    
    while(wait(NULL)!=-1);

    shmctl(seg_id ,IPC_RMID , NULL );
    shmctl(seg_cpt_id ,IPC_RMID , NULL );
    semctl(sem_id , 0 , IPC_RMID);
    
    return 0;
}
