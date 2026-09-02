// includes, constantes e declarações {{{1
#include "str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define MIN_ALLOC 8 // alocação mínima
struct str
{
  byte *vetor_codigos_utf8;
  int quantia_caracteries;
  int quantia_bytes;
  int capacidade;
};

// A memória para conter os bytes de uma string deve ser alocada e/ou
//   realocada conforme a necessidade, cuidando para que a quantidade
//   de memória alocada seja sempre:
//   - nula (não alocada) se a string for vazia, ou
//   - não inferior ao necessário para armazenar os bytes da codificação utf8;
//   - não inferior à alocação mínima;
//   - não superior ao triplo do número de bytes necessários
//     (exceto quando for o mínimo);
//   - uma potência de 2.

// funções auxiliares {{{1

// verifica se a string cad está de acordo com a especificação
// aborta o programa se não tiver
static void s_ok(Str_c s)
{
  if (s == NULL)
  {
    abort();
  }
  if (s->quantia_bytes > s->capacidade)
  {
    abort();
  }
  if (u8_conta_unichar_nos_bytes(s->quantia_bytes, s->vetor_codigos_utf8 == -1))
  {
    abort();
  }
}

// operações de criação e destruição {{{1

// Aloca, inicializa e retorna uma string que contém uma cópia da
//   string C que inicia em strC.
// A string C deve estar codificada em UTF8, e é delimitada pelo
//   caractere \0 (que não faz parte da string).
// Retorna uma string vazia se strC não contiver UTF8 válido ou se for NULL.

static byte *aloca_vetor(tamanho)
{
  byte *vet = malloc(tamanho);
  assert(vet != NULL);

  return vet;
}

static void preenche_struct_invalido(Str s, int tamanho, int nchar)
{
  s->vetor_codigos_utf8 = NULL;
  s->quantia_bytes = 0;
  s->quantia_caracteries = 0;
  s->capacidade = 0;
}

static void preenche_struct(Str s, int tamanho, int nchar)
{
  s->vetor_codigos_utf8 = aloca_vetor(tamanho);
  s->quantia_bytes = tamanho;
  s->quantia_caracteries = nchar;
  s->capacidade = tamanho;
}

// função de realocação
// fazer uma que ve se precisa de mais, se sim, quanto?
static void realoca_vetor(Str s, int novaCapacidade)
{
  byte *vetRealocado = realloc(s->vetor_codigos_utf8, novaCapacidade);
  assert(vetRealocado != NULL);
  s->vetor_codigos_utf8 = vetRealocado;
  s->capacidade = novaCapacidade;
}

Str s_cria(char const *strC)
{
  // comts strC já vem codificada em utf8
  Str s = malloc(sizeof(*s));
  assert(s != NULL);

  int tamanho = 0, nchar = 0;

  tamanho = strlen(strC);
  nchar = u8_conta_unichar_nos_bytes(tamanho, strC);

  if (strC == NULL || nchar == -1)
  {
    preenche_struct_invalido(s, tamanho, nchar);
    return s;
  }

  preenche_struct(s, tamanho, nchar);
  memcpy(s->vetor_codigos_utf8, strC, tamanho);
  return s;
}

void s_destroi(Str s)
{
  s_ok(s);
  free(s->vetor_codigos_utf8);
  free(s);
}
// Retorna uma nova string, que contém uma substring de s.
// A substring é como descrito em s_substring
// Pode ser implementada assim:
//   Str nova = s_cria("");
//   s_substring(nova, s, pos, tam);
//   return nova;
Str s_cria_substring(Str_c s, int pos, int tam)
{
  Str nova = s_cria("");
  s_substring(nova, s, pos, tam);
  return nova;
}

Str s_cria_cópia(Str_c s)
{
  return s_cria_substring(s, 0, -1);
}

// Retorna uma nova string com o conteúdo do arquivo chamado nome.
// Retorna uma string vazia em caso de erro.
Str s_cria_de_arquivo(char *nome)
{
  Str s;
  FILE *arq = fopen(nome, "r");
  if (arq == NULL)
  {
    return s = s_cria("");
  }
  int tamanho = 0, cont = 0;
  int c;
  while ((c = fgetc(arq)) != EOF)
  {
    tamanho++;
  }
  rewind(arq);

  char *reserva = malloc(tamanho + 1);
  assert(reserva != NULL);
  // so le até p esoaço
  while ((c = fgetc(arq)) != EOF)
  {
    reserva[cont] = c;
    cont++;
  }
  reserva[tamanho] = '\0';
  fclose(arq);
  s = s_cria(reserva);
  free(reserva);
  return s;
}

// operações de acesso {{{1
int s_tam(Str_c s)
{
  s_ok(s);

  return s->quantia_caracteries;
}

char *s_strc(Str_c s)
{
  s_ok(s);
  //...
  return NULL;
}

unichar s_ch(Str_c s, int pos)
{
  s_ok(s);
  //...
  return UNI_INV;
}

// operações de busca e comparação {{{1

bool s_igual(Str_c s, Str_c sb)
{
  s_ok(s);
  s_ok(sb);

  if (s->quantia_bytes != sb->quantia_bytes)
  {
    return false;
  }
  for (int i = 0; i < s->quantia_bytes; i++)
  {
    if (s->vetor_codigos_utf8[i] != sb->vetor_codigos_utf8[i])
    {
      return false;
    }
    return true;
  }
}

int s_busca_c(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);
  // posição não pode ser negativa
  int ncharS = s->quantia_caracteries;
  int ncharSb = sb->quantia_caracteries;
  int nbyteS, nbyteSb;

  if (pos < 0)
  {
    // n = 5 pos = -2  carro  5 + -2 +1 = 4
    pos = ncharS + pos + 1;
  }
  if (pos > ncharS)
  {
    pos = ncharS;
  }
  byte *bytePosicao = u8_avanca_unichar(s->vetor_codigos_utf8, pos); // primeiro ponteiro para ocmeçar a comparar com sb
  // pq a posção eesta ligada a um caracter e não a bits
  int indiceInicioS = bytePosicao - s->vetor_codigos_utf8;
  int indiceInicioSb;

  for (int i = pos; i < ncharS; i++)
  {
    nbyteS = u8_nbytes_no_unichar_que_comeca_com(s->vetor_codigos_utf8[indiceInicioS]);
    indiceInicioSb = 0;
    for (int j = 0; j < ncharSb; j++)
    {
      nbyteSb = u8_nbytes_no_unichar_que_comeca_com(sb->vetor_codigos_utf8[indiceInicioSb]);
      if (nbyteS == nbyteSb && memcmp(&s->vetor_codigos_utf8[indiceInicioS], &sb->vetor_codigos_utf8[indiceInicioSb], nbyteS) == 0)
      {
        return i;
      }
      indiceInicioSb += nbyteSb;
    }
    indiceInicioS += nbyteS;
  }
  return -1;
}

int s_busca_nc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);

  int ncharS = s->quantia_caracteries;
  int ncharSb = sb->quantia_caracteries;

  if (pos < 0)
  {
    pos = ncharS + pos + 1;
  }
  if (pos < 0)
  {
    pos = 0;
  }
  if (pos > ncharS)
  {
    pos = ncharS;
  }

  byte *bytePosicao = u8_avanca_unichar(s->vetor_codigos_utf8, pos);
  int indiceInicioS = bytePosicao - s->vetor_codigos_utf8;

  for (int i = pos; i < ncharS; i++)
  {
    int nbyteS = u8_nbytes_no_unichar_que_comeca_com(s->vetor_codigos_utf8[indiceInicioS]);

    bool pertence = false;
    int indiceInicioSb = 0;
    for (int j = 0; j < ncharSb; j++)
    {
      int nbyteSb = u8_nbytes_no_unichar_que_comeca_com(sb->vetor_codigos_utf8[indiceInicioSb]);

      if (nbyteS == nbyteSb && memcmp(&s->vetor_codigos_utf8[indiceInicioS], &sb->vetor_codigos_utf8[indiceInicioSb], nbyteS) == 0)
      {
        pertence = true;
        break;
      }
      indiceInicioSb += nbyteSb;
    }

    if (!pertence)
    {
      return i;
    }

    indiceInicioS += nbyteS;
  }

  return -1;
}

int s_busca_rc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);

  int ncharS = s->quantia_caracteries;
  int ncharSb = sb->quantia_caracteries;
  int nbyteS, nbyteSb;

  if (pos < 0)
  {
    // n = 5 pos = -2  carro  5 + -2 +1 = 4
    pos = ncharS + pos + 1;
  }
  if (pos > ncharS)
  {
    pos = ncharS;
  }
  byte *bytePosicao;
  int indiceInicioS;
  int indiceInicioSb;

  for (int i = pos - 1; i >= 0; i--)
  {
    bytePosicao = u8_avanca_unichar(s->vetor_codigos_utf8, i);
    indiceInicioS = bytePosicao - s->vetor_codigos_utf8;
    nbyteS = u8_nbytes_no_unichar_que_comeca_com(s->vetor_codigos_utf8[indiceInicioS]);
    indiceInicioSb = 0;
    for (int j = 0; j < ncharSb; j++)
    {
      nbyteSb = u8_nbytes_no_unichar_que_comeca_com(sb->vetor_codigos_utf8[indiceInicioSb]);
      if (nbyteS == nbyteSb && memcmp(&s->vetor_codigos_utf8[indiceInicioS], &sb->vetor_codigos_utf8[indiceInicioSb], nbyteS) == 0)
      {
        return i;
      }
      indiceInicioSb += nbyteSb;
    }
  }
  return -1;
}

int s_busca_rnc(Str_c s, int pos, Str_c sb)
{
  s_ok(s);
  s_ok(sb);

  int ncharS = s->quantia_caracteries;
  int ncharSb = sb->quantia_caracteries;
  int nbyteS, nbyteSb;
  bool participa;

  if (pos < 0)
  {
    // n = 5 pos = -2  carro  5 + -2 +1 = 4
    pos = ncharS + pos + 1;
  }
  if (pos > ncharS)
  {
    pos = ncharS;
  }
  byte *bytePosicao;
  int indiceInicioS;
  int indiceInicioSb;

  for (int i = pos - 1; i >= 0; i--)
  {
    bytePosicao = u8_avanca_unichar(s->vetor_codigos_utf8, i);
    indiceInicioS = bytePosicao - s->vetor_codigos_utf8;
    nbyteS = u8_nbytes_no_unichar_que_comeca_com(s->vetor_codigos_utf8[indiceInicioS]);
    indiceInicioSb = 0;
    participa = false;
    for (int j = 0; j < ncharSb; j++)
    {
      nbyteSb = u8_nbytes_no_unichar_que_comeca_com(sb->vetor_codigos_utf8[indiceInicioSb]);
      if (nbyteS == nbyteSb && memcmp(&s->vetor_codigos_utf8[indiceInicioS], &sb->vetor_codigos_utf8[indiceInicioSb], nbyteS) == 0)
      {
        participa = true;
      }
      indiceInicioSb += nbyteSb;
    }
    if (!participa)
    {
      return i;
    }
  }
  return -1;
}

int s_busca_s(Str_c s, int pos, Str_c buscada)
{
  s_ok(s);
  s_ok(buscada);

  int ncharS = s->quantia_caracteries;
  int ncharBuscada = buscada->quantia_caracteries;

  if (pos < 0)
  {
    pos = ncharS + pos + 1;
  }
  if (pos < 0)
  {
    pos = 0;
  }
  if (pos > ncharS)
  {
    pos = ncharS;
  }

  if (ncharBuscada == 0)
  {
    return pos;
  }

  for (int i = pos; i + ncharBuscada <= ncharS; i++)
  {
    byte *bytePosicao = u8_avanca_unichar(s->vetor_codigos_utf8, i);
    int indiceInicioS = bytePosicao - s->vetor_codigos_utf8;
    int indiceInicioBuscada = 0;

    bool combina = true;

    for (int j = 0; j < ncharBuscada; j++)
    {
      int nbyteS = u8_nbytes_no_unichar_que_comeca_com(s->vetor_codigos_utf8[indiceInicioS]);
      int nbyteBuscada = u8_nbytes_no_unichar_que_comeca_com(buscada->vetor_codigos_utf8[indiceInicioBuscada]);

      if (nbyteS != nbyteBuscada || memcmp(&s->vetor_codigos_utf8[indiceInicioS], &buscada->vetor_codigos_utf8[indiceInicioBuscada], nbyteS) != 0)
      {
        combina = false;
        break;
      }

      indiceInicioS += nbyteS;
      indiceInicioBuscada += nbyteBuscada;
    }

    if (combina)
    {
      return i;
    }
  }

  return -1;
}

void s_substitui(Str s, int pos, int tam, Str_c sb)
{
  s_ok(s);
  if (sb != NULL)
  {
    s_ok(sb);
  }
  int nCharS = s->quantia_caracteries;

  if (pos < 0)
  {
    pos = nCharS + pos + 1;
  }
  if (pos < 0)
  {
    pos = 0;
  }
  if (pos > nCharS)
  {
    pos = nCharS;
  }
  if (tam < 0)
  {
    tam = nCharS - pos;
  }
  if (pos + tam > nCharS)
  {
    tam = nCharS - pos;
  }

  byte *inicio = u8_avanca_unichar(s->vetor_codigos_utf8, pos);
  byte *fim = u8_avanca_unichar(inicio, tam);

  int indiceInicioS = inicio - s->vetor_codigos_utf8;
  int indiceFimS = fim - s->vetor_codigos_utf8;
  int nbyteSubstituir = indiceFimS - indiceInicioS;

  int nbytesS = s->quantia_bytes;
  int nbytesSb = sb->quantia_bytes;
  int ncharSb = sb->quantia_caracteries;
  int nbytesAposSb = nbytesS - indiceFimS;

  int tamanho = nbytesS - nbyteSubstituir + nbytesSb;

  if (tamanho > s->capacidade)
  {
    realoca_vetor(s, tamanho);
  }

  memmove(s->vetor_codigos_utf8 + indiceInicioS + nbytesSb, s->vetor_codigos_utf8 + indiceFimS, nbytesAposSb);

  if (nbytesSb > 0)
  {
    memcpy(s->vetor_codigos_utf8 + indiceInicioS, sb->vetor_codigos_utf8, nbytesSb);
  }

  s->quantia_bytes = tamanho;
  s->quantia_caracteries = nCharS - tam + ncharSb;
}

void s_substring(Str s, Str_c sb, int pos, int tam)
{
  s_ok(s);
  s_ok(sb);

  int nCharSb = sb->quantia_caracteries;

  if (pos < 0)
  {
    pos = nCharSb + pos + 1;
  }
  if (pos < 0)
  {
    pos = 0;
  }
  if (pos > nCharSb)
  {
    pos = nCharSb;
  }
  if (tam < 0)
  {
    tam = nCharSb - pos;
  }
  if (pos + tam > nCharSb)
  {
    tam = nCharSb - pos;
  }

  byte *inicio = u8_avanca_unichar(sb->vetor_codigos_utf8, pos);
  byte *fim = u8_avanca_unichar(inicio, tam);
  int nbyteSubstring = fim - inicio;

  byte *reserva = malloc(nbyteSubstring);
  if (nbyteSubstring > 0)
  {
    assert(reserva != NULL);
    memcpy(reserva, inicio, nbyteSubstring);
  }

  free(s->vetor_codigos_utf8);
  s->vetor_codigos_utf8 = reserva;
  s->quantia_bytes = nbyteSubstring;
  s->quantia_caracteries = tam;
  s->capacidade = nbyteSubstring;
}

void s_copia(Str s, Str_c sb)
{
  s_substring(s, sb, 0, -1);
}

void s_insere(Str s, int pos, Str_c sb)
{
  s_substitui(s, pos, 0, sb);
}

void s_insere_c(Str s, int pos, unichar c)
{
  s_ok(s);

  byte strC[5];

  int nbytesC = u8_converte_pra_utf8(c, strC);

  strC[nbytesC] = '\0';

  Str caractere = s_cria((char *)strC);

  s_substitui(s, pos, 0, caractere);

  s_destroi(caractere);
}

void s_anexa(Str s, Str_c sb)
{
  s_substitui(s, -1, 0, sb);
}

void s_anexa_c(Str s, unichar c)
{
  s_insere_c(s, -1, c);
}

void s_remove(Str s, int pos, int tam)
{
  s_substitui(s, pos, tam, NULL);
}

void s_apara(Str s, Str_c sobras)
{
  s_ok(s);
  s_ok(sobras);
  //...
}

// operações de E/S {{{1

void s_imprime(Str_c s)
{
  s_ok(s);
  fwrite(s->vetor_codigos_utf8, 1, s->quantia_bytes, stdout);
}

void s_grava_arquivo(Str_c s, char *nome)
{
  s_ok(s);
  FILE *arq = fopen(nome, "wb");
  assert(arq != NULL);
  fwrite(s->vetor_codigos_utf8, 1, s->quantia_bytes, arq);
  fclose(arq);
}

// vim: foldmethod=marker shiftwidth=2
