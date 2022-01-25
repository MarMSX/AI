/***************************************************************************
 *   Program velha2_auto.c                                                 *
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

int board[3][3];
int states[6000];
int game_moves, p_state, p_last[2], last_move[2];

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
		printf("%09d\n", states[i]);
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
		fprintf(fp, "%09d\n", states[i]);

	fclose(fp);
}

// Carrega estados em um arquivo
void load_states(char filename[20])
{
	FILE *fp;
	char line[20];
	int flag=1;

	fp = fopen(filename, "r");

	if (fp == 0)
	{
		printf("Erro ao ler o arquivo.\n");
		return;
	}

	p_state=0;
	while (fscanf(fp, "%s", line) != EOF)
		states[p_state++] = atoi(line);

	fclose(fp);
}

// Obtem estado puro sem os flags 4
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

// Procura por estado na memoria
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

// Converte de tabuleiro para código numérico
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

// Em caso de derrota, retorna recompensa
void set_reward(int cpu_id)
{
	int n=1, k, i;

	k = 8-last_move[cpu_id-1];
		
	for (i=0; i<k; i++)
		n *= 10;

	states[p_last[cpu_id-1]] += 4 * n;
}

// Movimento do computador
void computer_move(int cpu_id)
{
	int i, n, k;
	int current_state = get_state(cpu_id);
	int candidate_k[9], p_k=0;

	p_last[cpu_id-1] = search_state(current_state);

	if (p_last[cpu_id-1] < 0)
	{
		p_last[cpu_id-1] = p_state;
		states[p_state++] = current_state;
	}
	else
		current_state = states[p_last[cpu_id-1]]; // Necessita dos flags

	for (i=0; i<9; i++)
	{
		n = current_state % 10;
		current_state = (int) (current_state / 10);
		k = 8-i;

		if (n==0)
			candidate_k[p_k++] = k;
	}

	// Sorteia um caminho valido
	i=0;
	if (p_k > 1)
		i = rand() % p_k;

	// Realiza movimento
	k = candidate_k[i];
	board[k/3][k%3] = cpu_id;
	last_move[cpu_id-1] = k;
}

// Mensagem inicial
void print_intro()
{
	printf("Jogo da velha\n-------------\n\n");
	printf("Curso de Inteligencia Artificial - MarMSX\n\n");
	printf("O computador aprende conforme joga contra ele mesmo.\n");
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

	while (flag)
	{
		// Jogador
		computer_move(1);
		game_moves++;
		check = check_game();
		if (check)
			break;

		// Computador
		computer_move(2);
		game_moves++;
		check = check_game();
		if (check)
			break;
	}

	print_board();

	// Escreve resultado e aprende
	switch (check)
	{
		case 1: printf("CpU 1 venceu !!\n\n"); set_reward(2); break;
		case 2: printf("CPU 2 venceu !!\n\n"); set_reward(1); break;
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
	int flag=1, partidas, np;
	p_state = 0;
	print_intro();

	// Carrega estados?
	user_io_states(1);

	printf("Quantas partidas o computador deve jogar contra ele mesmo? ");
	scanf("%d", &partidas);

	srand(time(NULL));

	for (np=0; np<partidas; np++)
		start_game();

	print_states();

	// Salva estados?
	user_io_states(2);
}
