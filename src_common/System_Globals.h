#ifndef _GLOBALS_H
#define _GLOBALS_H

#include"System.h"

struct GlobalVariables
{
	// Routing variables
	struct
	{
		Uint16 operatingInLimpMode;
		struct PartitionTreeNode partitionTree[(MAX_NUM_NODES - 1) * 2 - 1];

#if defined(IS_ROOT)

		Uint32 limpModePaths[MAX_NUM_NODES];
		Uint16 routingTableGenerated;
		Uint16 numRouterNeighborsCollected;
		Uint16 expectedNumNodes;

		struct
		{
			Uint16 node1:1;
			Uint16 node2:1;
			Uint16 node3:1;
			Uint16 node4:1;
			Uint16 node5:1;
			Uint16 node6:1;
			Uint16 node7:1;
			Uint16 node8:1;
			Uint16 node9:1;
			Uint16 node10:1;
			Uint16 node11:1;
			Uint16 node12:1;
			Uint16 node13:1;
			Uint16 node14:1;
			Uint16 node15:1;
			Uint16 node16:1;				
		} nodeReceivedRoutingTable;

#elif defined(IS_ROUTER)

		Uint32 limpModePath;

#endif

		struct
		{
#ifdef IS_ROOT
			Uint16 numEdges;
			Uint16 numNodes;
			Uint16 treeHeight;

			struct
			{
				Uint16 lowerAddress:8;
				Uint16 upperAddress:8;
			} edgeList[MAX_NUM_NODES * NUM_PORTS / 2];
#endif
			struct
			{
				Uint16 node1:1;
				Uint16 node2:1;
				Uint16 node3:1;
				Uint16 node4:1;
				Uint16 node5:1;
				Uint16 node6:1;
				Uint16 node7:1;
				Uint16 node8:1;
				Uint16 node9:1;
				Uint16 node10:1;
				Uint16 node11:1;
				Uint16 node12:1;
				Uint16 node13:1;
				Uint16 node14:1;
				Uint16 node15:1;
				Uint16 node16:1;
			} adjacencyMatrix[MAX_NUM_NODES];
		} graphInfo;

	} routing;

	// MPI Parameters
	struct
	{
		struct
		{
			Uint16 packetIDs[MPI_MAX_DATA_PARTITIONS];
			Uint16* data;
			Uint16 dataPartitionLengths[MPI_MAX_DATA_PARTITIONS];
			Uint16 dataLength;
			MPI_Request request;
			struct
			{
				Uint32 numPartitions:4;
				Uint32 partitionsLeft:4;
				Uint32 source:8;
				Uint32 sourceServiceTag:8;
				Uint32 destinationServiceTag:8;
			} transferInfo;
		} receiveTransfers[MAX_MPI_TRANSFERS];

		struct
		{
			void** buf;
			int source;
			MPI_Request request;
		} nonBlockingReceives[MPI_MAX_NON_BLOCKING_RECEIVES];

#if defined(IS_ROOT)

		struct
		{
			Uint16 node1:1;
			Uint16 node2:1;
			Uint16 node3:1;
			Uint16 node4:1;
			Uint16 node5:1;
			Uint16 node6:1;
			Uint16 node7:1;
			Uint16 node8:1;
			Uint16 node9:1;
			Uint16 node10:1;
			Uint16 node11:1;
			Uint16 node12:1;
			Uint16 node13:1;
			Uint16 node14:1;
			Uint16 node15:1;
			Uint16 node16:1;				
		} nodeAckedBarrier;
		Uint16 barrierAckServiceTag;

#endif

	} mpiManager;

	// Protocol variables
	struct
	{
		Uint16 address;

#if defined(IS_ROOT)

		struct
		{
			Uint16 nodeType:6;
			Uint16 nodeAddress:8;
			Uint16 commFailLevel:2;
		} globalNeighborInfo[MAX_NUM_NODES][NUM_PORTS];

#elif defined(IS_ROUTER)

		struct
		{
			Uint16 nodeType:6;
			Uint16 nodeAddress:8;
			Uint16 commFailLevel:2;
		} neighborInfo[NUM_PORTS];

#endif

		Uint16 transmitCounts;
	} protocol;

	// Processing variables
	struct
	{
		// Que data
		Uint16 inboundFlitQueHead;
		Uint16 inboundFlitQueTail;
		Uint16 inboundFlitQueFull;
		Uint16 inboundFlitQue[INBOUND_FLIT_BUFFER_SIZE][NUM_CHARACTERS_IN_FLIT];
		Uint16 outboundFlitQueHead[NUM_PORTS];
		Uint16 outboundFlitQueTail[NUM_PORTS];
		Uint16 outboundFlitQueFull[NUM_PORTS];
		Uint16 outboundFlitQue[NUM_PORTS][OUTBOUND_FLIT_BUFFER_SIZE][NUM_CHARACTERS_IN_FLIT];

		// Seven segment values
		Uint16 sevenSegmentUpperDigit;
		Uint16 sevenSegmentLowerDigit;

		// Data transfer entries
		struct
		{
			Uint16* dataBuffer;
			Uint32 path;
			struct
			{
				Uint16 sourceServiceTag:8;
				Uint16 destinationServiceTag:8;
			} serviceTags;
			struct
			{
				Uint16 dataBufferLength:8;
				Int16 timeToLive:8;
			} dataBufferInfo;
			struct
			{
				Uint16 packetID:8;
				Uint16 port:3;
				Uint16 virtualChannel:5;
			} transferInfo;
		} dataTransferPackets[NUM_DATA_PACKET_LOOKUPS];

		// Virtual channel entires
		Uint16 lastVirtualChannelAssigned;
		struct
		{
			Int16 virtualChannelBlockedCount:8;
			Uint16 packetID:8;
		} virtualChannels[NUM_PORTS][NUM_VIRTUAL_DATA_CHANNELS];

		// Direct transfer entries
		struct
		{
			Uint32 destinationPort:8;
			Uint32 destinationVirtualChannel:8;
			Uint32 packetID:8;
			Int32 timeToLive:8;
		} directTransfers[NUM_PORTS][NUM_VIRTUAL_DATA_CHANNELS];

		// Direct buffer entries
		struct DirectBuffer
		{
			struct Packet packet;
			Uint16 (*callbackFunction)(struct Packet*);
			struct
			{
				Uint16 currentDataIndex:8;
				Int16 timeToLive:8;
			} bufferStatus;
		} directBuffer[NUM_PORTS][NUM_VIRTUAL_DATA_CHANNELS];
	} processing;

	// SPI Variables
	struct
	{
		Uint16 portActive[4];
	} spi;

	// Follow up monitor variables
	struct
	{
		struct
		{
			void (*callbackFunction)(void*,Uint16);
			void* callbackFunctionParameters;
			Uint16 timeToFollowUp;
			Uint16 followUpCounter;
			Uint16 doesNotExpire;
			Uint16 priority;
			Uint16 active;
		} followUpItems[NUM_FOLLOW_UP_ITEMS];

		Uint16 neighborFollowUpIndex;
	} followUpMonitor;

	// Memory manager variables
	struct
	{
		struct
		{
			Uint16* bufferBaseAddress;
			struct
			{
				Uint16 length:14;
				Uint16 accountedFor:1;
				Uint16 doesNotExpire:1;
			} bufferInfo;
		} allocations[NUM_MEMORY_ALLOCATIONS];

		Uint16* memoryBuffer;
	} memoryManager;

	// Process manager variables
	struct 
	{
		struct ServiceProvider serviceProviders[NUM_SERVICE_PROVIDERS];
	}serviceProviderManager;

	// Utilities variables
	struct
	{
		Uint32 asyncStartCounts[NUM_ASYNC_TIMERS];
		Uint32 asyncTargetCounts[NUM_ASYNC_TIMERS];
		int asyncCountOverflow[NUM_ASYNC_TIMERS];
		Uint16 functionCallStatus;
	} utilities;

	// Statistics variables
	struct
	{
		struct
		{
			Uint16 maxMemoryUsed;
			Uint16 maxMemoryAllocations;
			Uint16 numFailedMemoryAllocs;
		} memory;
		struct
		{
			Uint16 maxInboundQueLoad;
			Uint16 currentInboundQueLoad;
			Uint32 inboundQueFullCount;
			Uint16 maxOutboundQueLoad[NUM_PORTS];
			Uint16 currentOutboundQueLoad[NUM_PORTS];
			Uint32 outboundQueFullCount[NUM_PORTS];
		} que;
		struct
		{
			Uint32 numFlitsSent[NUM_PORTS];
			Uint32 numFlitsReceived[NUM_PORTS];
			Uint32 numHeaderFlitsSent;
			Uint32 numHeaderFlitsReceived;
			Uint32 numDataPayloadFlitsSent;
			Uint32 numDataPayloadFlitsReceived;
			Uint32 numDataPayloadEndFlitsSent;
			Uint32 numDataPayloadEndFlitsReceived;
			Uint16 successfullyTransmittedOnPort[NUM_PORTS];
			Uint32 numSpiArbitrationsFailed;
			Uint32 numSpiCollisions[NUM_PORTS];
			Uint32 numBlockedCollisions[NUM_PORTS];
		} flit;
		struct
		{
			Uint32 numControlPacketsSent;
			Uint32 numControlPacketsReceived;
			Uint32 numControlCrcErrors;
			Uint32 numDataTransfersSetup;
			Uint32 numDataTransfersSucceeded;
			Uint32 numDataTransfersExpired;
			Uint32 numDataTransfersFailed;
			Uint32 numDirectTransfersSetup;
			Uint32 numDirectTransfersSucceeded;
			Uint32 numDirectTransfersFailed;
			Uint32 numDirectTransfersExpired;
			Uint32 numDirectBuffersSetup;
			Uint32 numDirectBuffersSucceeded;
			Uint32 numDirectBuffersExpired;
			Uint32 numDataCrcErrors;
			Uint32 numPacketErrors;
		} packet;
		struct
		{
			Uint16 maxVirtualChannelLoad[NUM_PORTS];
			Uint16 currentVirtualChannelLoad[NUM_PORTS];
			Uint16 numVirtualChannelsExpired;
		} virtualChannel;
		struct
		{
			Uint32 ticksPassed;
			Uint32 numWarnings;
			Uint32 numErrors;
		} system;
		struct
		{
#if PROFILE_TEST_SERVICE == true
			double minTestServiceTime;
			double maxTestServiceTime;
			double avgTestServiceTime;
			Uint32 startTime;
			Uint32 numTestServiceSamples;
#endif
#if PROFILE_TRANSMIT_TIME == true
			double minTxTime;
			double maxTxTime;
			double avgTxTime;
			Uint32 numTxSamples;
#endif
#if PROFILE_HEADER_HANDLING == true
			double minHeaderHandlingTime;
			double maxHeaderHandlingTime;
			double avgHeaderHandlingTime;
			Uint32 numHeaderHandlingSamples;
#endif
#if PROFILE_PAYLOAD_HANDLING == true
			double minPayloadHandlingTime;
			double maxPayloadHandlingTime;
			double avgPayloadHandlingTime;
			Uint32 numPayloadHandlingSamples;
#endif
#if PROFILE_INTERRUPTS == true
			double minInterruptsTime;
			double maxInterruptsTime;
			double avgInterruptsTime;
			Uint32 numInterruptsSamples;
#endif
#if PROFILE_LATENCY == true
			double minLatency;
			double maxLatency;
			double avgLatency;
			Uint32 numLatencySamples;
			Uint32 latencyStartTime;
#endif
			Uint16 rsvd;	// This is here in case no profiling is enabled (structs w/o members is a semantic error)
		} profiling;
	} statistics;


	// Root variables
#ifdef IS_ROOT
	struct
	{
		struct
		{
			Uint16 rsvd1:7;
			Uint16 addressTaken:1;
			Uint16 address:8;
		} availableAddresses[MAX_NUM_NODES];

	} root;
#endif
};

extern struct GlobalVariables globals;

void InitializeGlobalVariables();

#endif
