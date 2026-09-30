#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<assert.h>
#include"lista.h"
#include "str.h"

struct no
{
    No *ant;
    No *prox;
    dado_t string;
};


struct lista
{
    int tamanho;
    No *sentinela;
};


static void l_ok(Lista l){
    assert(l != NULL);
    assert(l->sentinela != NULL);
    int tam=0;
    for(No *no = l->sentinela->prox; no != l->sentinela; no = no->prox){
        tam++;
    }
    
    assert(tam == l->tamanho);
}

static No *l_aloca_no(){
    No *no = malloc(sizeof(No));
    assert(no != NULL);
    return no;
}

static void l_libera_no(No *no){
    s_destroi(no->string);
    free(no);
}

static void l_init_lista(Lista l){
    l->tamanho = 0;
    l->sentinela->ant = l->sentinela;
    l->sentinela->prox = l->sentinela; 
}

Lista l_cria(){
    Lista l= malloc(sizeof(struct lista));
    assert(l != NULL);
    l->sentinela = l_aloca_no();
    l_init_lista(l);
    l_ok(l);
    return l;

}

// cria uma lista contendo substrings de s
// as substrings são separadas por quaisquer caractere de sep
// os caracteres de sep não aparecem nas substrings
// exemplos:
//   "a,ba,ca, te", ", " -> ["a" "ba" "ca" "te"]
//   "aba \ncate\n", "\n" -> ["aba " "cate"]
Lista l_cria_separando(Str s, Str sep){       
    Lista l = l_cria();
    int tam = s_tam(s);
    int inicio = 0;

    //procurar pelos de ceparação
    // pegar onde começa a sub até antes do sep

    for(int i =0 ; i < tam; i++){
        unichar c = s_ch(s,i);
        bool separa = false; 
        for(int j = 0; j < s_tam(sep); j++) {
            if(c == s_ch(sep,j)){
                separa = true;
                break;
            }
        }

        if(separa){
            
           if(i > inicio){
             Str substring = s_cria_substring(s, inicio, i - inicio);
             l_insere_fim(l, substring);
            }
            inicio = i + 1; 
        }    
    }
    if(inicio < tam){//para o final
        Str substring = s_cria_substring(s, inicio, tam - inicio);
        l_insere_fim(l, substring);
    }
    return l;
}


// libera a memória ocupada por uma lista
void l_destroi(Lista l){
    No *no = l->sentinela->prox;
    while (no != l->sentinela)
    {
        No *priximo = no->prox;
        l_libera_no(no);
        no = priximo;
    }
    free(l->sentinela);
    free(l);
}

// retorna o número de elementos na lista
int l_tam(Lista l){
    l_ok(l);
    return l->tamanho;
}

// retorna true se a lista tiver cheia
bool l_cheia(Lista l){
    l_ok(l);
    return false;

}
// retorna true se a lista tiver vazia
bool l_vazia(Lista l){
    return l_tam(l) == 0;
}


// imprime os dados que estão na lista
void l_imprime(Lista l){
   l_ok(l);
   if(l_vazia(l))return;

   for(No *no = l->sentinela->prox; no != l->sentinela; no = no->prox){
        s_imprime(no->string);
   }

}

// insere o dado d no início da lista l
void l_insere_inicio(Lista l, dado_t d)
{
    l_ok(l);

    bool vazia = l_vazia(l);

    No *no = l_aloca_no();

    dado_t copia = s_cria_cópia(d);
    no->string = copia;
    s_destroi(d);

    no->ant = l->sentinela;
    no->prox = l->sentinela->prox;

    no->prox->ant = no;
    l->sentinela->prox = no;

    if (vazia)
        l->sentinela->ant = no;

    l->tamanho++;

    l_ok(l);
}

// insere o dado d no final da lista l
void l_insere_fim(Lista l, dado_t d)
{
    l_ok(l);

    if (l_vazia(l))
        return l_insere_inicio(l, d);

    No *no = l_aloca_no();

    dado_t copia = s_cria_cópia(d);
    no->string = copia;
    s_destroi(d);

    no->prox = l->sentinela;
    no->ant = l->sentinela->ant;

    no->ant->prox = no;
    l->sentinela->ant = no;

    l->tamanho++;

    l_ok(l);
}

static int l_decide_percurso(Lista l, int pos){
    int div = l_tam(l)/2;
    if(pos > div)return 1;
    return 0;
}

static No *l_no_na_pos(Lista l, int pos){
    assert(pos >= 0 && pos < l->tamanho);
    No *no ;
    int p;

    if(l_decide_percurso(l,pos) == 0){
    no = l->sentinela->prox;
        p=0;
    while(p < pos && no != l->sentinela){
        no = no->prox;
        p++;
    }   
    }else{
    no = l->sentinela->ant;
        p = l->tamanho-1;
        while(p > pos && no != l->sentinela){
        no = no->ant;
        p--;
    }   
    }
    return no;
}


// insere o dado d na lista l, de forma que ele fique na posição p
// a primeira posição é 0
void l_insere_pos(Lista l, dado_t d, int p){
    assert(p >= 0 && p <= l->tamanho);
    if(p == 0)return l_insere_inicio(l,d);
    if(p == l->tamanho)return l_insere_fim(l,d);

    No *no = l_aloca_no();
     dado_t copia = s_cria_cópia(d);
    no->string = copia;
    s_destroi(d);

    No *anterior = l_no_na_pos(l, p - 1);
    No *proximo = l_no_na_pos(l,p);

    no->prox = proximo;
    proximo->ant = no;
    no->ant = anterior;
    anterior->prox = no;
    l->tamanho++;
    l_ok(l);
}

// retorna o dado no início da lista
dado_t l_dado_inicio(Lista l){
    l_ok(l);
    if(l_vazia(l))return NULL;
    return l->sentinela->prox->string;

}

// retorna o dado no final da lista
dado_t l_dado_fim(Lista l){
     l_ok(l);
    if(l_vazia(l))return NULL;
    return l->sentinela->ant->string;
}

// retorna o dado na posição pos da lista
dado_t l_dado_pos(Lista l, int pos){
    No *no = l_no_na_pos(l,pos);
    return no->string;
}

// remove e retorna o dado no início da lista
dado_t l_remove_inicio(Lista l){
    l_ok(l);
    if(l_vazia(l))return NULL;

    No *no = l->sentinela->prox;
    dado_t s = s_cria_cópia(no->string);

    l->sentinela->prox = no->prox;
    no->prox->ant = l->sentinela;
    l->tamanho--;
    l_libera_no(no);
    return s;
    
}

// remove e retorna o dado no final da lista
dado_t l_remove_fim(Lista l){
    l_ok(l);
    if(l_vazia(l))return NULL;

    No *no = l->sentinela->ant;
    dado_t s = s_cria_cópia(no->string);

    no->ant->prox = l->sentinela;
    l->sentinela->ant = no->ant;
    l->tamanho--;
    l_libera_no(no);
    return s;
}

// remove e retorna o dado na posição pos da lista
dado_t l_remove_pos(Lista l, int pos){
    assert(pos >= 0 && pos < l->tamanho);
    if(pos == 0)return l_remove_inicio(l);
    if(pos == l->tamanho - 1)return l_remove_fim(l);

    No *no = l_no_na_pos(l, pos);
    dado_t s = s_cria_cópia(no->string);
    No *proximo = no->prox;
    No *anterior = no->ant;

    proximo->ant = anterior;
    anterior->prox = proximo;
    l->tamanho--;
    l_libera_no(no);
    return s;
}



// funções para usar a lista como uma fila

// l_cria, l_destroi, l_vazia

// retorna o dado que está no início da fila
dado_t l_primeiro(Lista l){
    l_ok(l);
    if(l->tamanho == 0)return NULL;
    return l->sentinela->prox->string;

}

// insere um dado no fim da fila
void l_insere(Lista l, dado_t d){
    l_insere_fim(l,d);
}

// remove e retorna o dado que está no início da fila
dado_t l_remove(Lista l){
   return l_remove_inicio(l);
}


// funções para usar a lista como uma pilha

// l_cria, l_destroi, l_vazia

// retorna o dado que está no topo da pilha
dado_t l_topo(Lista l){
   l_ok(l);
   return l_primeiro(l);
}

// empilha um dado no topo da pilha
void l_empilha(Lista l, dado_t d){
    l_insere_inicio(l,d);
}

// remove e retorna o dado que está no topo da pilha
dado_t l_desempilha(Lista l){
    return l_remove_inicio(l);
}

