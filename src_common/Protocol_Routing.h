#ifndef _PROTOCOL_ROUTING_H
#define _PROTOCOL_ROUTING_H

#include"System.h"

struct PartitionTreeNode
{
	struct
	{
		Uint32 parentNode:6;
		Uint32 leftChildNode:6;
		Uint32 rightChildNode:6;
		Uint32 numGraphNodesInTreeNode:8;
		Uint32 treeLevel:6;
	} nodeInformation;
	Uint16 graphNodesInTreeNode[(MAX_NUM_NODES - 1)];
	Uint16 graphNodeProbabilities[(MAX_NUM_NODES - 1)];
};

Uint16 GenerateRoutingPath(Uint16 sourceNode, Uint16 targetNode, struct Packet* packet);

#if defined(IS_ROOT)

void GenerateRoutingTree();
void GenerateNodeList(Uint16 nodesToPartition[]);
void GenerateEdgeList();
void CalculateLongestMinPaths(Uint16 shortestDistances[][MAX_NUM_NODES + 1], Uint16 nodesToPartition[]);
void PartitionGraph(Uint16 currentPartitionTreeNode, Uint16 shortestDistances[][MAX_NUM_NODES + 1]);
void CalculatePartitionTreeWeights();

#endif

#endif
