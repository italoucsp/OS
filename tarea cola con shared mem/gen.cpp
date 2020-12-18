#include <iostream>
#include <queue>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <thread>
#include <unistd.h>

//g++ -std=c++x0 -pthread gen.exe gen.cpp -lpthread

using namespace std;

#define LIMIT 1000000 //1000000000, la cola no soporta tantos datos
#define SHMSZ sizeof(shmem_segment)

queue<int> dataset;

struct shmem_segment{
    int rand_number;
    sem_t server_mutex;
    sem_t client_mutex;
};

void generator(){
    int n = 0;
    while (n < LIMIT){
        int random = rand() % 10000;
        //cout << "Number generated: " << random << endl;
        dataset.push(random);
        ++n;
    }
    cout << "Producción de " << LIMIT << " números terminada.\n";
}

int main(){
    srand(time(NULL));
    shmem_segment *shm, *s;
    key_t key = 8910;

    int shmid = shmget(key,SHMSZ,0666|IPC_CREAT);
    shm = (shmem_segment*) shmat(shmid,(void*)0,0);
    s = shm;
    sem_init(&s->server_mutex, 1, 0);
    sem_init(&s->client_mutex, 1, 5);
    thread gen_num(generator);
    sleep(1);
    while (!dataset.empty()){
        sem_wait(&s->client_mutex);
        s->rand_number = dataset.front();
        cout << s->rand_number << endl;
        dataset.pop();
        sleep(3);
        sem_post(&s->server_mutex);
    }gen_num.join();

    sem_destroy(&s->server_mutex);
    sem_destroy(&s->client_mutex);
    shmdt(shm);

    return 0;
}
