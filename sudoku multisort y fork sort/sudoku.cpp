#include <iostream>
#include <thread>

/*
    Comandos: g++ -std=c++x0 -pthread sudoku.exe sudoku.cpp
              ./sudoku.exe
    Input: por cada fila (9), 9 numeros (separados por espacios)
    Output: Correcto si el sudoku es válido, sino, Incorrecto.
*/

using namespace std;

int puzzle[9][9];
bool ok = true;

void check_subgrid(int li, int lj){
    bool check[10] = {0};  
    for(int i = li; i < 3 + li; ++i){
        for(int j = lj; j < 3 + lj; ++j){
            check[puzzle[i][j]] = 1;
        }
    }
    for(int i = 1; i < 10; ++i){
        ok = ok && check[i];
    }
}

void check_rows(){
    for(int i = 0; i < 9; ++i){
        bool check[10] = {0};
        for(int j = 0; j < 9; ++j){
            check[puzzle[i][j]] = 1;
        }
        for(int i = 1; i < 10; ++i){
            ok = ok && check[i];
            if(!ok) return;
        }
    }
}

void check_cols(){
    for(int j = 0; j < 9; ++j){
        bool check[10] = {0};
        for(int i = 0; i < 9; ++i){
            check[puzzle[i][j]] = 1;
        }
        for(int i = 1; i < 10; ++i){
            ok = ok && check[i];
            if(!ok) return;
        }
    }
}

int main(){
    for(int i = 0; i < 9; ++i){
        for(int j = 0; j < 9; ++j){
            cin>>puzzle[i][j];
        }
    }
    thread NW(check_subgrid,0,0); thread N(check_subgrid,3,0); thread NE(check_subgrid,6,0);
    thread W(check_subgrid,0,3);  thread C(check_subgrid,3,3); thread E(check_subgrid,6,3);
    thread SW(check_subgrid,0,6); thread S(check_subgrid,3,6); thread SE(check_subgrid,6,6);
    NW.join();N.join();NE.join();
    W.join(); C.join();E.join();
    SW.join();S.join();SE.join();
    thread COL(check_cols);
    thread ROW(check_rows);
    COL.join(); ROW.join();
    cout << ((ok)?"Correcto\n":"Incorrecto\n");
    return 0;
}
