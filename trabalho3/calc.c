
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include "lista.h"
#include "str.h"
#include "calc.h"

struct tabela_precedencia
{
    Lista *vet_listas_linhas;
    int quant_linhas;
    int quant_colunas;
};

typedef struct tabela_precedencia *Tabela_pre;

struct calc
{
    Lista operadores;
    Lista operandos;
};

typedef enum token Token;

enum token
{
    ignora,
    numero,
    variavel,
    operador,
};

static Token c_tipo_entrada(unichar c)
{
    if (c == ' ' || c == "\n" || c == '\t')
        return ignora;
    if ((c >= '0' && c <= '9') || c == '.')
        return numero;
    if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '$')
        return variavel;

    return operador;
}

// converter operador para codigo
// pegaar do arquivo e colocar no vetor de potteiros
// pesquisa no vetor de pilhas
// operar
// emplilhar

static void c_ok(Calc c)
{
}

/////////////////// Funções tabela de precedencia ///////////

// libera o vetor de listas do struct tabela de precedencia
static void c_libera_vet_listas_linhas(Tabela_pre t)
{
    for (int i = 0; i < t->quant_linhas; i++)
    {
        l_destroi(t->vet_listas_linhas[i]);
    }
    free(t->vet_listas_linhas);
}

// libera a memória alocada pra o struc tabela de preccedencia
static void c_libera_tabela(Tabela_pre t)
{
    c_libera_vet_listas_linhas(t);
    free(t);
}

// inicializa o vetor de ponteiros e inicializa as listas para cada posição
static Lista *c_aloca_vet_listas_linhas(Tabela_pre t)
{
    t->vet_listas_linhas = malloc(t->quant_linhas * sizeof(*t->vet_listas_linhas));
    assert(t->vet_listas_linhas != NULL);

    for (int i = 0; i < t->quant_linhas; i++)
    {
        t->vet_listas_linhas[i] = l_cria();
    }
    return t->vet_listas_linhas;
}
// aloca e inicializa os dados da struct tabela de precedencia
static Tabela_pre c_cria_tabela(int quant_linhas, int quant_colunas)
{
    Tabela_pre tabela = malloc(sizeof(struct tabela_precedencia));
    assert(tabela != NULL);
    tabela->quant_linhas = quant_linhas;
    tabela->quant_colunas = quant_colunas;
    c_aloca_vet_listas_linhas(tabela);
    return tabela;
}

///////////////// Funções arquivo tabela de precedencia /////////

// retorna a quantia de colunas de texto do arquivo
static int c_conta_colunas_arq(FILE *arq)
{
    char linha[256];
    int cont = 0;
    if (fgets(linha, sizeof(linha), arq) == NULL)
        return 0;

    char *p = linha;

    while (*p != '\0' && *p != '\n')
    {

        if (*p != ' ')
        {
            cont++;

            while (*p != ' ' && *p != '\n' && *p != '\0')
            {
                p++;
            }
        }
        else
        {
            p++;
        }
    }
    rewind(arq);
    return cont;
}

// retorna a quantia de linhas de texto no arquivo
static int c_conta_linhas_arq(FILE *arq)
{
    char linha[256];
    int cont = 0;
    while (fgets(linha, sizeof(linha), arq) != NULL)
    {
        cont++;
    }
    assert(cont > 0);
    rewind(arq);
    return cont;
}

// le as linhas do arquivo e armazena cada linha em uma lista
static Tabela_pre c_le_arquivo(char *nome)
{
    FILE *arq = fopen(nome, "r");
    assert(arq != NULL);

    int num_linhas = c_conta_linhas_arq(arq);
    int num_colunas = c_conta_colunas_arq(arq);
    Tabela_pre tabela = c_cria_tabela(num_linhas, num_colunas);

    char operacao[10];
    for (int i = 0; i < tabela->quant_linhas; i++)
    {
        for (int j = 0; j < tabela->quant_colunas; j++)
        {
            {
                fscanf(arq, "%s", operacao);
                Str s = s_strc(operacao);
                l_insere(tabela->vet_listas_linhas[i], s);
            }
        }
    }

    fclose(arq);
    return tabela;
}

//////////Funções na tabela de precedendcia

// retona em número o tipo do operador
static int c_converte_operador_para_num(unichar c)
{
    if (c == '+')
        return 0;

    if (c == '-')
        return 1;

    if (c == '*')
        return 2;

    if (c == '/')
        return 3;

    if (c == '^')
        return 4;

    if (c == '(')
        return 5;

    if (c == ')')
        return 6;

    return -1;
}

// retorna qual operação deve ser execura pela calculadora de acordo com a entrada e topo da pilha
static unichar c_confere_operacao_pilha(Lista l, Str entrada, Tabela_pre t)
{
    Str topo_pilha = l_topo(l);
    unichar operador_pilha = s_ch(topo_pilha, 0);
    int linha = c_converte_operador_para_num(operador_pilha);
    unichar entrada_uni = s_ch(entrada, 0);
    int col = c_coluna_tabela_operador(entrada_uni);
    assert(linha != -1 && col != -1);

    dado_t dado_operador = l_dado_pos(t->vet_listas_linhas[linha], col);
    unichar operacao = s_ch(dado_operador, 0);

    return operacao;
}

static void c_libera_calc(Calc c)
{
    l_destroi(c->operadores);
    l_destroi(c->operandos);
    free(c);
}

static Calc c_cria_calc()
{
    Calc c = malloc(sizeof(struct calc));
    assert(c != NULL);

    c->operadores = l_cria();
    c->operandos = l_cria();

    return (c);
}

static double c_opera(double op1, double op2, unichar operador)
{
    if (operador == '+')
        return op1 + op2;

    if (operador == '-')
        return op1 - op2;

    if (operador == '*')
        return op1 * op2;

    if (operador == '/')
        return op1 / op2;

    if (operador == '^')
        return pow(op1, op2);

    assert(false);
    return 0;
}

Str calculadora(Str expressão)
{
    char *arquivo = "tabela-precedencia.txt";
    Tabela_pre t = c_le_arquivo(arquivo);
    Lista l = tokeniza(expressão);

    Calc c = c_cria_calc();

    for (int i = 0; i < l_tam(l); i++)
    {
        dado_t dado_str = l_dado_pos(l, i);
        unichar dado_uni = s_ch(dado_str, 0);

        Token tipo = c_tipo_entrada(dado_uni);

        if (dado_uni == numero)
        {
            l_insere(c->operandos, dado_str);
        }
        if (dado_uni == operador)
        {
            unichar operacao = c_confere_operacao_pilha(l, dado_str, t);
            if (operacao == 'E')
            {
                l_insere(c->operadores, dado_str);
            }
            else if(operacao == 'O')
            {
                Str op2_str = l_desempilha(c->operadores);
                Str op1_str = l_desempilha(c->operadores);
                
                double op2 = s_número(op2_str);
                double op1 = s_número(op1_str);

                double resultado = c_opera(op1,op2,dado_uni);

                Str resultado_str = s_cria_número(resultado);
                l_empilha(c->operandos,resultado_str);

            }
        }
    }

    //caso das outras letras, ver dps

    c_libera_calc(c);
    c_libera_tabela(t);
    l_destroi(l);
}

// Retorna uma nova Lista contendo substrings de txt.
// Uma substring inicia em um caractere diferente de espaço, tabulação,
//   fim de linha.
// Se a substring inicia por um dígito ou um ponto, contém os demais dígitos
//   ou pontos que seguem.
// Se a substring inicia por uma letra ou sublinhado ou `$`, contém os
//   demais letras, sublinhados, `$` ou dígitos que seguem.
// Se a substring inicia por outro caractere, contém somente esse caractere.
// Exemplos:
// " 9. 5" -> ["9." "5"]
// "92+a ba 3b3 ** *  " -> ["92" "+" "a" "ba" "3" "b3" "*" "*" "*"]
Lista tokeniza(Str txt)
{
    Lista l = l_cria;
    int tam = s_tam(txt);
    int cont = 0;

    while (cont < tam)
    {
        unichar c = s_ch(txt, cont);

        Token tipo = c_tipo_entrada(c);

        if (tipo == ignora)
        {
            cont++;
            continue;
        }
        int inicio = cont;

        if (tipo == numero)
        {
            cont++;
            while (cont < tam)
            {
                c = s_ch(txt, cont);
                if (!((c >= '0' && c <= '9') || c == '.'))
                {
                    break;
                }
                cont++;
            }
        }
        else if (tipo == variavel)
        {
            c = s_ch(txt, cont);

            if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_' || c == '$'))
                break;

            cont++;
        }
        else
        {
            cont++;
        }

        Str token = s_cria_substring(txt, inicio, cont);
        l_insere_fim(l, token);
    }
    s_destroi(txt);
    return l;
}