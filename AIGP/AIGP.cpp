// AIGP.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

/*
n = 6 holes
c = 2 colors
1 special seed (2 colors)
3 red / 3 black per hole
*/
#include "pch.h"
#include <iostream>
#define NUMBER_OF_CELLS 6
#define MAX_DEPTH 7
#define SEEDS_PER_HOLE 3
#define SPECIAL_SEED 1
#define WIN 100000
#define LOSE -100000
#define DRAW 0
#define INV_MOVE_CPT -100001
#define INV_MOVE_PLY 100001

struct Position {
	int cells_player_black[NUMBER_OF_CELLS];		// each cell contains a certain number of seeds
	int cells_computer_black[NUMBER_OF_CELLS];
	int cells_player_red[NUMBER_OF_CELLS];			// each cell contains a certain number of seeds
	int cells_computer_red[NUMBER_OF_CELLS];
	int special_seed;			// pos of the special seed (0->5 : computer // 6->11 : player)
	bool computer_play;			// boolean true if the computer has to play and false otherwise
	int seeds_player;			// seeds taken by the player
	int seeds_computer;			// seeds taken by the computer
};

class Tree {
public:
	Position* node;
	Tree** nodes;
	int depth;
	int alpha;
	int beta;

	//Constructors
	Tree(Position* pos) : node(pos), depth(0), alpha(1000), beta(1000), nodes() {}
	~Tree() { delete node; }

	bool leaf() { return nodes == nullptr; }
	Tree* getChild(int i) { return nodes[i]; }
};



//Function that determines if the position is a final position, which means it ends the game
bool finalPosition(Position* pos_current, bool computer_play, int depth) {
	int max_seeds = SEEDS_PER_HOLE * NUMBER_OF_CELLS * 2;
	bool all_seeds = pos_current->seeds_player + pos_current->seeds_computer == max_seeds * 2;
	if (all_seeds) return true;			//if all the seeds have been capturated, then it's an end of the game

	bool more_than_half = pos_current->seeds_player > max_seeds || pos_current->seeds_computer > max_seeds;
	if (more_than_half) return true;	//if a player has capturated more than half the seeds, then it's an end of the game

	//missing cases

	return false;
}

//Return the total amount of seeds in cell i (black+red)
int total_seeds(Position* pos, bool computer_side, int i) {
	if (computer_side) return pos->cells_computer_black[i] + pos->cells_computer_red[i];
	return pos->cells_player_black[i] + pos->cells_player_red[i];
}

//Return the amount of red seeds in cell i
int red_seeds(Position* pos, bool computer_side, int i) {
	if (computer_side) return pos->cells_computer_red[i];
	return pos->cells_player_red[i];
}

//Return the amount of black seeds in cell i
int black_seeds(Position* pos, bool computer_side, int i) {
	if (computer_side) return pos->cells_computer_black[i];
	return pos->cells_player_black[i];
}

//Add a red seed at cell i
void add_red(Position* pos, bool computer_side, int i) {
	if (computer_side) pos->cells_computer_red[i]++;
	else pos->cells_player_red[i]++;
}

//Add a black seed at cell i
void add_black(Position* pos, bool computer_side, int i) {
	if (computer_side) pos->cells_computer_black[i]++;
	else pos->cells_player_black[i]++;
}

//Remove the black seeds at cell i
void empty_black_cell(Position* pos, bool computer_side, int i) {
	if (computer_side) pos->cells_computer_black[i] = 0;
	else pos->cells_player_black[i] = 0;
}

//Remove the red seeds at cell i
void empty_red_cell(Position* pos, bool computer_side, int i) {
	if (computer_side) pos->cells_computer_red[i] = 0;
	else pos->cells_player_red[i] = 0;
}

//Empty the cell i
void empty_cell(Position* pos, bool computer_side, int i) {
	empty_black_cell(pos, computer_side, i);
	empty_red_cell(pos, computer_side, i);
}

//Evaluate a position
int evaluation(Position* pos_current, bool computer_play, int depth) {
	int points_difference = pos_current->seeds_computer - pos_current->seeds_player;
	int number_of_seeds_difference = 0;
	int number_of_playable_case_difference = 0;
	for (int i = 0; i < NUMBER_OF_CELLS; i++) {
		int seed_cpt = total_seeds(pos_current, true, i);
		int seed_ply = total_seeds(pos_current, false, i);
		number_of_seeds_difference += seed_cpt - seed_ply;
		if (seed_cpt > 0) number_of_playable_case_difference++;
		if (seed_ply > 0) number_of_playable_case_difference--;
	}
	return points_difference * 100 + 10 * number_of_playable_case_difference + number_of_seeds_difference;
}

int evaluation_old(Position* pos_current, bool computer_play, int depth) {
	int seeds_difference = pos_current->seeds_computer - pos_current->seeds_player;
	return seeds_difference;
}

//Return 1 if we can play the case i, 0 otherwise
bool validMove(Position* pos_current, bool computer_play, int i) {
	if (i < 0 || i >= NUMBER_OF_CELLS) return false;
	bool empty = total_seeds(pos_current, computer_play, i) <= 0;
	return !empty;
}

//Play the move on the hole i
void playMove(Position* pos_next, Position* pos_current, bool computer_play, int i, bool red_first) {
	//copy the arrays
	for (int j = 0; j < NUMBER_OF_CELLS; j++) {
		pos_next->cells_computer_black[j] = pos_current->cells_computer_black[j];
		pos_next->cells_computer_red[j] = pos_current->cells_computer_red[j];

		pos_next->cells_player_black[j] = pos_current->cells_player_black[j];
		pos_next->cells_player_red[j] = pos_current->cells_player_red[j];
	}
	pos_next->computer_play = !pos_current->computer_play;
	pos_next->seeds_computer = pos_current->seeds_computer;
	pos_next->seeds_player = pos_current->seeds_player;

	//initialize the number of seed and empty the cell being played
	int seeds_red = red_seeds(pos_next, computer_play, i);
	int seeds_black = black_seeds(pos_next, computer_play, i);
	int seeds = seeds_black + seeds_red;
	empty_cell(pos_next, computer_play, i);

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
				add_red(pos_next, computer_side, index);
			}
			else add_black(pos_next, computer_side, index);
		}
		if (!red_first) {
			if (seeds_black - seed > 0) {
				add_black(pos_next, computer_side, index);
			}
			else add_red(pos_next, computer_side, index);
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
		int red = red_seeds(pos_next, computer_side, ind);
		int black = black_seeds(pos_next, computer_side, ind);

		int number_of_seeds = 0;

		if (red_first) if (seeds - j == seeds_red) color = !color;
		if (!red_first) if (seeds - j == seeds_black) color = !color;

		//we look at the red seeds
		if (color && red >= 2 && red <= 3) {
			number_of_seeds = red;
			empty_red_cell(pos_next, computer_side, ind);
		}

		if (!color && black >= 2 && black <= 3) {
			number_of_seeds = black;
			empty_black_cell(pos_next, computer_side, ind);
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
		int seed = total_seeds(pos_next, !computer_play, i);
		//if there is at least one seed on opponent's side, then he can play
		if (seed > 0) no_seed = false;
	}
	if (no_seed) {
		for (int i = 0; i < NUMBER_OF_CELLS; i++) {
			if (computer_play) {
				pos_next->seeds_computer += total_seeds(pos_next, computer_play, i);
				empty_cell(pos_next, computer_play, i);
			}
			else {
				pos_next->seeds_player += total_seeds(pos_next, computer_play, i);
				empty_cell(pos_next, computer_play, i);
			}
		}
	}
}

int minMaxValue(Position* pos_current, int* next, bool* red_first, bool computer_play, int depth, int depthMax, bool old_eval) {
	// computer_play is true if the computer has to play and false otherwise
	int tab_values[NUMBER_OF_CELLS];
	int tab_values2[NUMBER_OF_CELLS];
	Position pos_next; // In C : created on the stack: = very fast
	Position pos_next2;
	if (finalPosition(pos_current, computer_play, depth)) {
		int difference = pos_current->seeds_computer - pos_current->seeds_player;
		if (difference == 0) return DRAW;
		if (difference > 0) return WIN;
		if (difference < 0) return LOSE;
		// WRITE the code: returns VALMAX (=96) if the computer wins, -96 if it loses; 0 if draw
	}
	if (depth == depthMax) {
		if(old_eval) return evaluation_old(pos_current, computer_play, depth);
		return evaluation(pos_current, computer_play, depth);
		// the simplest evealution fucntion is the difference of the taken seeds
	}
	for (int i = 0; i < NUMBER_OF_CELLS; i++) {
		// we play the move i
		// WRITE function validMove(pos_current, computer_play,i)
		// it checks whether we can select the seeds in cell i and play (if there is no seed the function returns false)
		if (validMove(pos_current, computer_play, i)) {
			// WRITE function playMove(&pos_next,pos_current, computer_play,i)
			// we play th emove i from pos_current and obtain the new position pos_next
			bool play_red = false;
			bool play_black = false;
			//if red first but there is no red, we don't explore the tree (perf gain)
			if ((red_first && red_seeds(pos_current, computer_play, i) == 0)) {
				if (computer_play) tab_values[i] = INV_MOVE_CPT;
				else tab_values[i] = INV_MOVE_PLY;
				play_red = true;
			} //if black first but there is no black, we don't explore the tree
			else if (!red_first && black_seeds(pos_current, computer_play, i) == 0) {
				if (computer_play) tab_values2[i] = INV_MOVE_CPT;
				else tab_values2[i] = INV_MOVE_PLY;
				play_black = false;
			}
			if (false) {}
			else {
				if (!play_red) {
					playMove(&pos_next, pos_current, computer_play, i, true);			//red first
					tab_values[i] = minMaxValue(&pos_next, next, red_first, !computer_play, depth + 1, depthMax, old_eval);
				}
				if (!play_black) {
					playMove(&pos_next2, pos_current, computer_play, i, false);			//black first
					tab_values2[i] = minMaxValue(&pos_next2, next, red_first, !computer_play, depth + 1, depthMax, old_eval);
				}
			}
		}
		else {
			if (computer_play) {
				tab_values[i] = INV_MOVE_CPT;
				tab_values2[i] = INV_MOVE_CPT;
			}
			else {
				tab_values[i] = INV_MOVE_PLY;
				tab_values2[i] = INV_MOVE_PLY;
			}
		}
	}
	int res = tab_values[0];
	*next = 0;
	*red_first = true;
	if (computer_play) {
		// WRITE the code: res contains the MAX of tab_values
		for (int i = 1; i < NUMBER_OF_CELLS; i++) {
			if (tab_values[i] >= res) { res = tab_values[i]; *next = i; *red_first = true; }
			if (tab_values2[i] >= res) { res = tab_values2[i]; *next = i; *red_first = false; }
		}
	}
	else {
		// WRITE the code: res contains the MIN of tab_values
		for (int i = 1; i < NUMBER_OF_CELLS; i++) {
			if (tab_values[i] <= res) { res = tab_values[i]; *next = i; *red_first = true; }
			if (tab_values2[i] <= res) { res = tab_values2[i]; *next = i; *red_first = false; }
		}
	}
	return res;
}

//Initialize a starting position
void init(Position* position, bool computer_start) {
	for (int i = 0; i < NUMBER_OF_CELLS; i++) {
		position->cells_computer_black[i] = SEEDS_PER_HOLE;
		position->cells_computer_red[i] = SEEDS_PER_HOLE;
		position->cells_player_black[i] = SEEDS_PER_HOLE;
		position->cells_player_red[i] = SEEDS_PER_HOLE;
	}
	position->computer_play = computer_start;
	position->seeds_computer = 0;
	position->seeds_player = 0;
}

//Print the state of the game at the given position
void print_position(Position* position) {
	std::cout << "(B R)" << std::endl;
	for (int i = 0; i < NUMBER_OF_CELLS; i++) {
		std::cout << "(" << position->cells_computer_black[NUMBER_OF_CELLS - i - 1] << " " << position->cells_computer_red[NUMBER_OF_CELLS - i - 1] << ") ";
	}
	std::cout << " COMPUTER\t(" << position->seeds_computer << ")" << std::endl;
	for (int i = 0; i < NUMBER_OF_CELLS; i++) {
		std::cout << "(" << position->cells_player_black[i] << " " << position->cells_player_red[i] << ") ";
	}
	std::cout << " PLAYER  \t(" << position->seeds_player << ")" << std::endl;
	std::cout << std::endl << std::endl;
}

int main() {
	Position position;
	Position next_position;
	//Computer starts to play ?
	bool computer_play = true;

	//Enable the printing of the computer's evaluation
	bool enable_evaluation = true;

	int next = 0;
	int value = 0;
	int cpt = 0;
	int color = 0;
	bool red_first = true;
	init(&position, computer_play);

	//While the game is not finished
	while (!finalPosition(&position, computer_play, 0)) {

		//Print the position
		print_position(&position);
		//Computer = evaluation ; Player = old_evaluation
		value = minMaxValue(&position, &next, &red_first, computer_play, 0, MAX_DEPTH, !computer_play);
		/*
		else {
			bool valid = false;
			while (!valid) {
				std::cout << "Please enter a cell to play : ";
				std::cin >> next;
				std::cout << "Play red first ? (0 : no) : ";
				std::cin >> color;
				if (color == 0) red_first = false;
				else red_first = true;
				valid = validMove(&position, computer_play, next);
				if (!valid) std::cout << "Invalid move" << std::endl;
			}
		}
		*/
		//Print the decision
		if (true) {
			if (computer_play) std::cout << "Computer plays case number " << NUMBER_OF_CELLS - 1 - next << ", ";
			else std::cout << "Player plays case number " << NUMBER_OF_CELLS - 1 - next << ", ";
			if (red_first) std::cout << "red first" << std::endl;
			else std::cout << "black first" << std::endl;
			if (enable_evaluation) {
				std::cout << "Evaluation : ";
				if (value > 0) {
					if (computer_play) std::cout << "WIN" << std::endl;
					else std::cout << "LOSE" << std::endl;
				}
				else if (value < 0) {
					if (computer_play) std::cout << "LOSE" << std::endl;
					else std::cout << "WIN" << std::endl;
				}
				else std::cout << "DRAW" << std::endl;
			}
		}

		std::cout << std::endl;
		playMove(&next_position, &position, computer_play, next, red_first);
		position = next_position;
		computer_play = !computer_play;

		cpt++;	//number of move
	}
	print_position(&position);
	std::cout << cpt << std::endl;
	system("PAUSE");
}
