#include<stdio.h>
#include<stdlib.h>
#include<assert.h>

typedef struct lista *Lista;

typedef struct no No;

typedef int t_dado;

struct no{ // estrutura do nó
    t_dado dado;
    No *prox;
};

struct lista{ // descritor
    No *primeiro;
    No *ultimo;
    int tamanho;

};

static void l_ok(Lista l){
    assert(l != NULL);
    int tam =0;
    for(No *no = l->primeiro; no != NULL; no = no->prox){
        tam++;
    }
    assert(tam == l->tamanho);
    if(tam == 0) assert(l->ultimo == NULL);
}

static void l_libera_no(Lista l, No *n){
    free(n);
}

static No *l_aloca_no(Lista l){
    No *n = malloc(sizeof(No));
    asser(n != NULL);
    return n;
}

static void l_init(Lista l){
    l->primeiro = NULL;
    l->ultimo = NULL;
    l->tamanho = 0;
}

Lista l_cria(){
    Lista nova = malloc(sizeof(struct lista));
    l_init(nova);
    l_ok(nova);
    return nova;
}

Lista l_insere_inicio(Lista l, t_dado d){
    l_ok(l);
    No *novo_no = l_aloca_no(l);

    novo_no->dado = d;
    novo_no->prox = l->primeiro;
    l->primeiro = novo_no;
    if(l->tamanho == 0)l->ultimo = novo_no;
    l->tamanho++;
    l_ok(l);
}

static bool l_vazia(Lista l)
{
  l_ok(l);
  return l->primeiro == NULL; // poderia ser l->tamanho == 0
}


Lista l_insere_fim(Lista l, t_dado d){
    No *nov_no = l_aloca_no(l);

    if(l_vazia){
        l_insere_inicio(l,d);
    }
    No *novo_no = l_aloca_no(l);

    novo_no->dado = d;
    nov_no->prox = NULL;
    l->ultimo->prox = novo_no;
    l->ultimo = novo_no;
    l->tamanho++;
    l_ok(l);

}

static No *l_no_na_posicao(Lista l, int pos){
    assert(pos > 0 && pos <= l->tamanho);
    if(pos == l->tamanho - 1) return l->ultimo;
    No *no = l->primeiro;
    int p = 0;
    while(p < pos && no != NULL){
        no = no->prox;
        p++;
    }
    return no;
}

void l_insere_pos(Lista l, t_dado d, int pos){
    l_ok(l);
    if(pos == 0){
    l_insere_inicio(l, d);
    return;
    }
    if(pos == l->tamanho){
        l_insere_fim(l, d);
        return;
    }
    
    No *anterior = l_no_na_posicao(l,pos - 1);
    No *proximo = l_no_na_posicao(l, pos);

    No *novo_no = l_aloca_no(l);
    novo_no->dado = d;
    novo_no->prox = proximo;
    anterior->prox = novo_no;
    l->tamanho++;
}



void l_imprime(Lista l){
    l_ok(l);
    for(No *no = l->primeiro; no != NULL; no = no->prox){
        printf(" %d ", no->dado);
    }
}


void l_libera_listas(Lista l){
    l_ok(l);
    No *no = l->primeiro;
    while(no != NULL){
        No *prox = no->prox;
        l_libera_no(l, no);
        no = prox;
    }
    free(l);
}

int l_tam(Lista l)
{
  l_ok(l);
  return l->tamanho;
}

bool l_cheia(Lista l)
{
  // não tem lista cheia! se encher a memória, mata o programa...
  l_ok(l);
  return false;
}





