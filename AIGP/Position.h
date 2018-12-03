#pragma once
#include <iostream>
#include "pch.h"

class Position {
public:
	int cells_player_black[NUMBER_OF_CELLS];		// each cell contains a certain number of seeds
	int cells_computer_black[NUMBER_OF_CELLS];
	int cells_player_red[NUMBER_OF_CELLS];			// each cell contains a certain number of seeds
	int cells_computer_red[NUMBER_OF_CELLS];
	int special_seed;			// pos of the special seed (0->5 : computer // 6->11 : player)
	bool computer_play;			// boolean true if the computer has to play and false otherwise
	int seeds_player;			// seeds taken by the player
	int seeds_computer;			// seeds taken by the computer

	//Return the total amount of seeds in cell i
	int total_seeds(bool computer_side, int i) {
		if (computer_side) return cells_computer_black[i] + cells_computer_red[i];
		return cells_player_black[i] + cells_player_red[i];
	}
	
	//Return the amount of red seeds in cell i
	int red_seeds(bool computer_side, int i) {
		if (computer_side) return cells_computer_red[i];
		return cells_player_red[i];
	}

	//Return the amount of black seeds in cell i
	int black_seeds(bool computer_side, int i) {
		if (computer_side) return cells_computer_black[i];
		return cells_player_black[i];
	}

	//Add a red seed at cell i
	void add_red(bool computer_side, int i) {
		if (computer_side) cells_computer_red[i]++;
		else cells_player_red[i]++;
	}

	//Add a black seed at cell i
	void add_black(bool computer_side, int i) {
		if (computer_side) cells_computer_black[i]++;
		else cells_player_black[i]++;
	}

	//Remove the black seeds at cell i
	void empty_black_cell(bool computer_side, int i) {
		if (computer_side) cells_computer_black[i] = 0;
		else cells_player_black[i] = 0;
	}

	//Remove the red seeds at cell i
	void empty_red_cell(bool computer_side, int i) {
		if (computer_side) cells_computer_red[i] = 0;
		else cells_player_red[i] = 0;
	}

	//Empty the cell i
	void empty_cell(bool computer_side, int i) {
		empty_black_cell(computer_side, i);
		empty_red_cell(computer_side, i);
	}

	//Is the position final ?
	bool isFinal() {
		int max_seeds = SEEDS_PER_HOLE * NUMBER_OF_CELLS * 2;
		bool all_seeds = seeds_player + seeds_computer == max_seeds * 2;
		if (all_seeds) return true;			//if all the seeds have been capturated, then it's an end of the game

		bool more_than_half = seeds_player > max_seeds || seeds_computer > max_seeds;
		if (more_than_half) return true;	//if a player has capturated more than half the seeds, then it's an end of the game

		return false;
	}
	
	//Is the move i valid ?
	bool validMove(int i, bool color) {
		if (i < 0 || i >= NUMBER_OF_CELLS) return false;
		if (color) return red_seeds(computer_play, i) > 0;
		else return black_seeds(computer_play, i) > 0;
	}
	
	//Evaluate the position
	int evaluate() {
		int points_difference = seeds_computer - seeds_player;
		int number_of_seeds_difference = 0;
		int number_of_playable_case_difference = 0;
		int number_of_unplayable_cases = 0;
		for (int i = 0; i < NUMBER_OF_CELLS; i++) {
			int seed_cpt = total_seeds(true, i);
			int seed_ply = total_seeds(false, i);
			number_of_seeds_difference += seed_cpt - seed_ply;
			//difference between the number of playable cases for each side
			if (seed_cpt > 0) number_of_playable_case_difference++;
			if (seed_ply > 0) number_of_playable_case_difference--;

			if (seed_ply == 0) number_of_unplayable_cases++;
		}
		return points_difference * 1000 + 100 * number_of_playable_case_difference + 10 * number_of_unplayable_cases + number_of_seeds_difference;
	}
	
	//Evaluate the position (OLD EVALUATION FUNCTION)
	int evaluate_OLD() {
		int points_difference = seeds_computer - seeds_player;
		int number_of_seeds_difference = 0;
		int number_of_playable_case_difference = 0;
		for (int i = 0; i < NUMBER_OF_CELLS; i++) {
			int seed_cpt = total_seeds(true, i);
			int seed_ply = total_seeds(false, i);
			number_of_seeds_difference += seed_cpt - seed_ply;
			if (seed_cpt > 0) number_of_playable_case_difference++;
			if (seed_ply > 0) number_of_playable_case_difference--;
		}
		return points_difference;
	}

	void init(bool computer_start) {
		for (int i = 0; i < NUMBER_OF_CELLS; i++) {
			cells_computer_black[i] = SEEDS_PER_HOLE;
			cells_computer_red[i] = SEEDS_PER_HOLE;
			cells_player_black[i] = SEEDS_PER_HOLE;
			cells_player_red[i] = SEEDS_PER_HOLE;
		}
		computer_play = computer_start;
		seeds_player = 0;
		seeds_computer = 0;
	}

	void init(Position* pos) {
		for (int i = 0; i < NUMBER_OF_CELLS; i++) {
			cells_computer_black[i] = pos->cells_computer_black[i];
			cells_computer_red[i] = pos->cells_computer_red[i];
			cells_player_black[i] = pos->cells_player_black[i];
			cells_player_red[i] = pos->cells_player_red[i];
		}
		computer_play = pos->computer_play;
		seeds_player = pos->seeds_player;
		seeds_computer = pos->seeds_computer;
	}

	void print() {
		std::cout << "(B R)" << std::endl;
		for (int i = 0; i < NUMBER_OF_CELLS; i++) {
			std::cout << "(" << cells_computer_black[NUMBER_OF_CELLS - i - 1] << " " << cells_computer_red[NUMBER_OF_CELLS - i - 1] << ") ";
		}
		std::cout << " COMPUTER\t(" << seeds_computer << ")" << std::endl;
		for (int i = 0; i < NUMBER_OF_CELLS; i++) {
			std::cout << "(" << cells_player_black[i] << " " << cells_player_red[i] << ") ";
		}
		std::cout << " PLAYER  \t(" << seeds_player << ")" << std::endl;
		std::cout << std::endl << std::endl;
	}

};