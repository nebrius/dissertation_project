// This file is the main "collection" of include files and such. It should be included in every
// custom source file. It should be left out of the pre-existing files for consistency

#ifndef _SYSTEM_H
#define _SYSTEM_H

#include<std.h>
#include<log.h>
#include<trc.h>
#include<mem.h>
#include<tsk.h>
#include<idl.h>
#include<clk.h>
#include<float.h>
#include<limits.h>

extern Int MSARAM;
extern Int LSARAM;
extern Int H0SARAM;

#include"NodeType.h"
#if defined(IS_ROOT)
	#include"root_interconnectcfg.h"
#elif defined(IS_ROUTER)
	#include"router_interconnectcfg.h"
#else
	#error Node type was not defined. Define with IS_ROOT or IS_ROUTER in NodeType.h
#endif
#include"System_Preferences.h"
#include"Hardware_F2808Registers.h"
#include"Hardware_SevenSegment.h"
#include"Hardware_Spi.h"
#include"mpi.h"
#include"Protocol_PacketHandeling.h"
#include"Protocol_Actions.h"
#include"Protocol_Routing.h"
#include"Processing_DirectTransfer.h"
#include"Processing_Que.h"
#include"Processing_TransmitPacket.h"
#include"Service_Registration.h"
#include"Service_TestService.h"
#include"System_MemoryManager.h"
#include"System_FollowUpMonitor.h"
#include"System_Symbols.h"
#include"System_Globals.h"
#include"System_Utilities.h"
#include"Protocol_CommandDiscoverNeighbors.h"
#include"Protocol_CommandRequestAddress.h"
#include"Protocol_CommandReportNeighbors.h"
#include"Protocol_CommandRequestRoutingTable.h"
#include"Protocol_CommandCommFailure.h"
#include"Protocol_CommandnewRoutingTableAvailable.h"
#include"Protocol_CommandDataTransfer.h"
#include"Protocol_CommandMPIControl.h"

#endif
