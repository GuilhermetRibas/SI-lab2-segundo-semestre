#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "lista.h"
#include "str.h"
#include "calc.h"

int main()
{
    Lista entradas;
    Lista resultados;
    char *nome = "entrada.txt";

    entradas = c_le_linha_arquivo_entrada(nome);

    resultados = c_calcula_expressoes_do_aquivo(entradas);

    c_escreve_resultado_arquivo(resultados);
  
    c_escreve_resultado_arquivo(resultados);
    printf("escreveu\n");

    
    l_destroi(entradas);
    l_destroi(resultados);

}