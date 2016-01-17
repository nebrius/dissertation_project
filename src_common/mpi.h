#ifndef _MPI_H
#define _MPI_H

#include"System.h"


struct MPI_Request_struct
{
	Uint16 requestType;
	Uint16 requestID;
	Uint16 mpiSource;
	Uint16 mpiDestination;
};

typedef struct MPI_Request_struct MPI_Request;
typedef Uint16 MPI_Status;
typedef Uint16 MPI_Comm;
typedef Uint16 MPI_Datatype;
extern MPI_Request* MPI_ANY_REQUEST;

// MPI Data Types
#define MPI_SHORT 1					// 16-bit
#define MPI_UNSIGNED_SHORT 2		// 16-bit unsigned integer
#define MPI_INT 3					// 32-bit integer
#define MPI_UNSIGNED 4				// 32-bit unsigned integer
#define MPI_LONG_LONG 5				// 64-bit integer
#define MPI_UNSIGNED_LONG_LONG 6	// 64-bit unsigned integer
#define MPI_FLOAT 7 				// 32-bit floating point
#define MPI_DOUBLE 8				// 64-bit floating point
#define MPI_LONG_DOUBLE 9			// 64-bit floating point

// Environment Management Functions
int MPI_Init(int *argc,char ***argv);
int MPI_Comm_size(MPI_Comm comm,int *size);
int MPI_Comm_rank(MPI_Comm comm,int *rank);
int MPI_Finalize(void);

// Send/Receive functions
int MPI_Send(void* buf,int count,MPI_Datatype datatype,int dest,int tag,MPI_Comm comm);
int MPI_Recv(void** buf,int count,MPI_Datatype datatype,int source,int tag,MPI_Comm comm,MPI_Status *status);
int MPI_Isend(void* buf,int count,MPI_Datatype datatype,int dest,int tag,MPI_Comm comm,MPI_Request *request);
int MPI_Irecv(void* buf,int count,MPI_Datatype datatype,int source,int tag,MPI_Comm comm,MPI_Request *request);
int MPI_Test(MPI_Request *request,int *flag,MPI_Status *status);
int MPI_Barrier(MPI_Comm comm);
int MPI_Get_count(MPI_Status *status,MPI_Datatype datatype,int *count);


#endif
