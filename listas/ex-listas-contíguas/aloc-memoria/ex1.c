#include <stdio.h>
#include <stdlib.h>
#include <lista.h>

bool realoca_dad_com_realloc(Lista l)
{

    if (l->cap <= l->num)
    {
        int nova_capacidade = l->cap * 2;

        dado_t *nova = realloc(l->dad, nova_capacidade * sizeof(dado_t));
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

bool realoca_dad_com_malloc(Lista l)
{

    if (l->cap <= l->num)
    {
        int nova_capacidade = l->cap * 2;

        dado_t *nova = malloc(nova_capacidade * sizeof(dado_t));
        if (nova == NULL)
        {
            return false;
        }
        for (int i = 0; i < l->num; i++)
        {
            nova[i] = l->dad[i];
        }
        free(l->dad);
        l->dad = nova;
        l->cap = nova_capacidade;

        return true;
    }
    return true;
}

bool insere_lista_inicio(Lista l, dado_t d)
{

     realloc_dad_com_realloc(l);
    realloc_dad_com_malloc(l);

    int indice;
    if (l->pri == 0)
    {
        indice = l->cap - 1;
    }
    else
    {
        indice = l->pri - 1;
    }
    l->num++;
    l->dad[indice] = d;
    l->pri = indice;
    return true;
}

bool remove_lista_inicio(Lista l)
{
    if (l->num == 0)
        return false;
    if (l->cap == l->pri)
    { // ultimo elemento
        l->pri = 0;
    }
    else
    {   
        l->pri++;
    }
    l->num--;
    return true;
}

int main()
{
}