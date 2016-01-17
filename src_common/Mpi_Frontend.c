#include"System.h"

Uint16 currentMPIRequestID = 0;
MPI_Request MPI_ANY_REQUEST_STRUCT = {MPI_REQUEST_NONE,0};
MPI_Request* MPI_ANY_REQUEST = &MPI_ANY_REQUEST_STRUCT;

// Note 1: MPI definitions and descriptions from https://computing.llnl.gov/tutorials/mpi/
// Note 2: comm_rank is defined here as the address minus 2. All frontend functions handle the conversion to addresses
// Note 3: Only MPI_COMM_WORLD is supported. Other communicators are ignored (aka treated as MPI_COMM_WORLD).

/*
Description

This subroutine initializes MPI. All MPI programs must call MPI_INIT before
any other MPI routine (with the exception of MPI_INITIALIZED). More than
one call to MPI_INIT by any task is erroneous.
*/
#pragma CODE_SECTION(MPI_Init,"FlashCode");
int MPI_Init(int *argc,char ***argv)
{
	// Synchronize here since some nodes will get here much later than other nodes
	MPI_Barrier(MPI_COMM_WORLD);
	return SUCCESS;
}

/*
Description

This subroutine returns the rank of the local task in the group associated
with a communicator.

You can use this subroutine with MPI_COMM_SIZE to determine the amount of
concurrency available for a specific job. MPI_COMM_RANK indicates the rank
of the task that calls it in the range from 0...size - 1, where size is the
return value of MPI_COMM_SIZE.

This subroutine is a shortcut to accessing the communicator's group with
MPI_COMM_GROUP, computing the rank using MPI_GROUP_RANK and freeing the
temporary group by using MPI_GROUP_FREE.

If comm is an intercommunicator, rank is the rank of the local task in the
local group

Parameters

comm
     is the communicator (handle) (IN)

rank
     is an integer specifying the rank of the calling task in group of comm
     (OUT)
.
*/
#pragma CODE_SECTION(MPI_Comm_size,"FlashCode");
int MPI_Comm_size(MPI_Comm comm,int *size)
{
	return globals.routing.partitionTree[0].nodeInformation.numGraphNodesInTreeNode - 1;
}

/*
Description

This subroutine returns the size of the group associated with a
communicator. MPI_COMM_SIZE is a shortcut to:

   * accessing the communicator's group with MPI_COMM_GROUP,
   * computing the size using MPI_GROUP_SIZE, and
   * freeing the temporary group using MPI_GROUP_FREE.

If comm is an intercommunicator, size will be the size of the local group.
To determine the size of the remote group of an intercommunicator, use
MPI_COMM_REMOTE_SIZE.

You can use this subroutine with MPI_COMM_RANK to determine the amount of
concurrency available for a specific library or program. MPI_COMM_RANK
indicates the rank of the task that calls it in the range from
0...size - 1, where size is the return value of MPI_COMM_SIZE.
The rank and size information can then be used to partition work
across the available tasks.

Parameters

comm
     is the communicator (handle) (IN)

size
     is an integer specifying the number of tasks in the group of comm
     (OUT)
*/
#pragma CODE_SECTION(MPI_Comm_rank,"FlashCode");
int MPI_Comm_rank(MPI_Comm comm,int *rank)
{
	*rank = globals.protocol.address - 2;
	return *rank;
}

/*
Description

Make sure this subroutine is the last MPI call. Any MPI calls made after
MPI_FINALIZE raise an error. You must be sure that all pending
communications involving a task have completed before the task calls
MPI_FINALIZE. You must also be sure that all files opened by the task have
been closed before the task calls MPI_FINALIZE.

Although MPI_FINALIZE terminates MPI processing, it does not terminate the
task. It is possible to continue with non-MPI processing after calling
MPI_FINALIZE, but no other MPI calls (including MPI_INIT) can be made.

In a threads environment, both MPI_INIT and MPI_FINALIZE must be called on
the same thread. MPI_FINALIZE closes the communication library and
terminates the service threads. It does not affect any threads you created,
other than returning an error if one subsequently makes an MPI call. If you
had registered a SIGIO handler, it is restored as a signal handler;
however, the SIGIO signal is blocked when MPI_FINALIZE returns. If you want
to catch SIGIO after MPI_FINALIZE has been called, you should unblock it.

At MPI_FINALIZE there is now an implicit MPI_COMM_FREE of MPI_COMM_SELF.
Because MPI_COMM_SELF cannot have been freed by user code and cannot be
used after MPI_FINALIZE, there is no direct effect of this change. The
value of this implicit free is that any attribute that a user may attach to
MPI_COMM_SELF will be deleted in MPI_FINALIZE and its attribute delete
function called. A library layered on MPI can take advantage of this to
force its own cleanup code to run whenever MPI_FINALIZE gets called. This
is done by packaging the cleanup logic as an attribute delete function and
attaching an attribute to MPI_COMM_SELF. It is legitimate to make MPI calls
in the attribute callbacks and a call to MPI_FINALIZED inside a delete
function will report that MPI is still active.

If an attribute delete function returns a non-zero return code, the code it
does return is passed to the error handler associated with MPI_COMM_WORLD.
The default handler, MPI_ERROR_ARE_FATAL, will embed the error code in the
message it prints. If there is a returning error handler on MPI_COMM_WORLD,
MPI_FINALIZE will return a code indicating that a delete callback failed.
MPI_FINALIZE does not return the error return code issued by the delete
function.
*/
#pragma CODE_SECTION(MPI_Finalize,"FlashCode");
int MPI_Finalize(void)
{
	return SUCCESS;
}

/*
Description

This subroutine is a blocking standard mode send operation. MPI_SEND causes
count elements of type datatype to be sent from buf to the task specified
by dest. dest is a task rank that can be any value from 0 to (n-1), where n
is the number of tasks in comm.

Parameters

buf
     is the initial address of the send buffer (choice) (IN)
count
     is the number of elements in the send buffer (non-negative integer)
     (IN)
datatype
     is the datatype of each send buffer element (handle) (IN)
dest
     is the rank of the destination task in comm(integer) (IN)
tag
     is the message tag (positive integer) (IN)
comm
     is the communicator (handle) (IN)
*/
#pragma CODE_SECTION(MPI_Send,"FlashCode");
int MPI_Send(void* buf,int count,MPI_Datatype datatype,int dest,int tag,MPI_Comm comm)
{
	Uint16 i, connectionEstablished = false, dataStillTransferring = true, serviceTag = GetMyServiceTag();
	Uint16 numPartitions = 0, partitionsLeft, connectionPacketID;
	Uint16 mpiSetupData[MPI_MAX_DATA_PARTITIONS * 2], packetIDs[MPI_MAX_DATA_PARTITIONS], dataPartitionLengths[MPI_MAX_DATA_PARTITIONS];
	Uint16 *dataPartitions[MPI_MAX_DATA_PARTITIONS];
	struct Packet newPacket;
	struct Mail message;

	// Translate the MPI rank to an address
	dest += 2;

	// Partition the data into 255 byte chunks
	for(i = 0; i < MPI_MAX_DATA_PARTITIONS; i++)
	{
		if(count >= i * 255)
		{
			// Set the partition length
			if(count - i * 255 > 255)
				dataPartitionLengths[i] = 255;
			else
				dataPartitionLengths[i] = count - i * 255;

			// Set the data pointer
			dataPartitions[i] = &(((Uint16*)buf)[i * 255]);

			// Update the number of partitions
			numPartitions++;
		}
		else
		{
			dataPartitionLengths[i] = 0;
			dataPartitions[i] = NULL;
		}
	}
	partitionsLeft = numPartitions;

	// Generate the packetIDs
	for(i = 0; i < numPartitions; i++)
		packetIDs[i] = globals.protocol.transmitCounts++;
	for(i = numPartitions; i < MPI_MAX_DATA_PARTITIONS; i++)
		packetIDs[i] = 0;

	// Generate the MPI connection setup data payload
	for(i = 0; i < MPI_MAX_DATA_PARTITIONS; i++)
	{
		mpiSetupData[2 * i] = packetIDs[i];
		mpiSetupData[2 * i + 1] = dataPartitionLengths[i];
	}
		
	// Send the MPI transfer initialization command
	InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
	
	// Create the data transfer packet
	newPacket.a.communicationType = COMM_TYPE_UNICAST;
	newPacket.transmissionInfo.destination = dest;
	newPacket.b.command = COMMAND_DATA_TRANSFER;
	newPacket.b.packetSequenceStep = SEQUENCE_DATA_TRANSFER_REQUEST_TRANSFER;
	newPacket.c.sourceServiceTag = serviceTag;
	newPacket.c.destinationServiceTag = serviceTag;

	// Set the data
	newPacket.dataBuffer = mpiSetupData;
	newPacket.dataBufferInfo.dataBufferLength = MPI_MAX_DATA_PARTITIONS * 2;

	// Send the data packet
	SendDataPacket(&newPacket);

	// Set the status
	connectionPacketID = newPacket.a.packetID;

	//Establish the connection
	while(!connectionEstablished)
	{
		// Check for a successful ack
		FetchMail(dest,MPI_ANY_DESTINATION,MAIL_DATA_TRANSFER_SUCCESS,MPI_ANY_REQUEST,&message);
		if(message.subject != MAIL_NO_MESSAGE && message.packetID == connectionPacketID)
			connectionEstablished = true;

		FetchMail(MPI_ANY_SOURCE,MPI_ANY_DESTINATION,MAIL_DATA_TRANSFER_FAILED,MPI_ANY_REQUEST,&message);
		if(message.subject != MAIL_NO_MESSAGE && message.packetID == connectionPacketID)
		{
			// Send the MPI transfer initialization command
			InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
			
			// Create the data transfer packet
			newPacket.a.communicationType = COMM_TYPE_UNICAST;
			newPacket.transmissionInfo.destination = dest;
			newPacket.b.command = COMMAND_DATA_TRANSFER;
			newPacket.b.packetSequenceStep = SEQUENCE_DATA_TRANSFER_REQUEST_TRANSFER;
			newPacket.c.sourceServiceTag = serviceTag;
			newPacket.c.destinationServiceTag = serviceTag;

			// Set the data
			newPacket.dataBuffer = mpiSetupData;
			newPacket.dataBufferInfo.dataBufferLength = MPI_MAX_DATA_PARTITIONS * 2;

			// Send the data packet
			SendDataPacket(&newPacket);

			// Set the status
			connectionPacketID = newPacket.a.packetID;
		}
	}

	// Send each data partition separately
	for(i = 0; i < numPartitions; i++)
	{
		// Transfer the data
		InitializePacket(&newPacket, packetIDs[i]);
		
		// Create the data transfer packet
		newPacket.a.communicationType = COMM_TYPE_UNICAST;
		newPacket.transmissionInfo.destination = dest;
		newPacket.b.command = COMMAND_DATA_TRANSFER;
		newPacket.b.packetSequenceStep = SEQUENCE_DATA_TRANSFER_REQUEST_TRANSFER;
		newPacket.c.sourceServiceTag = serviceTag;
		newPacket.c.destinationServiceTag = serviceTag;

		// Set the data
		newPacket.dataBuffer = dataPartitions[i];
		newPacket.dataBufferInfo.dataBufferLength = dataPartitionLengths[i];

		// Send the data packet
		SendDataPacket(&newPacket);
	}

	// Send the data packets
	while(dataStillTransferring)
	{
		// Check mail to see if any packets have been received and mark said packets
		FetchMail(dest,MPI_ANY_DESTINATION,MAIL_DATA_TRANSFER_SUCCESS,MPI_ANY_REQUEST,&message);
		if(message.subject != MAIL_NO_MESSAGE)
		{
			for(i = 0; i < numPartitions; i++)
			{
				// If the packet IDs match, mark the transfer as complete
				if(packetIDs[i] == message.packetID)
				{
					// Decrease the number of partitions left to be transferred
					partitionsLeft--;

					// Mark the packet as complete
					packetIDs[i] = PACKET_ID_UNDEFINED;

					// If this transfer is done, mail the service provider
					if(partitionsLeft == 0)
						dataStillTransferring = false;

					break;
				}
			}
		}

		// Check mail to see if any packets have been received and mark said packets
		FetchMail(MPI_ANY_SOURCE,MPI_ANY_DESTINATION,MAIL_DATA_TRANSFER_FAILED,MPI_ANY_REQUEST,&message);
		if(message.subject != MAIL_NO_MESSAGE)
		{
			// Send each data partition separately
			for(i = 0; i < numPartitions; i++)
			{
				// If this packet failed to transmit properly, resend it
				if(packetIDs[i] == message.packetID)
				{
					// Transfer the data
					InitializePacket(&newPacket, packetIDs[i]);
					
					// Create the data transfer packet
					newPacket.a.communicationType = COMM_TYPE_UNICAST;
					newPacket.transmissionInfo.destination = dest;
					newPacket.b.command = COMMAND_DATA_TRANSFER;
					newPacket.b.packetSequenceStep = SEQUENCE_DATA_TRANSFER_REQUEST_TRANSFER;
					newPacket.c.sourceServiceTag = serviceTag;
					newPacket.c.destinationServiceTag = serviceTag;

					// Set the data
					newPacket.dataBuffer = dataPartitions[i];
					newPacket.dataBufferInfo.dataBufferLength = dataPartitionLengths[i];

					// Send the data packet
					SendDataPacket(&newPacket);

					break;
				}
			}
		}
	}

	// Return success
	return SUCCESS;
}

/*
Description

MPI_RECV is a blocking receive operation. The receive buffer is storage
containing room for count consecutive elements of the type specified by
datatype, starting at address buf.

The message received must be less than or equal to the length of the
receive buffer. If all incoming messages do not fit without truncation, an
overflow error occurs. If a message arrives that is shorter than the
receive buffer, then only those locations corresponding to the actual
message are changed.

Passing MPI_STATUS_IGNORE for the status argument causes PE MPI to skip
filling in the status fields. By passing this value for status, you can
avoid having to allocate a status object in programs that do not need to
examine the status fields.

Parameters

buf
     is the initial address of the receive buffer (choice) (OUT)
count
     is the number of elements to be received (integer) (IN)
datatype
     is the datatype of each receive buffer element (handle) (IN)
source
     is the rank of the source task in comm or MPI_ANY_SOURCE (integer)
     (IN)
tag
     is the message tag or MPI_ANY_TAG (positive integer) (IN)
comm
     is the communicator (handle) (IN)
status
     is the status object (Status) (INOUT). Note that in FORTRAN a single
     status object is an array of integers.
*/
#pragma CODE_SECTION(MPI_Recv,"FlashCode");
int MPI_Recv(void** buf,int count,MPI_Datatype datatype,int source,int tag,MPI_Comm comm,MPI_Status *status)
{
	struct Mail message;
	
	// Check if we are receiving from a specific node or from any node
	if(source != MPI_ANY_SOURCE)
		source += 2;
	
	// Keep looping until the message is received
	while(true)
	{
		SEM_pend(&TestServiceSem,SYS_FOREVER);
		FetchMail(source,MPI_ANY_DESTINATION,MAIL_NEW_DATA_TRANSFER_AVAILABLE,MPI_ANY_REQUEST,&message);
		if(message.subject != MAIL_NO_MESSAGE)
			break;
	}
	
	// Set the buffer pointer
	*buf = message.data;
	*status = message.dataLength;
	
	return SUCCESS;
}

/*
Description

This subroutine starts a nonblocking standard mode send. The send buffer
may not be modified until the request has been completed by MPI_WAIT,
MPI_TEST, or one of the other MPI wait or test functions.

Parameters

buf
     is the initial address of the send buffer (choice) (IN)
count
     is the number of elements in the send buffer (integer) (IN)
datatype
     is the datatype of each send buffer element (handle) (IN)
dest
     is the rank of the destination task in comm (integer) (IN)
tag
     is the message tag (positive integer) (IN)
comm
     is the communicator (handle) (IN)
request
     is the communication request (handle) (OUT)
*/
/*#pragma CODE_SECTION(MPI_Isend,"FlashCode");
int MPI_Isend(void* buf,int count,MPI_Datatype datatype,int dest,int tag,MPI_Comm comm,MPI_Request *request)
{
	struct Mail message;

	// Setup the request
	request->requestType = MPI_REQUEST_NON_BLOCKING_SEND;
	request->requestID = currentMPIRequestID++;
	request->mpiSource = MPI_ANY_SOURCE;
	request->mpiDestination= dest;

	// Create the message to send to the MPI service
	CreateMessage(&message, 
		(Uint16*)buf,
		count,
		MAIL_SEND_NEW_TRANSFER,
		PACKET_ID_UNDEFINED,
		request->requestType,
		request->requestID,
		globals.protocol.address,
		dest + 2,
		GetMyServiceTag(),
		GetMyServiceTag());

	// Send the message to the MPI service
	SendMail(GetServiceIDFromServiceTag(MPI_SEND_SERVICE_TAG),&message);

	// Return success
	return SUCCESS;
}*/

/*
Description

This subroutine starts a nonblocking receive and returns a handle to a
request object. You can later use the request to query the status of the
communication or wait for it to complete.

A nonblocking receive call means the system may start writing data into the
receive buffer. Once the nonblocking receive operation is called, do not
access any part of the receive buffer until the receive is complete.

Parameters

buf
     is the initial address of the receive buffer (choice) (OUT)
count
     is the number of elements in the receive buffer (integer) (IN)
datatype
     is the datatype of each receive buffer element (handle) (IN)
source
     is the rank of source or MPI_ANY_SOURCE (integer) (IN)
tag
     is the message tag or MPI_ANY_TAG (positive integer) (IN)
comm
     is the communicator (handle) (IN)
request
     is the communication request (handle) (OUT)
*/
#pragma CODE_SECTION(MPI_Irecv,"FlashCode");
int MPI_Irecv(void* buf,int count,MPI_Datatype datatype,int source,int tag,MPI_Comm comm,MPI_Request *request)
{
	Uint16 i;

	for(i = 0; i < MPI_MAX_NON_BLOCKING_RECEIVES; i++)
	{
		if(globals.mpiManager.nonBlockingReceives[i].buf == NULL)
		{
			// Setup the request
			request->requestType = MPI_REQUEST_NON_BLOCKING_RECEIVE;
			request->requestID = currentMPIRequestID++;
			request->mpiSource = source;
			request->mpiDestination= MPI_ANY_DESTINATION;

			// Set the follow up information
			globals.mpiManager.nonBlockingReceives[i].buf = buf;
			globals.mpiManager.nonBlockingReceives[i].source = source;
			globals.mpiManager.nonBlockingReceives[i].request.requestType = request->requestType;
			globals.mpiManager.nonBlockingReceives[i].request.requestID = request->requestID;
			globals.mpiManager.nonBlockingReceives[i].request.mpiSource = request->mpiSource;
			globals.mpiManager.nonBlockingReceives[i].request.mpiDestination = request->mpiDestination;
			return SUCCESS;
		}
	}
	
	return ERROR;
}

/*
Description

MPI_TEST returns flag = true if the operation identified by request is
complete. The status object is set to contain information on the completed
operation. The request object is deallocated and the request handle is set
to MPI_REQUEST_NULL. Otherwise, flag = false and the status object is
undefined. MPI_TEST is a local operation. The status object can be queried
for information about the operation. (See MPI_WAIT.)

You can call MPI_TEST with a null or inactive request argument. The
operation returns flag = true and empty status.

The error field of MPI_Status is never modified. The success or failure is
indicated by the return code only.

Passing MPI_STATUS_IGNORE for the status argument causes PE MPI to skip
filling in the status fields. By passing this value for status, you can
avoid having to allocate a status object in programs that do not need to
examine the status fields.

When one of the MPI wait or test calls returns status for a nonblocking
operation request and the corresponding blocking operation does not provide
a status argument, the status from this wait or test call does not contain
meaningful source, tag, or message size information.

When you use this subroutine in a threads application, make sure the
request is tested on only one thread. The request does not have to be
tested on the thread that created the request. See IBM Parallel Environment
for AIX: MPI Programming Guide for more information on programming with MPI
in a threads environment.

Parameters

request
     is the operation request (handle) (INOUT)
flag
     is true if the operation completed (logical) (OUT)
status
     is the status object (Status) (INOUT). Note that in FORTRAN a single
     status object is an array of integers.
*/
#pragma CODE_SECTION(MPI_Test,"FlashCode");
int MPI_Test(MPI_Request *request,int *flag,MPI_Status *status)
{
	Uint16 messageSubject, i;
	struct Mail message;
	MPI_Request localRequest;

	// Determine the mail message type to look for
	switch(request->requestType)
	{
		case MPI_REQUEST_NON_BLOCKING_SEND: messageSubject = MAIL_DATA_TRANSFER_SUCCESS; break;
		case MPI_REQUEST_NON_BLOCKING_RECEIVE: messageSubject = MAIL_NEW_DATA_TRANSFER_AVAILABLE; break;
		case MPI_REQUEST_BARRIER: messageSubject = MAIL_BARRIER_COMPLETE; break;
		default: messageSubject = MAIL_NO_MESSAGE; break;
	}

	// Make sure a valid request was sent
	if(messageSubject == MAIL_NO_MESSAGE)
		*flag = false;
	else
	{
		if(request->requestType == MPI_REQUEST_NON_BLOCKING_RECEIVE)
		{
			localRequest.requestType = MPI_ANY_REQUEST->requestType;
			localRequest.requestID = request->requestID;
			localRequest.mpiDestination = request->mpiDestination;
			localRequest.mpiSource = request->mpiSource;
			FetchMail(request->mpiSource + 2,MPI_ANY_DESTINATION,messageSubject,&localRequest,&message);
			*flag = (message.subject == messageSubject);
			
			if(*flag)
			{
				for(i = 0; i < MPI_MAX_NON_BLOCKING_RECEIVES; i++)
				{
					if(globals.mpiManager.nonBlockingReceives[i].request.requestType == request->requestType && 
						globals.mpiManager.nonBlockingReceives[i].request.requestID == request->requestID)
					{
						*status = message.dataLength;
						*(globals.mpiManager.nonBlockingReceives[i].buf) = message.data;
					}
				}
			}
		}
		else
		{
			FetchMail(MPI_ANY_SOURCE,MPI_ANY_DESTINATION,messageSubject,request,&message);
			*flag = (message.subject == messageSubject);
			*status = message.dataLength;
		}
	}
		
	return *flag;
}

/*
Description

This subroutine returns the number of elements in a message. The datatype
argument and the argument provided by the call that set the status variable
should match.

When one of the MPI wait or test calls returns status for a non-blocking
operation request and the corresponding blocking operation does not provide
a status argument, the status from this wait or test call does not contain
meaningful source, tag, or message size information.

Parameters

status
     is a status object (Status) (IN). Note that in FORTRAN a single status
     object is an array of integers.

datatype
     is the datatype of each message element (handle) (IN)

count
     is the number of elements (integer) (OUT)
*/
#pragma CODE_SECTION(MPI_Get_count,"FlashCode");
int MPI_Get_count(MPI_Status *status,MPI_Datatype datatype,int *count)
{
	*count = *status;
	return *status;
}

/*
Description

This subroutine blocks until all tasks have called it. Tasks cannot exit
the operation until all group members have entered.

When you use this subroutine in a threads application, make sure all
collective operations on a particular communicator occur in the same order
at each task. See IBM Parallel Environment for AIX: MPI Programming Guide
for more information on programming with MPI in a threads environment.

Parameters

comm
     is a communicator (handle) (IN)
*/
#pragma CODE_SECTION(MPI_Barrier,"FlashCode");
int MPI_Barrier(MPI_Comm comm)
{
	Uint16 barrierAcked = false;
	struct Packet newPacket;
	struct Mail message;

	// Signal the root node that this node has reached the barrier
	InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
	newPacket.a.communicationType = COMM_TYPE_UNICAST;
	newPacket.b.command = COMMAND_MPI_CONTROL;
	newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
	newPacket.c.sourceServiceTag = GetMyServiceTag();
	newPacket.c.destinationServiceTag = SERVICE_TAG_UNDEFINED;
	newPacket.transmissionInfo.destination = ROOT_ADDRESS;
	SendControlPacket(&newPacket);

	// Wait for the barrier
	while(true)
	{
		SEM_pend(&TestServiceSem,20);

		// Check and see if the barrier has been reached
		FetchMail(MPI_ANY_SOURCE,MPI_ANY_DESTINATION,MAIL_BARRIER_COMPLETE,MPI_ANY_REQUEST,&message);
		if(message.subject != MAIL_NO_MESSAGE)
			break;

		// Check and see if the barrier request was acked
		FetchMail(MPI_ANY_SOURCE,MPI_ANY_DESTINATION,MAIL_BARRIER_ACKED,MPI_ANY_REQUEST,&message);
		if(message.subject != MAIL_NO_MESSAGE)
			barrierAcked = true;

		// Check if it's time to resend the barrier message
		if(!barrierAcked)
		{
			InitializePacket(&newPacket, PACKET_ID_UNDEFINED);
			newPacket.a.communicationType = COMM_TYPE_UNICAST;
			newPacket.b.command = COMMAND_MPI_CONTROL;
			newPacket.b.packetSequenceStep = SEQUENCE_MPI_CONTROL;
			newPacket.c.sourceServiceTag = GetMyServiceTag();
			newPacket.c.destinationServiceTag = SERVICE_TAG_UNDEFINED;
			newPacket.transmissionInfo.destination = ROOT_ADDRESS;
			SendControlPacket(&newPacket);
		}
	}

	return SUCCESS;
}
