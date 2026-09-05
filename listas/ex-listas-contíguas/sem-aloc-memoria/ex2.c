#include<stdio.h>
#include<lista.h>


bool insere_final_lista(Lista l, dado_t d){
    if(l->cap <= l->num)return false;
    int indice;
    indice = (l->pri + l->num) % l->cap;
    l->num++;
    l->dad[indice] = d;

    return true;
}


int main(){



}