/***************************************************************************
 *   Program velha3.c                                                      *
 *                                                                         *
 *   Copyright (C) 2021 by Marcelo Silveira - MarMSX                       *
 *   Project: AI course                                                    *
 *   Homepage: http://marmsx.msxall.com                                    *
 *   Contact: flamar98@hotmail.com                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct state_cell
{
	int state;
	double prob;
};

int board[3][3];
struct state_cell states[6000];
int game_moves, p_state, p_last, last_move, prev_move;
double alpha = 0.3;

// Limpa tabuleiro
void clear_board()
{
	int i,j;

	for (i=0; i<3; i++)
	{
		for (j=0; j<3; j++)
			board[i][j] = 0;
	}
}

// Imprime tabuleiro
void print_board()
{
	int i,j;
	char c;

	for (i=0; i<3; i++)
	{
		for (j=0; j<3; j++)
		{
			switch (board[i][j])
			{
				case 1 : c='X'; break;
				case 2 : c='O'; break;
				default : c=' ';
			}
			if (j<2) printf(" %c |",c); else printf(" %c\n",c);
		}
		if (i<2) printf("---+---+---\n");
	}
	printf("\n");
}

// Imprime todos os estados salvos
void print_states()
{
	int i;
	printf("Lista de estados armazenados:\n");

	for (i=0; i<p_state; i++)
		printf("%09d - %.2f\n", states[i].state, states[i].prob);
}

// Salva estados em um arquivo
void save_states(char filename[20])
{
	FILE *fp;
	int i;

	fp = fopen(filename, "wt");

	if (fp == 0)
	{
		printf("Erro ao salvar dados.\n");
		return;
	}

	for (i=0; i<p_state; i++)
		fprintf(fp, "%09d,%.4f\n", states[i].state, states[i].prob);

	fclose(fp);
}

// Carrega estados em um arquivo
void load_states(char filename[20])
{
	FILE *fp;
	char num[20];
	int flag=1;
	float prob;

	fp = fopen(filename, "r");

	if (fp == 0)
	{
		printf("Erro ao ler o arquivo.\n");
		return;
	}

	p_state=0;

//	while (fscanf(fp, "%9s,%f", num, &prob) != EOF)
	while (fscanf(fp, "%[^,],%f\n", num, &prob) != EOF)
	{
		states[p_state].state = atoi(num);
		states[p_state++].prob = prob;
	}

	fclose(fp);
}

// Procura por estado na memoria
int search_state(int s)
{
	int i, pure_s;

	for (i=0; i<p_state; i++)
	{
		if (states[i].state == s)
			return i;
	}

	return -1;
}

// Converte de tabuleiro para código numérico
int get_state()
{
	int i, j, code=0;

	for (i=0; i<3; i++)
	{
		for (j=0; j<3; j++)
			code = code*10 + board[i][j];
	}

	return code;
}

// Recompensa
void set_reward(int status)
{
	if (status == 2)
	{
		states[p_last].prob = 1.0;
		states[prev_move].prob += alpha * (1.0 - states[prev_move].prob);
	}
	else
		states[p_last].prob += alpha * (0.0 - states[p_last].prob);
}

// Movimento do jogador
void player_move()
{
	char player_move[3];
	int flag=1, x, y;

	while (flag)
	{
		printf("Qual o proximo movimento (x,y)? ");
		scanf("%s", player_move);

		x = atof(&player_move[0]) - 1;
		y = atof(&player_move[2]) - 1;

		if (x>=0 && x<=2 && y>=0 && y<=2)
		{
			if (board[x][y] == 0)
				break;
		}
	}

	board[x][y] = 1;
}

// Verifica estado. Se novo, adiciona. Retorna ponteiro
int check_new_state(int s)
{
	int p = search_state(s);

	if (p < 0)
	{
		p = p_state;
		states[p_state].state = s;
		states[p_state++].prob = 0.5;
	}

	return p;
}

// Procura por melhor estado futuro - ganancioso
int get_best_state(int s)
{
	int p, best_s_pos, best_s, i;
	double best_prob = -1.0, prob;
	char num[9];

	sprintf(num, "%09d", s);

	for (i=0; i<9; i++)
	{
		// Procura estado futuro valido
		if (num[i] == '0')
		{
			num[i] = '2';
			p = search_state(atoi(num));

			if (p < 0)
				prob = 0.5;
			else
				prob = states[p].prob;

			if (prob > best_prob)
			{
				best_prob = prob;
				best_s = atoi(num);
				last_move = i;
			}
			num[i] = '0';
		}
	}

	best_s_pos = check_new_state(best_s);

	return best_s_pos;
}

// Movimento exploratório
int exploratory_move(int s)
{
	char num[9];
	int i, flag=1, p, new_s;

	srand(time(NULL));
	sprintf(num, "%09d", s);

	i = rand() % 9;

	while (flag)
	{
		if (num[i] == '0')
		{
			num[i] = '2';
			new_s = atoi(num);
			last_move = i;
			break;
		}

		i = (i + 1) % 9;
	}

	return check_new_state(new_s);
}

// Movimento do computador
void computer_move()
{
	int current_state = get_state(), p, k;

	srand(time(NULL));
	k = rand() % 100;

	if (k > 30)
	{
		p = get_best_state(current_state);

		// Notifica movimento
		if (p_last > 0)
			states[p_last].prob += alpha * (states[p].prob - states[p_last].prob);
	}
	else
		p = exploratory_move(current_state);	

	prev_move = p_last;
	p_last = p;

	board[last_move/3][last_move%3] = 2;
}

// Mensagem inicial
void print_intro()
{
	printf("Jogo da velha\n-------------\n\n");
	printf("Curso de Inteligencia Artificial - MarMSX\n\n");
	printf("O computador aprende conforme joga contra voce.\n");
	printf("Digite a coordenada do tabuleiro que deseja colocar o X, variando de 1 a 3. Ex: 1,1\n\n");
}

// Verifica jogo
// 0 - jogo inacabado, 1- vitoria do X, 2- vitoria do y, 3- empate
int check_game()
{
	int i;

	// Verifica linhas
	for (i=0; i<3; i++)
	{
		if (board[i][0] == 1 && board[i][1] == 1 && board[i][2] == 1)
			return 1;
		if (board[i][0] == 2 && board[i][1] == 2 && board[i][2] == 2)
			return 2;
	}

	// Verifica colunas
	for (i=0; i<3; i++)
	{
		if (board[0][i] == 1 && board[1][i] == 1 && board[2][i] == 1)
			return 1;
		if (board[0][i] == 2 && board[1][i] == 2 && board[2][i] == 2)
			return 2;
	}

	// Verifica diagonais
	if (board[0][0] == 1 && board[1][1] == 1 && board[2][2] == 1)
		return 1;
	if (board[0][0] == 2 && board[1][1] == 2 && board[2][2] == 2)
		return 2;
	if (board[2][0] == 1 && board[1][1] == 1 && board[0][2] == 1)
		return 1;
	if (board[2][0] == 2 && board[1][1] == 2 && board[0][2] == 2)
		return 2;

	// Verifica ultima jogada
	if (game_moves > 8)
		return 3;

	return 0;
}

// Controla jogo
void start_game()
{
	int flag=1, check;

	clear_board();
	game_moves = 0;
	p_last = -1;

	while (flag)
	{
		// Jogador
		print_board();
		player_move();
		game_moves++;
		check = check_game();
		if (check)
			break;

		// Computador
		computer_move();
		game_moves++;
		check = check_game();
		if (check)
			break;
	}

	print_board();

	// Aprende
	set_reward(check);

	// Escreve resultado
	switch (check)
	{
		case 1: printf("Voce venceu !!\n\n"); break;
		case 2: printf("Eu venci !!\n\n"); break;
		default: printf("Empate.\n\n");
	}
}

// Carregar ou salvar estados - 1-carregar, 2-salvar
void user_io_states(int operation)
{
	char resp[5], filename[20];

	if (operation == 2)
		printf("Salvar estados do jogo (s/n)? ");
	else
		printf("Carregar estados do jogo (s/n)? ");
	scanf("%s", resp);

	if (resp[0] != 's' && resp[0] != 'S')
		return;

	printf("Nome do arquivo de dados: ");
	scanf("%s", filename);

	if (operation == 2)
		save_states(filename);
	else
		load_states(filename);
}

main()
{
	char resp[5];
	int flag=1;
	p_state = 0;
	print_intro();

	// Carrega estados?
	user_io_states(1);

	while (flag)
	{
		start_game();

		printf("Deseja jogar novamente (s/n)? ");
		scanf("%s", resp);

		if (resp[0] != 's' && resp[0] != 'S')
			break;
	}

	print_states();

	// Salva estados?
	user_io_states(2);
}
