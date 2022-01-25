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

// Architecture
const N=4, NT=4, epochs=10, num_attributes=2, target = 2;
double weight[2];
double bias, a=0.01;

// Dados - OU exclusivo (XOR)
double train_set[4][3] = {{0,0,0}, {0,1,1}, {1,0,1}, {1,1,0}};
double test_set[4][3] = {{0,0,0}, {0,1,1}, {1,0,1}, {1,1,0}};

double initialize_weight()
{
  int N = rand() % 100;

  return 25.0 / N;
}

void initialize_weights() 
{
  int i;
  srand(time(NULL));

  for (i=0; i<num_attributes; i++)
    weight[i] = initialize_weight();

  bias = initialize_weight();
}

double evaluate(int line_no, int stage_train)
{
  int soma=0, i;

  for (i=0; i<num_attributes; i++)
  {
    if (stage_train)
      soma += train_set[line_no][i] * weight[i];
    else
      soma += test_set[line_no][i] * weight[i];
  }

  soma += bias;

  return soma > 0.5;
}

void train()
{
  double s, e, erro;
  int line_no, i, ep;

  for (ep=0; ep<epochs; ep++)
  {
    erro = 0;

    for (line_no=0; line_no<N; line_no++)
    {
      s = evaluate(line_no, 1);
      e = train_set[line_no][target] - s;

      for (i=0; i<num_attributes; i++)
        weight[i] += a * e * train_set[line_no][i];

      bias += a * e;

      if (e != 0)
        erro++;
    }
    erro = erro / N;
    printf("Epoch %d - error = %.2f\n", ep+1, erro);
  }
}

void test()
{
  int errors=0, line_no;
  double s;

  for (line_no=0; line_no<NT; line_no++)
  {
    s = evaluate(line_no, 0);

    printf("Element: %d / target: %.2f / Pred: %.2f\n", line_no+1, test_set[line_no][target], s);

    if (s != test_set[line_no][target])
      errors++;
  }

  printf("Errors in test: %d\n", errors);
}

main() 
{
  printf("Initialize weights ...\n");
  initialize_weights();
  printf("Start training ...\n");
  train();
  printf("Start testing ...\n");
  test();
}
