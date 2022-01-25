/***************************************************************************
 *   rede_neural_iris.c                                                    *
 *                                                                         *
 *   Copyright (C) 2021 by Marcelo Silveira                                *
 *   MarMSX: http://marmsx.msxall.com                                      *
 *   Contact: flamar98@hotmail.com                                         *
 *                                                                         *
 *  License: GNU-GPL v. 3.x                                                *
 *  http://www.gnu.org/licenses/gpl-3.0.txt                                *
 *                                                                         *
 *  Compilar no PC:                                                        *
 *  gcc rede_neural_iris.c -o neural -lm                                   *
 *  g++ rede_neural_iris.c -o neural                                       *
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
	num_neuronio[1] = 4; // Camada escondida 1
	num_neuronio[2] = 3;  // Camada de saída
	func_ativacao[0] = 3; // FA da camada escondida 1
	func_ativacao[1] = 0; // FA da camada saída
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
	W[0][0][0]=-1.26811; W[0][1][0]=10.54471;
	W[1][0][0]=-2.31851; W[1][1][0]=-2.13512;
	W[2][0][0]=-0.49873; W[2][1][0]=-0.62087;
	W[3][0][0]=1.61826; W[3][1][0]=-10.82781;
	B[0][0]=-11.6003; B[1][0]=7.4798; B[2][0]=-1.7431;  B[3][0]=10.3022;

	// Camada 2
	W[0][0][1]=-0.0025978; W[0][1][1]=0.5036170; W[0][2][1]=-0.4509608; W[0][3][1]=-0.0029980;
	W[1][0][1]=-2.2281652; W[1][1][1]=-0.5020465; W[1][2][1]=0.1524250; W[1][3][1]=-1.7334303;
	W[2][0][1]=2.2271598; W[2][1][1]=0.0010528; W[2][2][1]=-0.2051632; W[2][3][1]=1.7328068;
	B[0][1]=0.052374; B[1][1]=0.156305; B[2][1]=0.290303;
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
	E[0][0] = 1.4;
	E[1][0] = 0.2;
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
