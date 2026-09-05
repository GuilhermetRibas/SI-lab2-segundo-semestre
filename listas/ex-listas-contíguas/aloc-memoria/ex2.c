#include <stdio.h>
#include <stdlib.h>
#include <lista.h>

bool realoca_dad(Lista l)
{
    if (l->cap <= l->num)
    {
        int nova_capacidade = l->cap * 2;
        int *nova = realloc(l->dad, nova_capacidade * sizeof(dado_t));
        if (nova == NULL)
        {
            return false;
        }
        l->dad = nova;
        l->cap = nova_capacidade;
        return true;
    }
    return true;
}

bool insere_final_lista(Lista l, dado_t d)
{
    realoca_dad(l);
    int indice;
    indice = (l->pri + l->num) % l->cap;
    l->num++;
    l->dad[indice] = d;

    return true;
}

bool remove_lista_final(Lista l){
    if(l->num == 0)return false;
    l->num--;
    return true;
}

int main()
{
}