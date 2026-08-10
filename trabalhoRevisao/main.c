// programa para testar o terminal em modo "cru"
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef struct
{
    int pontos;
    int municao;
    char armas[11];
    int arma_indice;
    char escudo;
    char inimigos;
    int inimigos_inativos;
    char posicoes[14];
    bool status;
} estado_t;

void configura_terminal()
{
    if (system("stty raw opost -echo min 0 time 1") != 0)
    {
        perror("erro na execução de system(\"stty\")");
        fprintf(stderr, "você tem o programa stty instalado?\n");
        exit(1);
    };
    if (setvbuf(stdin, NULL, _IONBF, 0) != 0)
    {
        perror("erro na execução de setvbuf()");
        exit(1);
    }
}

void normaliza_terminal()
{
    system("stty sane");
}

// implementação de um cronômetro
typedef struct timespec crono;

// inicializa um cronômetro com a hora atual
void crono_inicia(crono *c)
{
    clock_gettime(CLOCK_MONOTONIC, c);
}

// retorna o tempo passado desde que o cronômetro *c foi iniciado, em segundos
double crono_parcial(crono *c)
{
    crono agora;
    clock_gettime(CLOCK_MONOTONIC, &agora);

    double segundos = agora.tv_sec - c->tv_sec;
    double nanosegundos = agora.tv_nsec - c->tv_nsec;
    return segundos + 1e-9 * nanosegundos;
}

int f(int x)
{
    if (x <= 0)
        return 0;
    return f(x - 1);
}

// lê um caractere do teclado.
// retorna o código do caractere lido ou 0 casa nada tenha sido digitado.
// só funciona corretamente se o terminal estiver em modo "cru".
char lechar() // com essa função posso pergar o que foi digitado e comparar com o código
{
    fflush(stdout);
    char c;
    if (fread(&c, 1, 1, stdin) == 1)
        return c;
    return 0;
}

void preenche_armas(estado_t *est)
{
    for (int i = 0; i < 10; i++)
    {
        est->armas[i] = '0' + i;
    }
    est->armas[10] = 'n';
}

char gera_inimigo()
{
    int inimigoInt;
    char inimigoChar;
    inimigoInt = rand() % 12;

    if (inimigoInt == 10)
    {
        inimigoChar = 'n';
    }
    else if (inimigoInt == 11)
    {
        inimigoChar = 'N';
    }
    else
    {
        inimigoChar = '0' + inimigoInt;
    }
    return inimigoChar;
}

void inicializa_posicoes(estado_t *est)
{
    est->posicoes[0] = est->escudo; // dps printo como )
    est->posicoes[1] = est->escudo;
    est->posicoes[2] = est->escudo;
    for (int i = 3; i < 13; i++)
    {
        est->posicoes[i] = ' ';
    }
    est->posicoes[13] = est->inimigos;
}

void inicializa_estado(estado_t *est)
{

    est->pontos = 0;
    preenche_armas(est);
    est->arma_indice = 0;
    est->arma_indice = 0;
    est->municao = 30;
    est->escudo = ')';
    est->inimigos = gera_inimigo();
    est->inimigos_inativos = 19;
    est->status = true;
    inicializa_posicoes(est);
}

void printa_tela(estado_t *est)
{
    printf("%d  ", est->pontos);
    printf("%d ", est->municao);
    printf("%c ", est->armas[est->arma_indice]);
    for (int i = 0; i < 14; i++)
    {
        printf("%c ", est->posicoes[i]);
    }
    printf("\r");
}

// no final mostra resultado e tem que digitra r para ir para próxima
//  se os inimigos ibativos e ativos tiverem acabados
// número de tiros, ataques inimigos são reinicializados,
// o intervalo dos eventos temporais é atualizado, sorteia-se o tipo da nova onda (se diurna ou noturna) e a nova onda é iniciads
void contabiliza_pontos_por_inimigo(estado_t *est, int indice)
{
    // um ponto a cada posição qieo  inimigo se deslocou
    //  13 = 1
    //  12 == 2 11 = 3 10
    //  14 - a posição que esta no vetor que da os pontos
    int pontosPorInimigo = 14 - indice;

    if (est->posicoes[indice] != 'n')
    {
        est->pontos = pontosPorInimigo;
    }
    else
    {
        est->pontos = pontosPorInimigo * 2;
    }
}

void destroi_inimigos(estado_t *est)
{

    // destrói o inimigo mais à esquerda
    // que seja igual à arma
    // se minha arma é igual ao inimgigo da primeira posição não vazia ent destroi ele
    //  se apo sição mais a esquerda dps do escudo não estiver vazia
    for (int i = 3; i < 14; i++)
    {
        if (est->posicoes[i] != ' ')
        {
            if (est->armas[est->arma_indice] == est->posicoes[i])
            {
                est->posicoes[i] = ' ';
                (est->inimigos_inativos)--;
                contabiliza_pontos_por_inimigo(est, i);
            }
            break;
        }
    }
}

void contabiliza_municao(estado_t *est)
{
    if (est->municao > 0)
    {
        (est->municao)--;
        destroi_inimigos(est);
    }
}

void troca_arma(estado_t *est)
{
    (est->arma_indice)++;
    est->armas[est->arma_indice + 1];
    if (est->arma_indice == 11)
    {
        est->arma_indice = 0;
    }
}

void controles(estado_t *est, char c)
{

    if (c == 27)
    {
        est->status = false;
    }

    if (c == 9)
    {
        troca_arma(est);
    }

    if (c == 13)
    {
        contabiliza_municao(est);
    }

    if (c == 32)
    {
        // ESPAÇO
    }
}

void movimenta_inimigoa_por_intervalo(estado_t *est){

    for(int i = 4; i < 14; i++){
        if(est->posicoes[i] != ' '){
            est->posicoes[i] = est->posicoes[ i- 1];
            est->posicoes[i] = ' ';
        }
    }

}

void joga_onda(estado_t *est)
{
    while (est->inimigos_inativos != 0)
    {
        printa_tela(est);
        char c = lechar();

        controles(est, c);
        // passagem_tempo();
    }

    // faz o que tem que fazer ao final
}

void joga_partida(estado_t *est)
{
    // laço 2
    // laço se desiste ou a partida terminou

    while (est->status)
        joga_onda(est);
}

int main()
{
    estado_t estado;
    configura_terminal();
    srand(time(NULL));

    inicializa_estado(&estado);

    // primeiro laço
    while (estado.status)
    {
        joga_partida(&estado);
    }

    normaliza_terminal();
}