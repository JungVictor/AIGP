#include "pch.h"
#include "PositionPool.h"


PositionPool::PositionPool()
{
	tab = new Position*[1000];
	n = 1000;
	pos = 999;
	for (int i = 0; i < 1000; i++) {
		tab[i] = new Position();
	}
}


PositionPool::~PositionPool()
{
	delete[] tab;
}

Position* PositionPool::acquire() {
	if (pos < 0) {
		Position** newTab = new Position*[n * 2];
		for (int i = 0; i < n; i++) {
			newTab[n + i] = tab[i];
		}
		for (int i = 0; i < n; i++) {
			newTab[i] = new Position();
		}
		pos = n - 1;
		n = n * 2;
		delete[] tab;
		tab = newTab;
	}
	pos--;
	return tab[pos + 1];
}

void PositionPool::release() {
	pos++;
}