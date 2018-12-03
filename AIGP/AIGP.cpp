// AIGP.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

/*
n = 6 holes
c = 2 colors
1 special seed (2 colors)
3 red / 3 black per hole
*/
#pragma once

#include "pch.h"
#include "Position.h"
#include <iostream>
#include <cstdio>
#include <ctime>

//Play the move on the hole i
void playMove(Position* pos_next, Position* pos_current, bool computer_play, int i, bool red_first) {
	pos_next->init(pos_current);
	pos_next->computer_play = !pos_next->computer_play;

	//initialize the number of seed and empty the cell being played
	int seeds_red = pos_next->red_seeds(computer_play, i);
	int seeds_black = pos_next->black_seeds(computer_play, i);
	int seeds = seeds_black + seeds_red;
	pos_next->empty_cell(computer_play, i);

	bool computer_side = computer_play;	//true if we add seeds in the computer side
	int index = i;
	for (int seed = 0; seed < seeds; seed++) {
		index = (index + 1) % NUMBER_OF_CELLS;
		//change the side if we reach the end of one side
		if (index == 0) computer_side = !computer_side;
		if (computer_side == computer_play && index == i) {
			index = (index + 1) % NUMBER_OF_CELLS;
			if (index == 0) computer_side = !computer_side;
		}
		if (red_first) {
			if (seeds_red - seed > 0) {
				pos_next->add_red(computer_side, index);
			}
			else pos_next->add_black(computer_side, index);
		}
		if (!red_first) {
			if (seeds_black - seed > 0) {
				pos_next->add_black(computer_side, index);
			}
			else pos_next->add_red(computer_side, index);
		}
	}

	//points
	int j = 0;
	bool earning_points = true;
	bool color = !red_first;	//false = black, true = red
	int ind = index;
	if (computer_play == computer_side) earning_points = false;
	//while we get seeds and we have not finished
	while (earning_points && j < seeds) {
		if (ind < 0) { ind = NUMBER_OF_CELLS - 1;  computer_side = !computer_side; }
		int red = pos_next->red_seeds(computer_side, ind);
		int black = pos_next->black_seeds(computer_side, ind);

		int number_of_seeds = 0;

		if (red_first) if (seeds - j == seeds_red) color = !color;
		if (!red_first) if (seeds - j == seeds_black) color = !color;

		//we look at the red seeds
		if (color && red >= 2 && red <= 3) {
			number_of_seeds = red;
			pos_next->empty_red_cell(computer_side, ind);
		}

		if (!color && black >= 2 && black <= 3) {
			number_of_seeds = black;
			pos_next->empty_black_cell(computer_side, ind);
		}

		if (number_of_seeds == 0) earning_points = false;
		j++;
		ind--;

		//if computer is playing, then it earns the points
		if (computer_play) pos_next->seeds_computer += number_of_seeds;
		//else it's the player that earns the points
		else pos_next->seeds_player += number_of_seeds;
	}

	bool no_seed = true;	//opponent cannot play
	for (int i = 0; i < NUMBER_OF_CELLS; i++) {
		int seed = pos_next->total_seeds(!computer_play, i);
		//if there is at least one seed on opponent's side, then he can play
		if (seed > 0) no_seed = false;
	}
	if (no_seed) {
		for (int i = 0; i < NUMBER_OF_CELLS; i++) {
			if (computer_play) {
				pos_next->seeds_computer += pos_next->total_seeds(computer_play, i);
				pos_next->empty_cell(computer_play, i);
			}
			else {
				pos_next->seeds_player += pos_next->total_seeds(computer_play, i);
				pos_next->empty_cell(computer_play, i);
			}
		}
	}
}

int minMaxValue(Position* pos_current, int alpha, int beta, int* next, bool* red_first, bool computer_play, int depth, int depthMax, bool old_eval) {
	// computer_play is true if the computer has to play and false otherwise

	//Evaluation of a final position (win, lose or draw ?)
	if (pos_current->isFinal()) {
		int difference = pos_current->seeds_computer - pos_current->seeds_player;
		if (difference == 0) return DRAW;
		if (difference > 0) return WIN;
		if (difference < 0) return LOSE;
	}
	//Evaluation if a leaf
	if (depth == depthMax) {
		if (old_eval) return pos_current->evaluate_OLD();
		return pos_current->evaluate();
	}
	//Evaluation if a tree
	Position pos_next; // In C : created on the stack: = very fast
	int value;
	if (computer_play) value = -INF;			//computer plays : take the max
	else value = INF;							//player plays : take the min

	int next_move = -1;
	bool next_color = false;

	for (int i = 0; i < NUMBER_OF_CELLS; i++) {
		//color : red => 0 // black => 1
		for (int c = 0; c < 2; c++) {
			bool color = c == 0;	//red = true // black = false;
			//We play the case i
			//If the move is valid, we play it and evaluate it
			if (pos_current->validMove(i, color)) {
				playMove(&pos_next, pos_current, computer_play, i, color);
				int minmax = minMaxValue(&pos_next, alpha, beta, next, red_first, !computer_play, depth + 1, depthMax, old_eval);

				//ALPHA BETA CUT
				//if node : max
				if (computer_play) {
					if (value <= minmax) {
						value = minmax;
						next_move = i;
						next_color = color;
					}
					//beta cut
					if (value >= beta) {
						*next = i;
						*red_first = color;
						return value;
					}
					if (value > alpha) alpha = value;
				}
				//if node : min
				else {
					if (value >= minmax) {
						value = minmax;
						next_move = i;
						next_color = color;
					}
					//alpha cut
					if (value <= alpha) {
						*next = i;
						*red_first = color;
						return value;
					}
					if (value < beta) beta = value;
				}
				//end if evaluation
			}
			//end if valid
		}
		//end for color
	}
	//end for i
	*next = next_move;
	*red_first = next_color;
	return value;
}

void exec() {
	Position* position = new Position();
	Position* next_position = new Position();

	//Enable the printing of the computer's evaluation
	bool enable_evaluation = true;

	int next = 0;
	int value = 0;
	int cpt = 0;
	int color = 0;
	bool red_first = true;
	bool computer_play = COMPUTER_START;
	position->init(computer_play);

	//While the game is not finished
	while (!position->isFinal()) {

		//Print the position
		position->print();
		//Computer = evaluation ; Player = old_evaluation
		value = minMaxValue(position, -INF, INF, &next, &red_first, computer_play, 0, MAX_DEPTH, !computer_play);

		if (true) {
			if (computer_play) std::cout << "Computer plays case number " << NUMBER_OF_CELLS - 1 - next << ", ";
			else std::cout << "Player plays case number " << NUMBER_OF_CELLS - 1 - next << ", ";
			if (red_first) std::cout << "red first" << std::endl;
			else std::cout << "black first" << std::endl;
			if (enable_evaluation) {
				std::cout << "Evaluation : " << value << std::endl;
				/*
				if (value > 0) {
					if (computer_play) std::cout << "WIN" << std::endl;
					else std::cout << "LOSE" << std::endl;
				}
				else if (value < 0) {
					if (computer_play) std::cout << "LOSE" << std::endl;
					else std::cout << "WIN" << std::endl;
				}
				else std::cout << "DRAW" << std::endl;
				*/
			}
		}

		std::cout << std::endl;
		playMove(next_position, position, computer_play, next, red_first);
		position = next_position;
		computer_play = !computer_play;

		cpt++;	//number of move
	}
	position->print();
	std::cout << cpt << std::endl;
}

int main() {
	std::clock_t start;
	double duration;

	start = std::clock();

	exec();

	duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;

	std::cout << "printf: " << duration << '\n';
	system("PAUSE");
}
