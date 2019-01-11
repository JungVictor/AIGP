#pragma once
#include "pch.h"

class Move {
public:
	int hole;
	bool color;
	int special;
	int value;

	Move() {
		init();
	}

	void init() {
		hole = -1;
		color = false;
		special = -1;
		value = -INF;
	}

	bool is_default() {
		return this->hole == -1 && this->color == false && this->special == -1;
	}

	bool isSame(int hole, bool color, int special) {
		return this->hole == hole && this->color == color && this->special == special;
	}

	bool isSame(Move* move) {
		return isSame(move->hole, move->color, move->special);
	}

	void update_hole(int hole) {
		this->hole = hole;
	}

	void update_color(bool color) {
		this->color = color;
	}

	void update_special(int special) {
		this->special = special;
	}

	void update_value(int value) {
		this->value = value;
	}

	void update(int hole, bool color, int special, int value) {
		update_hole(hole);
		update_color(color);
		update_special(special);
		update_value(value);
	}

	void update(Move* move) {
		update_hole(move->hole);
		update_color(move->color);
		update_special(move->special);
		update_value(move->value);
	}

	void update_print(Move* move) {
		std::cout << "Updated last best position from " << value << " to " << move->value << std::endl;
		update(move);
	}
};