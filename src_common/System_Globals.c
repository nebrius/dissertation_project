#include"System.h"

#ifdef __cplusplus
#pragma DATA_SECTION("GlobalRegistersFile")
#else
#pragma DATA_SECTION(globals,"GlobalRegistersFile");
#endif
struct GlobalVariables globals;

#pragma DATA_SECTION(localMemoryBuffer,"MemoryManagerBuffer");
Uint16 localMemoryBuffer[MEMORY_BUFFER_SIZE];

#pragma CODE_SECTION(InitializeGlobalVariables,"FlashCode");
void InitializeGlobalVariables()
{
	int i, j;

	// Initialize the root variables if this guy is a root node
#if defined(IS_ROOT)

	// Set this nodes address to the root address
	globals.protocol.address = ROOT_ADDRESS;

	// Initialize the available addresses structure
	globals.root.availableAddresses[0].addressTaken = true;
	globals.root.availableAddresses[0].address = 1;
	for(i = 1; i < MAX_NUM_NODES; i++)
	{
		// Set the address to available
		globals.root.availableAddresses[i].addressTaken = false;

		// We do i + 2 because address 0 is undefined and address 1 is the root address
		globals.root.availableAddresses[i].address = i + 1;
	}

#elif defined(IS_ROUTER)

	// Set this router's address to undefined
	globals.protocol.address = ADDRESS_UNDEFINED;

#endif

	// Set the dynamic memory buffer pointer
	globals.memoryManager.memoryBuffer = localMemoryBuffer;
	for(i = 0; i < NUM_MEMORY_ALLOCATIONS; i++)
		globals.memoryManager.allocations[i].bufferBaseAddress = NULL;

	// Loop through the ports and initialize their variables
	globals.processing.lastVirtualChannelAssigned = 0;
	for(i = 0; i < NUM_PORTS; i++)
	{
		// Initialize the SPI variables for this port
		globals.spi.portActive[i] = 0;
		
		// Initialize the processing variables for this port
		for(j = 0; j < NUM_VIRTUAL_DATA_CHANNELS; j++)
		{
			// Set the virtual channels to available
			globals.processing.virtualChannels[i][j].packetID = VIRTUAL_CHANNEL_AVAILABLE;
			globals.processing.virtualChannels[i][j].virtualChannelBlockedCount = 0;

			// Set the direct transfers to available
			globals.processing.directTransfers[i][j].destinationPort = PORT_UNDEFINED;
			globals.processing.directBuffer[i][j].packet.dataBuffer = NULL;
		}

		// Set the outbound que associated with this port to empty
		globals.processing.outboundFlitQueFull[i] = false;
		globals.processing.outboundFlitQueHead[i] = 0;
		globals.processing.outboundFlitQueTail[i] = 0;

#if defined(IS_ROOT)

		// Initialize the protocol variables
		for(j = 0; j < MAX_NUM_NODES; j++)
		{
			globals.protocol.globalNeighborInfo[j][i].nodeAddress = ADDRESS_UNDEFINED;
			globals.protocol.globalNeighborInfo[j][i].nodeType = NODE_TYPE_UNDEFINED;
			globals.protocol.globalNeighborInfo[j][i].commFailLevel = 0;
		}

#elif defined(IS_ROUTER)

		// Initialize the protocol variables
		globals.protocol.neighborInfo[i].nodeAddress = ADDRESS_UNDEFINED;
		globals.protocol.neighborInfo[i].nodeType = NODE_TYPE_UNDEFINED;
		globals.protocol.neighborInfo[i].commFailLevel = 0;

#endif
	}

	// Data transfer lookup initializations
	for(i = 0; i < NUM_DATA_PACKET_LOOKUPS; i++)
		globals.processing.dataTransferPackets[i].transferInfo.packetID = 0;

	// Set the transmit counts to 0. This is used for the packet ID
	globals.protocol.transmitCounts = 1;

	// Initialize the outbound flit que variables
	globals.processing.inboundFlitQueFull = false;
	globals.processing.inboundFlitQueHead = 0;
	globals.processing.inboundFlitQueTail = 0;

	// Set all of the follow up items to unused
	for(i = 0; i < NUM_FOLLOW_UP_ITEMS; i++)
		globals.followUpMonitor.followUpItems[i].active = false;

	// Set all of the asynchronous timers to unused
	for(i = 0; i < NUM_ASYNC_TIMERS; i++)
		globals.utilities.asyncCountOverflow[i] = -1;

	// Initialize the service providers
	for(i = 0; i < NUM_SERVICE_PROVIDERS; i++)
	{
		globals.serviceProviderManager.serviceProviders[i].serviceID = SERVICE_PROVIDER_AVAILABLE;
		for(j = 0; j < SERVICE_COMMUNICATIONS_MAILBOXES_SIZE; j++)
			globals.serviceProviderManager.serviceProviders[i].communicationsMailbox[j].subject = MAIL_NO_MESSAGE;
	}

	// Initialize the MPI variables
	for(i = 0; i < MAX_MPI_TRANSFERS; i++)
		globals.mpiManager.receiveTransfers[i].transferInfo.destinationServiceTag = SERVICE_TAG_UNDEFINED;
	for(i = 0; i < MPI_MAX_NON_BLOCKING_RECEIVES; i++)
		globals.mpiManager.nonBlockingReceives[i].buf = NULL;

	// Initialize the routing variables
	globals.routing.operatingInLimpMode = true;
#ifdef IS_ROOT
	globals.routing.graphInfo.numEdges = 0;
	globals.routing.graphInfo.numNodes = 0;
	globals.routing.graphInfo.treeHeight = 0;
#endif
	for(i = 0; i < MAX_NUM_NODES; i++)
	{
		globals.routing.graphInfo.adjacencyMatrix[i].node1 = 0;
		globals.routing.graphInfo.adjacencyMatrix[i].node2 = 0;
		globals.routing.graphInfo.adjacencyMatrix[i].node3 = 0;
		globals.routing.graphInfo.adjacencyMatrix[i].node4 = 0;
		globals.routing.graphInfo.adjacencyMatrix[i].node5 = 0;
		globals.routing.graphInfo.adjacencyMatrix[i].node6 = 0;
		globals.routing.graphInfo.adjacencyMatrix[i].node7 = 0;
		globals.routing.graphInfo.adjacencyMatrix[i].node8 = 0;
		globals.routing.graphInfo.adjacencyMatrix[i].node9 = 0;
		globals.routing.graphInfo.adjacencyMatrix[i].node10 = 0;
		globals.routing.graphInfo.adjacencyMatrix[i].node11 = 0;
		globals.routing.graphInfo.adjacencyMatrix[i].node12 = 0;
		globals.routing.graphInfo.adjacencyMatrix[i].node13 = 0;
		globals.routing.graphInfo.adjacencyMatrix[i].node14 = 0;
		globals.routing.graphInfo.adjacencyMatrix[i].node15 = 0;
		globals.routing.graphInfo.adjacencyMatrix[i].node16 = 0;
	}

#if defined(IS_ROOT)

	globals.routing.numRouterNeighborsCollected = 1;	// We set this to 1 because the root is already here
	globals.routing.expectedNumNodes = (gpioDataRegisters.GPBDAT.bit.SWITCH1 << 3) + (gpioDataRegisters.GPBDAT.bit.SWITCH2 << 2) + 
			(gpioDataRegisters.GPADAT.bit.SWITCH3 << 1) + (gpioDataRegisters.GPADAT.bit.SWITCH4);
	globals.routing.routingTableGenerated = false;
	for(i = 0; i < MAX_NUM_NODES; i++)
		globals.routing.limpModePaths[i] = 0;

#elif defined(IS_ROUTER)

	globals.routing.limpModePath = 0;

#endif

	// Initialize the statistics structure
	globals.statistics.memory.maxMemoryUsed = 0;
	globals.statistics.memory.maxMemoryAllocations = 0;
	globals.statistics.memory.numFailedMemoryAllocs = 0;
	globals.statistics.que.maxInboundQueLoad = 0;
	globals.statistics.que.currentInboundQueLoad = 0;
	globals.statistics.que.inboundQueFullCount = 0;
	for(i = 0; i < NUM_PORTS; i++)
	{
		globals.statistics.que.maxOutboundQueLoad[i] = 0;
		globals.statistics.que.currentOutboundQueLoad[i] = 0;
		globals.statistics.que.outboundQueFullCount[i] = 0;
		globals.statistics.virtualChannel.maxVirtualChannelLoad[i] = 0;
		globals.statistics.virtualChannel.currentVirtualChannelLoad[i] = 0;
		globals.statistics.flit.successfullyTransmittedOnPort[i] = false;
		globals.statistics.flit.numFlitsSent[i] = 0;
		globals.statistics.flit.numFlitsReceived[i] = 0;
		globals.statistics.flit.numSpiCollisions[i] = 0;
		globals.statistics.flit.numBlockedCollisions[i] = 0;
	}
	globals.statistics.flit.numHeaderFlitsSent = 0;
	globals.statistics.flit.numHeaderFlitsReceived = 0;
	globals.statistics.flit.numDataPayloadFlitsSent = 0;
	globals.statistics.flit.numDataPayloadFlitsReceived = 0;
	globals.statistics.flit.numDataPayloadEndFlitsSent = 0;
	globals.statistics.flit.numDataPayloadEndFlitsReceived = 0;
	globals.statistics.flit.numSpiArbitrationsFailed = 0;
	globals.statistics.packet.numControlPacketsSent = 0;
	globals.statistics.packet.numControlPacketsReceived = 0;
	globals.statistics.packet.numControlCrcErrors = 0;
	globals.statistics.packet.numDataTransfersSetup = 0;
	globals.statistics.packet.numDataTransfersSucceeded = 0;
	globals.statistics.packet.numDataTransfersExpired = 0;
	globals.statistics.packet.numDataTransfersFailed = 0;
	globals.statistics.packet.numDirectTransfersSetup = 0;
	globals.statistics.packet.numDirectTransfersSucceeded = 0;
	globals.statistics.packet.numDirectTransfersFailed = 0;
	globals.statistics.packet.numDirectTransfersExpired = 0;
	globals.statistics.packet.numDirectBuffersSetup = 0;
	globals.statistics.packet.numDirectBuffersSucceeded = 0;
	globals.statistics.packet.numDirectBuffersExpired = 0;
	globals.statistics.packet.numDataCrcErrors = 0;
	globals.statistics.packet.numPacketErrors = 0;
	globals.statistics.virtualChannel.numVirtualChannelsExpired = 0;
	globals.statistics.system.ticksPassed = 0;
	globals.statistics.system.numWarnings = 0;
	globals.statistics.system.numErrors = 0;

#if PROFILE_TEST_SERVICE == true
	globals.statistics.profiling.minTestServiceTime = DBL_MAX;
	globals.statistics.profiling.maxTestServiceTime = DBL_MIN;
	globals.statistics.profiling.avgTestServiceTime = 0;
	globals.statistics.profiling.numTestServiceSamples = 0;
#endif
#if PROFILE_TRANSMIT_TIME == true
	globals.statistics.profiling.minTxTime = DBL_MAX;
	globals.statistics.profiling.maxTxTime = DBL_MIN;
	globals.statistics.profiling.avgTxTime = 0;
	globals.statistics.profiling.numTxSamples = 0;
#endif
#if PROFILE_HEADER_HANDLING == true
	globals.statistics.profiling.minHeaderHandlingTime = DBL_MAX;
	globals.statistics.profiling.maxHeaderHandlingTime = DBL_MIN;
	globals.statistics.profiling.avgHeaderHandlingTime = 0;
	globals.statistics.profiling.numHeaderHandlingSamples = 0;
#endif
#if PROFILE_PAYLOAD_HANDLING == true
	globals.statistics.profiling.minPayloadHandlingTime = DBL_MAX;
	globals.statistics.profiling.maxPayloadHandlingTime = DBL_MIN;
	globals.statistics.profiling.avgPayloadHandlingTime = 0;
	globals.statistics.profiling.numPayloadHandlingSamples = 0;
#endif
#if PROFILE_INTERRUPTS == true
	globals.statistics.profiling.minInterruptsTime = DBL_MAX;
	globals.statistics.profiling.maxInterruptsTime = DBL_MIN;
	globals.statistics.profiling.avgInterruptsTime = 0;
	globals.statistics.profiling.numInterruptsSamples = 0;
#endif
#if PROFILE_LATENCY == true
	globals.statistics.profiling.minLatency = DBL_MAX;
	globals.statistics.profiling.maxLatency = DBL_MIN;
	globals.statistics.profiling.avgLatency = 0;
	globals.statistics.profiling.numLatencySamples = 0;
#endif
}
