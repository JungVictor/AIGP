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

int MAXIMUM_REACHED = 0;
std::clock_t start;

double time() {
	return (std::clock() - start) / (double)CLOCKS_PER_SEC;
}

//Play the move on the hole i
void playMove(Position* pos_next, Position* pos_current, bool computer_play, int i, bool red_first, int special_pos) {
	pos_next->init(pos_current);
	pos_next->computer_play = !pos_next->computer_play;

	//initialize the number of seed and empty the cell being played
	int seeds_red = pos_next->red_seeds(i);
	int seeds_black = pos_next->black_seeds(i);
	int seeds_special = pos_next->special_seeds(i);
	int seeds = seeds_black + seeds_red + seeds_special;
	pos_next->empty_cell(i);

	int special_seed1 = special_pos;
	int special_seed2 = -1;
	if (seeds_special == 2) special_seed2 = special_pos + 1;

	int last_seed_type = 0; //0 : red ; 1 : black ; 2 : special

	int index = i;
	int distributed = 0;
	int special_count = 0;
	for (int seed = 0; seed < seeds; seed++) {
		index++;
		if (index == i) index++;				//don't distribute seed on the starting hole
		if (index >= TOTAL_CELLS) index -= TOTAL_CELLS;
		//if we're distributing either red or black seeds
		if (special_count < seeds_special && (seed == special_seed1 || seed == special_seed2)) {
			pos_next->add_special(index);
			special_count++;
			last_seed_type = 2;
		}
		//if it's a special seed
		else {
			if (red_first) {
				if (seeds_red - distributed > 0) {
					pos_next->add_red(index);
					last_seed_type = 0;
				}
				else {
					pos_next->add_black(index);
					last_seed_type = 1;
				}
			}
			if (!red_first) {
				if (seeds_black - distributed > 0) {
					pos_next->add_black(index);
					last_seed_type = 1;
				}
				else {
					pos_next->add_red(index);
					last_seed_type = 0;
				}
			}
			distributed++;
		}
	}

	//points
	special_count = 0;
	int colored_seeds = 0;
	bool earning_points = true;
	if (computer_play && index < NUMBER_OF_CELLS) earning_points = false;
	else if (!computer_play && index >= NUMBER_OF_CELLS) earning_points = false;

	bool color = false;		//if the color has been fixed
	int ind = index;
	
	//while we get seeds and we have not finished
	while (earning_points) {
		if (ind < 0) ind += TOTAL_CELLS;
		int red = pos_next->red_seeds(ind) + pos_next->special_seeds(ind);
		int black = pos_next->black_seeds(ind) + pos_next->special_seeds(ind);

		int number_of_seeds = 0;

		//Special seed : both types
		if (last_seed_type == 2) {
			bool capture = false;
			if (red >= 2 && red <= 3) {
				number_of_seeds = red - pos_next->special_seeds(ind);
				pos_next->empty_red_cell(ind);
				if(!color) last_seed_type = 0;					//last seed is red
				capture = true;
			}
			//we look at the black seeds
			if (black >= 2 && black <= 3) {
				number_of_seeds += black - pos_next->special_seeds(ind);
				pos_next->empty_black_cell(ind);
				if (capture && !color) last_seed_type = 2;    //last seed is both
				else if(!color) last_seed_type = 1;			  //last seed is black
				capture = true;
			}
			if (capture) {
				number_of_seeds += pos_next->special_seeds(ind);
				pos_next->empty_special_seed(ind);
			}
			special_count++;
		}
		else {
			//we look at the red seeds
			if (last_seed_type == 0 && red >= 2 && red <= 3) {
				number_of_seeds = red;
				pos_next->empty_red_cell(ind);
				pos_next->empty_special_seed(ind);
			}
			//we look at the black seeds
			if (last_seed_type == 1 && black >= 2 && black <= 3) {
				number_of_seeds = black;
				pos_next->empty_black_cell(ind);
				pos_next->empty_special_seed(ind);
			}
			colored_seeds++;
		}
		ind--;
		color = true;

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

int minMaxValue(Position* pos_current, int alpha, int beta, int* next, bool* red_first, bool computer_play, int depth, int depthMax, bool old_eval, int* special_pos) {
	// computer_play is true if the computer has to play and false otherwise

	//Evaluation of a final position (win, lose or draw ?)
	if (pos_current->isFinal()) {
		int difference = pos_current->seeds_computer - pos_current->seeds_player;
		if (difference == 0) return DRAW;
		if (difference > 0) return WIN;
		if (difference < 0) return LOSE;
	}
	//Evaluation if a leaf
	if (depth >= depthMax) {
		if (depth > MAXIMUM_REACHED) MAXIMUM_REACHED = depth;
		if (old_eval) return pos_current->evaluate_OLD();
		return pos_current->evaluate();
	}
	//Evaluation if a tree
	Position pos_next;
	int value;
	if (computer_play) value = -INF;			//computer plays : take the max
	else value = INF;							//player plays : take the min

	int next_move = -1;
	bool next_color = false;
	int next_s1 = -1;
	int next_s2 = -1;

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
			int special_seeds_options = 0;
			int special_seed = -1;
			if (pos_current->special_seeds(i) > 0) {
				special_seeds_options += pos_current->total_seeds(i) - pos_current->special_seeds(i) + 1;
				special_seed = 0;
			}
			//every position for the special seeds
			for (special_seed; special_seed < special_seeds_options; special_seed++) {
				//We play the case i
				//If the move is valid, we play it and evaluate it
				if (pos_current->validMove(i, color, special_seed)) {
					playMove(&pos_next, pos_current, computer_play, i, color, special_seed);

					//Maraboutage of the depth...
					int options = 0;
					if (computer_play) {
						for (int opt = 0; opt < NUMBER_OF_CELLS; opt++) if (pos_current->total_seeds(opt) > 0) options++;
					}
					else {
						for (int opt = NUMBER_OF_CELLS; opt < TOTAL_CELLS; opt++) if (pos_current->total_seeds(opt) > 0) options++;
					}
					int new_depth = 0;
					//if (options < 4 && pos_current->special_number <= 0 && time() < 2 && depthMax < 25) new_depth++;
					if (special_seed > 0) new_depth = MAX_DEPTH_SPECIAL;
					else new_depth += depthMax;
					//Maraboutage finished

					int minmax = minMaxValue(&pos_next, alpha, beta, next, red_first, !computer_play, depth + 1, new_depth, old_eval, special_pos);

					if (computer_play && minmax == WIN) {
						*next = i;
						*red_first = color;
						*special_pos = special_seed;
						return WIN;
					}
					if (!computer_play && minmax == LOSE) {
						*next = i;
						*red_first = color;
						*special_pos = special_seed;
						return LOSE;
					}

					//ALPHA BETA CUT
					//if node : max
					if (computer_play) {
						if (value <= minmax) {
							value = minmax;
							next_move = i;
							next_color = color;
							next_s1 = special_seed;
						}
						//beta cut
						if (value >= beta) {
							*next = i;
							*red_first = color;
							*special_pos = special_seed;
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
							next_s1 = special_seed;
						}
						//alpha cut
						if (value <= alpha) {
							*next = i;
							*red_first = color;
							*special_pos = special_seed;
							return value;
						}
						if (value < beta) beta = value;
					}
					//end if evaluation
				}
				//end if valid
			}
			//end special seed 1
		}
		//end for color
	}
	//end for i
	*next = next_move;
	*red_first = next_color;
	*special_pos = next_s1;
	return value;
}

void exec() {
	double duration;
	double total_duration = 0;

	Position* position = new Position();
	Position* next_position = new Position();

	//Enable the printing of the computer's evaluation
	bool enable_evaluation = true;

	int next = 0;
	int value = 0;
	int special_pos = -1;
	char h_color;

	int cpt = 0;
	int color = 0;
	bool red_first = true;
	bool computer_play = COMPUTER_START;
	bool invalid = false;
	position->init(computer_play);

	for (int i = 0; i < 2; i++) {
		position->print();
		std::cout << "Place special seed ";
		if (computer_play) std::cout << "(computer) : ";
		else std::cout << "(player) : ";
		std::cin >> next;

		next = next - 1;
		//When AI plays but don't begin
		if (computer_play && !COMPUTER_START) next = next - NUMBER_OF_CELLS;
		//When Player plays but begin
		if (!computer_play && !COMPUTER_START) next = next + NUMBER_OF_CELLS;

		position->add_special(next);
		computer_play = !computer_play;
	}

	next = 0;

	//While the game is not finished
	while (!position->isFinal() && !invalid) {

		//Print the position
		position->print();
		//Computer = evaluation ; Player = old_evaluation
		start = std::clock();
		MAXIMUM_REACHED = 0;
		special_pos = -1;
		if(computer_play) value = minMaxValue(position, -INF, INF, &next, &red_first, computer_play, 0, MAX_DEPTH, !COMPUTER_START, &special_pos);
		else {
			//Human
			std::cout << "Play : ";
			std::cin >> next;
			next = next - 1;
			if (!COMPUTER_START) next = next + NUMBER_OF_CELLS;
			std::cout << "Color : ";
			std::cin >> h_color;
			red_first = (h_color == 'R' || h_color == 'r');
			if (red_first && position->red_seeds(next) <= 0) red_first = false;
			if (!red_first && position->black_seeds(next) <= 0) red_first = true;
			if (position->special_seeds(next) > 0) {
				std::cout << "Special seed position : ";
				std::cin >> special_pos;
				special_pos = special_pos - 1;
			}
			if (!position->validMove(next, red_first, special_pos)) {
				position->validMove_print(next, red_first, special_pos);
			}
			if(!COMPUTER_START) std::cout << "Player plays : " << next + 1 - NUMBER_OF_CELLS;
			else std::cout << "Player plays : " << next + 1;
			if (red_first) std::cout << "R";
			else std::cout << "B";
			if (special_pos >= 0) std::cout << special_pos + 1 << std::endl;
			else std::cout << std::endl;
		}
		duration = time();
		//std::cout << "Temps du tour : " << duration << " MAX DEPTH REACHED : " << MAXIMUM_REACHED << '\n';
		total_duration += duration;

		playMove(next_position, position, computer_play, next, red_first, special_pos);
		position = next_position;

		if (computer_play && !COMPUTER_START) next = NUMBER_OF_CELLS + next;
		//if (!computer_play && !COMPUTER_START) next = next - NUMBER_OF_CELLS;

		if (computer_play) {
			if (computer_play) std::cout << "Computer plays : " << next + 1;
			else std::cout << "Player plays : " << next + 1;
			if (red_first) std::cout << "R";
			else std::cout << "B";
			if (special_pos >= 0) std::cout << special_pos + 1 << std::endl;
			else std::cout << std::endl;
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
