#pragma once
#include "AIGP.cpp"

class Tree {
public:
	Position* node;
	Tree** nodes;
	int depth;
	int alpha;
	int beta;

	//Constructors
	Tree() : node(), depth(0), alpha(1000), beta(1000), nodes() {}
	~Tree() { delete node; }

	bool leaf() { return nodes == nullptr; }
	Tree* getChild(int i) { return nodes[i]; }
};