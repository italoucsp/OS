#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <stdbool.h> 

#define MAXSIZE     128
#define SHMSZ sizeof(struct shmem_segment)

void die(char *s)
{
  perror(s);
  exit(1);
}

struct msgbuf
{
    long mtype;
    char mtext[MAXSIZE];
};

struct shmem_segment{
    int rnd_number;
    sem_t server_mutex;
    sem_t client_mutex;
};

int count_digits(int number){
    int n_of_d = 0;
    while(number){
        number = number / 10;
        ++n_of_d;
    }
    return n_of_d;
}

main()
{
    int shmid, msqid;
    key_t key = 5678;
    struct shmem_segment *shm, *s;
    struct msgbuf sbuf;
    size_t buflen;

    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (struct shmem_segment*) -1) {
        perror("shmat");
        exit(1);
    }

    if ((msqid = msgget(key, IPC_CREAT | 0666)) < 0)
      die("msgget");

    sbuf.mtype = 1;

    s = shm;
    
    if(s->rnd_number % 2 == 1){
        sem_wait(&s->server_mutex);
        printf("%i es impar.\n",s->rnd_number);
        int enough = count_digits(s->rnd_number);
        itoa(s->rnd_number, sbuf.mtext, 10);
        sbuf.mtext[enough] = '.';
        int i = enough + 1;
        for(; i < enough*2 + 1; ++i){
            sbuf.mtext[i] = sbuf.mtext[i - enough - 1];
        }
        buflen = strlen(sbuf.mtext) + 1;
        if (msgsnd(msqid, &sbuf, buflen, IPC_NOWAIT) < 0) {
            printf ("%d, %ld, %s, %d \n", msqid, sbuf.mtype, sbuf.mtext, (int)buflen);
            die("msgsnd");
        }
        sem_post(&s->client_mutex);
    }

    exit(0);
}