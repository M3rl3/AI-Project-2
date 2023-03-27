#pragma once

#include <queue>
#include <vector>
#include <string>

#include <assert.h>

struct PathNode
{
	PathNode(char name)
		: name(name)
		, parent(nullptr)
	{
	}

	PathNode* parent;
	int Id;
	char name;
	std::vector<PathNode*> neighbours;
private:
	PathNode() : name(' ') {}
};

struct Graph
{
	//PathNode* AddNode();
	//void SetNeighbours(PathNode* a, PathNode* b);
	void SetNeighbours(int a, int b) {
		
		assert(a >= 0);
		assert(b >= 0);
		assert(a < nodes.size());
		assert(b < nodes.size());

		nodes[a]->neighbours.push_back(nodes[b]);
		nodes[b]->neighbours.push_back(nodes[a]);
	}

	std::vector<PathNode*> nodes;
};

void PrintSet(const std::string& name, const std::vector<PathNode*>& set)
{
	printf("%s { ", name.c_str());

	for (int i = 0; i < set.size(); i++)
	{
		printf("%c ", set[i]->name);
	}

	printf("}\n");
}