#include"System.h"

#pragma CODE_SECTION(GenerateRoutingPath,"FlashCode");
Uint16 GenerateRoutingPath(Uint16 sourceNode, Uint16 targetNode, struct Packet* packet)
{
	Uint16 sourceNodeIndex = ERROR, targetNodeIndex = ERROR, i, j, k, edgeValid, sourcePathLength, targetPathLength, pathLength, pathLengthDifference, highestProbability, suggestedNodeIndex, changesMade = true;
	Uint16 sourcePath[MAX_NUM_NODES], targetPath[MAX_NUM_NODES], treePath[MAX_NUM_NODES], suggestedPath[MAX_NUM_NODES];
	float fpath, frand, result;
	struct
	{
		Uint16 probability:15;
		Uint16 valid;
	} pathProbabilities[MAX_NUM_NODES][MAX_PATH_LENGTH];

	// Find the nodes in the tree
	for(i = 0; i < (MAX_NUM_NODES - 1) * 2 - 1; i++)
	{
		if(globals.routing.partitionTree[i].nodeInformation.numGraphNodesInTreeNode == 1 && globals.routing.partitionTree[i].graphNodesInTreeNode[0] == sourceNode)
			sourceNodeIndex = i;
		if(globals.routing.partitionTree[i].nodeInformation.numGraphNodesInTreeNode == 1 && globals.routing.partitionTree[i].graphNodesInTreeNode[0] == targetNode)
			targetNodeIndex = i;
	}

	// Make sure the nodes were found
	if(sourceNodeIndex == ERROR || targetNodeIndex == ERROR)
	{
		LogError(__FILE__,__LINE__);
		return ERROR;
	}

	// Calculate the source to root path
	sourcePath[0] = sourceNodeIndex;
	i = 0;
	while(globals.routing.partitionTree[sourcePath[i]].nodeInformation.parentNode != TREE_NODE_UNDEFINED)
	{
		sourcePath[i + 1] = globals.routing.partitionTree[sourcePath[i]].nodeInformation.parentNode;
		i++;
	}
	sourcePathLength = i + 1;

	// Calculate the target to root path
	targetPath[0] = targetNodeIndex;
	i = 0;
	while(globals.routing.partitionTree[targetPath[i]].nodeInformation.parentNode != TREE_NODE_UNDEFINED)
	{
		targetPath[i + 1] = globals.routing.partitionTree[targetPath[i]].nodeInformation.parentNode;
		i++;
	}
	targetPathLength = i + 1;

	// Remove redundant nodes
	if(sourcePathLength > targetPathLength)
	{
		pathLengthDifference = sourcePathLength - targetPathLength;
		for(i = sourcePathLength - 1; i > pathLengthDifference; i--)
		{
			if(sourcePath[i - 1] == targetPath[i - pathLengthDifference - 1])
			{
				sourcePathLength = i;
				targetPathLength = i - pathLengthDifference;
			}
		}
	}
	else
	{
		pathLengthDifference = targetPathLength - sourcePathLength;
		for(i = targetPathLength - 1; i > pathLengthDifference; i--)
		{
			if(targetPath[i - 1] == sourcePath[i - pathLengthDifference - 1])
			{
				targetPathLength = i;
				sourcePathLength = i - pathLengthDifference;
			}
		}
	}
	
	// Create the full path
	pathLength = 0;
	for(i = 0; i < sourcePathLength; i++)
		treePath[pathLength++] = sourcePath[i];
	for(i = targetPathLength - 2; i > 0; i--)
		treePath[pathLength++] = targetPath[i];
	treePath[pathLength++] = targetPath[0];
	suggestedPath[0] = sourceNode;

	// Generate the probabilities for this tree node
	for(i = 1; i < pathLength; i++)
	{
		for(j = 0; j < globals.routing.partitionTree[treePath[i]].nodeInformation.numGraphNodesInTreeNode; j++)
		{
			frand = (float)Random(1,100) / 100;
			fpath = (float)globals.routing.partitionTree[treePath[i]].graphNodeProbabilities[j] / 8;
			result = 1000 * fpath * frand + 1;
			pathProbabilities[i][j].probability = (Uint16)result;
			pathProbabilities[i][j].valid = true;
		}
	}

	// Select a node from each step in the tree
	for(i = 1; i < pathLength; i++)
	{
		// Select the path
		edgeValid = false;
		while(!edgeValid)
		{
			// Find the node with the highest probability
			highestProbability = 0;
			for(j = 0; j < globals.routing.partitionTree[treePath[i]].nodeInformation.numGraphNodesInTreeNode; j++)
			{
				if(pathProbabilities[i][j].probability > highestProbability && pathProbabilities[i][j].valid)
				{
					highestProbability = pathProbabilities[i][j].probability;
					suggestedPath[i] = globals.routing.partitionTree[treePath[i]].graphNodesInTreeNode[j];
					suggestedNodeIndex = j;
				}
			}

			// Check if we have exhausted the the edges at this level and need to backtrack
			if(highestProbability == 0)
			{
				for(j = 0; j < globals.routing.partitionTree[treePath[i]].nodeInformation.numGraphNodesInTreeNode; j++)
					pathProbabilities[i][j].valid = true;
				if(i == 1)
				{
					LogError(__FILE__,__LINE__);
					return ERROR;
				}
				i--;
				for(j = 0; j < globals.routing.partitionTree[treePath[i]].nodeInformation.numGraphNodesInTreeNode; j++)
					if(suggestedPath[i] == globals.routing.partitionTree[treePath[i]].graphNodesInTreeNode[j])
						pathProbabilities[i][j].valid = false;
			}
			else
			{
				// Verify that the edge is valid
				edgeValid = true;
				if(suggestedPath[i] != suggestedPath[i - 1])
				{
					switch(suggestedPath[i])
					{
						case 1: edgeValid &= globals.routing.graphInfo.adjacencyMatrix[suggestedPath[i - 1]].node1; break;
						case 2: edgeValid &= globals.routing.graphInfo.adjacencyMatrix[suggestedPath[i - 1]].node2; break;
						case 3: edgeValid &= globals.routing.graphInfo.adjacencyMatrix[suggestedPath[i - 1]].node3; break;
						case 4: edgeValid &= globals.routing.graphInfo.adjacencyMatrix[suggestedPath[i - 1]].node4; break;
						case 5: edgeValid &= globals.routing.graphInfo.adjacencyMatrix[suggestedPath[i - 1]].node5; break;
						case 6: edgeValid &= globals.routing.graphInfo.adjacencyMatrix[suggestedPath[i - 1]].node6; break;
						case 7: edgeValid &= globals.routing.graphInfo.adjacencyMatrix[suggestedPath[i - 1]].node7; break;
						case 8: edgeValid &= globals.routing.graphInfo.adjacencyMatrix[suggestedPath[i - 1]].node8; break;
						case 9: edgeValid &= globals.routing.graphInfo.adjacencyMatrix[suggestedPath[i - 1]].node9; break;
						case 10: edgeValid &= globals.routing.graphInfo.adjacencyMatrix[suggestedPath[i - 1]].node10; break;
						case 11: edgeValid &= globals.routing.graphInfo.adjacencyMatrix[suggestedPath[i - 1]].node11; break;
						case 12: edgeValid &= globals.routing.graphInfo.adjacencyMatrix[suggestedPath[i - 1]].node12; break;
						case 13: edgeValid &= globals.routing.graphInfo.adjacencyMatrix[suggestedPath[i - 1]].node13; break;
						case 14: edgeValid &= globals.routing.graphInfo.adjacencyMatrix[suggestedPath[i - 1]].node14; break;
						case 15: edgeValid &= globals.routing.graphInfo.adjacencyMatrix[suggestedPath[i - 1]].node15; break;
						case 16: edgeValid &= globals.routing.graphInfo.adjacencyMatrix[suggestedPath[i - 1]].node16; break;
					}
				}
				if(!edgeValid)
					pathProbabilities[i][suggestedNodeIndex].valid = false;
			}
		}
	}

	// Mark the nodes that need to be removed (aka convert to a simple path)
	for(i = 0; i < pathLength - 1; i++)
	{
		for(j = pathLength - 1; j > i; j--)
		{
			// Check if these two nodes are the same, and if so, compress the path
			if(suggestedPath[i] == suggestedPath[j])
			{
				// Shift everything from j onward up to i
				for(k = i; k < j; k++)
					suggestedPath[k] = ADDRESS_UNDEFINED;
			}
		}
	}

	// Remove the marked nodes
	while(changesMade)
	{
		changesMade = false;
		for(i = 0; i < pathLength; i++)
		{
			if(suggestedPath[i] == ADDRESS_UNDEFINED)
			{
				for(j = i; j < pathLength - 1; j++)
					suggestedPath[j] = suggestedPath[j + 1];
				changesMade = true;
				pathLength--;
				break;
			}
		}
	}
	for(i = pathLength; i < MAX_PATH_LENGTH; i++)
		suggestedPath[i] = ADDRESS_UNDEFINED;

	// Marshall the path into the packet header
	packet->path = (Uint32)(suggestedPath[0]) << 28;
	packet->path += (Uint32)(suggestedPath[1]) << 24;
	packet->path += (Uint32)(suggestedPath[2]) << 20;
	packet->path += (Uint32)(suggestedPath[3]) << 16;
	packet->path += (Uint32)(suggestedPath[4]) << 12;
	packet->path += (Uint32)(suggestedPath[5]) << 8;
	packet->path += (Uint32)(suggestedPath[6]) << 4;
	packet->path += (Uint32)(suggestedPath[7]);

	return SUCCESS;
}

#if defined(IS_ROOT)

#pragma CODE_SECTION(GenerateRoutingTree,"FlashCode");
void GenerateRoutingTree()
{
	Uint16 i;
	Uint16 nodesToPartition[MAX_NUM_NODES], shortestDistances[MAX_NUM_NODES + 1][MAX_NUM_NODES + 1];

	// Calculate the node and edge list
	GenerateNodeList(nodesToPartition);
	GenerateEdgeList();

	// Calculate the shortest paths for all nodes to all nodes
	CalculateLongestMinPaths(shortestDistances,nodesToPartition);

	// Generate the root node in the partition tree
	globals.routing.partitionTree[0].nodeInformation.leftChildNode = TREE_NODE_UNDEFINED;
	globals.routing.partitionTree[0].nodeInformation.rightChildNode = TREE_NODE_UNDEFINED;
	globals.routing.partitionTree[0].nodeInformation.parentNode = TREE_NODE_UNDEFINED;
	globals.routing.partitionTree[0].nodeInformation.treeLevel = 0;
	globals.routing.partitionTree[0].nodeInformation.numGraphNodesInTreeNode = globals.routing.graphInfo.numNodes;
	for(i = 0; i < globals.routing.graphInfo.numNodes; i++)
		globals.routing.partitionTree[0].graphNodesInTreeNode[i] = nodesToPartition[i];
	for(i = 1; i < (MAX_NUM_NODES - 1) * 2 - 1; i++)
		globals.routing.partitionTree[i].nodeInformation.numGraphNodesInTreeNode = 0;

	// Partition the routing tree
	PartitionGraph(0,shortestDistances);

	// Calculate the weights of the nodes
	CalculatePartitionTreeWeights();
}

#pragma CODE_SECTION(GenerateNodeList,"FlashCode");
void GenerateNodeList(Uint16 nodesToPartition[])
{
	Uint16 i;

	for(i = 0; i < MAX_NUM_NODES; i++)
		if(globals.root.availableAddresses[i].addressTaken)
			nodesToPartition[globals.routing.graphInfo.numNodes++] = globals.root.availableAddresses[i].address;
}

#pragma CODE_SECTION(GenerateEdgeList,"FlashCode");
void GenerateEdgeList()
{
	Uint16 i, j, k, lowerAddress, upperAddress, insertionPoint;

	// Find the set of all edges
	for(i = 0; i < MAX_NUM_NODES; i++)
	{
		for(j = 0; j < NUM_PORTS; j++)
		{
			if(globals.protocol.globalNeighborInfo[i + 1][j].nodeAddress != ADDRESS_UNDEFINED)
			{
				// Find which address is higher, and which is lower. This is very handy for sorting
				if(i + 1 > globals.protocol.globalNeighborInfo[i + 1][j].nodeAddress)
				{
					lowerAddress = globals.protocol.globalNeighborInfo[i + 1][j].nodeAddress;
					upperAddress = i + 1;
				}
				else
				{
					lowerAddress = i + 1;
					upperAddress = globals.protocol.globalNeighborInfo[i + 1][j].nodeAddress;
				}

				// Check if this is the first item in the list
				if(globals.routing.graphInfo.numEdges== 0)
				{
					globals.routing.graphInfo.edgeList[0].lowerAddress = lowerAddress;
					globals.routing.graphInfo.edgeList[0].upperAddress = upperAddress;
					globals.routing.graphInfo.numEdges = 1;

					// Set the adjancancy matrix
					switch(upperAddress)
					{
						case 1: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node1 = 1; break;
						case 2: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node2 = 1; break;
						case 3: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node3 = 1; break;
						case 4: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node4 = 1; break;
						case 5: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node5 = 1; break;
						case 6: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node6 = 1; break;
						case 7: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node7 = 1; break;
						case 8: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node8 = 1; break;
						case 9: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node9 = 1; break;
						case 10: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node10 = 1; break;
						case 11: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node11 = 1; break;
						case 12: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node12 = 1; break;
						case 13: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node13 = 1; break;
						case 14: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node14 = 1; break;
						case 15: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node15 = 1; break;
						case 16: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node16 = 1; break;
					}
					switch(lowerAddress)
					{
						case 1: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node1 = 1; break;
						case 2: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node2 = 1; break;
						case 3: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node3 = 1; break;
						case 4: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node4 = 1; break;
						case 5: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node5 = 1; break;
						case 6: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node6 = 1; break;
						case 7: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node7 = 1; break;
						case 8: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node8 = 1; break;
						case 9: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node9 = 1; break;
						case 10: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node10 = 1; break;
						case 11: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node11 = 1; break;
						case 12: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node12 = 1; break;
						case 13: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node13 = 1; break;
						case 14: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node14 = 1; break;
						case 15: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node15 = 1; break;
						case 16: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node16 = 1; break;
					}
				}
				else
				{
					// Check if this edge is already in the list or not
					insertionPoint = TREE_NODE_UNDEFINED;
					for(k = 0; k < globals.routing.graphInfo.numEdges; k++)
					{
						if(globals.routing.graphInfo.edgeList[k].lowerAddress == lowerAddress && globals.routing.graphInfo.edgeList[k].upperAddress == upperAddress)
							break;
						else if(globals.routing.graphInfo.edgeList[k].lowerAddress > lowerAddress)
						{
							insertionPoint = k;
							break;
						}
						else if(k == globals.routing.graphInfo.numEdges - 1)
						{
							insertionPoint = k + 1;
							break;
						}
					}

					// If the item is not in the list, insert and shift, add elements to the adjacency matrix
					if(insertionPoint != TREE_NODE_UNDEFINED)
					{
						// Shift everything past the insertion point backwards
						for(k = globals.routing.graphInfo.numEdges; k > insertionPoint; k--)
						{
							globals.routing.graphInfo.edgeList[k].lowerAddress = globals.routing.graphInfo.edgeList[k - 1].lowerAddress;
							globals.routing.graphInfo.edgeList[k].upperAddress = globals.routing.graphInfo.edgeList[k - 1].upperAddress;
						}

						// Save the new edge to the insertion point
						globals.routing.graphInfo.edgeList[insertionPoint].lowerAddress = lowerAddress;
						globals.routing.graphInfo.edgeList[insertionPoint].upperAddress = upperAddress;

						// Set the adjancancy matrix
						switch(upperAddress)
						{
							case 1: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node1 = 1; break;
							case 2: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node2 = 1; break;
							case 3: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node3 = 1; break;
							case 4: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node4 = 1; break;
							case 5: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node5 = 1; break;
							case 6: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node6 = 1; break;
							case 7: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node7 = 1; break;
							case 8: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node8 = 1; break;
							case 9: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node9 = 1; break;
							case 10: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node10 = 1; break;
							case 11: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node11 = 1; break;
							case 12: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node12 = 1; break;
							case 13: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node13 = 1; break;
							case 14: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node14 = 1; break;
							case 15: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node15 = 1; break;
							case 16: globals.routing.graphInfo.adjacencyMatrix[lowerAddress].node16 = 1; break;
						}
						switch(lowerAddress)
						{
							case 1: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node1 = 1; break;
							case 2: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node2 = 1; break;
							case 3: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node3 = 1; break;
							case 4: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node4 = 1; break;
							case 5: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node5 = 1; break;
							case 6: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node6 = 1; break;
							case 7: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node7 = 1; break;
							case 8: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node8 = 1; break;
							case 9: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node9 = 1; break;
							case 10: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node10 = 1; break;
							case 11: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node11 = 1; break;
							case 12: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node12 = 1; break;
							case 13: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node13 = 1; break;
							case 14: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node14 = 1; break;
							case 15: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node15 = 1; break;
							case 16: globals.routing.graphInfo.adjacencyMatrix[upperAddress].node16 = 1; break;
						}

						// Increment the numer of graph edges
						globals.routing.graphInfo.numEdges++;
					}
				}
			}
		}
	}
}

// Code modified from "A Java Library of Graph Algorithms and Optimizations", 2007, Hang T. Lau, Champan & Hall publisher
#pragma CODE_SECTION(CalculateLongestMinPaths,"FlashCode");
void CalculateLongestMinPaths(Uint16 shortestDistances[][MAX_NUM_NODES + 1], Uint16 nodesToPartition[])
{
	Uint16 i, j, k, nodeU, nodeV, minLength, r, root, temp, minJ = 0, minV = 0;
	int location[MAX_NUM_NODES + 1];
	int distance[MAX_NUM_NODES + 1][MAX_NUM_NODES + 1];

	// Calculate the shortest path lengths for all nodes
	for(r = 1; r <= globals.routing.graphInfo.numNodes; r++)
	{
		// set up the distance matrix
		for (i = 1; i <= globals.routing.graphInfo.numNodes; i++)
		{
			for (j = 1; j <= globals.routing.graphInfo.numNodes; j++)
			{
				if(i == j)
					distance[i][j] = 0;
				else
					distance[i][j] = globals.routing.graphInfo.numEdges + 1;
			}
		}
		for(k = 1; k <= globals.routing.graphInfo.numEdges; k++) 
		{
			distance[globals.routing.graphInfo.edgeList[k].lowerAddress][globals.routing.graphInfo.edgeList[k].upperAddress] = 1;
			distance[globals.routing.graphInfo.edgeList[k].upperAddress][globals.routing.graphInfo.edgeList[k].lowerAddress] = 1;
		}

		root = r;
		if (root != 1) 
		{
			// interchange rows 1 and root
			for(i = 1; i <= globals.routing.graphInfo.numNodes; i++) 
			{
				temp = distance[1][i];
				distance[1][i] = distance[root][i];
				distance[root][i] = temp;
			}

			// interchange columns 1 and root
			for(i = 1; i <= globals.routing.graphInfo.numNodes; i++)
			{
				temp = distance[i][1];
				distance[i][1] = distance[i][root];
				distance[i][root] = temp;
			}
		}

		nodeU = 1;
		for (i = 1; i <= globals.routing.graphInfo.numNodes; i++)
		{
			location[i] = i;
			shortestDistances[root][i] = distance[nodeU][i];
		}

		for (i = 2; i <= globals.routing.graphInfo.numNodes; i++)
		{
			k = globals.routing.graphInfo.numNodes + 2 - i;
			minLength = globals.routing.graphInfo.numEdges+1;

			for (j = 2; j <= k; j++)
			{
				nodeV = location[j];
				temp = shortestDistances[root][nodeU] + distance[nodeU][nodeV];

				if (temp < shortestDistances[root][nodeV])
					shortestDistances[root][nodeV] = temp;

				if (minLength > shortestDistances[root][nodeV])
				{
					minLength = shortestDistances[root][nodeV];
					minV = nodeV;
					minJ = j;
				}
			}

			nodeU = minV;
			location[minJ] = location[k];
		}

		if (root != 1)
		{
			shortestDistances[root][1] = shortestDistances[root][root];
			shortestDistances[root][root] = 0;

			// interchange rows 1 and root
			for (i = 1; i <= globals.routing.graphInfo.numNodes; i++)
			{
				temp = distance[1][i];
				distance[1][i] = distance[root][i];
				distance[root][i] = temp;
			}

			// interchange columns 1 and root
			for (i = 1; i <= globals.routing.graphInfo.numNodes; i++)
			{
				temp = distance[i][1];
				distance[i][1] = distance[i][root];
				distance[i][root] = temp;
			}
		}
	}
}

#pragma CODE_SECTION(CalculateDvalues,"FlashCode");
void CalculateDvalues(Int16 Da[], Int16 Db[], Uint16* A1, Uint16* B1)
{
	Uint16 i, j;
	//compute the initial D values for all a in A1 and b in B1 where Dx = ExternalConnectionsTox - InternalConnectionsTox
	for(i = 0; i < MAX_NUM_NODES - 1; i++)
	{
		Da[i] = 0;
		Db[i] = 0;

		for(j = 0; j < MAX_NUM_NODES - 1; j++)
		{
			// Calculate a external A1 and internal B1
			switch(B1[j])
			{
				case 1: 
					Da[i] += globals.routing.graphInfo.adjacencyMatrix[A1[i]].node1;
					Db[i] -= globals.routing.graphInfo.adjacencyMatrix[B1[i]].node1;
					break;
				case 2: 
					Da[i] += globals.routing.graphInfo.adjacencyMatrix[A1[i]].node2; 
					Db[i] -= globals.routing.graphInfo.adjacencyMatrix[B1[i]].node2;
					break;
				case 3: 
					Da[i] += globals.routing.graphInfo.adjacencyMatrix[A1[i]].node3; 
					Db[i] -= globals.routing.graphInfo.adjacencyMatrix[B1[i]].node3;
					break;
				case 4: 
					Da[i] += globals.routing.graphInfo.adjacencyMatrix[A1[i]].node4; 
					Db[i] -= globals.routing.graphInfo.adjacencyMatrix[B1[i]].node4;
					break;
				case 5: 
					Da[i] += globals.routing.graphInfo.adjacencyMatrix[A1[i]].node5; 
					Db[i] -= globals.routing.graphInfo.adjacencyMatrix[B1[i]].node5;
					break;
				case 6: 
					Da[i] += globals.routing.graphInfo.adjacencyMatrix[A1[i]].node6; 
					Db[i] -= globals.routing.graphInfo.adjacencyMatrix[B1[i]].node6;
					break;
				case 7: 
					Da[i] += globals.routing.graphInfo.adjacencyMatrix[A1[i]].node7; 
					Db[i] -= globals.routing.graphInfo.adjacencyMatrix[B1[i]].node7;
					break;
				case 8: 
					Da[i] += globals.routing.graphInfo.adjacencyMatrix[A1[i]].node8; 
					Db[i] -= globals.routing.graphInfo.adjacencyMatrix[B1[i]].node8;
					break;
				case 9: 
					Da[i] += globals.routing.graphInfo.adjacencyMatrix[A1[i]].node9; 
					Db[i] -= globals.routing.graphInfo.adjacencyMatrix[B1[i]].node9;
					break;
				case 10: 
					Da[i] += globals.routing.graphInfo.adjacencyMatrix[A1[i]].node10; 
					Db[i] -= globals.routing.graphInfo.adjacencyMatrix[B1[i]].node10;
					break;
				case 11: 
					Da[i] += globals.routing.graphInfo.adjacencyMatrix[A1[i]].node11; 
					Db[i] -= globals.routing.graphInfo.adjacencyMatrix[B1[i]].node11;
					break;
				case 12: 
					Da[i] += globals.routing.graphInfo.adjacencyMatrix[A1[i]].node12; 
					Db[i] -= globals.routing.graphInfo.adjacencyMatrix[B1[i]].node12;
					break;
				case 13: 
					Da[i] += globals.routing.graphInfo.adjacencyMatrix[A1[i]].node13; 
					Db[i] -= globals.routing.graphInfo.adjacencyMatrix[B1[i]].node13;
					break;
				case 14: 
					Da[i] += globals.routing.graphInfo.adjacencyMatrix[A1[i]].node14; 
					Db[i] -= globals.routing.graphInfo.adjacencyMatrix[B1[i]].node14;
					break;
				case 15: 
					Da[i] += globals.routing.graphInfo.adjacencyMatrix[A1[i]].node15; 
					Db[i] -= globals.routing.graphInfo.adjacencyMatrix[B1[i]].node15;
					break;
				case 16: 
					Da[i] += globals.routing.graphInfo.adjacencyMatrix[A1[i]].node16; 
					Db[i] -= globals.routing.graphInfo.adjacencyMatrix[B1[i]].node16;
					break;
			}

			// Calculate external B and internal A
			switch(A1[j])
			{
				case 1: 
					Da[i] -= globals.routing.graphInfo.adjacencyMatrix[A1[i]].node1; 
					Db[i] += globals.routing.graphInfo.adjacencyMatrix[B1[i]].node1;
					break;
				case 2: 
					Da[i] -= globals.routing.graphInfo.adjacencyMatrix[A1[i]].node2;  
					Db[i] += globals.routing.graphInfo.adjacencyMatrix[B1[i]].node2;
					break;
				case 3: 
					Da[i] -= globals.routing.graphInfo.adjacencyMatrix[A1[i]].node3;  
					Db[i] += globals.routing.graphInfo.adjacencyMatrix[B1[i]].node3;
					break;
				case 4: 
					Da[i] -= globals.routing.graphInfo.adjacencyMatrix[A1[i]].node4;  
					Db[i] += globals.routing.graphInfo.adjacencyMatrix[B1[i]].node4;
					break;
				case 5: 
					Da[i] -= globals.routing.graphInfo.adjacencyMatrix[A1[i]].node5;  
					Db[i] += globals.routing.graphInfo.adjacencyMatrix[B1[i]].node5;
					break;
				case 6: 
					Da[i] -= globals.routing.graphInfo.adjacencyMatrix[A1[i]].node6;  
					Db[i] += globals.routing.graphInfo.adjacencyMatrix[B1[i]].node6;
					break;
				case 7: 
					Da[i] -= globals.routing.graphInfo.adjacencyMatrix[A1[i]].node7;  
					Db[i] += globals.routing.graphInfo.adjacencyMatrix[B1[i]].node7;
					break;
				case 8: 
					Da[i] -= globals.routing.graphInfo.adjacencyMatrix[A1[i]].node8;  
					Db[i] += globals.routing.graphInfo.adjacencyMatrix[B1[i]].node8;
					break;
				case 9: 
					Da[i] -= globals.routing.graphInfo.adjacencyMatrix[A1[i]].node9;  
					Db[i] += globals.routing.graphInfo.adjacencyMatrix[B1[i]].node9;
					break;
				case 10: 
					Da[i] -= globals.routing.graphInfo.adjacencyMatrix[A1[i]].node10;  
					Db[i] += globals.routing.graphInfo.adjacencyMatrix[B1[i]].node10;
					break;
				case 11: 
					Da[i] -= globals.routing.graphInfo.adjacencyMatrix[A1[i]].node11;  
					Db[i] += globals.routing.graphInfo.adjacencyMatrix[B1[i]].node11;
					break;
				case 12: 
					Da[i] -= globals.routing.graphInfo.adjacencyMatrix[A1[i]].node12;  
					Db[i] += globals.routing.graphInfo.adjacencyMatrix[B1[i]].node12;
					break;
				case 13: 
					Da[i] -= globals.routing.graphInfo.adjacencyMatrix[A1[i]].node13;  
					Db[i] += globals.routing.graphInfo.adjacencyMatrix[B1[i]].node13;
					break;
				case 14: 
					Da[i] -= globals.routing.graphInfo.adjacencyMatrix[A1[i]].node14;  
					Db[i] += globals.routing.graphInfo.adjacencyMatrix[B1[i]].node14;
					break;
				case 15: 
					Da[i] -= globals.routing.graphInfo.adjacencyMatrix[A1[i]].node15;  
					Db[i] += globals.routing.graphInfo.adjacencyMatrix[B1[i]].node15;
					break;
				case 16: 
					Da[i] -= globals.routing.graphInfo.adjacencyMatrix[A1[i]].node16;  
					Db[i] += globals.routing.graphInfo.adjacencyMatrix[B1[i]].node16;
					break;
			}
		}
	}
}

#pragma CODE_SECTION(PartitionGraph,"FlashCode");
void PartitionGraph(Uint16 currentPartitionTreeNode, Uint16 shortestDistances[][MAX_NUM_NODES + 1])
{
	Uint16 i, j, k, bestA, bestB, bestG, longestMinPath[2], longestPath = 0, numNodes, numIterations = 0, *A1, *B1;
	Int16 gMax, temp, gCheck, g[MAX_NUM_NODES - 1], Da[MAX_NUM_NODES - 1], Db[MAX_NUM_NODES - 1];

	numNodes = globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.numGraphNodesInTreeNode;
	if(numNodes == 2)
	{
		// Allocate and initialize the child nodes in the tree
		for(i = 0; i < (MAX_NUM_NODES - 1) * 2 - 1; i++)
		{
			if(globals.routing.partitionTree[i].nodeInformation.numGraphNodesInTreeNode == 0)
			{
				globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.leftChildNode = i;
				globals.routing.partitionTree[i].nodeInformation.parentNode = currentPartitionTreeNode;
				globals.routing.partitionTree[i].nodeInformation.leftChildNode = TREE_NODE_UNDEFINED;
				globals.routing.partitionTree[i].nodeInformation.rightChildNode = TREE_NODE_UNDEFINED;
				globals.routing.partitionTree[i].nodeInformation.numGraphNodesInTreeNode = 1;
				globals.routing.partitionTree[i].nodeInformation.treeLevel = globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.treeLevel + 1;
				if(globals.routing.partitionTree[i].nodeInformation.treeLevel + 1 > globals.routing.graphInfo.treeHeight)
					globals.routing.graphInfo.treeHeight = globals.routing.partitionTree[i].nodeInformation.treeLevel + 1;
				globals.routing.partitionTree[i].graphNodesInTreeNode[0] = globals.routing.partitionTree[currentPartitionTreeNode].graphNodesInTreeNode[0];
				break;
			}
		}
		for(i = globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.leftChildNode + 1; i < (MAX_NUM_NODES - 1) * 2 - 1; i++)
		{
			if(globals.routing.partitionTree[i].nodeInformation.numGraphNodesInTreeNode == 0)
			{
				globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.rightChildNode = i;
				globals.routing.partitionTree[i].nodeInformation.parentNode = currentPartitionTreeNode;
				globals.routing.partitionTree[i].nodeInformation.leftChildNode = TREE_NODE_UNDEFINED;
				globals.routing.partitionTree[i].nodeInformation.rightChildNode = TREE_NODE_UNDEFINED;
				globals.routing.partitionTree[i].nodeInformation.numGraphNodesInTreeNode = 1;
				globals.routing.partitionTree[i].nodeInformation.treeLevel = globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.treeLevel + 1;
				if(globals.routing.partitionTree[i].nodeInformation.treeLevel + 1 > globals.routing.graphInfo.treeHeight)
					globals.routing.graphInfo.treeHeight = globals.routing.partitionTree[i].nodeInformation.treeLevel + 1;
				globals.routing.partitionTree[i].graphNodesInTreeNode[0] = globals.routing.partitionTree[currentPartitionTreeNode].graphNodesInTreeNode[1];
				break;
			}
		}
	}
	else
	{
		// Find the longest path using the given nodes
		longestMinPath[0] = 0;
		longestMinPath[1] = 0;
		for(i = 0; i < numNodes; i++)
		{
			for(j = 0; j < numNodes; j++)
			{
				if(shortestDistances[globals.routing.partitionTree[currentPartitionTreeNode].graphNodesInTreeNode[i]][globals.routing.partitionTree[currentPartitionTreeNode].graphNodesInTreeNode[j]] > longestPath)
				{
					longestPath = shortestDistances[globals.routing.partitionTree[currentPartitionTreeNode].graphNodesInTreeNode[i]][globals.routing.partitionTree[currentPartitionTreeNode].graphNodesInTreeNode[j]];
					longestMinPath[0] = globals.routing.partitionTree[currentPartitionTreeNode].graphNodesInTreeNode[i];
					longestMinPath[1] = globals.routing.partitionTree[currentPartitionTreeNode].graphNodesInTreeNode[j];
				}
			}
		}

		// Sort the nodes to partition by distance from longestMinPath[0]
		for(i = 0; i < numNodes - 1; i++)
		{
			for(j = i + 1; j < numNodes; j++)
			{
				if(shortestDistances[longestMinPath[0]][globals.routing.partitionTree[currentPartitionTreeNode].graphNodesInTreeNode[j]] < 
					shortestDistances[longestMinPath[0]][globals.routing.partitionTree[currentPartitionTreeNode].graphNodesInTreeNode[i]])
				{
					temp = globals.routing.partitionTree[currentPartitionTreeNode].graphNodesInTreeNode[i];
					globals.routing.partitionTree[currentPartitionTreeNode].graphNodesInTreeNode[i] = globals.routing.partitionTree[currentPartitionTreeNode].graphNodesInTreeNode[j];
					globals.routing.partitionTree[currentPartitionTreeNode].graphNodesInTreeNode[j] = temp;
				}
			}
		}

		// Allocate and initialize the child nodes in the tree
		for(i = 0; i < (MAX_NUM_NODES - 1) * 2 - 1; i++)
		{
			if(globals.routing.partitionTree[i].nodeInformation.numGraphNodesInTreeNode == 0)
			{
				globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.leftChildNode = i;
				globals.routing.partitionTree[i].nodeInformation.parentNode = currentPartitionTreeNode;
				globals.routing.partitionTree[i].nodeInformation.treeLevel = globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.treeLevel + 1;
				if(globals.routing.partitionTree[i].nodeInformation.treeLevel + 1 > globals.routing.graphInfo.treeHeight)
					globals.routing.graphInfo.treeHeight = globals.routing.partitionTree[i].nodeInformation.treeLevel + 1;
				break;
			}
		}
		for(i = globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.leftChildNode + 1; i < (MAX_NUM_NODES - 1) * 2 - 1; i++)
		{
			if(globals.routing.partitionTree[i].nodeInformation.numGraphNodesInTreeNode == 0)
			{
				globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.rightChildNode = i;
				globals.routing.partitionTree[i].nodeInformation.parentNode = currentPartitionTreeNode;
				globals.routing.partitionTree[i].nodeInformation.treeLevel = globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.treeLevel + 1;
				if(globals.routing.partitionTree[i].nodeInformation.treeLevel + 1 > globals.routing.graphInfo.treeHeight)
					globals.routing.graphInfo.treeHeight = globals.routing.partitionTree[i].nodeInformation.treeLevel + 1;
				break;
			}
		}

		// Partition the tree
		globals.routing.partitionTree[globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.leftChildNode].nodeInformation.numGraphNodesInTreeNode = numNodes / 2;
		for(i = 0; i < numNodes / 2; i++)
			globals.routing.partitionTree[globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.leftChildNode].graphNodesInTreeNode[i] = 
				globals.routing.partitionTree[currentPartitionTreeNode].graphNodesInTreeNode[i];
		for(i = numNodes / 2; i < MAX_NUM_NODES - 1; i++)
			globals.routing.partitionTree[globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.leftChildNode].graphNodesInTreeNode[i] = 0;

		globals.routing.partitionTree[globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.rightChildNode].nodeInformation.numGraphNodesInTreeNode = numNodes - (numNodes / 2);
		for(i = numNodes / 2; i < numNodes; i++)
			globals.routing.partitionTree[globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.rightChildNode].graphNodesInTreeNode[i - (numNodes / 2)] = 
				globals.routing.partitionTree[currentPartitionTreeNode].graphNodesInTreeNode[i];
		for(i = numNodes; i < MAX_NUM_NODES + (numNodes / 2); i++)
			globals.routing.partitionTree[globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.rightChildNode].graphNodesInTreeNode[i - (numNodes / 2)] = 0;
		
		// Minimize the cut point using a slightly modified version Kernighan-Lin's algorithm (basically I added support for an odd number of graph nodes)
		A1 = globals.routing.partitionTree[globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.leftChildNode].graphNodesInTreeNode;
		B1 = globals.routing.partitionTree[globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.rightChildNode].graphNodesInTreeNode;
		
		do
		{
			CalculateDvalues(Da,Db,A1,B1);

			for(i = 0; i < numNodes; i++)
			{
				//find a[x] from A1 and b[y] from B1, such that g[j] = D[a[x]] + D[b[y]] - 2*connection(a[x],b[y]) is maximal. A1 and B1 are sorted to align with the sorted g
				g[i] = SHRT_MIN;
				for (j = i; j < numNodes; j++)
				{
					for(k = i; k < numNodes; k++)
					{
						if(A1[j] != 0 && B1[k] != 0)
						{
							switch(A1[j])
							{
								case 1: gCheck = Da[j] + Db[k] - 2 * globals.routing.graphInfo.adjacencyMatrix[B1[k]].node1; break;
								case 2: gCheck = Da[j] + Db[k] - 2 * globals.routing.graphInfo.adjacencyMatrix[B1[k]].node2; break;
								case 3: gCheck = Da[j] + Db[k] - 2 * globals.routing.graphInfo.adjacencyMatrix[B1[k]].node3; break;
								case 4: gCheck = Da[j] + Db[k] - 2 * globals.routing.graphInfo.adjacencyMatrix[B1[k]].node4; break;
								case 5: gCheck = Da[j] + Db[k] - 2 * globals.routing.graphInfo.adjacencyMatrix[B1[k]].node5; break;
								case 6: gCheck = Da[j] + Db[k] - 2 * globals.routing.graphInfo.adjacencyMatrix[B1[k]].node6; break;
								case 7: gCheck = Da[j] + Db[k] - 2 * globals.routing.graphInfo.adjacencyMatrix[B1[k]].node7; break;
								case 8: gCheck = Da[j] + Db[k] - 2 * globals.routing.graphInfo.adjacencyMatrix[B1[k]].node8; break;
								case 9: gCheck = Da[j] + Db[k] - 2 * globals.routing.graphInfo.adjacencyMatrix[B1[k]].node9; break;
								case 10: gCheck = Da[j] + Db[k] - 2 * globals.routing.graphInfo.adjacencyMatrix[B1[k]].node10; break;
								case 11: gCheck = Da[j] + Db[k] - 2 * globals.routing.graphInfo.adjacencyMatrix[B1[k]].node11; break;
								case 12: gCheck = Da[j] + Db[k] - 2 * globals.routing.graphInfo.adjacencyMatrix[B1[k]].node12; break;
								case 13: gCheck = Da[j] + Db[k] - 2 * globals.routing.graphInfo.adjacencyMatrix[B1[k]].node13; break;
								case 14: gCheck = Da[j] + Db[k] - 2 * globals.routing.graphInfo.adjacencyMatrix[B1[k]].node14; break;
								case 15: gCheck = Da[j] + Db[k] - 2 * globals.routing.graphInfo.adjacencyMatrix[B1[k]].node15; break;
								case 16: gCheck = Da[j] + Db[k] - 2 * globals.routing.graphInfo.adjacencyMatrix[B1[k]].node16; break;
							}
							if(gCheck > g[i])
							{
								bestA = j;
								bestB = k;
								g[i] = gCheck;
							}
						}
					}
				}
				
				if(g[i] != SHRT_MIN)
				{
					//move A1[j] to B1[k] and B1[k] to A1[j]
					temp = A1[bestA];
					A1[bestA] = B1[bestB];
					B1[bestB] = temp;

					// Reorder A1 and B1 so that g[i] aligns with A1[i] and B1[i]. This is how nodes are marked as having been reordered (everything left of i has been marked).
					temp = A1[bestA];
					A1[bestA] = A1[i];
					A1[i] = temp;
					temp = B1[bestB];
					B1[bestB] = B1[i];
					B1[i] = temp;
				}

				// Update the D values
				CalculateDvalues(Da,Db,A1,B1);
			}

			//find k which maximizes gMax, the sum of g[1],...,g[k]
			gMax = SHRT_MIN;
			for(j = 0; j < numNodes; j++)
			{
				gCheck = 0;
				for(k = 0; k <= j; k++)
				{
					if(g[k] != SHRT_MIN)
						gCheck += g[k];
				}
				if(gCheck > gMax)
				{
					gMax = gCheck;
					bestG = j;
				}
			}
			if (gMax > 0)
			{
				// Undo the the previous swap for all indeces after k
				for(j = bestG + 1; j < numNodes; j++)
				{
					if(A1[j] != 0 && B1[j] != 0)
					{
						temp = A1[j];
						A1[j] = B1[j];
						B1[j] = temp;
					}
				}
			}
			else
			{
				// Undo all swaps
				for(j = 0; j < numNodes; j++)
				{
					if(A1[j] != 0 && B1[j] != 0)
					{
						temp = A1[j];
						A1[j] = B1[j];
						B1[j] = temp;
					}
				}
			}

			numIterations++;

		} while(gMax > 0 && numIterations < MAX_GRAPH_PARTITION_ITERATIONS);

		// Partition the sub-groups if there are more than two nodes left, otherwise finalize this branch
		if(globals.routing.partitionTree[globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.leftChildNode].nodeInformation.numGraphNodesInTreeNode > 1)
			PartitionGraph(globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.leftChildNode,shortestDistances);
		else
		{
			globals.routing.partitionTree[globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.leftChildNode].nodeInformation.leftChildNode = TREE_NODE_UNDEFINED;
			globals.routing.partitionTree[globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.leftChildNode].nodeInformation.rightChildNode = TREE_NODE_UNDEFINED;
		}
		if(globals.routing.partitionTree[globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.rightChildNode].nodeInformation.numGraphNodesInTreeNode > 1)
			PartitionGraph(globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.rightChildNode,shortestDistances);
		else
		{
			globals.routing.partitionTree[globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.rightChildNode].nodeInformation.leftChildNode = TREE_NODE_UNDEFINED;
			globals.routing.partitionTree[globals.routing.partitionTree[currentPartitionTreeNode].nodeInformation.rightChildNode].nodeInformation.rightChildNode = TREE_NODE_UNDEFINED;
		}
	}
}

#pragma CODE_SECTION(CalculatePartitionTreeWeights,"FlashCode");
void CalculatePartitionTreeWeights()
{
	Uint16 i, j, k;

	// Define the weights for the root node
	for(i = 0; i < globals.routing.partitionTree[0].nodeInformation.numGraphNodesInTreeNode; i++)
	{
		globals.routing.partitionTree[0].graphNodeProbabilities[i] = 0;
		for(j = 0; j < globals.routing.partitionTree[0].nodeInformation.numGraphNodesInTreeNode; j++)
		{
			switch(globals.routing.partitionTree[0].graphNodesInTreeNode[j])
			{
				case 1: globals.routing.partitionTree[0].graphNodeProbabilities[i] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[0].graphNodesInTreeNode[i]].node1; break;
				case 2: globals.routing.partitionTree[0].graphNodeProbabilities[i] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[0].graphNodesInTreeNode[i]].node2; break;
				case 3: globals.routing.partitionTree[0].graphNodeProbabilities[i] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[0].graphNodesInTreeNode[i]].node3; break;
				case 4: globals.routing.partitionTree[0].graphNodeProbabilities[i] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[0].graphNodesInTreeNode[i]].node4; break;
				case 5: globals.routing.partitionTree[0].graphNodeProbabilities[i] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[0].graphNodesInTreeNode[i]].node5; break;
				case 6: globals.routing.partitionTree[0].graphNodeProbabilities[i] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[0].graphNodesInTreeNode[i]].node6; break;
				case 7: globals.routing.partitionTree[0].graphNodeProbabilities[i] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[0].graphNodesInTreeNode[i]].node7; break;
				case 8: globals.routing.partitionTree[0].graphNodeProbabilities[i] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[0].graphNodesInTreeNode[i]].node8; break;
				case 9: globals.routing.partitionTree[0].graphNodeProbabilities[i] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[0].graphNodesInTreeNode[i]].node9; break;
				case 10: globals.routing.partitionTree[0].graphNodeProbabilities[i] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[0].graphNodesInTreeNode[i]].node10; break;
				case 11: globals.routing.partitionTree[0].graphNodeProbabilities[i] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[0].graphNodesInTreeNode[i]].node11; break;
				case 12: globals.routing.partitionTree[0].graphNodeProbabilities[i] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[0].graphNodesInTreeNode[i]].node12; break;
				case 13: globals.routing.partitionTree[0].graphNodeProbabilities[i] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[0].graphNodesInTreeNode[i]].node13; break;
				case 14: globals.routing.partitionTree[0].graphNodeProbabilities[i] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[0].graphNodesInTreeNode[i]].node14; break;
				case 15: globals.routing.partitionTree[0].graphNodeProbabilities[i] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[0].graphNodesInTreeNode[i]].node15; break;
				case 16: globals.routing.partitionTree[0].graphNodeProbabilities[i] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[0].graphNodesInTreeNode[i]].node16; break;
			}
		}
	}

	// Loop through each partition tree node and calculate the weights for each node in the tree node
	for(i = 1; i < (MAX_NUM_NODES - 1) * 2 - 1; i++)
	{
		if(globals.routing.partitionTree[i].nodeInformation.numGraphNodesInTreeNode > 0)
		{
			// Calculate the probabilities of transmitting through a given node in tree node i
			for(j = 0; j < globals.routing.partitionTree[i].nodeInformation.numGraphNodesInTreeNode; j++)
			{
				globals.routing.partitionTree[i].graphNodeProbabilities[j] = 0;
				for(k = 0; k < globals.routing.partitionTree[globals.routing.partitionTree[i].nodeInformation.parentNode].nodeInformation.numGraphNodesInTreeNode; k++)
				{
					switch(globals.routing.partitionTree[globals.routing.partitionTree[i].nodeInformation.parentNode].graphNodesInTreeNode[k])
					{
						case 1: globals.routing.partitionTree[i].graphNodeProbabilities[j] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[i].graphNodesInTreeNode[j]].node1; break;
						case 2: globals.routing.partitionTree[i].graphNodeProbabilities[j] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[i].graphNodesInTreeNode[j]].node2; break;
						case 3: globals.routing.partitionTree[i].graphNodeProbabilities[j] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[i].graphNodesInTreeNode[j]].node3; break;
						case 4: globals.routing.partitionTree[i].graphNodeProbabilities[j] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[i].graphNodesInTreeNode[j]].node4; break;
						case 5: globals.routing.partitionTree[i].graphNodeProbabilities[j] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[i].graphNodesInTreeNode[j]].node5; break;
						case 6: globals.routing.partitionTree[i].graphNodeProbabilities[j] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[i].graphNodesInTreeNode[j]].node6; break;
						case 7: globals.routing.partitionTree[i].graphNodeProbabilities[j] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[i].graphNodesInTreeNode[j]].node7; break;
						case 8: globals.routing.partitionTree[i].graphNodeProbabilities[j] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[i].graphNodesInTreeNode[j]].node8; break;
						case 9: globals.routing.partitionTree[i].graphNodeProbabilities[j] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[i].graphNodesInTreeNode[j]].node9; break;
						case 10: globals.routing.partitionTree[i].graphNodeProbabilities[j] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[i].graphNodesInTreeNode[j]].node10; break;
						case 11: globals.routing.partitionTree[i].graphNodeProbabilities[j] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[i].graphNodesInTreeNode[j]].node11; break;
						case 12: globals.routing.partitionTree[i].graphNodeProbabilities[j] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[i].graphNodesInTreeNode[j]].node12; break;
						case 13: globals.routing.partitionTree[i].graphNodeProbabilities[j] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[i].graphNodesInTreeNode[j]].node13; break;
						case 14: globals.routing.partitionTree[i].graphNodeProbabilities[j] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[i].graphNodesInTreeNode[j]].node14; break;
						case 15: globals.routing.partitionTree[i].graphNodeProbabilities[j] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[i].graphNodesInTreeNode[j]].node15; break;
						case 16: globals.routing.partitionTree[i].graphNodeProbabilities[j] += globals.routing.graphInfo.adjacencyMatrix[globals.routing.partitionTree[i].graphNodesInTreeNode[j]].node16; break;
					}
				}
			}
		}
	}
}

#endif
