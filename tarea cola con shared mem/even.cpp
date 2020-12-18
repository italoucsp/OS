#include <iostream>
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <stdio.h>
#include <semaphore.h>

//g++ -o even.exe even.cpp -lpthread

using namespace std;

#define SHMSZ sizeof(shmem_segment)

struct shmem_segment{
    int rand_number;
    sem_t server_mutex;
    sem_t client_mutex;
};
  
int main() 
{ 
    shmem_segment *shm, *s;
    key_t key = 8910;

    int shmid = shmget(key,SHMSZ,0666|IPC_CREAT);
    shm = (shmem_segment*) shmat(shmid,(void*)0,0);
    s = shm;

    if(s->rand_number % 2 == 0){
        sem_wait(&s->server_mutex);
        cout << s->rand_number << " es par.\n";
        sem_post(&s->client_mutex);
    }
  
    return 0; 
} 