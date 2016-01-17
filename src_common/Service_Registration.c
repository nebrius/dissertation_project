#include"System.h"

#pragma CODE_SECTION(RegisterServiceProvider,"FlashCode");
Uint16 RegisterServiceProvider(Uint16 serviceTag, Uint16 taskPriority, TSK_Handle serviceTask, SEM_Handle serviceSem)
{
	Uint16 i, j;

	// Sanity check
	if(taskPriority >= TSK_MAXPRI || taskPriority < TSK_MINPRI)
	{
		LogError(__FILE__,__LINE__);
		return ERROR;
	}

	// Find an available service provider
	for(i = 0; i < NUM_SERVICE_PROVIDERS; i++)
	{
		// Check if this service provider is available
		if(globals.serviceProviderManager.serviceProviders[i].serviceID == SERVICE_PROVIDER_AVAILABLE)
		{
			// Store the service provider attributes
			globals.serviceProviderManager.serviceProviders[i].serviceTag = serviceTag;
			globals.serviceProviderManager.serviceProviders[i].serviceID = i + 1;
			globals.serviceProviderManager.serviceProviders[i].taskPriority = taskPriority;
			globals.serviceProviderManager.serviceProviders[i].serviceTask = serviceTask;
			globals.serviceProviderManager.serviceProviders[i].serviceSem = serviceSem;

			// Initialize the mailbox
			for(j = 0; j < SERVICE_COMMUNICATIONS_MAILBOXES_SIZE; j++)
				globals.serviceProviderManager.serviceProviders[i].communicationsMailbox[j].data = NULL;

			// Return the service id
			return globals.serviceProviderManager.serviceProviders[i].serviceID;
		}
	}

	// If it got here, then there isn't an available service provider slot
	LogError(__FILE__,__LINE__);
	return ERROR;
}

#pragma CODE_SECTION(StartServiceProvider,"FlashCode");
Uint16 StartServiceProvider(Uint16 serviceID)
{
	Uint16 i;

	// Find the service provider and raise it's task priority
	for(i = 0; i < NUM_SERVICE_PROVIDERS; i++)
	{
		if(globals.serviceProviderManager.serviceProviders[i].serviceID == serviceID)
		{
			if(SEM_count(globals.serviceProviderManager.serviceProviders[i].serviceSem) == 0)
				SEM_post(globals.serviceProviderManager.serviceProviders[i].serviceSem);
			return SUCCESS;
		}
	}

	// If it got here, then the service provider couldn't be found
	LogError(__FILE__,__LINE__);
	return ERROR;
}

#pragma CODE_SECTION(GetServiceIDFromServiceTag,"FlashCode");
Uint16 GetServiceIDFromServiceTag(Uint16 serviceTag)
{
	Uint16 i;

	// Check each service provider against the supplied serviceTag
	for(i = 0; i < NUM_SERVICE_PROVIDERS; i++)
	{
		if(globals.serviceProviderManager.serviceProviders[i].serviceTag == serviceTag)
			return globals.serviceProviderManager.serviceProviders[i].serviceID;
	}

	LogError(__FILE__,__LINE__);
	return ERROR;
}

#pragma CODE_SECTION(GetMyServiceTag,"FlashCode");
Uint16 GetMyServiceTag()
{
	Uint16 i;

	// Check each service provider's task handle against this task's handle
	for(i = 0; i < NUM_SERVICE_PROVIDERS; i++)
	{
		if(globals.serviceProviderManager.serviceProviders[i].serviceTask == TSK_self())
			return globals.serviceProviderManager.serviceProviders[i].serviceID;
	}

	// If it got here, then it couldn't find the service ID
	LogError(__FILE__,__LINE__);
	return ERROR;
}

#pragma CODE_SECTION(SendMail,"FlashCode");
Uint16 SendMail(Uint16 serviceID, struct Mail* message)
{
	Uint16 i, j;

	// Check each service provider's serviceID against the provided serviceID
	for(i = 0; i < NUM_SERVICE_PROVIDERS; i++)
	{
		if(globals.serviceProviderManager.serviceProviders[i].serviceID == serviceID)
		{
			// Find an empty mailboc
			for(j = 0; j < SERVICE_COMMUNICATIONS_MAILBOXES_SIZE; j++)
			{
				if(globals.serviceProviderManager.serviceProviders[i].communicationsMailbox[j].subject == MAIL_NO_MESSAGE)
				{
					// Save the message
					CopyMessage(message,&globals.serviceProviderManager.serviceProviders[i].communicationsMailbox[j]);

					// If the task is sleeping, wake it up
					if(SEM_count(globals.serviceProviderManager.serviceProviders[i].serviceSem) == 0)
						SEM_post(globals.serviceProviderManager.serviceProviders[i].serviceSem);

					return SUCCESS;
				}
			}

			// If it got here, it couldn't find an empty mailbox
			LogError(__FILE__,__LINE__);
			return ERROR;
		}
	}

	// If it got here, it couldn't find the service provider
	LogError(__FILE__,__LINE__);
	return ERROR;
}

#pragma CODE_SECTION(FetchMail,"FlashCode");
Uint16 FetchMail(Uint16 source, Uint16 destination, Uint16 subject, MPI_Request* request, struct Mail* message)
{
	Uint16 i, j, messageIndex, serviceID;

	// Fetch the service ID
	serviceID = GetServiceIDFromServiceTag(GetMyServiceTag());

	// Check each service provider's serviceID against the provided serviceID
	for(i = 0; i < NUM_SERVICE_PROVIDERS; i++)
	{
		if(globals.serviceProviderManager.serviceProviders[i].serviceID == serviceID)
		{
			// Fetch the data
			message->subject = MAIL_NO_MESSAGE;
			for(j = 0; j < SERVICE_COMMUNICATIONS_MAILBOXES_SIZE; j++)
			{
				if((source == MPI_ANY_SOURCE || 
						globals.serviceProviderManager.serviceProviders[i].communicationsMailbox[j].transmissionInfo.source == source) &&
					(destination == MPI_ANY_DESTINATION || 
						globals.serviceProviderManager.serviceProviders[i].communicationsMailbox[j].transmissionInfo.destination == destination) &&
					(subject >= MAIL_ANY_SUBJECT || 
						globals.serviceProviderManager.serviceProviders[i].communicationsMailbox[j].subject == subject) &&
					(request->requestType == MPI_ANY_REQUEST->requestType || 
						globals.serviceProviderManager.serviceProviders[i].communicationsMailbox[j].request.requestID == request->requestID))
				{
					CopyMessage(&globals.serviceProviderManager.serviceProviders[i].communicationsMailbox[j],message);
					messageIndex = j;
					break;
				}
			}

			// Shift the other elements towards the front to keep it FIFO
			if(message->subject != MAIL_NO_MESSAGE)
			{
				if(messageIndex == NUM_SERVICE_PROVIDERS - 1)
					globals.serviceProviderManager.serviceProviders[i].communicationsMailbox[messageIndex].subject = MAIL_NO_MESSAGE;
				else
				{
					for(j = messageIndex + 1; j < NUM_SERVICE_PROVIDERS; j++)
					{
						CopyMessage(&globals.serviceProviderManager.serviceProviders[i].communicationsMailbox[j],
							&globals.serviceProviderManager.serviceProviders[i].communicationsMailbox[j - 1]);
					}
					globals.serviceProviderManager.serviceProviders[i].communicationsMailbox[NUM_SERVICE_PROVIDERS - 1].subject = MAIL_NO_MESSAGE;
				}
			}
			else
			{
				// If there were no messages, create a no mail message
				CreateMessage(message,
					NULL,
					0,
					MAIL_NO_MESSAGE,
					PACKET_ID_UNDEFINED,
					MPI_ANY_REQUEST->requestType,
					MPI_ANY_REQUEST->requestID,
					globals.protocol.address,
					globals.protocol.address,
					GetMyServiceTag(),
					GetMyServiceTag());
			}

			return SUCCESS;
		}
	}

	// If it got here, it couldn't find the service provider
	LogError(__FILE__,__LINE__);
	return ERROR;
}


#pragma CODE_SECTION(CreateMessage,"FlashCode");
void CreateMessage(struct Mail* message, Uint16* data, Uint16 dataLength, Uint16 subject, Uint16 packetID, Uint16 requestType, Uint16 requestID,
	Uint16 source, Uint16 destination, Uint16 sourceServiceTag, Uint16 destinationServiceTag)
{
	message->data = data;
	message->dataLength = dataLength;
	message->subject = subject;
	message->packetID = packetID;
	message->request.requestType = requestType;
	message->request.requestID = requestID;
	message->transmissionInfo.source = source;
	message->transmissionInfo.destination = destination;
	message->transmissionInfo.sourceServiceTag = sourceServiceTag;
	message->transmissionInfo.destinationServiceTag = destinationServiceTag;
}

#pragma CODE_SECTION(CopyMessage,"FlashCode");
void CopyMessage(struct Mail* sourceMessage, struct Mail* destinationMessage)
{
	destinationMessage->data = sourceMessage->data;
	destinationMessage->dataLength = sourceMessage->dataLength;
	destinationMessage->subject = sourceMessage->subject;
	destinationMessage->packetID = sourceMessage->packetID;
	destinationMessage->request.requestType = sourceMessage->request.requestType;
	destinationMessage->request.requestID = sourceMessage->request.requestID;
	destinationMessage->transmissionInfo.source = sourceMessage->transmissionInfo.source;
	destinationMessage->transmissionInfo.destination = sourceMessage->transmissionInfo.destination;
	destinationMessage->transmissionInfo.sourceServiceTag = sourceMessage->transmissionInfo.sourceServiceTag;
	destinationMessage->transmissionInfo.destinationServiceTag = sourceMessage->transmissionInfo.destinationServiceTag;
}
