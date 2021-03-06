/***************************************************************************
 *   rede_neural_bp_iris.c                                                 *
 *                                                                         *
 *   Copyright (C) 2021 by Marcelo Silveira                                *
 *   MarMSX: http://marmsx.msxall.com                                      *
 *   Contact: flamar98@hotmail.com                                         *
 *                                                                         *
 *  License: GNU-GPL v. 3.x                                                *
 *  http://www.gnu.org/licenses/gpl-3.0.txt                                *
 *                                                                         *
 *  Compilar no PC:                                                        *
 *  gcc rede_neural_bp_iris.c -o neural -lm                                *
 *  g++ rede_neural_bp_iris.c -o neural                                    *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define num_entradas 2
#define num_saidas 3
#define num_amostras_treino 105
#define num_amostras_teste 45

double W[10][10][2];
double B[10][2], E[20][3];
double limiar;
int num_neuronio[4], func_ativacao[3];
int num_camadas_escondidas;
char nome_FA[4][10] = {"Degrau", "Linear", "Sigmoide", "Tansig"};
double Err[num_saidas];
double 	taxa_aprendizado;
double min_dados=0.1, max_dados=6.9;

// Define conjunto de treinamento e testes
double P[num_amostras_treino][num_entradas] = {{1.4, 0.2}, {1.4, 0.2}, {1.3, 0.2}, {1.5, 0.2}, {1.4, 0.2}, {1.7, 0.4}, {1.4, 0.3}, {1.5, 0.2}, {1.4, 0.2}, {1.5, 0.1}, {1.5, 0.2}, {1.6, 0.2}, {1.4, 0.1}, {1.1, 0.1}, {1.2, 0.2}, {1.5, 0.4}, {1.3, 0.4}, {1.4, 0.3}, {1.7, 0.3}, {1.5, 0.3}, {1.7, 0.2}, {1.5, 0.4}, {1.0, 0.2}, {1.7, 0.5}, {1.9, 0.2}, {1.6, 0.2}, {1.6, 0.4}, {1.5, 0.2}, {1.4, 0.2}, {1.6, 0.2}, {1.6, 0.2}, {1.5, 0.4}, {1.5, 0.1}, {1.4, 0.2}, {1.5, 0.1}, {4.7, 1.4}, {4.5, 1.5}, {4.9, 1.5}, {4.0, 1.3}, {4.6, 1.5}, {4.5, 1.3}, {4.7, 1.6}, {3.3, 1.0}, {4.6, 1.3}, {3.9, 1.4}, {3.5, 1.0}, {4.2, 1.5}, {4.0, 1.0}, {4.7, 1.4}, {3.6, 1.3}, {4.4, 1.4}, {4.5, 1.5}, {4.1, 1.0}, {4.5, 1.5}, {3.9, 1.1}, {4.8, 1.8}, {4.0, 1.3}, {4.9, 1.5}, {4.7, 1.2}, {4.3, 1.3}, {4.4, 1.4}, {4.8, 1.4}, {5.0, 1.7}, {4.5, 1.5}, {3.5, 1.0}, {3.8, 1.1}, {3.7, 1.0}, {3.9, 1.2}, {5.1, 1.6}, {4.5, 1.5}, {6.0, 2.5}, {5.1, 1.9}, {5.9, 2.1}, {5.6, 1.8}, {5.8, 2.2}, {6.6, 2.1}, {4.5, 1.7}, {6.3, 1.8}, {5.8, 1.8}, {6.1, 2.5}, {5.1, 2.0}, {5.3, 1.9}, {5.5, 2.1}, {5.0, 2.0}, {5.1, 2.4}, {5.3, 2.3}, {5.5, 1.8}, {6.7, 2.2}, {6.9, 2.3}, {5.0, 1.5}, {5.7, 2.3}, {4.9, 2.0}, {6.7, 2.0}, {4.9, 1.8}, {5.7, 2.1}, {6.0, 1.8}, {4.8, 1.8}, {4.9, 1.8}, {5.6, 2.1}, {5.8, 1.6}, {6.1, 1.9}, {6.4, 2.0}, {5.6, 2.2}, {5.1, 1.5}, {5.6, 1.4}};

double T[num_amostras_treino][num_saidas] = {{1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}};

double Ps[num_amostras_teste][num_entradas] = {{1.2, 0.2}, {1.3, 0.2}, {1.5, 0.1}, {1.3, 0.2}, {1.5, 0.2}, {1.3, 0.3}, {1.3, 0.3}, {1.3, 0.2}, {1.6, 0.6}, {1.9, 0.4}, {1.4, 0.3}, {1.6, 0.2}, {1.4, 0.2}, {1.5, 0.2}, {1.4, 0.2}, {4.5, 1.6}, {4.7, 1.5}, {4.4, 1.3}, {4.1, 1.3}, {4.0, 1.3}, {4.4, 1.2}, {4.6, 1.4}, {4.0, 1.2}, {3.3, 1.0}, {4.2, 1.3}, {4.2, 1.2}, {4.2, 1.3}, {4.3, 1.3}, {3.0, 1.1}, {4.1, 1.3}, {6.1, 2.3}, {5.6, 2.4}, {5.5, 1.8}, {4.8, 1.8}, {5.4, 2.1}, {5.6, 2.4}, {5.1, 2.3}, {5.1, 1.9}, {5.9, 2.3}, {5.7, 2.5}, {5.2, 2.3}, {5.0, 1.9}, {5.2, 2.0}, {5.4, 2.3}, {5.1, 1.8}};

double Ts[num_amostras_teste][num_saidas] = {{1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 1, 0}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}, {0, 0, 1}};


// Defina a arquitetura aqui
void define_arquitetura()
{
	num_camadas_escondidas = 1;
	num_neuronio[0] = num_entradas; // Numero de entradas
	num_neuronio[1] = 4; // Camada escondida 1
	num_neuronio[2] = num_saidas;  // Camada de sa??da
	func_ativacao[0] = 3; // FA da camada escondida 1
	func_ativacao[1] = 1; // FA da camada sa??da
	limiar = 0.5; // Limiar da fun????o degrau
}

// Imprime arquitetura
void imprime_arquitetura()
{
	int i, cam_saida = num_camadas_escondidas + 1;
	printf("Arquitetura da rede neural:\n");
	printf(" N??mero de camadas escondidas: %d\n", num_camadas_escondidas);
	printf(" N??mero de entradas: %d\n", num_neuronio[0]);

	for (i=0; i<num_camadas_escondidas; i++)
	{
		printf(" Camada escondida %d:\n", i+1);
		printf("  Neur??nios: %d\n", num_neuronio[i+1]);
		printf("  Fun????o de ativa????o: %s\n", nome_FA[func_ativacao[i]]);
	}

	printf(" Camada de sa??da:\n");
	printf("  Neur??nios: %d\n", num_neuronio[cam_saida]);
	printf("  Fun????o de ativa????o: %s\n", nome_FA[func_ativacao[cam_saida-1]]);
}

// Inicializa pesos
void inicializa_pesos()
{
	int camada, i, j;
	srand(time(NULL));

	// Pesos
	for (camada = 0; camada < num_camadas_escondidas + 1; camada++)
	{
		for (i=0; i < num_neuronio[camada+1]; i++)
		{
			for (j=0; j < num_neuronio[camada]; j++)
				W[i][j][camada] = 1 - 2 * ((double) rand()) / ((double) RAND_MAX);
		}
	}

	// Bias
	for (camada = 0; camada < num_camadas_escondidas + 1; camada++)
	{
		for (i=0; i < num_neuronio[camada+1]; i++)
			B[i][camada] = 1 - 2 * ((double) rand()) / ((double) RAND_MAX);
	}
}

// Imprime pesos
void imprime_pesos()
{
	int camada, i, j;

	printf("Pesos:\n");

	for (camada = 0; camada < num_camadas_escondidas + 1; camada++)
	{
		if (camada < num_camadas_escondidas)
			printf(" Camada escondida %d:\n", camada + 1);
		else
			printf(" Camada de sa??da:\n");

		for (i=0; i < num_neuronio[camada+1]; i++)
		{
			for (j=0; j < num_neuronio[camada]; j++)
				printf(" %.4f", W[i][j][camada]);
			printf("\n");
		}
		printf("\n");
	}

	printf("Bias:\n");

	for (camada = 0; camada < num_camadas_escondidas + 1; camada++)
	{
		if (camada < num_camadas_escondidas)
			printf(" Camada escondida %d:\n", camada + 1);
		else
			printf(" Camada de sa??da:\n");

		for (i=0; i < num_neuronio[camada+1]; i++)
			printf(" %.4f", B[i][camada]);

		printf("\n\n");
	}
}

// Define entradas
void carrega_entradas(int linha, int treino)
{
	int i;

	for (i=0; i<num_entradas; i++)
	{
		if (treino == 1)
			E[i][0] = (P[linha][i] - min_dados) / (max_dados - min_dados);
		else
			E[i][0] = (Ps[linha][i] - min_dados) / (max_dados - min_dados);
	}
}

// Fun????o de ativa????o
double calcula_FA(double x, int tipo)
{
	switch (tipo)
	{
		case 0 : return 1 * (x > limiar); // Degrau
		case 1 : return x; // Linear
		case 2 : return 1 / (1 + exp(-x)); // Sigmoide
		default : return 2 / (1 + exp(-2*x)) - 1; // Tansig
	}
}

// Feed forward
void feed_forward()
{
	int camada, i,j;

	for (camada = 0; camada < num_camadas_escondidas + 1; camada++)
	{
		for (i=0; i < num_neuronio[camada+1]; i++)
		{
			E[i][camada+1] = 0.0;
			for (j=0; j < num_neuronio[camada]; j++)
				E[i][camada+1] += W[i][j][camada] * E[j][camada];
			E[i][camada+1] = calcula_FA(E[i][camada+1] + B[i][camada], func_ativacao[camada]);
		}
	}
}

// Calcula erro na sa??da 
// Utiliza Mean Squared Error - MSE
double calcula_performance(int linha)
{
	int i, cam_saida = num_camadas_escondidas + 1;
	double erro = 0.0;

	for (i=0; i < num_saidas; i++)
	{
		Err[i] = T[linha][i] - E[i][cam_saida];
		erro += pow(Err[i], 2);
	}

	return erro / ((double) 2 * num_saidas);
}

// Fun????o de ativa????o - derivada
double calcula_FA_linha(double x, int tipo)
{
	double f;

	switch (tipo)
	{
		case 0 : return 1; // Degrau
		case 1 : return 1; // Linear
		case 2 : f = calcula_FA(x,2); return f*(1-f); // Sigmoide
		default : f = calcula_FA(x,3); return 1 - pow(f, 2.0); // Tansig
	}
}

// Realiza backpropagation
void backpropagation()
{
	int camada, i, j;
	double erro;

	// Percorre camadas no sentido inverso
	// O vetor E tem deslocamento horizontal de +1 em rela????o a W, B e func_ativacao
	for (camada = num_camadas_escondidas; camada >= 0; camada--)
	{
		// Passo 1 - Atualiza sa??das
		if (camada < num_camadas_escondidas)
		{
			// Atualiza camadas escondidas
			for (i=0; i < num_neuronio[camada+1]; i++)
			{
				erro = 0.0;
				for (j=0; j < num_neuronio[camada+2]; j++)
					erro += W[j][i][camada+1] * E[j][camada+2];
				E[i][camada+1] = erro * calcula_FA_linha(E[i][camada+1], func_ativacao[camada]);
			}
		}
		else
		{
			// Atualiza a camada de sa??da com erros mais F'(x)
			for (i=0; i < num_saidas; i++)
				E[i][camada+1] = Err[i] * calcula_FA_linha(E[i][camada+1], func_ativacao[camada]);
		}

		// Passo 2 - Atualiza pesos e bias
		for (i=0; i < num_neuronio[camada+1]; i++)
		{
			for (j=0; j < num_neuronio[camada]; j++)
				W[i][j][camada] += taxa_aprendizado * E[i][camada+1] * E[j][camada];

			B[i][camada] += taxa_aprendizado * E[i][camada+1];
		}
	}
}

// Treinamento
void treina()
{
	int epoca, i;
	double erro_medio_batch, erro;

	// Define par??metros de treinamento
	int num_epocas = 2000;
	int imprime_epoca = 100;
	double goal = 0.01;
	taxa_aprendizado = 0.01;
 
	for (epoca=1; epoca <= num_epocas; epoca++)
	{
		erro_medio_batch = 0.0;

		for (i=0; i < num_amostras_treino; i++)
		{
			carrega_entradas(i,1);
			feed_forward();
			erro = calcula_performance(i);
			erro_medio_batch += erro;

			if (erro >= goal)
				backpropagation();
		}

		// Calcula erro para todas as amostras (batch)
		erro_medio_batch /= ((double) num_amostras_treino);

		if (epoca % imprime_epoca == 0)
			printf("Epoca %d/%d, MSE: %.2f\n", epoca, num_epocas, erro_medio_batch);

		if (erro_medio_batch <= goal)
		{
			printf("Treinamento convergiu.\n");
			return;
		}
	}
}

// Imprime resultado
void show()
{
	int i, cam_saida = num_camadas_escondidas + 1;

	for (i=0; i < num_neuronio[cam_saida]; i++)
		printf("%.5f\n", E[i][cam_saida]);
}

// Testa
void testa()
{
	int i, k, num_acertos=0, cam_saida = num_camadas_escondidas + 1, S;
	double degrau;

	printf("\nTeste:\n");

	for (i=0; i < num_amostras_teste; i++)
	{
		carrega_entradas(i,0);
		feed_forward();

		for (k=0; k < num_saidas; k++)
		{
			degrau = (double) E[k][cam_saida] > 0.5;
			if (degrau != Ts[i][k])
				break;
			if (k == (num_saidas - 1))
				num_acertos++;
		}
	}

	printf("Numero de amostras: %d\n", num_amostras_teste);
	printf("Acertos: %d (%.2f %%)\n", num_acertos, (100.0 * num_acertos) / num_amostras_teste);
}

main()
{
	define_arquitetura();
	inicializa_pesos();
	treina();
	testa();

	// Debug
//	imprime_arquitetura();
//	imprime_pesos();
//	show();
}
