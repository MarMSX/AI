/***************************************************************************
 *   perceptron_xor.c                                                      *
 *                                                                         *
 *   Copyright (C) 2021 by Marcelo Silveira                                *
 *   MarMSX: http://marmsx.msxall.com                                      *
 *   Contact: flamar98@hotmail.com                                         *
 *                                                                         *
 *  License: GNU-GPL v. 3.x                                                *
 *  http://www.gnu.org/licenses/gpl-3.0.txt                                *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Arquitetura
const N=4, NT=4, epocas=10, num_atributos=2, alvo = 2;
double peso[2];
double bias, a=0.01;

// Dados - OU exclusivo (XOR)
double treino[4][3] = {{0,0,0}, {0,1,1}, {1,0,1}, {1,1,0}};
double testes[4][3] = {{0,0,0}, {0,1,1}, {1,0,1}, {1,1,0}};

double inicializa_peso()
{
  int N = rand() % 100;

  return 25.0 / N;
}

void inicializa_pesos() 
{
  int i;
  srand(time(NULL));

  for (i=0; i<num_atributos; i++)
    peso[i] = inicializa_peso();

  bias = inicializa_peso();
}

double avalia(int linha, int fase_treino)
{
  int soma=0, i;

  for (i=0; i<num_atributos; i++)
  {
    if (fase_treino)
      soma += treino[linha][i] * peso[i];
    else
      soma += testes[linha][i] * peso[i];
  }

  soma += bias;

  return soma > 0.5;
}

void treina()
{
  double s, e, erro;
  int linha, i, ep;

  for (ep=0; ep<epocas; ep++)
  {
    erro = 0;

    for (linha=0; linha<N; linha++)
    {
      s = avalia(linha, 1);
      e = treino[linha][alvo] - s;

      for (i=0; i<num_atributos; i++)
        peso[i] += a * e * treino[linha][i];

      bias += a * e;

      if (e != 0)
        erro++;
    }
    erro = erro / N;
    printf("Epoca %d - erro = %.2f\n", ep+1, erro);
  }
}

void testa()
{
  int erros=0, linha;
  double s;

  for (linha=0; linha<NT; linha++)
  {
    s = avalia(linha, 0);

    printf("Elemento: %d / Alvo: %.2f / Prev: %.2f\n", linha+1, testes[linha][alvo], s);

    if (s != testes[linha][alvo])
      erros++;
  }

  printf("Erros em teste: %d\n", erros);
}

main() 
{
  printf("Inicializa pesos ...\n");
  inicializa_pesos();
  printf("Inicia treinamento ...\n");
  treina();
  printf("Inicia testes ...\n");
  testa();
}
