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
  if (s->quantia_bytes < 0 || s->quantia_caracteries < 0 || s->capacidade < 0)
  {
    abort();
  }
  if (s->vetor_codigos_utf8 == NULL && s->capacidade > 0)
  {
    abort();
  }
  int nchar = u8_conta_unichar_nos_bytes(s->quantia_bytes, s->vetor_codigos_utf8);
  if (nchar == -1)
  {
    abort();
  }
  if (nchar != s->quantia_caracteries)
  {
    abort();
  }
}

// calcula a capacidade a alocar para caber "necessario" bytes, respeitando
//   as regras do topo do arquivo: nunca menor que MIN_ALLOC, potência de 2,
//   e nunca maior que o triplo do necessário (exceto quando for o mínimo)
static int calcula_nova_capacidade(int necessario)
{
  if (necessario == 0)
  {
    return 0;
  }

  int capacidade = MIN_ALLOC;
  while (capacidade < necessario)
  {
    capacidade *= 2;
  }
  return capacidade;
}

// operações de criação e destruição {{{1

static byte *aloca_vetor(int tamanho)
{
  if (tamanho == 0)
  {
    return NULL;
  }
  byte *vet = malloc(tamanho);
  assert(vet != NULL);

  return vet;
}

static void preenche_struct_invalido(Str s)
{
  s->vetor_codigos_utf8 = NULL;
  s->quantia_bytes = 0;
  s->quantia_caracteries = 0;
  s->capacidade = 0;
}

static void preenche_struct(Str s, int tamanho, int nchar)
{
  int capacidade = calcula_nova_capacidade(tamanho);
  s->vetor_codigos_utf8 = aloca_vetor(capacidade);
  s->quantia_bytes = tamanho;
  s->quantia_caracteries = nchar;
  s->capacidade = capacidade;
}

// função de realocação
// só realoca se novaCapacidade for maior que a capacidade atual
static void realoca_vetor(Str s, int necessario)
{
  if (necessario <= s->capacidade)
  {
    return;
  }
  int novaCapacidade = calcula_nova_capacidade(necessario);
  byte *vetRealocado = realloc(s->vetor_codigos_utf8, novaCapacidade);
  assert(vetRealocado != NULL);
  s->vetor_codigos_utf8 = vetRealocado;
  s->capacidade = novaCapacidade;
}

// Aloca, inicializa e retorna uma string que contém uma cópia da
//   string C que inicia em strC.
// A string C deve estar codificada em UTF8, e é delimitada pelo
//   caractere \0 (que não faz parte da string).
// Retorna uma string vazia se strC não contiver UTF8 válido ou se for NULL.
Str s_cria(char const *strC)
{
  // comts strC já vem codificada em utf8
  Str s = malloc(sizeof(*s));
  assert(s != NULL);

  if (strC == NULL)
  {
    preenche_struct_invalido(s);
    return s;
  }

  int tamanho = strlen(strC);
  int nchar = u8_conta_unichar_nos_bytes(tamanho, (byte *)strC);

  if (nchar == -1)
  {
    preenche_struct_invalido(s);
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
  FILE *arq = fopen(nome, "rb");
  if (arq == NULL)
  {
    return s_cria("");
  }

  int tamanho = 0;
  int c;
  while ((c = fgetc(arq)) != EOF)
  {
    tamanho++;
  }
  rewind(arq);

  byte *reserva = malloc(tamanho > 0 ? tamanho : 1);
  assert(reserva != NULL);

  int cont = 0;
  while ((c = fgetc(arq)) != EOF)
  {
    reserva[cont] = (byte)c;
    cont++;
  }
  fclose(arq);

  int nchar = u8_conta_unichar_nos_bytes(tamanho, reserva);
  if (nchar == -1)
  {
    free(reserva);
    return s_cria("");
  }

  Str s = malloc(sizeof(*s));
  assert(s != NULL);
  s->vetor_codigos_utf8 = reserva;
  s->quantia_bytes = tamanho;
  s->quantia_caracteries = nchar;
  s->capacidade = tamanho > 0 ? tamanho : 1;

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

  char *copia = malloc(s->quantia_bytes + 1);
  assert(copia != NULL);

  memcpy(copia, s->vetor_codigos_utf8, s->quantia_bytes);
  copia[s->quantia_bytes] = '\0';

  return copia;
}

unichar s_ch(Str_c s, int pos)
{
  s_ok(s);

  int ncharS = s->quantia_caracteries;

  if (pos < 0)
  {
    pos = ncharS + pos + 1;
  }

  if (pos < 0 || pos >= ncharS)
  {
    return UNI_INV;
  }

  byte *inicio = u8_avanca_unichar(s->vetor_codigos_utf8, pos);
  int indiceInicio = inicio - s->vetor_codigos_utf8;
  int nbyte = u8_nbytes_no_unichar_que_comeca_com(s->vetor_codigos_utf8[indiceInicio]);

  unichar c;
  u8_unichar_nos_bytes(nbyte, &s->vetor_codigos_utf8[indiceInicio], &c);

  return c;
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
  }
  return true;
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
  if (pos < 0)
  {
    pos = 0;
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
  int nbytesAposSb = nbytesS - indiceFimS;

  int nbytesSb;
  int ncharSb;

  if (sb != NULL)
  {
    nbytesSb = sb->quantia_bytes;
    ncharSb = sb->quantia_caracteries;
  }
  else
  {
    nbytesSb = 0;
    ncharSb = 0;
  }

  int tamanho = nbytesS - nbyteSubstituir + nbytesSb;

  realoca_vetor(s, tamanho);

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

  int novaCapacidade = calcula_nova_capacidade(nbyteSubstring);
  byte *reserva = aloca_vetor(novaCapacidade);
  if (nbyteSubstring > 0)
  {
    memcpy(reserva, inicio, nbyteSubstring);
  }

  free(s->vetor_codigos_utf8);
  s->vetor_codigos_utf8 = reserva;
  s->quantia_bytes = nbyteSubstring;
  s->quantia_caracteries = tam;
  s->capacidade = novaCapacidade;
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

  byte strC[4];
  int nbytesC = u8_converte_pra_utf8(c, strC);
  assert(nbytesC != -1);

  Str caractere = malloc(sizeof(*caractere));
  assert(caractere != NULL);
  caractere->vetor_codigos_utf8 = aloca_vetor(nbytesC);
  memcpy(caractere->vetor_codigos_utf8, strC, nbytesC);
  caractere->quantia_bytes = nbytesC;
  caractere->quantia_caracteries = 1;
  caractere->capacidade = nbytesC;

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

  int ncharS = s->quantia_caracteries;
  int inicio = s_busca_nc(s, 0, sobras);

  if (inicio == -1)
  {
    s_substitui(s, 0, ncharS, NULL);
    return;
  }
  int fim = s_busca_rnc(s, -1, sobras);
  s_substitui(s, fim + 1, -1, NULL);
  s_substitui(s, 0, inicio, NULL);
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