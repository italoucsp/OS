////////////////////////////SISTEMAS OPERATIVOS///////////////////////////////////////
______________________________________________________________________________________
Alumno: Italo Mamani Huaricallo
______________________________________________________________________________________
Grupo: CCOMP6-1
______________________________________________________________________________________
######################################################################################

1. Compilación de archivos .c:

gcc -o gen.exe gen.c -lpthread -lrt
gcc -o even.exe even.c -lpthread -lrt
gcc -o odd.exe odd.c -lpthread -lrt

2. Detalles (VERSION CON FLUJO):

Los tres programas tienen un flujo utilizando 2 semáforos y un flag para 
indicar el orden de procesamiento del número random generado.

Uso de los semáforos similar a "productor y consumidor de la maquina de 
latas" visto en clase.

Los semáforos básicamente reemplazan al bussy wait usado en la versión 1
del código que usaba un caracter "*".

El flujo es: generar número -> verficar si es par -> sino es par entonces es 
impar->generar otro número->... (y si es par, de inmediato generar otro 
random).

Si se intenta ejecutar el archivo odd.exe sin haber ejecutado even.exe, no
pasará nada, aunque el número sea impar, porque los programas siguen el
flujo ya mencionado.

#######################################################################################

VERSION SIN FLUJO: (no hay orden para odd.exe e even.exe)
####################################
///////gen.c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define SHMSZ sizeof(struct shmem_segment)

struct shmem_segment{
    int rnd_number;
    sem_t server_mutex;
    sem_t client_mutex;
};

main()
{
    int shmid;
    key_t key;
    struct shmem_segment *shm, *s;
    key = 1234;

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
        sem_post(&s->server_mutex);
    }
    sem_destroy(&s->server_mutex);
    sem_destroy(&s->client_mutex);
    exit(0);
}
####################################
/////////even.c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define SHMSZ sizeof(struct shmem_segment)

struct shmem_segment{
    int rnd_number;
    sem_t server_mutex;
    sem_t client_mutex;
};

main()
{
    int shmid;
    key_t key;
    struct shmem_segment *shm, *s;
    key = 1234;

    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (struct shmem_segment*) -1) {
        perror("shmat");
        exit(1);
    }

    s = shm;
    
    if(s->rnd_number % 2 == 0){
       sem_wait(&s->server_mutex);
       printf("%i es par.\n",s->rnd_number);
       sem_post(&s->client_mutex);
    }

    exit(0);
}

######################################3
////////////odd.c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define SHMSZ sizeof(struct shmem_segment)

struct shmem_segment{
    int rnd_number;
    sem_t server_mutex;
    sem_t client_mutex;
};

main()
{
    int shmid;
    key_t key;
    struct shmem_segment *shm, *s;
    key = 1234;

    if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shm = shmat(shmid, NULL, 0)) == (struct shmem_segment*) -1) {
        perror("shmat");
        exit(1);
    }

    s = shm;
    
    if(s->rnd_number % 2 == 1){
        sem_wait(&s->server_mutex);
        printf("%i es impar.\n",s->rnd_number);
        sem_post(&s->client_mutex);
    }

    exit(0);
}

/////////////////////////////////FIN DE README/////////////////////////////