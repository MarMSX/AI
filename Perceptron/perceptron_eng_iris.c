/***************************************************************************
 *   perceptron_iris.c                                                     *
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
const N=70, NT=30, epochs=10, num_attributes=2, target = 2;
double weight[2];
double bias, a=0.01;

// Data for iris setosa and versicolor - petal length and petal width
double train_set[70][3] = {{1.4, 0.2, 0}, {1.4, 0.2, 0}, {1.3, 0.2, 0}, {1.5, 0.2, 0}, {1.4, 0.2, 0}, {1.7, 0.4, 0}, {1.4, 0.3, 0}, {1.5, 0.2, 0}, {1.4, 0.2, 0}, {1.5, 0.1, 0}, {1.5, 0.2, 0}, {1.6, 0.2, 0}, {1.4, 0.1, 0}, {1.1, 0.1, 0}, {1.2, 0.2, 0}, {1.5, 0.4, 0}, {1.3, 0.4, 0}, {1.4, 0.3, 0}, {1.7, 0.3, 0}, {1.5, 0.3, 0}, {1.7, 0.2, 0}, {1.5, 0.4, 0}, {1.0, 0.2, 0}, {1.7, 0.5, 0}, {1.9, 0.2, 0}, {1.6, 0.2, 0}, {1.6, 0.4, 0}, {1.5, 0.2, 0}, {1.4, 0.2, 0}, {1.6, 0.2, 0}, {1.6, 0.2, 0}, {1.5, 0.4, 0}, {1.5, 0.1, 0}, {1.4, 0.2, 0}, {1.5, 0.1, 0}, {4.7, 1.4, 1}, {4.5, 1.5, 1}, {4.9, 1.5, 1}, {4.0, 1.3, 1}, {4.6, 1.5, 1}, {4.5, 1.3, 1}, {4.7, 1.6, 1}, {3.3, 1.0, 1}, {4.6, 1.3, 1}, {3.9, 1.4, 1}, {3.5, 1.0, 1}, {4.2, 1.5, 1}, {4.0, 1.0, 1}, {4.7, 1.4, 1}, {3.6, 1.3, 1}, {4.4, 1.4, 1}, {4.5, 1.5, 1}, {4.1, 1.0, 1}, {4.5, 1.5, 1}, {3.9, 1.1, 1}, {4.8, 1.8, 1}, {4.0, 1.3, 1}, {4.9, 1.5, 1}, {4.7, 1.2, 1}, {4.3, 1.3, 1}, {4.4, 1.4, 1}, {4.8, 1.4, 1}, {5.0, 1.7, 1}, {4.5, 1.5, 1}, {3.5, 1.0, 1}, {3.8, 1.1, 1}, {3.7, 1.0, 1}, {3.9, 1.2, 1}, {5.1, 1.6, 1}, {4.5, 1.5, 1}};

double test_set[30][3] = {{1.2, 0.2, 0}, {1.3, 0.2, 0}, {1.5, 0.1, 0}, {1.3, 0.2, 0}, {1.5, 0.2, 0}, {1.3, 0.3, 0}, {1.3, 0.3, 0}, {1.3, 0.2, 0}, {1.6, 0.6, 0}, {1.9, 0.4, 0}, {1.4, 0.3, 0}, {1.6, 0.2, 0}, {1.4, 0.2, 0}, {1.5, 0.2, 0}, {1.4, 0.2, 0}, {4.5, 1.6, 1}, {4.7, 1.5, 1}, {4.4, 1.3, 1}, {4.1, 1.3, 1}, {4.0, 1.3, 1}, {4.4, 1.2, 1}, {4.6, 1.4, 1}, {4.0, 1.2, 1}, {3.3, 1.0, 1}, {4.2, 1.3, 1}, {4.2, 1.2, 1}, {4.2, 1.3, 1}, {4.3, 1.3, 1}, {3.0, 1.1, 1}, {4.1, 1.3, 1}};

double start_weight()
{
  int N = rand() % 100;

  return 25.0 / N;
}

void initialize_weights() 
{
  int i;
  srand(time(NULL));

  for (i=0; i<num_attributes; i++)
    weight[i] = start_weight();

  bias = start_weight();
}

double evaluate(int line_no, int stage_train)
{
  int sum=0, i;

  for (i=0; i<num_attributes; i++)
  {
    if (stage_train)
      sum += train_set[line_no][i] * weight[i];
    else
      sum += test_set[line_no][i] * weight[i];
  }

  sum += bias;

  return sum > 0.5;
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
