/***************************************************************************
 *   Program ttt3_auto_play.c                                              *
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
int game_moves, p_state, p_last[2], last_move[2], prev_move[2];
double alpha = 0.3;

void clear_board()
{
	int i,j;

	for (i=0; i<3; i++)
	{
		for (j=0; j<3; j++)
			board[i][j] = 0;
	}
}

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

void print_states()
{
	int i;
	printf("Saved states list:\n");

	for (i=0; i<p_state; i++)
		printf("%09d - %.2f\n", states[i].state, states[i].prob);
}

void save_states(char filename[20])
{
	FILE *fp;
	int i;

	fp = fopen(filename, "wt");

	if (fp == 0)
	{
		printf("Error while saving data.\n");
		return;
	}

	for (i=0; i<p_state; i++)
		fprintf(fp, "%09d,%.4f\n", states[i].state, states[i].prob);

	fclose(fp);
}

void load_states(char filename[20])
{
	FILE *fp;
	char num[20];
	int flag=1;
	float prob;

	fp = fopen(filename, "r");

	if (fp == 0)
	{
		printf("Failed to read file.\n");
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

// Convert from board to numeric code
int get_state(int cpu_id)
{
	int i, j, code=0, val;

	for (i=0; i<3; i++)
	{
		for (j=0; j<3; j++)
		{
			val = board[i][j];
			if (cpu_id == 1 && val > 0)
				val = (val % 2) + 1;
			code = code*10 + val;
		}
	}

	return code;
}

void set_reward(int result)
{
	if (result == 1)
	{
		states[p_last[0]].prob = 1.0;
		states[prev_move[0]].prob += alpha * (1.0 - states[prev_move[0]].prob);
		states[p_last[1]].prob += alpha * (0.0 - states[p_last[1]].prob);
	}

	if (result == 2)
	{
		states[p_last[1]].prob = 1.0;
		states[prev_move[1]].prob += alpha * (1.0 - states[prev_move[1]].prob);
		states[p_last[0]].prob += alpha * (0.0 - states[p_last[0]].prob);
	}

	if (result == 3)
	{
		states[p_last[0]].prob += alpha * (0.0 - states[p_last[0]].prob);
		states[p_last[1]].prob += alpha * (0.0 - states[p_last[1]].prob);
	}
}

// Check new state. If new, add. Return pointer
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

// Search for best future state - greed
int get_best_state(int s, int cpu_id)
{
	int p, best_s_pos, best_s, i;
	double best_prob = -1.0, prob;
	char num[9];
	int candidate_s[9], candidate_k[9], p_k;

	sprintf(num, "%09d", s);

	for (i=0; i<9; i++)
	{
		// Search for a valid future state
		if (num[i] == '0')
		{
			num[i] = '2';
			p = search_state(atoi(num));

			if (p < 0)
				prob = 0.5;
			else
				prob = states[p].prob;

			if (prob >= best_prob)
			{
				if (prob > best_prob)
					p_k=0;
				best_prob = prob;
				candidate_s[p_k] = atoi(num);
				candidate_k[p_k++] = i;
			}
			num[i] = '0';
		}
	}

	// Check for a valid way
	i=0;
	if (p_k > 1)
		i = rand() % p_k;

	// Do move
	last_move[cpu_id -1] = candidate_k[i];
	best_s_pos = check_new_state(candidate_s[i]);

	return best_s_pos;
}

// Exploratory move
int exploratory_move(int s, int cpu_id)
{
	char num[9];
	int i, flag=1, p, new_s;

	sprintf(num, "%09d", s);

	i = rand() % 9;

	while (flag)
	{
		if (num[i] == '0')
		{
			num[i] = '2';
			new_s = atoi(num);
			last_move[cpu_id -1] = i;
			break;
		}

		i = (i + 1) % 9;
	}

	return check_new_state(new_s);
}

void computer_move(int cpu_id)
{
	int current_state = get_state(cpu_id), p, k;

	k = rand() % 100;

	if (k > 10)
	{
		p = get_best_state(current_state, cpu_id);

		// Notifica movimento
		if (p_last[cpu_id -1] > 0)
			states[p_last[cpu_id -1]].prob += alpha * (states[p].prob - states[p_last[cpu_id -1]].prob);
	}
	else
		p = exploratory_move(current_state, cpu_id);	

	prev_move[cpu_id -1] = p_last[cpu_id -1];
	p_last[cpu_id -1] = p;

	board[last_move[cpu_id -1]/3][last_move[cpu_id -1]%3] = cpu_id;
}

void print_intro()
{
	printf("Tic Tac Toe\n-------------\n\n");
	printf("Artificial Intelligence Course - MarMSX\n\n");
	printf("The computer learns while as it plays with itself.\n");
}

// Check game
// 0 - unfinished game, 1- X wins, 2- Y wins, 3- drawn
int check_game()
{
	int i;

	// Check lines
	for (i=0; i<3; i++)
	{
		if (board[i][0] == 1 && board[i][1] == 1 && board[i][2] == 1)
			return 1;
		if (board[i][0] == 2 && board[i][1] == 2 && board[i][2] == 2)
			return 2;
	}

	// Check columns
	for (i=0; i<3; i++)
	{
		if (board[0][i] == 1 && board[1][i] == 1 && board[2][i] == 1)
			return 1;
		if (board[0][i] == 2 && board[1][i] == 2 && board[2][i] == 2)
			return 2;
	}

	// Check diagonals
	if (board[0][0] == 1 && board[1][1] == 1 && board[2][2] == 1)
		return 1;
	if (board[0][0] == 2 && board[1][1] == 2 && board[2][2] == 2)
		return 2;
	if (board[2][0] == 1 && board[1][1] == 1 && board[0][2] == 1)
		return 1;
	if (board[2][0] == 2 && board[1][1] == 2 && board[0][2] == 2)
		return 2;

	// Check last play
	if (game_moves > 8)
		return 3;

	return 0;
}

void start_game()
{
	int flag=1, check;

	clear_board();
	game_moves = 0;
	p_last[0] = -1;
	p_last[1] = -1;

	while (flag)
	{
		// Player
		computer_move(1);
		game_moves++;
		check = check_game();
		if (check)
			break;

		// Computer
		computer_move(2);
		game_moves++;
		check = check_game();
		if (check)
			break;
	}

	print_board();

	// Learn
	set_reward(check);

	switch (check)
	{
		case 1: printf("CPU 1 win !!\n\n"); break;
		case 2: printf("CPU 2 win !!\n\n"); break;
		default: printf("Drawn.\n\n");
	}
}

// Load or save states - 1-load, 2-save
void user_io_states(int operation)
{
	char resp[5], filename[20];

	if (operation == 2)
		printf("Save game states (y/n)? ");
	else
		printf("Load game states (y/n)? ");
	scanf("%s", resp);

	if (resp[0] != 'y' && resp[0] != 'Y')
		return;

	printf("Data file name: ");
	scanf("%s", filename);

	if (operation == 2)
		save_states(filename);
	else
		load_states(filename);
}

main()
{
	char resp[5];
	int flag=1, partidas, np;
	p_state = 0;
	print_intro();

	// Load states?
	user_io_states(1);

	printf("How many games the computer has to play against itself? ");
	scanf("%d", &partidas);

	srand(time(NULL));

	for (np=0; np<partidas; np++)
		start_game();

	print_states();

	// Save states?
	user_io_states(2);
}
