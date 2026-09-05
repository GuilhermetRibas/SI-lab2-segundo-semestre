#include <stdio.h>
#include <lista.h>

bool insere_pos_lista(Lista l, dado_t d, int pos)
{
    if (l->cap <= l->num)
        return false;
        
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

int main()
{
}