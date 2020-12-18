#include <iostream>
#include <thread>
#include <algorithm>

/*
    Comandos: g++ -std=c++x0 -pthread multisort.exe multisort.cpp
              ./multisort.exe
    Input: n : número de elementos del arreglo
           xi * n veces : los elementos del arreglo (separados por espacios)
    Output: Arreglo ordenado
*/

using namespace std;

int arr[500000];

void semi_sort(size_t begin, size_t body){
    sort(arr + begin, arr + body);
}

void merge_semi_lists(int A, int B, int n){
    int temp[500000];
    for(int i = 0; i < n; ++i){
        temp[i] = arr[i];
    }
    int A_s = B, B_s = n, i = 0;
    while(A < A_s && B < B_s){
        if(temp[A] < temp[B]){
            arr[i++] = temp[A++];
        }
        else{
            arr[i++] = temp[B++];
        }
    }
    while(A < A_s){arr[i++] = temp[A++];}
    while(B < B_s){arr[i++] = temp[B++];}
}

int main(){
    int n; cin>>n;
    for(int i = 0; i < n; ++i){
        cin>>arr[i];
    }
    thread L(semi_sort,0,n/2);
    thread R(semi_sort,n/2,n);
    L.join(); R.join();
    thread M(merge_semi_lists,0,n/2,n);
    M.join();
    cout << "Sorted List: ";
    for(int i = 0; i < n; ++i){
        cout << arr[i] << ' ';
    }cout << '\n';
    return 0;
}
