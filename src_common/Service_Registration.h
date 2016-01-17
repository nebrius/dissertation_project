#ifndef _SERVICE_REGISTRATION_H
#define _SERVICE_REGISTRATION_H

#include"System.h"

struct Mail
{
	Uint16* data;
	Uint16 dataLength;
	Uint16 subject;
	Uint16 packetID;
	MPI_Request request;
	struct
	{
		Uint32 source:8;
		Uint32 destination:8;
		Uint32 sourceServiceTag:8;
		Uint32 destinationServiceTag:8;
	} transmissionInfo;
};

struct ServiceProvider
{
	Uint16 serviceID;
	Uint16 serviceTag;
	Uint16 taskPriority;
	SEM_Handle serviceSem;
	TSK_Handle serviceTask;
	Fxn entrypointFunction;
	struct Mail communicationsMailbox[SERVICE_COMMUNICATIONS_MAILBOXES_SIZE];
};

Uint16 RegisterServiceProvider(Uint16 serviceTag, Uint16 taskPriority, TSK_Handle serviceTask, SEM_Handle serviceSem);
Uint16 StartServiceProvider(Uint16 serviceID);
Uint16 StopServiceProvider(Uint16 serviceID);
Uint16 GetServiceIDFromServiceTag(Uint16 serviceTag);
Uint16 GetMyServiceTag();
Uint16 SendMail(Uint16 serviceID, struct Mail* message);
Uint16 FetchMail(Uint16 source, Uint16 destination, Uint16 subject, MPI_Request* request, struct Mail* message);
void CreateMessage(struct Mail* message, Uint16* data, Uint16 dataLength, Uint16 subject, Uint16 packetID, Uint16 requestType, Uint16 requestID,
	Uint16 source, Uint16 destination, Uint16 sourceServiceTag, Uint16 destinationServiceTag);
void CopyMessage(struct Mail* sourceMessage, struct Mail* destinationMessage);
#endif
