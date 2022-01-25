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
 *  Compile on PC:                                                         *
 *  gcc nn_iris.c -o nn -lm                                                *
 *  g++ nn_iris.c -o nn                                                    *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <math.h>

double W[10][10][2];
double B[10][2], E[20][3];
double threshold;
int num_neurons[4], actv_function[3];
int num_hidden_layers;
char AF_name[4][10] = {"Step", "Linear", "Sigmoid", "Tansig"};

// Define architecture here
void define_architecture()
{
	num_hidden_layers = 1;
	num_neurons[0] = 2; // Number of inputs
	num_neurons[1] = 4; // Hidden layer 1
	num_neurons[2] = 3;  // Output layer
	actv_function[0] = 3; // AF of HL 1
	actv_function[1] = 0; // AF of output layer
	threshold = 0.5; // threshold of step function
}

// Print architecture
void print_architecture()
{
	int i, output_layer = num_hidden_layers + 1;
	printf("Neural network architecture:\n");
	printf(" Number of hidden layers: %d\n", num_hidden_layers);
	printf(" Number of inputs: %d\n", num_neurons[0]);

	for (i=0; i<num_hidden_layers; i++)
	{
		printf(" Hidden layer %d:\n", i+1);
		printf("  Neurons: %d\n", num_neurons[i+1]);
		printf("  Activation function: %s\n", AF_name[actv_function[i]]);
	}

	printf(" Output layer:\n");
	printf("  Neurons: %d\n", num_neurons[output_layer]);
	printf("  Activation function: %s\n", AF_name[actv_function[output_layer-1]]);
}

// Define weights
void load_weights()
{
	// Layer 1
	W[0][0][0]=-1.26811; W[0][1][0]=10.54471;
	W[1][0][0]=-2.31851; W[1][1][0]=-2.13512;
	W[2][0][0]=-0.49873; W[2][1][0]=-0.62087;
	W[3][0][0]=1.61826; W[3][1][0]=-10.82781;
	B[0][0]=-11.6003; B[1][0]=7.4798; B[2][0]=-1.7431;  B[3][0]=10.3022;

	// Layer 2
	W[0][0][1]=-0.0025978; W[0][1][1]=0.5036170; W[0][2][1]=-0.4509608; W[0][3][1]=-0.0029980;
	W[1][0][1]=-2.2281652; W[1][1][1]=-0.5020465; W[1][2][1]=0.1524250; W[1][3][1]=-1.7334303;
	W[2][0][1]=2.2271598; W[2][1][1]=0.0010528; W[2][2][1]=-0.2051632; W[2][3][1]=1.7328068;
	B[0][1]=0.052374; B[1][1]=0.156305; B[2][1]=0.290303;
}

// Print weights
void print_weights()
{
	int layer, i, j;

	printf("Weights:\n");

	for (layer = 0; layer < num_hidden_layers + 1; layer++)
	{
		if (layer < num_hidden_layers)
			printf(" Hidden layer %d:\n", layer + 1);
		else
			printf(" Output layer:\n");

		for (i=0; i < num_neurons[layer+1]; i++)
		{
			for (j=0; j < num_neurons[layer]; j++)
				printf(" %.4f", W[i][j][layer]);
			printf("\n");
		}
		printf("\n");
	}

	printf("Bias:\n");

	for (layer = 0; layer < num_hidden_layers + 1; layer++)
	{
		if (layer < num_hidden_layers)
			printf(" Hidden layer %d:\n", layer + 1);
		else
			printf(" Output layer:\n");

		for (i=0; i < num_neurons[layer+1]; i++)
			printf(" %.4f", B[i][layer]);

		printf("\n\n");
	}
}

// Define inputs
void load_inputs()
{
	E[0][0] = 1.4;
	E[1][0] = 0.2;
}

// Activation function
double calculate_FA(double x, int type)
{
	switch (type)
	{
		case 0 : return 1 * (x > threshold); // step
		case 1 : return x; // Linear
		case 2 : return 1 / (1 + exp(-x)); // Sigmoid
		default : return 2 / (1 + exp(-2*x)) - 1; // Tansig
	}
}

// Feed forward
void feed_forward()
{
	int layer, i,j;

	for (layer = 0; layer < num_hidden_layers + 1; layer++)
	{
		for (i=0; i < num_neurons[layer+1]; i++)
		{
			E[i][layer+1] = 0.0;
			for (j=0; j < num_neurons[layer]; j++)
				E[i][layer+1] += W[i][j][layer] * E[j][layer];
			E[i][layer+1] = calculate_FA(E[i][layer+1] + B[i][layer], actv_function[layer]);
		}
	}
}

// Print results
void show()
{
	int i, output_layer = num_hidden_layers + 1;

	for (i=0; i < num_neurons[output_layer]; i++)
		printf("%.5f\n", E[i][output_layer]);
}

main()
{
	define_architecture();
	load_weights();
	load_inputs();
	feed_forward();

	// Debug
	print_architecture();
	print_weights();
	show();
}
