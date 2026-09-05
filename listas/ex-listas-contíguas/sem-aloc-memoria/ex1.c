#include<stdio.h>

typedef int dado_t;

typedef struct lista *Lista;

struct lista {
    int cap;      // quantos dados cabem em dad
    int pri;      // onde em dad está o primeiro elemento da lista
    int num;      // quantos elementos tem na lista
    dado_t *dad;  // vetor de cap dados alocado dinamicamente
};

bool l_ins_ini(Lista l, dado_t d)
{
    if (l->num >= l->cap) return false;
    // o teste acima seria melhor como `if (l->cheia(l))`
    int ind;
    if (l->pri == 0) ind = l->cap - 1;// como o primeir oesta na posição 0, colocndo o novo elemento na ultima posição, 
    //quando for contar os indices o que erao primeiro vai passar a ser o segundo 
    /*
    lista cheia
    cap 6
    pri 0
    num 5
    dad 1 2 3 4 5 x
        0 1 2 3 4 5 (índices na lista)
    lista adicinando na primeira posição
    cap 6
    pri 3
    num 0
    dad 1 2 3 4 5 9
        1 2 3 4 5 0 (índices na lista)

    */ 
    else ind = l->pri - 1;// qualquer outra posição sendo a primeira, so tira um de pri

    // o cálculo de ind poderia ser `ind = (l->pri + l->cap - 1) % l->cap`
    // ou uma função para fazer esse cálculo: `ind = indice_pos(l, -1)`
    l->dad[ind] = d;        
    l->pri = ind;
    l->num++;
    return true;
}

int main(){

 



}