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

    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (struct shmem_segment*) -1) {
        perror("shmat");
        exit(1);
    }

    s = shm;

    srand(time(NULL));
    sem_init(&s->server_mutex, 1, 0);
    sem_init(&s->client_mutex, 1, 1);

    while (1){
        sem_wait(&s->client_mutex);
        s->rnd_number = rand() % 10000;
        printf("The number is: %i\n",s->rnd_number);
        s->first_even = true;
        sem_post(&s->server_mutex);
    }
    sem_destroy(&s->server_mutex);
    sem_destroy(&s->client_mutex);
    exit(0);
}