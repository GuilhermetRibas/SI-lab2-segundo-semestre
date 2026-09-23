#include <stdio.h>
#include <stdlib.h>
#include<stdbool.h>
#include <assert.h>

typedef struct lista *Lista;
typedef struct no No;

struct lista
{
    int tamanho;
    No *sentinela;
};

struct no
{
    int dado;
    No *ant;
    No *prox;
};

static void l_ok(Lista l)
{
    assert(l != NULL);
    assert(l->sentinela != NULL);
    int tam = 0;
    for(No *no = l->sentinela->prox; no != l->sentinela; no = no->prox){
        tam++;
    }
    assert(tam == l->tamanho);
}

static No *l_aloca_no()
{
  No *no = malloc(sizeof(No));
  assert(no != NULL);
  return no;
}

static void l_libera_no(No *no){
    free(no);
}

static void l_init(Lista l){
    l->tamanho = 0;
    l->sentinela->prox = l->sentinela;
    l->sentinela->ant = l->sentinela;
}

Lista l_cria(){
    Lista nova = malloc(sizeof(struct lista));
    assert(nova != NULL);
    nova->sentinela = l_aloca_no();

    l_init(nova);
    l_ok(nova);
    return nova;
}

static bool l_vazia(Lista l){
    l_ok(l);
    return l->tamanho == 0;
}

Lista l_insere_inicio(Lista l, int d){
    l_ok(l);
    No *novo_no = l_aloca_no();

    novo_no->dado = d;

    novo_no->ant = l->sentinela;
    novo_no->prox = l->sentinela->prox;

    novo_no->prox->ant = novo_no;
    l->sentinela->prox = novo_no; 

    if(l_vazia(l))l->sentinela->ant = novo_no;
    l->tamanho++;
    l_ok(l);
    return l;
}

Lista l_insere_final(Lista l, int d){
    l_ok(l);
    if(l_vazia(l))return l_insere_inicio(l,d);

    No *novo_no = l_aloca_no();
    novo_no->dado = d;
    novo_no->ant = l->sentinela->ant;
    novo_no->prox = l->sentinela;
    novo_no->ant->prox = novo_no;
    l->sentinela->ant = novo_no;
    l->tamanho++;
    l_ok(l);
    return l;
}

static int l_decide_percurso(Lista l, int n)
{
    int medida = l->tamanho / 2;

    if (n < medida)
        return 1;

    return 0;
}

static No *l_no_na_posicao(Lista l, int pos){
    assert(pos >= 0 && pos < l->tamanho);
    No *no ;
    int p;

    if(l_decide_percurso(l,pos) == 1){
    no = l->sentinela->prox;
        p=0;
    while(p < pos && no != l->sentinela){
        no = no->prox;
        p++;
    }   
    }else{
    no = l->sentinela->ant;
        p = l->tamanho-1;
        while(p < pos && no != l->sentinela){
        no = no->ant;
        p--;
    }   
    }
    return no;
}

Lista l_insere_na_pos(Lista l, int d, int pos){
    l_ok(l);
    assert(pos >=  0 && pos <= l->tamanho);
    if(pos == 0) return l_insere_inicio(l,d);
    if(pos == l->tamanho) return l_insere_final(l,d);

    No *novo_no = l_aloca_no();
    novo_no->dado = d;

    No *anterior = l_no_na_posicao(l, pos - 1);
    No *proximo = l_no_na_posicao(l, pos);

    novo_no->prox = proximo;
    novo_no->ant = anterior; 

    proximo->ant = novo_no;
    anterior->prox = novo_no;
    l->tamanho++;
    l_ok(l);
    return l;

}

static No *l_no_com_valor_maior_que_n(Lista l, int n){
    No *no = l->sentinela->prox;
    while(no != l->sentinela){
        if(no->dado > n)return no;
        no = no->prox;
    }
    return NULL;
}

static int l_pos_no(Lista l, No *no){
    No *inicio = l->sentinela->prox;
    int pos = 0;
    while(inicio != no){
        inicio = inicio->prox;
        pos++;
    }
    return pos;
}

Lista l_insere_ordenado(Lista l, int d){
    l_ok(l);
    No *maior = l_no_com_valor_maior_que_n(l,d);
    if(maior == NULL)return l_insere_final(l, d);

    No *menor = maior->ant;
    if(menor  == l->sentinela)return l_insere_inicio(l,d);
    int pos_maior = l_pos_no(l, maior);
    return l_insere_na_pos(l,d,pos_maior);
    
    l_ok(l);

}



void l_imprime(Lista l){
    l_ok(l);
    for(No *no = l->sentinela->prox; no != l->sentinela; no = no->prox){
        printf("%d", no->dado);
    }
}

void l_libera_lista(Lista l){

    l_ok(l);
    No *no = l->sentinela->prox;

    while (no != l->sentinela)
    {
        No *proximo = no->prox;
        l_libera_no(no);
        no = proximo;
    }
    free(l->sentinela);
    free(l);
}



