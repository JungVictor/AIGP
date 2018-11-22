// AIGP.cpp : Ce fichier contient la fonction 'main'. L'exécution du programme commence et se termine à cet endroit.
//

#include <iostream>
#define NUMBER_OF_CELLS 6
#define MAX_DEPTH 10
#define SEEDS_PER_HOLE 4
#define WIN 100
#define LOSE -100
#define DRAW 0

struct Position {
	int cells_player[NUMBER_OF_CELLS];		// each cell contains a certain number of seeds
	int cells_computer[NUMBER_OF_CELLS];
	bool computer_play;			// boolean true if the computer has to play and false otherwise
	int seeds_player;			// seeds taken by the player
	int seeds_computer;			// seeds taken by the computer
};

//Function that determines if the position is a final position, which means it ends the game
bool finalPosition(Position* pos_current, bool computer_play, int depth) {
	int max_seeds = SEEDS_PER_HOLE * NUMBER_OF_CELLS;
	bool all_seeds = pos_current->seeds_player + pos_current->seeds_computer == max_seeds * 2;
	if (all_seeds) return true;			//if all the seeds have been capturated, then it's an end of the game

	bool more_than_half = pos_current->seeds_player > max_seeds || pos_current->seeds_computer > max_seeds;
	if (more_than_half) return true;	//if a player has capturated more than half the seeds, then it's an end of the game

	//missing cases

	return false;
}

//Evaluate a position
int evaluation(Position* pos_current, bool computer_play, int depth) {
	return pos_current->seeds_computer - pos_current->seeds_player;
}

//Return 1 if we can play the case i, 0 otherwise
bool validMove(Position* pos_current, bool computer_play, int i) {
	if (i < 0 || i > NUMBER_OF_CELLS) return false;
	if (computer_play) return pos_current->cells_computer[i] > 0;
	return pos_current->cells_player[i] > 0;
}

//Play the move on the hole i
void playMove(Position* pos_next, Position* pos_current, bool computer_play, int i) {
	//copy the arrays
	for (int i = 0; i < NUMBER_OF_CELLS; i++) {
		pos_next->cells_computer[i] = pos_current->cells_computer[i];
		pos_next->cells_player[i] = pos_current->cells_player[i];
	}
	pos_next->computer_play = !pos_current->computer_play;
	pos_next->seeds_computer = pos_current->seeds_computer;
	pos_next->seeds_player = pos_current->seeds_player;
	int seeds;	//number of seeds to distribute

	//initialize the number of seed and empty the cell being played
	if (computer_play) {
		seeds = pos_current->cells_computer[i];
		pos_next->cells_computer[i] = 0;
	}
	else {
		seeds = pos_current->cells_player[i];
		pos_next->cells_player[i] = 0;
	}

	bool computer_side = computer_play;	//true if we add seeds in the computer side
	int index = i;
	for (int seed = 0; seed < seeds; seed++) {
		index = (index + 1) % NUMBER_OF_CELLS;
		if(computer_side == computer_play && index == i) index = (index + 1) % NUMBER_OF_CELLS;
		//change the side if we reach the end of one side
		if (index == 0) computer_side = !computer_side;
		if (computer_side) pos_next->cells_computer[index] += 1;
		else pos_next->cells_player[index] += 1;
	}

	//points
	int j = 0;
	bool earning_points = true;
	if (computer_play == computer_side) earning_points = false;
	//while we get seeds and we have not finished
	while (earning_points && j < seeds) {
		int ind = (index + NUMBER_OF_CELLS - j) % NUMBER_OF_CELLS;
		if (ind < 0 && j > 0) computer_side = !computer_side;

		int number_of_seeds = 0;
		//if we are one the computer side, we look at the computer's cells
		if (computer_side && pos_next->cells_computer[ind] >= 2 && pos_next->cells_computer[ind] <= 3) {
			number_of_seeds = pos_next->cells_computer[ind];
			pos_next->cells_computer[ind] = 0;
		}
		else if (!computer_side && pos_next->cells_player[ind] >= 2 && pos_next->cells_player[ind] <= 3) {
			number_of_seeds = pos_next->cells_player[ind];
			pos_next->cells_player[ind] = 0;
		}

		if (number_of_seeds == 0) earning_points = false;
		j++;

		//if computer is playing, then it earns the points
		if (computer_play) pos_next->seeds_computer += number_of_seeds;
		//else it's the player that earns the points
		else pos_next->seeds_player += number_of_seeds;
	}

	bool no_seed = true;	//opponent cannot play
	for (int i = 0; i < NUMBER_OF_CELLS; i++) {
		//if there is at least one seed on opponent's side, then he can play
		if (computer_play && pos_next->cells_player[i] > 0) no_seed = false;
		if(!computer_play &&  pos_next->cells_computer[i] > 0) no_seed = false;
	}
	if (no_seed) {
		for (int i = 0; i < NUMBER_OF_CELLS; i++) {
			if (computer_play) {
				pos_next->seeds_player += pos_next->cells_computer[i];
				pos_next->cells_computer[i] = 0;
			}
			else {
				pos_next->seeds_computer += pos_next->cells_player[i];
				pos_next->cells_player[i] = 0;
			}
		}
	}
}

int minMaxValue(Position* pos_current, int* next, bool computer_play, int depth, int depthMax) {
	// computer_play is true if the computer has to play and false otherwise
	int tab_values[NUMBER_OF_CELLS];
	Position pos_next; // In C : created on the stack: = very fast
	if (finalPosition(pos_current, computer_play, depth)) {
		int difference = pos_current->seeds_computer - pos_current->seeds_player;
		if (difference == 0) return DRAW;
		if (difference > 0) return WIN;
		if (difference < 0) return LOSE;
		// WRITE the code: returns VALMAX (=96) if the computer wins, -96 if it loses; 0 if draw
	}
	if (depth == depthMax) {
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
			playMove(&pos_next, pos_current, computer_play, i);
			// pos_next is the new current poisition and we change the player
			tab_values[i] = minMaxValue(&pos_next, next, !computer_play, depth + 1, depthMax);
		}
		else {
			if (computer_play) tab_values[i] = LOSE;
			else tab_values[i] = WIN;
		}
	}
	int res = tab_values[0];
	*next = 0; 
	if (computer_play) {
		// WRITE the code: res contains the MAX of tab_values
		for (int i = 1; i < NUMBER_OF_CELLS; i++) if (tab_values[i] >= res) { res = tab_values[i]; *next = i; }
	}
	else {
		// WRITE the code: res contains the MIN of tab_values
		for (int i = 1; i < NUMBER_OF_CELLS; i++) if (tab_values[i] <= res) { res = tab_values[i]; *next = i; }
	}
	return res;
}

//Initialize a starting position
void init(Position* position, bool computer_start) {
	for (int i = 0; i < NUMBER_OF_CELLS; i++) {
		position->cells_computer[i] = SEEDS_PER_HOLE;
		position->cells_player[i] = SEEDS_PER_HOLE;
	}
	position->computer_play = computer_start;
	position->seeds_computer = 0;
	position->seeds_player = 0;
}

//Print the state of the game at the given position
void print_position(Position* position) {
	for (int i = 0; i < NUMBER_OF_CELLS; i++) {
		std::cout << position->cells_computer[NUMBER_OF_CELLS-i-1] << " ";
	}
	std::cout << " COMPUTER\t(" << position->seeds_computer << ")" << std::endl;
	for (int i = 0; i < NUMBER_OF_CELLS; i++) {
		std::cout << position->cells_player[i] << " ";
	}
	std::cout << " PLAYER\t(" << position->seeds_player << ")" << std::endl;
	std::cout << std::endl << std::endl;
}

int main() {
	Position position;
	Position next_position;
	bool computer_play = false;
	int next = 0;
	int value = 0;
	int cpt = 0;
	init(&position, computer_play);

	//While the game is not finished
	while (!finalPosition(&position, computer_play, 0)) {

		//Print the position
		print_position(&position);

		if (computer_play) value = minMaxValue(&position, &next, computer_play, 0, MAX_DEPTH);
		else {
			bool valid = false;
			while (!valid) {
				std::cout << "Please enter a cell to play : ";
				std::cin >> next;
				valid = validMove(&position, computer_play, next);
				if (!valid) std::cout << "Invalid move" << std::endl;
			}
		}

		//Print the decision
		if (computer_play) {
			std::cout << "Computer plays case number " << NUMBER_OF_CELLS - 1 - next << std::endl;
			std::cout << "Computer says : ";
			if (value > 0) std::cout << "LOL I'M GONNA FUCK YOU" << std::endl;
			else if (value < 0) std::cout << "PLEASE BE GENTLE WITH ME DADDY" << std::endl;
			else std::cout << "STILL COMPUTING MY WIN" << std::endl;
		}
		std::cout << std::endl;
		playMove(&next_position, &position, computer_play, next);
		position = next_position;
		computer_play = !computer_play;

		cpt++;	//number of move
	}
	print_position(&position);
	std::cout << cpt << std::endl;
	system("PAUSE");

}