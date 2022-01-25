/***************************************************************************
 *   nn_bp_xor.c                                                           *
 *                                                                         *
 *   Copyright (C) 2021 by Marcelo Silveira                                *
 *   MarMSX: http://marmsx.msxall.com                                      *
 *   Contact: flamar98@hotmail.com                                         *
 *                                                                         *
 *  License: GNU-GPL v. 3.x                                                *
 *  http://www.gnu.org/licenses/gpl-3.0.txt                                *
 *                                                                         *
 *  Compile on PC:                                                         *
 *  gcc nn_bp_xor.c -o nn -lm                                              *
 *  g++ nn_bp_xor.c -o nn                                                  *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define num_inputs 2
#define num_outputs 1
#define num_samples_train 4
#define num_samples_test 4

double W[10][10][2];
double B[10][2], E[20][3];
double threshold;
int num_neuron[4], actv_function[3];
int num_hidden_layers;
char AF_name[4][10] = {"Step", "Linear", "Sigmoid", "Tansig"};
double Err[num_outputs];
double learning_rate;

// Define training and test sets
double P[num_samples_train][num_inputs] = { {0,0}, {0,1}, {1,0}, {1,1} };
double T[num_samples_train][num_outputs] = { 0, 1, 1, 0 };
double Ps[num_samples_test][num_inputs] = { {0,0}, {0,1}, {1,0}, {1,1} };

// Define architecture here
void define_architecture()
{
	num_hidden_layers = 1;
	num_neuron[0] = num_inputs; // Number of inputs
	num_neuron[1] = 2; // Hidden layer 1
	num_neuron[2] = num_outputs;  // Output layer
	actv_function[0] = 3; // AF of HL 1
	actv_function[1] = 1; // AF of output layer
	threshold = 0.5; // threshold of step function
}

// Print architecture
void print_architecture()
{
	int i, output_layer = num_hidden_layers + 1;
	printf("Neural network architecture:\n");
	printf(" Number of hidden layers: %d\n", num_hidden_layers);
	printf(" Number of inputs: %d\n", num_neuron[0]);

	for (i=0; i<num_hidden_layers; i++)
	{
		printf(" Hidden layer %d:\n", i+1);
		printf("  Neurons: %d\n", num_neuron[i+1]);
		printf("  Activation function: %s\n", AF_name[actv_function[i]]);
	}

	printf(" Output layer:\n");
	printf("  Neurons: %d\n", num_neuron[output_layer]);
	printf("  Activation function: %s\n", AF_name[actv_function[output_layer-1]]);
}

// Start weights
void initialize_weights()
{
	int layer, i, j;
	srand(time(NULL));

	// Weights
	for (layer = 0; layer < num_hidden_layers + 1; layer++)
	{
		for (i=0; i < num_neuron[layer+1]; i++)
		{
			for (j=0; j < num_neuron[layer]; j++)
				W[i][j][layer] = 1 - 2 * ((double) rand()) / ((double) RAND_MAX);
		}
	}

	// Bias
	for (layer = 0; layer < num_hidden_layers + 1; layer++)
	{
		for (i=0; i < num_neuron[layer+1]; i++)
			B[i][layer] = 1 - 2 * ((double) rand()) / ((double) RAND_MAX);
	}
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

		for (i=0; i < num_neuron[layer+1]; i++)
		{
			for (j=0; j < num_neuron[layer]; j++)
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

		for (i=0; i < num_neuron[layer+1]; i++)
			printf(" %.4f", B[i][layer]);

		printf("\n\n");
	}
}

// Define inputs
void load_inputs(int line_no, int train)
{
	int i;

	for (i=0; i<num_inputs; i++)
	{
		if (train == 1)
			E[i][0] = P[line_no][i];
		else
			E[i][0] = Ps[line_no][i];
	}
}

// Activation function
double calculate_AF(double x, int type)
{
	switch (type)
	{
		case 0 : return 1 * (x > threshold); // Step
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
		for (i=0; i < num_neuron[layer+1]; i++)
		{
			E[i][layer+1] = 0.0;
			for (j=0; j < num_neuron[layer]; j++)
				E[i][layer+1] += W[i][j][layer] * E[j][layer];
			E[i][layer+1] = calculate_AF(E[i][layer+1] + B[i][layer], actv_function[layer]);
		}
	}
}

// Calculate error at output layer
// Uses Mean Squared Error - MSE
double calculate_performance(int line_no)
{
	int i, output_layer = num_hidden_layers + 1;
	double erro = 0.0;

	for (i=0; i < num_outputs; i++)
	{
		Err[i] = T[line_no][i] - E[i][output_layer];
		erro += pow(Err[i], 2);

	}
	return erro / ((double) 2 * num_outputs);
}

// Activation function - F(X)' - derive
double calculate_AF_line(double x, int type)
{
	double f;

	switch (type)
	{
		case 0 : return 1; // Step
		case 1 : return 1; // Linear
		case 2 : f = calculate_AF(x,2); return f*(1-f); // Sigmoid
		default : f = calculate_AF(x,3); return 1 - pow(f, 2.0); // Tansig
	}
}

// Do backpropagation
void backpropagation()
{
	int layer, i, j;
	double erro;

	// Back propagation - from output to input
	// The array E has horizontal shift of +1 in relationship to W, B and activation_function
	for (layer = num_hidden_layers; layer >= 0; layer--)
	{
		// Step 1 - update outputs
		if (layer < num_hidden_layers)
		{
			// Update hidden layers
			for (i=0; i < num_neuron[layer+1]; i++)
			{
				erro = 0.0;
				for (j=0; j < num_neuron[layer+2]; j++)
					erro += W[j][i][layer+1] * E[j][layer+2];
				E[i][layer+1] = erro * calculate_AF_line(E[i][layer+1], actv_function[layer]);
			}
		}
		else
		{
			// Update output layer with errors plus F'(x)
			for (i=0; i < num_outputs; i++)
				E[i][layer+1] = Err[i] * calculate_AF_line(E[i][layer+1], actv_function[layer]);
		}

		// Step 2 - UPdate weights and biases
		for (i=0; i < num_neuron[layer+1]; i++)
		{
			for (j=0; j < num_neuron[layer]; j++)
				W[i][j][layer] += learning_rate * E[i][layer+1] * E[j][layer];

			B[i][layer] += learning_rate * E[i][layer+1];
		}
	}
}

// Training
void train()
{
	int epoch, i;
	double mean_error_batch, erro;

	// Define training parameters
	int num_epochs = 2000;
	int imprime_epoch = 100;
	double goal = 0.001;
	learning_rate = 0.03;
 
	for (epoch=1; epoch <= num_epochs; epoch++)
	{
		mean_error_batch = 0.0;

		for (i=0; i < num_samples_train; i++)
		{
			load_inputs(i,1);
			feed_forward();
			erro = calculate_performance(i);
			mean_error_batch += erro;

			if (erro >= goal)
				backpropagation();
		}

		// Calculate error for all the samples (batch)
		mean_error_batch /= num_samples_train;

		if (epoch % imprime_epoch == 0)
			printf("epoch %d/%d, MSE: %.2f\n", epoch, num_epochs, mean_error_batch);

		if (mean_error_batch <= goal)
		{
			printf("training converged.\n");
			return;
		}
	}
}

// Print results
void show()
{
	int i, output_layer = num_hidden_layers + 1;

	for (i=0; i < num_neuron[output_layer]; i++)
		printf("%.5f\n", E[i][output_layer]);
}

// Test
void test()
{
	int i;

	printf("\nTest:\n");

	for (i=0; i < num_samples_test; i++)
	{
		load_inputs(i,0);
		feed_forward();
		printf("Sample %d\n", i+1);
		show();
		printf("\n");
	}
}

main()
{
	define_architecture();
	initialize_weights();
	train();
	test();

	// Debug
//	print_architecture();
//	print_weights();
//	show();
}
