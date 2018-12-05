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
	int seeds_red = pos_next->red_seeds(i);
	int seeds_black = pos_next->black_seeds(i);
	int seeds = seeds_black + seeds_red;
	pos_next->empty_cell(i);
	int index = i;
	for (int seed = 0; seed < seeds; seed++) {
		index--;
		if (index == i) index--;				//don't distribute seed on the starting hole
		if (index < 0) index += TOTAL_CELLS;
		
		if (red_first) {
			if (seeds_red - seed > 0) pos_next->add_red(index);
			else pos_next->add_black(index);
		}
		if (!red_first) {
			if (seeds_black - seed > 0) pos_next->add_black(index);
			else pos_next->add_red(index);
		}
	}

	//points
	int j = 0;
	bool earning_points = true;
	if (computer_play && index < NUMBER_OF_CELLS) earning_points = false;
	else if (!computer_play && index >= NUMBER_OF_CELLS) earning_points = false;

	bool color = !red_first;	//false = black, true = red
	int ind = index;
	
	//while we get seeds and we have not finished
	while (earning_points && j < seeds) {
		if (ind >= TOTAL_CELLS) ind -= TOTAL_CELLS;
		int red = pos_next->red_seeds(ind);
		int black = pos_next->black_seeds(ind);

		int number_of_seeds = 0;

		if (red_first) if (seeds - j == seeds_red) color = !color;
		if (!red_first) if (seeds - j == seeds_black) color = !color;

		//we look at the red seeds
		if (color && red >= 2 && red <= 3) {
			number_of_seeds = red;
			pos_next->empty_red_cell(ind);
		}

		if (!color && black >= 2 && black <= 3) {
			number_of_seeds = black;
			pos_next->empty_black_cell(ind);
		}

		j++;
		ind++;

		//Updating conditions
		if (number_of_seeds == 0) earning_points = false;
		if (computer_play && ind < NUMBER_OF_CELLS) earning_points = false;
		else if (!computer_play && ind >= NUMBER_OF_CELLS) earning_points = false;

		//if computer is playing, then it earns the points
		if (computer_play) pos_next->seeds_computer += number_of_seeds;
		//else it's the player that earns the points
		else pos_next->seeds_player += number_of_seeds;
	}

	bool starving = true;	//opponent cannot play
	for (int i = 0; i < NUMBER_OF_CELLS; i++) {
		int seed;
		if (computer_play) seed = pos_next->total_seeds(i + NUMBER_OF_CELLS);
		else seed = pos_next->total_seeds(i);
		//if there is at least one seed on opponent's side, then he can play
		if (seed > 0) starving = false;
	}
	if (starving) {
		for (int i = 0; i < NUMBER_OF_CELLS; i++) {
			if (computer_play) {
				pos_next->seeds_computer += pos_next->total_seeds(i);
				pos_next->empty_cell(i);
			}
			else {
				pos_next->seeds_player += pos_next->total_seeds(i + NUMBER_OF_CELLS);
				pos_next->empty_cell(i + NUMBER_OF_CELLS);
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
	int i;
	int iteration;
	if (computer_play) {
		i = 0;
		iteration = NUMBER_OF_CELLS;
	}
	else {
		i = NUMBER_OF_CELLS;
		iteration = TOTAL_CELLS;
	}
	for (i; i < iteration; i++) {
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
	std::clock_t start;
	double duration;
	double total_duration = 0;

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
		start = std::clock();
		value = minMaxValue(position, -INF, INF, &next, &red_first, computer_play, 0, MAX_DEPTH, !computer_play);
		duration = (std::clock() - start) / (double)CLOCKS_PER_SEC;
		std::cout << "Temps du tour : " << duration << '\n';
		total_duration += duration;

		playMove(next_position, position, computer_play, next, red_first);
		position = next_position;

		if (computer_play && !COMPUTER_START) next = NUMBER_OF_CELLS + next;
		if (!computer_play && !COMPUTER_START) next = next - NUMBER_OF_CELLS;

		if (true) {
			if (computer_play) std::cout << "Computer plays case number " << next + 1 << ", ";
			else std::cout << "Player plays case number " << next + 1 << ", ";
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
		computer_play = !computer_play;

		cpt++;	//number of move
	}
	position->print();
	std::cout << "Nombre de tours : " << cpt << std::endl;
	std::cout << "Temps total : " << total_duration << std::endl;
}

int main() {
	exec();
	system("PAUSE");
}
