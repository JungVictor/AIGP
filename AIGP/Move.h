#pragma once
#include "pch.h"

class Move {
public:
	int hole;		//Number of the hole we're playing
	bool color;		//Color of the first seed
	int special;	//Position of the special seed
	int value;		//Evaluation of the move

	//Constructor
	Move() {
		init();
	}

	//Initialisation
	void init() {
		hole = -1;
		color = false;
		special = -1;
		value = -INF;
	}

	//Is the move the same as during the initialisation ?
	bool is_default() {
		return this->hole == -1 && this->color == false && this->special == -1;
	}

	//Is the move the same as the one we're passing
	bool isSame(int hole, bool color, int special) {
		return this->hole == hole && this->color == color && this->special == special;
	}

	//Is the move the same as the one we're passing
	bool isSame(Move* move) {
		return isSame(move->hole, move->color, move->special);
	}

	//Changing the hole we're playing
	void update_hole(int hole) {
		this->hole = hole;
	}

	//Changing the color we're playing
	void update_color(bool color) {
		this->color = color;
	}

	//Changin the index of the special seed we're playing
	void update_special(int special) {
		this->special = special;
	}

	//Update the value of the move
	void update_value(int value) {
		this->value = value;
	}

	//Update the move
	void update(int hole, bool color, int special, int value) {
		update_hole(hole);
		update_color(color);
		update_special(special);
		update_value(value);
	}

	//Update the move
	void update(Move* move) {
		update_hole(move->hole);
		update_color(move->color);
		update_special(move->special);
		update_value(move->value);
	}

	//Print the value's update
	void update_print(Move* move) {
		std::cout << "Updated last best position from " << value << " to " << move->value << std::endl;
		update(move);
	}
};