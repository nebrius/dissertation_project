/*   Do *not* directly modify this file.  It was    */
/*   generated by the Configuration Tool; any  */
/*   changes risk being overwritten.                */

/* INPUT root_interconnect.cdb */

/*  Include Header Files  */
#include <std.h>
#include <prd.h>
#include <hst.h>
#include <swi.h>
#include <tsk.h>
#include <log.h>
#include <sem.h>
#include <sts.h>

#ifdef __cplusplus
extern "C" {
#endif

extern PRD_Obj FollowUpMonitorPeriod;
extern PRD_Obj HeartbeatLEDPeriod;
extern PRD_Obj SevenSegmentPeriod;
extern HST_Obj RTA_fromHost;
extern HST_Obj RTA_toHost;
extern SWI_Obj PRD_swi;
extern SWI_Obj KNL_swi;
extern TSK_Obj TSK_idle;
extern TSK_Obj ProcessInboundFlitsTask;
extern TSK_Obj ProcessOutboundFlitsTask;
extern TSK_Obj FollowUpMonitorTask;
extern TSK_Obj TestServiceTask;
extern LOG_Obj LOG_system;
extern LOG_Obj LOG_error;
extern SEM_Obj FollowUpMonitorSem;
extern SEM_Obj ProcessInboundFlitsSem;
extern SEM_Obj ProcessOutboundFlitsSem;
extern SEM_Obj TestServiceSem;
extern STS_Obj IDL_busyObj;


#ifdef __cplusplus
}
#endif /* extern "C" */
