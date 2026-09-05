#include <stdio.h>
#include <stdlib.h>
#include <lista.h>

bool realoca_dad(Lista l)
{
    if (l->cap <= l->num)
    {
        int nova_capacidade = l->cap * 2;
        dado_t *nova = reallo(l->dad, nova_capacidade * sizeof(dado_t));
        if (nova = NULL)
        {
            return false;
        }
        l->dad = nova;
        l->cap = nova_capacidade;
        return true;
    }
    return true;
}

bool insere_pos_lista(Lista l, dado_t d, int pos)
{
    realoca_dad(l);

    int final_lista = (l->num + l->pri) % l->cap;
    if (pos < l->pri || pos > final_lista)
        return false;

    for (int i = l->num; i > 0; i--)
    {
        int destino = (l->pri + i) % l->cap;
        int origem = (l->pri + i - 1) % l->cap;

        l->dad[destino] = l->dad[origem];
    }
    int indice = (l->pri + pos)% l->cap;
    l->dad[indice] = d;
    l->num++;
    return true;
}

bool remove_lista_pos(Lista l, int pos)
{

    if (l->num == 0)
        return false;

    int indice_pos = (l->pri + pos)% l->cap;

    int final = (l->pri + l->num) % l->cap;
    if (pos < 0 || pos > final)
        return false;

    if (pos == 0)
    {
        if (l->pri == l->cap)
        {
            l->pri = 0;
        }
        else
            l->pri++;
    }

    int atual;
    int proximo;
    
    for(int i = pos; i < final; i++){
        atual = (l->pri + i)% l->cap;
        proximo = (l->pri + i + 1) % l->cap;
        l->dad[atual] = l->dad[proximo];
    }

    l->num--;
    return true;
}

int main()
{
}