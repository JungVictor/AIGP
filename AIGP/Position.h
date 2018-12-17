#pragma once
#include <iostream>
#include "pch.h"

class Position {
public:
	int cells_red[TOTAL_CELLS];
	int cells_black[TOTAL_CELLS];
	int special_seed[TOTAL_CELLS];
	int special_number;
	bool computer_play;			// boolean true if the computer has to play and false otherwise
	int seeds_player;			// seeds taken by the player
	int seeds_computer;			// seeds taken by the computer

	//Return the total amount of seeds in cell i
	int total_seeds(int i) {
		return cells_black[i] + cells_red[i] + special_seed[i];
	}

	//Return the total amount of seeds on the board
	int total_seeds() {
		int seeds = 0;
		for (int i = 0; i < TOTAL_CELLS; i++) {
			seeds += total_seeds(i);
		}
		return seeds + seeds_computer + seeds_player;
	}
	
	//Return the amount of red seeds in cell i
	int red_seeds(int i) {
		return cells_red[i];
	}

	//Return the amount of black seeds in cell i
	int black_seeds(int i) {
		return cells_black[i];
	}

	int special_seeds(int i) {
		return special_seed[i];
	}

	//Add a red seed at cell i
	void add_red(int i) {
		cells_red[i]++;
	}

	//Add a black seed at cell i
	void add_black(int i) {
		cells_black[i]++;
	}

	//Add a special seed at cell i
	void add_special(int i) {
		special_seed[i]++;
	}

	//Remove the black seeds at cell i
	void empty_black_cell(int i) {
		cells_black[i] = 0;
	}

	//Remove the red seeds at cell i
	void empty_red_cell(int i) {
		cells_red[i] = 0;
	}

	//Remove the special seed at cell i
	void empty_special_seed(int i) {
		if (special_seed[i] > 0) special_number -= special_seed[i];
		special_seed[i] = 0;
	}

	//Empty the cell i
	void empty_cell(int i) {
		empty_black_cell(i);
		empty_red_cell(i);
		empty_special_seed(i);
	}

	//Is the position final ?
	bool isFinal() {
		int max_seeds = SEEDS_PER_HOLE * TOTAL_CELLS + 1;
		bool all_seeds = seeds_player + seeds_computer == max_seeds * 2;
		if (all_seeds) return true;			//if all the seeds have been capturated, then it's an end of the game

		bool more_than_half = seeds_player > max_seeds || seeds_computer > max_seeds;
		if (more_than_half) return true;	//if a player has capturated more than half the seeds, then it's an end of the game

		bool starving = true;
		if (computer_play) {
			for (int j = 0; j < NUMBER_OF_CELLS; j++) if (total_seeds(j) > 0) starving = false;
		}
		else {
			for (int j = NUMBER_OF_CELLS; j < TOTAL_CELLS; j++) if (total_seeds(j) > 0) starving = false;
		}

		return starving;
	}
	
	//Is the move i valid ?
	bool validMove(int i, bool color, int special_pos) {
		if (i < 0 || i >= TOTAL_CELLS) {
			//std::cout << "Invalid move : can't play this cell" << std::endl;
			return false;
		}

		//if we try to play a special seed and there is none, then not valid (-1 is no play)
		if (special_pos > 0 && special_seed[i] <= 0) {
			//std::cout << "Invalid move : there is no special seed while we try to play one" << std::endl;
			return false;
		}

		//if we don't play a special seed while there is one, then it's not valid
		if (special_pos < 0 && special_seed[i] > 0) {
			//std::cout << "Invalid move : there is a special seed while we try not to play one" << std::endl;
			return false;
		}

		//if there is not enough seeds to feed the player
		//C�t� ordi : i + total_seeds(i) >= NUMBER_OF_CELLS
		//C�t� joueur : (i - NUMBER_OF_CELLS) + total_seeds(i) >= NUMBER_OF_CELLS

		// i % NUMBER_OF_CELLS + total_seeds(i) >= NUMBER_OF_CELLS
		if (i % NUMBER_OF_CELLS + total_seeds(i) < NUMBER_OF_CELLS) {
			bool starving = true;
			if (i < NUMBER_OF_CELLS) {
				for (int j = NUMBER_OF_CELLS; j < TOTAL_CELLS; j++) if (total_seeds(j) > 0) starving = false;
			}
			else {
				for (int j = 0; j < NUMBER_OF_CELLS; j++) if (total_seeds(j) > 0) starving = false;
			}
			if (starving) {
				//std::cout << "Invalid move : starving is not allowed" << std::endl;
				return false;
			}
		}

		if (color) return red_seeds(i) > 0;
		else return black_seeds(i) > 0;
	}
	
	//Evaluate the position
	int evaluate() {
		int points_difference = seeds_computer - seeds_player;
		int number_of_seeds_difference = 0;
		int number_of_playable_case_difference = 0;
		int number_of_unplayable_cases = 0;
		for (int i = 0; i < NUMBER_OF_CELLS; i++) {
			int seed_cpt = total_seeds(i);
			int seed_ply = total_seeds(i + NUMBER_OF_CELLS);
			number_of_seeds_difference += seed_cpt - seed_ply;
			//difference between the number of playable cases for each side
			if (seed_cpt > 0) number_of_playable_case_difference++;
			if (seed_ply > 0) number_of_playable_case_difference--;

		}
		//number_of_seeds_difference = 0;
		return points_difference * 1000 + 100 * number_of_playable_case_difference + 200 * number_of_seeds_difference;
	}
	
	//Evaluate the position (OLD EVALUATION FUNCTION)
	int evaluate_OLD() {
		int points_difference = seeds_computer - seeds_player;
		int number_of_seeds_difference = 0;
		int number_of_playable_case_difference = 0;
		int number_of_unplayable_cases = 0;
		for (int i = 0; i < NUMBER_OF_CELLS; i++) {
			int seed_cpt = total_seeds(i);
			int seed_ply = total_seeds(i + NUMBER_OF_CELLS);
			number_of_seeds_difference += seed_cpt - seed_ply;
			//difference between the number of playable cases for each side
			if (seed_cpt > 0) number_of_playable_case_difference++;
			if (seed_ply > 0) number_of_playable_case_difference--;

		}
		//number_of_seeds_difference = 0;
		return points_difference * 1000 + 100 * number_of_playable_case_difference + 100 * number_of_seeds_difference;
	}

	int evaluate_BASE() {
		return seeds_computer - seeds_player;
	}

	void init(bool computer_start) {
		for (int i = 0; i < TOTAL_CELLS; i++) {
			cells_black[i] = SEEDS_PER_HOLE;
			cells_red[i] = SEEDS_PER_HOLE;
			special_seed[i] = 0;
		}

		special_number = 2;
		computer_play = computer_start;
		seeds_player = 0;
		seeds_computer = 0;
	}

	void init(Position* pos) {
		for (int i = 0; i < TOTAL_CELLS; i++) {
			cells_black[i] = pos->cells_black[i];
			cells_red[i] = pos->cells_red[i];
			special_seed[i] = pos->special_seed[i];
		}
		special_number = pos->special_number;
		computer_play = pos->computer_play;
		seeds_player = pos->seeds_player;
		seeds_computer = pos->seeds_computer;
	}

	void print() {
		std::cout << "(B S R) " << total_seeds() << std::endl;
		std::cout << "1  2  3  4 5 6 " << std::endl;
		std::cout << "12 11 10 9 8 7" << std::endl;
		if (COMPUTER_START) {
			for (int i = 0; i < NUMBER_OF_CELLS; i++) {
				std::cout << "(" << cells_black[i] << " " << special_seed[i] << " " << cells_red[i] << ") ";
			}
			std::cout << " COMPUTER\t(" << seeds_computer << ")" << std::endl;
			for (int i = 0; i < NUMBER_OF_CELLS; i++) {
				std::cout << "(" << cells_black[TOTAL_CELLS - i - 1] << " " << special_seed[TOTAL_CELLS - i - 1] << " " << cells_red[TOTAL_CELLS - i - 1] << ") ";
			}
			std::cout << " PLAYER  \t(" << seeds_player << ")" << std::endl;
		}
		else {
			for (int i = 0; i < NUMBER_OF_CELLS; i++) {
				std::cout << "(" << cells_black[NUMBER_OF_CELLS + i] << " " << special_seed[NUMBER_OF_CELLS + i] << " " << cells_red[NUMBER_OF_CELLS + i] << ") ";
			}
			std::cout << " PLAYER  \t(" << seeds_player << ")" << std::endl;
			for (int i = 0; i < NUMBER_OF_CELLS; i++) {
				std::cout << "(" << cells_black[NUMBER_OF_CELLS - i - 1] << " " << special_seed[NUMBER_OF_CELLS - i - 1] << " " << cells_red[NUMBER_OF_CELLS - i - 1] << ") ";
			}
			std::cout << " COMPUTER\t(" << seeds_computer << ")" << std::endl;
		}
		std::cout << std::endl << std::endl;
	}

};