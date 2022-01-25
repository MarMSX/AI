/***************************************************************************
 *   rede_neural_xor.c                                                     *
 *                                                                         *
 *   Copyright (C) 2021 by Marcelo Silveira                                *
 *   MarMSX: http://marmsx.msxall.com                                      *
 *   Contact: flamar98@hotmail.com                                         *
 *                                                                         *
 *  License: GNU-GPL v. 3.x                                                *
 *  http://www.gnu.org/licenses/gpl-3.0.txt                                *
 *                                                                         *
 *  Compilar no PC:                                                        *
 *  gcc rede_neural_xor.c -o neural -lm                                    *
 *  g++ rede_neural_xor.c -o neural                                        *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <math.h>

double W[10][10][2];
double B[10][2], E[20][3];
double limiar;
int num_neuronio[4], func_ativacao[3];
int num_camadas_escondidas;
char nome_FA[4][10] = {"Degrau", "Linear", "Sigmoide", "Tansig"};

// Defina a arquitetura aqui
void define_arquitetura()
{
	num_camadas_escondidas = 1;
	num_neuronio[0] = 2; // Numero de entradas
	num_neuronio[1] = 2; // Camada escondida 1
	num_neuronio[2] = 1;  // Camada de saída
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

// Define pesos
void carrega_pesos()
{
	// Camada 1
	W[0][0][0]=8.1085; W[0][1][0]=3.2252;
	W[1][0][0]=3.6846; W[1][1][0]=2.9790;
	B[0][0]=0.38717; B[1][0]=-6.5109;

	// Camada 2
	W[0][0][1]=1.59069; W[0][1][1]=-0.86057;
	B[0][1]=-1.4474;
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
void carrega_entradas()
{
	E[0][0] = 0;
	E[1][0] = 1;
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

// Imprime resultado
void show()
{
	int i, cam_saida = num_camadas_escondidas + 1;

	for (i=0; i < num_neuronio[cam_saida]; i++)
		printf("%.5f\n", E[i][cam_saida]);
}

main()
{
	define_arquitetura();
	carrega_pesos();
	carrega_entradas();
	feed_forward();

	// Debug
//	imprime_arquitetura();
//	imprime_pesos();
	show();
}
