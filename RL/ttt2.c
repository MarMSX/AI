/***************************************************************************
 *   Program ttt2.c                                                        *
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

int board[3][3];
int states[6000];
int game_moves, p_state, p_last, last_move;

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
		printf("%09d\n", states[i]);
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
		fprintf(fp, "%09d\n", states[i]);

	fclose(fp);
}

void load_states(char filename[20])
{
	FILE *fp;
	char line[20];
	int flag=1;

	fp = fopen(filename, "r");

	if (fp == 0)
	{
		printf("Failed to read file.\n");
		return;
	}

	p_state=0;
	while (fscanf(fp, "%s", line) != EOF)
		states[p_state++] = atoi(line);

	fclose(fp);
}

int get_pure_state(int s)
{
	int i;
	char num[9];
	sprintf(num,"%0d",s);

	for (i=0; i<9; i++)
	{
		if (num[i] == '4')
			num[i] = '0';
	}

	return atof(num);
}

int search_state(int s)
{
	int i, pure_s;

	for (i=0; i<p_state; i++)
	{
		pure_s = get_pure_state(states[i]);

		if (pure_s == s)
			return i;
	}

	return -1;
}

// Convert from board to numeric code
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

void player_move()
{
	char player_move[3];
	int flag=1, x, y;

	while (flag)
	{
		printf("Next move (x,y)? ");
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

void set_reward()
{
	int n=1, k = 8-last_move, i;

	for (i=0; i<k; i++)
		n *= 10;

	states[p_last] += 4 * n;
}

void computer_move()
{
	int i, n, k;
	int current_state = get_state();

	p_last = search_state(current_state);

	if (p_last < 0)
	{
		p_last = p_state;
		states[p_state++] = current_state;
	}
	else
		current_state = states[p_last];

	for (i=0; i<9; i++)
	{
		n = current_state % 10;
		current_state = (int) (current_state / 10);
		k = 8-i;

		if (n==0)
		{
			board[k/3][k%3] = 2;
			last_move = k;
			break;
		}
	}
}

// Mensagem inicial
void print_intro()
{
	printf("Tic Tac Toe\n-------------\n\n");
	printf("Artificial Intelligence Course - MarMSX\n\n");
	printf("The computer learns as it plays against you.\n");
	printf("Type the board coordinates, ranging from 1 to 3. Eg: 1,1\n\n");
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

	// Learn with error
	if (check == 1)
		set_reward();

	switch (check)
	{
		case 1: printf("You win !!\n\n"); break;
		case 2: printf("I win !!\n\n"); break;
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
	int flag=1;
	p_state = 0;
	print_intro();

	// Load states?
	user_io_states(1);

	while (flag)
	{
		start_game();

		printf("Play again (y/n)? ");
		scanf("%s", resp);

		if (resp[0] != 'y' && resp[0] != 'Y')
			break;
	}

	print_states();

	// Save estates?
	user_io_states(2);
}
