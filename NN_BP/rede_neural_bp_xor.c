/***************************************************************************
 *   rede_neural_bp_xor.c                                                  *
 *                                                                         *
 *   Copyright (C) 2021 by Marcelo Silveira                                *
 *   MarMSX: http://marmsx.msxall.com                                      *
 *   Contact: flamar98@hotmail.com                                         *
 *                                                                         *
 *  License: GNU-GPL v. 3.x                                                *
 *  http://www.gnu.org/licenses/gpl-3.0.txt                                *
 *                                                                         *
 *  Compilar no PC:                                                        *
 *  gcc rede_neural_bp_xor.c -o neural -lm                                 *
 *  g++ rede_neural_bp_xor.c -o neural                                     *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define num_entradas 2
#define num_saidas 1
#define num_amostras_treino 4
#define num_amostras_teste 4

double W[10][10][2];
double B[10][2], E[20][3];
double limiar;
int num_neuronio[4], func_ativacao[3];
int num_camadas_escondidas;
char nome_FA[4][10] = {"Degrau", "Linear", "Sigmoide", "Tansig"};
double Err[num_saidas];
double 	taxa_aprendizado;

// Define conjunto de treinamento e testes
double P[num_amostras_treino][num_entradas] = { {0,0}, {0,1}, {1,0}, {1,1} };
double T[num_amostras_treino][num_saidas] = { 0, 1, 1, 0 };
double Ps[num_amostras_teste][num_entradas] = { {0,0}, {0,1}, {1,0}, {1,1} };

// Defina a arquitetura aqui
void define_arquitetura()
{
	num_camadas_escondidas = 1;
	num_neuronio[0] = num_entradas; // Numero de entradas
	num_neuronio[1] = 2; // Camada escondida 1
	num_neuronio[2] = num_saidas;  // Camada de saída
	func_ativacao[0] = 3; // FA da camada escondida 1
	func_ativacao[1] = 1; // FA da camada saída
	limiar = 0.5; // Limiar da função degrau
}

// Imprime arquitetura
void imprime_arquitetura()
{
	int i, cam_saida = num_camadas_escondidas + 1;
	printf("Arquitetura da rede neural:\n");
	printf(" Número de camadas escondidas: %d\n", num_camadas_escondidas);
	printf(" Número de entradas: %d\n", num_neuronio[0]);

	for (i=0; i<num_camadas_escondidas; i++)
	{
		printf(" Camada escondida %d:\n", i+1);
		printf("  Neurônios: %d\n", num_neuronio[i+1]);
		printf("  Função de ativação: %s\n", nome_FA[func_ativacao[i]]);
	}

	printf(" Camada de saída:\n");
	printf("  Neurônios: %d\n", num_neuronio[cam_saida]);
	printf("  Função de ativação: %s\n", nome_FA[func_ativacao[cam_saida-1]]);
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
			printf(" Camada de saída:\n");

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
			printf(" Camada de saída:\n");

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
			E[i][0] = P[linha][i];
		else
			E[i][0] = Ps[linha][i];
	}
}

// Função de ativação
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

// Calcula erro na saída 
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

// Função de ativação - derivada
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
	// O vetor E tem deslocamento horizontal de +1 em relação a W, B e func_ativacao
	for (camada = num_camadas_escondidas; camada >= 0; camada--)
	{
		// Passo 1 - Atualiza saídas
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
			// Atualiza a camada de saída com erros mais F'(x)
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

	// Define parâmetros de treinamento
	int num_epocas = 2000;
	int imprime_epoca = 100;
	double goal = 0.001;
	taxa_aprendizado = 0.03;
 
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
		erro_medio_batch /= num_amostras_treino;

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
	int i;

	printf("\nTeste:\n");

	for (i=0; i < num_amostras_teste; i++)
	{
		carrega_entradas(i,0);
		feed_forward();
		printf("Amostra %d\n", i+1);
		show();
		printf("\n");
	}
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
