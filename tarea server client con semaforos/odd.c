#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <stdbool.h> 

#define SHMSZ sizeof(struct shmem_segment)

struct shmem_segment{
    int rnd_number;
    sem_t server_mutex;
    sem_t client_mutex;
    bool first_even;
};

main()
{
    int shmid;
    key_t key;
    struct shmem_segment *shm, *s;
    key = 4321;

    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (struct shmem_segment*) -1) {
        perror("shmat");
        exit(1);
    }

    s = shm;
    
    if(!s->first_even){
        sem_wait(&s->server_mutex);
        printf("%i es impar.\n",s->rnd_number);
        sem_post(&s->client_mutex);
    }

    exit(0);
}