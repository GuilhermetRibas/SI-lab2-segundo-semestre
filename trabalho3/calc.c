#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <math.h>
#include "lista.h"
#include "str.h"
#include "calc.h"
#include "dicionario.h"

struct tabela_precedencia
{
    Lista *vet_listas_linhas;
    int quant_linhas;
    int quant_colunas;
};

typedef struct tabela_precedencia *Tabela_pre;

struct calc
{
    Lista expressao;
    Lista operadores;
    Lista operandos;
    Tabela_pre tabela;
    Dicionário dicionario;
};

static Calc c = NULL;

typedef enum token Token;

enum token
{
    ignora,
    numero,
    variavel,
    operador,
    erro,
};

static Token c_tipo_entrada(unichar c)
{
    if (c == ' ' || c == '\t' || c == '\n')
        return ignora;

    if ((c >= '0' && c <= '9') || c == '.')
        return numero;

    if ((c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        c == '$')
        return variavel;

    if (c == '+' || c == '-' ||
        c == '*' || c == '/' ||
        c == '^' || c == '(' ||
        c == ')' || c == '=')
        return operador;

    return erro;
}

static bool c_caractere_variavel(unichar c)
{
    return ((c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            c == '_');
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
                Str s = s_cria(operacao);
                l_insere(tabela->vet_listas_linhas[i], s);
            }
        }
    }

    fclose(arq);
    return tabela;
}

///////// Função do Dicionário de Variáveis ///////

static bool c_menor_chave(chave_t a, chave_t b)
{
    Str chave_a = (Str)a;
    Str chave_b = (Str)b;

    char *str_a = s_strc(chave_a);
    char *str_b = s_strc(chave_b);

    int resultado = strcmp(str_a, str_b);

    free(str_a);
    free(str_b);

    return resultado < 0;
}

static bool c_igual_chave(chave_t a, chave_t b)
{
    Str chave_a = (Str)a;
    Str chave_b = (Str)b;

    return s_igual(chave_a, chave_b);
}

static Dicionário c_init_dicionario()
{
    return dic_cria(c_menor_chave, c_igual_chave);
}

//////////Funções para Calcular ///////////////////

static void c_destroi_variavel(chave_t chave, valor_t valor)
{
    s_destroi((Str)chave);
    s_destroi((Str)valor);
}

static void c_libera_calc(Calc c)
{
    c_libera_tabela(c->tabela);
    dic_para_todos(c->dicionario, c_destroi_variavel);
    dic_destrói(c->dicionario);
    free(c);
}

static void c_libera_listas_calc(Calc c)
{
    l_destroi(c->operadores);
    l_destroi(c->operandos);
    l_destroi(c->expressao);
}

static void c_init_listas_calc(Calc c, Str expressao)
{
    c->expressao = tokeniza(expressao);
    c->operadores = l_cria();
    c->operandos = l_cria();
}

static Calc c_cria_calc()
{
    Calc novo = malloc(sizeof(struct calc));
    assert(novo != NULL);

    char *arquivo = "tabela-precedencia.txt";
    novo->tabela = c_le_arquivo(arquivo);
    novo->dicionario = c_init_dicionario();

    return (novo);
}

/*
V  → Vazio
F  → Fator
E  → Empilhar
O  → Operar
D  → Descartar
T  → Terminar
Er → Erro
*/

// retona em número o tipo do operador do topo da pilha
static int c_converte_operador_para_num_topo_pilha(unichar c)
{

    if (c == '+' || c == '-')
        return 1;

    if (c == '*' || c == '/')
        return 2;

    if (c == '^')
        return 3;

    if (c == '(')
        return 4;

    if (c == '=')
        return 5;

    return -1;
}
// retona em número o tipo do operador da entrada
static int c_converte_operador_para_num_entrada(unichar c)
{
    if (c == 'F')
        return 0;

    if (c == '+' || c == '-')
        return 1;

    if (c == '*' || c == '/')
        return 2;

    if (c == '^')
        return 3;

    if (c == '(')
        return 4;

    if (c == ')')
        return 5;

    if (c == '=')
        return 6;

    return -1;
}

// retorna qual operação deve ser execura pela calculadora de acordo com a entrada e topo da pilha
static Str c_confere_operacao_pilha(Calc c, Str entrada)
{
    int linha;
    if (!(l_vazia(c->operadores)))
    {
        Str topo_pilha = l_topo(c->operadores);
        unichar operador_pilha = s_ch(topo_pilha, 0);
        linha = c_converte_operador_para_num_topo_pilha(operador_pilha);
    }
    else
    {
        linha = 0;
    }

    unichar entrada_uni = s_ch(entrada, 0);
    int col = c_converte_operador_para_num_entrada(entrada_uni);

    assert(linha != -1 && col != -1);

    Str operacao = l_dado_pos(c->tabela->vet_listas_linhas[linha], col);

    return operacao;
}

static void c_atribui_variavel(Calc c)
{
    Str valor_str = l_desempilha(c->operandos);
    Str variavel_str = l_desempilha(c->operandos);

    double valor = s_número(valor_str);
    Str valor_novo = s_cria_número(valor);

    valor_t antigo = dic_insere(c->dicionario, (chave_t)variavel_str, (valor_t)valor_novo);

    if (antigo != VALOR_NÃO_EXISTE)
        s_destroi((Str)antigo);

    s_destroi(valor_str);

    l_empilha(c->operandos, valor_novo);
}

static double c_calcula_operacao(unichar operador, double op1, double op2)
{
    if (operador == '+')
    {
        return op1 + op2;
    }
    else if (operador == '-')
    {
        return op1 - op2;
    }
    else if (operador == '*')
    {
        return op1 * op2;
    }
    else if (operador == '/')
    {
        return op1 / op2;
    }
    else if (operador == '^')
    {
        return pow(op1, op2);
    }
    else
    {
        assert(false);
    }

    return -1;
}

static bool c_numero_valido(Str operando)
{
    char *str = s_strc(operando);
    double valor;
    char resto;

    bool valido = sscanf(str, "%lf %c", &valor, &resto) == 1;

    free(str);

    return valido;
}

static bool c_valor_operando(Calc c, Str operando, double *valor)
{
    unichar primeiro = s_ch(operando, 0);

    if ((primeiro >= '0' && primeiro <= '9') || primeiro == '.')
    {
        if (!c_numero_valido(operando))
            return false;

        *valor = s_número(operando);
        return true;
    }

    if ((primeiro >= 'a' && primeiro <= 'z') || (primeiro >= 'A' && primeiro <= 'Z') || primeiro == '$')
    {
        Str valor_str = dic_busca(c->dicionario, operando);

        if (valor_str == VALOR_NÃO_EXISTE)
        {
            return false;
        }

        *valor = s_número(valor_str);
        return true;
    }

    return false;
}

// retona o resultado da operação
static bool c_opera(Calc c)
{
    assert(l_tam(c->operandos) >= 2);
    Str operador_str = l_topo(c->operadores);
    unichar operador = s_ch(operador_str, 0);
    if (operador == '=')
    {
        c_atribui_variavel(c);

        Str operador_descartado = l_desempilha(c->operadores);
        s_destroi(operador_descartado);

        return true;
    }
    else
    {
        Str op2_str = l_desempilha(c->operandos);
        Str op1_str = l_desempilha(c->operandos);
        double op2;
        double op1;

        if (!c_valor_operando(c, op1_str, &op1) ||
            !c_valor_operando(c, op2_str, &op2))
        {
            s_destroi(op1_str);
            s_destroi(op2_str);
            return false;
        }

        double resultado = c_calcula_operacao(operador, op1, op2);

        Str resultado_str = s_cria_número(resultado);
        l_empilha(c->operandos, resultado_str);
        s_destroi(op2_str);
        s_destroi(op1_str);
        l_desempilha(c->operadores);
        return true;
    }
}

static bool c_termina_execusao(Calc c)
{
    return (l_tam(c->operandos) == 1 && l_tam(c->operadores) == 0);
}

//////// Funções arquivo de entrada e saída ////////////

static Lista c_le_linha_arquivo_entrada(char *nome)
{
    FILE *arq = fopen(nome, "r");
    assert(arq != NULL);
    Lista l = l_cria();
    char linha[256];
    while (fgets(linha, sizeof(linha), arq) != NULL)
    {
        Str s = s_cria(linha);
        l_insere_fim(l, s);
    }
    fclose(arq);
    return l;
}

static void c_escreve_resultado_arquivo(Lista l)
{
    FILE *arq = fopen("Resultado", "w");
    assert(arq != NULL);
    for (int i = 0; i < l_tam(l); i++)
    {
        Str resultado = l_dado_pos(l, i);
        char *num_char = s_strc(resultado);

        fprintf(arq, "%s", num_char);
        fprintf(arq, "\n");
        free(num_char);
    }
    fclose(arq);
}

static Lista c_calcula_expressoes_do_aquivo(Lista l)
{
    Lista l_resultado = l_cria();
    for (int i = 0; i < l_tam(l); i++)
    {

        Str expressao = l_dado_pos(l, i);

        Str resultado = calculadora(expressao);
        l_insere_fim(l_resultado, resultado);

        s_destroi(resultado);
    }
    // c_libera_calc(c);
    c = NULL;
    return l_resultado;
}

Str calculadora(Str expressão)
{
    if (c == NULL)
    {
        c = c_cria_calc();
    }
    c_init_listas_calc(c, expressão);

    Str resultado = NULL;
    bool erro = true;
    bool terminou = false;

    for (int i = 0; i < l_tam(c->expressao); i++)
    {
        Str dado_str = s_cria_cópia(l_dado_pos(c->expressao, i));
        unichar dado_uni = s_ch(dado_str, 0);

        Token tipo = c_tipo_entrada(dado_uni);

        if (tipo == numero)
        {
            l_empilha(c->operandos, dado_str);
            continue;
        }

        if (tipo == operador)
        {
            bool analisar_token = true;

            while (analisar_token)
            {
                Str operacao = c_confere_operacao_pilha(c, dado_str);
                unichar operacao_uni = s_ch(operacao, 0);

                if (operacao_uni == 'E' && s_tam(operacao) == 2 &&
                    s_ch(operacao, 1) == 'r')
                {
                    erro = false;
                    analisar_token = false;
                    break;
                }
                else if (operacao_uni == 'E')
                {
                    l_empilha(c->operadores, dado_str);
                    analisar_token = false;
                }
                else if (operacao_uni == 'O')
                {
                      if (!c_opera(c))
                    {
                        erro = false;
                        analisar_token = false;
                    }
                }
                else if (operacao_uni == 'D')
                {
                    Str descartado = l_desempilha(c->operadores);
                    s_destroi(descartado);
                }
                else if (operacao_uni == 'T')
                {
                    if (c_termina_execusao(c))
                    {
                        resultado = l_desempilha(c->operandos);
                        terminou = true;
                        analisar_token = false;
                    }
                    else
                    {
                        erro = false;
                        analisar_token = false;
                    }
                }
            }

            s_destroi(dado_str);

            if (!erro || terminou)
                break;
        }
    }
    c_libera_listas_calc(c);

    if (!erro || !terminou)
        return s_cria("Erro");

    return resultado;
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
    Lista l = l_cria();
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
        if (tipo == erro)
        {
            s_destroi(txt);
            l_destroi(l);
            return NULL;
        }
        int inicio = cont;

        if (tipo == numero)
        {
            cont++;
            while (cont < tam)
            {
                c = s_ch(txt, cont);
                if (c_tipo_entrada(c) != numero)
                {
                    break;
                }
                cont++;
            }
        }
        else if (tipo == variavel)
        {
            cont++;
            while (cont < tam)
            {
                c = s_ch(txt, cont);
                if (!c_caractere_variavel(c))
                {
                    break;
                }
                cont++;
            }
        }
        else
        {
            cont++;
        }
        int tam_substring = cont - inicio;
        Str token = s_cria_substring(txt, inicio, tam_substring);
        l_insere_fim(l, token);
    }
    s_destroi(txt);
    return l;
}