#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <unistd.h>

/*
    Comandos: gcc -o fjsort.exe fjsort.c /// no deberia ser necesario agregar -lpthread -lrt, pero por si acaso...
              ./fjsort.exe
    Input: N : número de elementos del arreglo
           xi * N veces : los elementos del arreglo (separados por espacios)
           1 o 2 si se quiere usar merge sort o quicksort respectivamente
    Output: Arreglo ordenado
*/

#define UMBRAL 10

void swap(int *A, int *B){
    int temp = *A;
    *A = *B;
    *B = temp;
}

void selection(int *arr, int n) {  
    int i, j, min;
    for (i = 0; i < n - 1; ++i) {
        min = i;  
        for (j = i + 1; j < n; ++j)  
            if (arr[j] < arr[min])  
                min = j;  
        swap(&arr[i], &arr[min]);  
    }  
}  

void merge(int *arr, int l, int m, int r) 
{ 
    int len = r - l + 1; 
    int sorted[len]; 
    int a = l, b = m + 1, i; 
    for(i = 0; a <= m && b <= r; ++i) { 
        if (arr[a] < arr[b]) 
            sorted[i] = arr[a++]; 
        else
            sorted[i] = arr[b++];
    } 
  
    while (a <= m) sorted[i++]=arr[a++];
    while (b <= r) sorted[i++]=arr[b++];
  
    for (i = 0; i < len; i++,l++) 
        arr[l] = sorted[i]; 
} 

void merge_sort(int *arr, int l, int r) { 
    int len = r - l + 1; 
    if (len <= UMBRAL) { 
        selection(arr + l, len); 
        return; 
    } 
    pid_t lpid, rpid; 
    lpid = fork(); 
    if (lpid == 0) { 
        merge_sort(arr, l, l + len/2 - 1);
        exit(0); 
    } 
    else { 
        rpid = fork();
        if(rpid == 0) { 
            merge_sort(arr, l + len/2, r); 
            exit(0); 
        } 
    } 
    int status; 
    waitpid(lpid, &status, 0); 
    waitpid(rpid, &status, 0); 
    merge(arr, l, l + len/2 - 1, r); 
} 

int partition(int *arr, int low, int high) { 
    int pivot = arr[high]; 
    int i, j; 
    j = low;    
    for (i = low; i < high; i++) { 
        if(arr[i] < pivot) { 
            swap(&arr[i],&arr[j]);
            ++j; 
        } 
    }       
    swap(&arr[high],&arr[j]);   
    return j;  
} 

void quick_sort(int *arr, int low, int high) { 
    if (low < high) { 
        int pivot = partition(arr, low, high);
        pid_t lpid, rpid; 
        lpid = fork(); 
        if (lpid == 0) { 
            quick_sort(arr, low, pivot - 1); 
            exit(0); 
        } 
        else { 
            rpid = fork();
            if(rpid == 0) { 
                quick_sort(arr, pivot + 1, high); 
                exit(0); 
            } 
        } 
        int status; 
        waitpid(lpid, &status, 0); 
        waitpid(rpid, &status, 0);
    }  
} 

int main(){
    int N, i; scanf("%i",&N);
    key_t key = 1234;
    int shmid, *shmem, SHMSZ = N * sizeof(int);    
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    if ((shmem = shmat(shmid, NULL, 0)) == (int*) -1) {
        perror("shmat");
        exit(1);
    }
    
    for(i = 0; i < N; ++i){
        scanf("%i", &shmem[i]);
    }

    printf("Escriba 1 para usar Merge Sort o 2 para usar Quick Sort: ");
    int op; scanf("%i",&op);

    if(op == 1){
        merge_sort(shmem, 0, N - 1);
    }
    else if(op == 2){
        quick_sort(shmem, 0, N - 1);
    }

    for(i = 0; i < N; ++i){
        printf("%i ", shmem[i]);
    }printf("\n");

    if (shmdt(shmem) == -1) { 
        perror("shmdt"); 
        exit(1); 
    } 

    if (shmctl(shmid, IPC_RMID, NULL) == -1) { 
        perror("shmctl"); 
        exit(1); 
    }
    return 0;
}