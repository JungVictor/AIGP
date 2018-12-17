#pragma once
#include "pch.h"
#include "Position.h"

class PositionPool
{
public:
	Position* tab;
	int pos;
	int n;

	PositionPool() {
		tab = new Position[ELEMENTS];
		n = ELEMENTS;
		pos = n - 1;
		for (int i = 0; i < n; i++) {
			tab[i] = Position();
		}
	}
	~PositionPool() {
		delete[] tab;
	}

	Position acquire() {
		if (pos < 0) {
			Position* newTab = new Position[n * 2];
			for (int i = 0; i < n; i++) {
				newTab[n + i] = tab[i];
			}
			for (int i = 0; i < n; i++) {
				newTab[i] = Position();
			}
			pos = n - 1;
			n = n * 2;
			delete[] tab;
			tab = newTab;
		}
		pos--;
		return tab[pos + 1];
	}

	void release() {
		pos++;
	}
};