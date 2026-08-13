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
    int quantia_armas;
    int arma_indice;
    char escudo;
    int cont_escudos;
    char inimigos;
    int quantia_inimigos_possiveis;
    int inimigos_inativos;
    char posicoes[14];
    crono cronometro;
    double tempo;
    bool movimento_intervalo;
    bool status;
    int ranking_atual[3];
    int tipo_onda;
    int porcentagem_onda_diurna;
    int cont_onda;

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
    if (est->tipo_onda == 1)
    {
        for (int i = 0; i < 10; i++)
        {
            est->armas[i] = '0' + i;
        }
        est->armas[10] = 'n';
    }
    else
    {
        for (int i = 0; i < 5; i++)
        {
            est->armas[i] = '0' + (i * 2);
        }
        est->armas[5] = 'n';
    }
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
    est->quantia_inimigos_possiveis = 12;
    est->pontos = 0;
    est->tipo_onda = 1;
    preenche_armas(est);
    est->quantia_armas = 11;
    est->arma_indice = 0;
    est->municao = 30;
    est->escudo = ')';
    est->cont_escudos = 3;
    est->inimigos_inativos = 20;
    est->status = true;
    crono_inicia(&est->cronometro);
    inicializa_posicoes(est);
    est->movimento_intervalo = true;
    est->tempo = 2.00;
    est->porcentagem_onda_diurna = 100;
    est->cont_onda = 0;
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
        inimigoInt = rand() % est->quantia_inimigos_possiveis;
        if (est->tipo_onda == 0)
        {
            inimigoInt = inimigoInt * 2;
        }

        if (inimigoInt < est->quantia_inimigos_possiveis - 2)
        {
            inimigoChar = '0' + inimigoInt;
        }
        else if (inimigoInt == est->quantia_inimigos_possiveis - 2)
        {
            inimigoChar = 'n';
        }
        else
        {
            inimigoChar = 'N';
        }

        est->posicoes[est->quantia_inimigos_possiveis + 1] = inimigoChar;
        // return inimigoChar;
        (est->inimigos_inativos)--;
    }
}

void printa_tela(estado_t *est)
{
    printf("\r %d  ", est->pontos);
    if (est->tipo_onda == 1)
    {
        printf("%d ", est->municao);
        printf("%c ", est->armas[est->arma_indice]);
        // gera_inimigo(est);
        for (int i = 0; i < 14; i++)
        {
            printf("%c ", est->posicoes[i]);
        }
    }
    printf("\r");
}

void contabiliza_pontos_por_inimigo(estado_t *est, int indice)
{
    // um ponto a cada posição qieo  inimigo se deslocou
    //  13 = 1
    //  12 == 2 11 = 3 10
    //  14 - a posição que esta no vetor que da os pontos
    int pontosPorInimigo = est->quantia_inimigos_possiveis + 1 - indice;

    if (est->posicoes[indice] != 'n')
    {
        if (est->tipo_onda == 1)
        {
            est->pontos += pontosPorInimigo;
        }
        else
        {
            est->pontos += pontosPorInimigo * 2;
        }
    }
    else
    {
        if (est->tipo_onda == 1)
        {
            est->pontos += pontosPorInimigo * 2;
        }
        else
        {
            est->pontos += pontosPorInimigo * 4;
        }
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
            else
            {
                system("aplay -q Sons/12.2.wav ");
                break;
            }
        }

        if (est->armas[est->arma_indice] == 'n' && est->posicoes[i] == 'N')
        {
            est->posicoes[i] = 'n';
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
    if (est->arma_indice == est->quantia_armas)
    {
        est->arma_indice = 0;
    }
}

void sonar(estado_t *est)
{

    char arquivo_som[10];
    char comando_som[20];

    for (int i = 0; i < est->quantia_inimigos_possiveis + 1; i++)
    {
        if (est->posicoes[i] == ')')
        {
            system("aplay -q Sons/12.3.wav ");
        }
        if (est->posicoes[i] == ' ')
        {
            system("aplay -q Sons/12.2.wav ");
        }

        if (est->posicoes[i] >= '0' && est->posicoes[i] <= '9')
        {
            sprintf(arquivo_som, "%c.2.wav", est->posicoes[i]);
            sprintf(comando_som, "aplay -q Sons/%s ", arquivo_som);

            system(comando_som);
        }
        if (est->posicoes[i] == 'n' || est->posicoes[i] == 'N')
        {
            system("aplay -q Sons/11.2.wav ");
        }
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
        sonar(est);
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

int sorteia_tipo_onda(estado_t *est)
{
    // O primeira onda tem 100% de chance de ser diurna. Essa chance diminui para 80%
    // na segunda onda, 60% na terceira, 40% na quarta e 20% nas demais.
    // posso sortee de 0 a 99 e pegar cada intervalo percentual de 100 para
    // representar a onda
    //  100% - 0% 1
    //  80% - 20% 2
    //  60% - 40% 3
    //  40% - 60% 4
    //  20% - 80% 5,6,6...
    est->cont_onda++;
    int num = rand() % 100;
    if (est->cont_onda < 6)
    {
        est->porcentagem_onda_diurna = est->porcentagem_onda_diurna - 20;
    }
    if (num < est->porcentagem_onda_diurna)
    {
        est->tipo_onda = 1; // diurna
        return 1;
    }
    else
    {
        est->tipo_onda = 0; // noturna
        return 0;
    }
}

void atualiza_onda(estado_t *est)
{
    if (sorteia_tipo_onda(est) == 1)
    {
        est->quantia_armas = 11;
        preenche_armas(est);
        est->inimigos_inativos = 20;
        double aumenta_tempo = (est->tempo * 10) / 100;
        est->tempo -= aumenta_tempo;
        est->quantia_inimigos_possiveis = 12;
    }
    else
    {
        est->quantia_inimigos_possiveis = 7;
        preenche_armas(est);
        est->inimigos_inativos = 15;
        est->tempo = est->tempo * 3;
        est->quantia_armas = 7;
    }

    est->municao = 30;
    est->arma_indice = 0;
    est->status = true;
    // 2.00 - 100
    // x  - 10%
    // sortear o tipo da onda
    inicializa_posicoes(est);
}

void resumo_onda(estado_t *est)
{
    // pontos munição e ultima arma utilizada
    printf("\nTotal de pontos: %d ", est->pontos);
    printf("\nTotal de munição restante: %d \n", est->municao);
    printf("Ultima arma utilizada: %d ", est->arma_indice);
}

void joga_onda(estado_t *est)
{
    while (est->inimigos_inativos != 0 && est->status)
    {
        intervalo_movimento(est);
        gera_inimigo(est);
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
        while (q != 'r' && q != 27)
        {
            q = lechar();
        }
        if (q == 'r')
        {
            atualiza_onda(est);
            system("clear");
        }
        if (q == 27)
        {
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

void le_ranking(estado_t *est)
{
    FILE *arq = fopen("ranking.txt", "r");

    if (arq == NULL)
    {
        return;
    }
    for (int i = 0; i < 3; i++)
    {
        fscanf(arq, "%d", &est->ranking_atual[i]);
    }
    fclose(arq);
}

bool verifica_atualizacao_ranking(estado_t *est)
{
    // so le de novo caso  o rankin tenha mudado - fazer dps
    le_ranking(est);
    int cont;
    for (int i = 0; i < 3; i++)
    {
        if (est->pontos > est->ranking_atual[i])
        {
            for (int j = 2; j > i; j--)
            {
                est->ranking_atual[j] = est->ranking_atual[j - 1];
            }

            est->ranking_atual[i] = est->pontos;
            return true;
        }
    }
    return false;
}

void adiciona_ranking(estado_t *est)
{
    if (verifica_atualizacao_ranking(est))
    {
        FILE *arq = fopen("ranking.txt", "w");
        if (arq == NULL)
        {
            return;
        }

        for (int i = 0; i < 3; i++)
        {
            fprintf(arq, "%d ", est->ranking_atual[i]);
        }

        fclose(arq);
    }
}

void resumo_partida(estado_t *est)
{
    printf("\nTotal de Pontos da partida: %d \n", est->pontos);
    adiciona_ranking(est);
    printf("Ranking Atual:\n");
    for (int i = 0; i < 3; i++)
    {
        printf("%d. %d \n", i + 1, est->ranking_atual[i]);
    }
}

int main()
{
    configura_terminal();
    srand(time(NULL));
    estado_t estado;
    char c;
    do
    {
        c = ' ';
        inicializa_estado(&estado);

        // primeiro laço
        while (estado.status)
        {
            joga_partida(&estado);
        }
        if (!estado.status)
        {
            resumo_partida(&estado);
            printf("Deseja jogar novamente [S/N]:");

            while (c != 'S' && c != 'N')
            {
                c = lechar();
            }
            if (c == 'S')
            {
                system("clear");
            }
        }

    } while (c != 'N');
    normaliza_terminal();
}