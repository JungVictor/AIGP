#pragma once
#include "AIGP.cpp"
class PositionPool
{
public:
	Position** tab;
	int pos;
	int n;

	PositionPool();
	~PositionPool();

	Position* acquire();
	void release();
};
