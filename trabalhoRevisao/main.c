// programa para testar o terminal em modo "cru"
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

typedef struct timespec crono;

typedef struct
{
    int pontos;
    int municao;
    char armas[11];
    int arma_indice;
    char escudo;
    int cont_escudos;
    char inimigos;
    int inimigos_inativos;
    char posicoes[14];
    crono cronometro;
    double tempo;
    bool movimento_intervalo;
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

void crono_inicia(crono *c)
{
    clock_gettime(CLOCK_MONOTONIC, c);
}

double crono_parcial(crono *c)
{
    crono agora;
    clock_gettime(CLOCK_MONOTONIC, &agora);

    double segundos = agora.tv_sec - c->tv_sec;
    double nanosegundos = agora.tv_nsec - c->tv_nsec;
    return segundos + 1e-9 * nanosegundos;
}

char lechar()
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

void inicializa_posicoes(estado_t *est)
{
    est->posicoes[0] = est->escudo; // dps printo como )
    est->posicoes[1] = est->escudo;
    est->posicoes[2] = est->escudo;
    for (int i = 3; i < 13; i++)
    {
        est->posicoes[i] = ' ';
    }
    // est->posicoes[13] = est->inimigos;
}

void inicializa_estado(estado_t *est)
{

    est->pontos = 0;
    preenche_armas(est);
    est->arma_indice = 0;
    est->arma_indice = 0;
    est->municao = 30;
    est->escudo = ')';
    est->cont_escudos = 3;
    est->inimigos_inativos = 20;
    est->status = true;
    crono_inicia(&est->cronometro);
    inicializa_posicoes(est);
    est->movimento_intervalo = true;
    est->tempo = 1.00;
}

void intervalo_movimento(estado_t *est)
{
    double intervaloTempo;
    intervaloTempo = crono_parcial(&est->cronometro);
    if (intervaloTempo >= est->tempo)
    {
        crono_inicia(&est->cronometro);
        est->movimento_intervalo = true;
    }
    else
    {
        est->movimento_intervalo = false;
    }
}

void gera_inimigo(estado_t *est)
{
    int inimigoInt;
    char inimigoChar;
    if (est->movimento_intervalo)
    {
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
        est->posicoes[13] = inimigoChar;
        // return inimigoChar;
        (est->inimigos_inativos)--;
    }
}

void printa_tela(estado_t *est)
{
    printf("\r %d  ", est->pontos);
    printf("%d ", est->municao);
    printf("%c ", est->armas[est->arma_indice]);
    gera_inimigo(est);
    for (int i = 0; i < 14; i++)
    {
        printf("%c ", est->posicoes[i]);
    }
    printf("\r");
}

void contabiliza_pontos_por_inimigo(estado_t *est, int indice)
{
    // um ponto a cada posição qieo  inimigo se deslocou
    //  13 = 1
    //  12 == 2 11 = 3 10
    //  14 - a posição que esta no vetor que da os pontos
    int pontosPorInimigo = 14 - indice;

    if (est->posicoes[indice] != 'n')
    {
        est->pontos += pontosPorInimigo;
    }
    else
    {
        est->pontos += pontosPorInimigo * 2;
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

                //(est->inimigos_inativos)--;
                est->posicoes[i] = ' ';
                contabiliza_pontos_por_inimigo(est, i);
                break;
            }

            if (est->armas[est->arma_indice] == 'n' && est->posicoes[i] == 'N')
            {
                est->posicoes[i] = 'n';
                break;
            }
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

void movimenta_inimigo(estado_t *est)
{

    for (int i = 0; i < 14; i++)
    {
        if ((est->posicoes[i] >= '0' && est->posicoes[i] <= '9') || est->posicoes[i] == 'n' || est->posicoes[i] == 'N')
        {
            if (est->movimento_intervalo)
            {

                if (est->posicoes[i - 1] == ')')
                {
                    est->posicoes[i - 1] = ' ';
                    est->posicoes[i] = ' ';
                    est->pontos += 10;
                    est->cont_escudos--;
                    // contador que dps que chega em 3 e um inimigo chegar n
                    // posição 0 acaba o jogo
                }
                if (est->cont_escudos == 0 && i == 0)
                { // ver isso aqui
                    est->status = false;
                }
                else
                {
                    est->posicoes[i - 1] = est->posicoes[i];
                    est->posicoes[i] = ' ';
                }
            }
        }
    }
}
// inimigo_destroi_escudo(est);

/*No final de uma onda (quando não houver mais ataques inativos e os ativos tiverem sido destruídos), os pontos são atualizados,
e é apresentado um resumo do estado do jogo. Aguarda-se que o usuário digite r, e o número de tiros,
ataques inimigos são reinicializados, o intervalo dos eventos temporais é atualizado, sorteia-se o tipo da nova onda (se diurna ou noturna) e a nova onda é iniciada.

*/
void atualiza_estado(estado_t *est)
{   
    est->municao = 30;
    est->inimigos_inativos = 20;
    double aumenta_tempo = (est->tempo * 10) / 100;
    est->tempo -= aumenta_tempo;
    est->status = true;
    // 2.00 - 100
    // x  - 10%
    // sortear o tipo da onda
    inicializa_posicoes(est);
}

void resumo_onda(estado_t *est)
{
    // pontos munição e ultima arma utilizada
    printf("Total de pontos: %d ", est->pontos); // ainda tenho que ver com os escudos
    printf("Total de munição restante: %d \n", est->municao);
    printf("Ultima arma utilizada: %d ", est->arma_indice);
}

void joga_onda(estado_t *est)
{
    while (est->inimigos_inativos != 0 && est->status)
    {
        intervalo_movimento(est);
        movimenta_inimigo(est);
        printa_tela(est);

        char c = lechar();
        controles(est, c);
    }

    if (est->inimigos_inativos == 0)
    {
        resumo_onda(est);

        printf("\nDigite r se quiser ir para a próxima onda:\n ");
        char q = '0';
        while(q != 'r' && q!= 27){
            q = lechar();
        }
        if (q == 'r')
        {
            atualiza_estado(est);
            system("clear");
        }
        if(q == 27){
            est->status = false;
        }
    }
}

void joga_partida(estado_t *est)
{
    // laço 2
    // laço se desiste ou a partida terminou

    while (est->status)
    {
        joga_onda(est);
    }
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