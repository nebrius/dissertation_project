// This file includes system files and defines various global settings

#ifndef _PREFERENCES_H
#define _PREFERENCES_H

// Select which test to run by uncommenting it
#define TEST TEST_MPI

// Debugging
#define PRINT_WARNINGS true
#define PRINT_ERRORS true
#define PRINT_ERROR_STACK false
#define PRINT_PACKET_ERRORS false

// Profiling
#define PROFILE_TRANSMIT_TIME false
#define PROFILE_TEST_SERVICE false
#define PROFILE_HEADER_HANDLING false
#define PROFILE_PAYLOAD_HANDLING false
#define PROFILE_INTERRUPTS false
#define PROFILE_LATENCY false

// Utilities parameters
#define NUM_ASYNC_TIMERS 8
#define CRC8_POLYNOMIAL 0x07
#define CRC8_INITIAL 0x00FF
#define CRC32_POLYNOMIAL 0x04C11DB7
#define CRC32_INITIAL 0xFFFFFFFF

// MPI parameters
#define MPI_SEND_DELAY 4000
#define MPI_MAX_DATA_PARTITIONS 8
#define MPI_MAX_SEND_ATTEMPTS 4
#define MAX_MPI_TRANSFERS 4
#define MPI_MAX_NON_BLOCKING_RECEIVES 8
#define MPI_BARRIER_TIMEOUT 2

// Service Provider Manager parameters
#define NUM_SERVICE_PROVIDERS 4
#define SERVICE_COMMUNICATIONS_MAILBOXES_SIZE 4
#define NUM_SERVICE_ID_LOOKUPS 4
#define SERVICE_PROVIDER_ID_LOOKUP_TIME_TO_LIVE 10

// Memory Manager Parameters
#define NUM_MEMORY_ALLOCATIONS 64
#define MEMORY_BUFFER_SIZE 0x500

// Task parameters
#define PROCESS_INBOUND_FLITS_TASK_PRIORITY 5
#define PROCESS_OUTBOUND_FLITS_TASK_PRIORITY 5
#define FOLLOW_UP_MONITOR_TASK_PRIORITY 4
#define TEST_SERVICE_TASK_PRIORITY 4
#define MPI_SEND_SERVICE_TASK_PRIORITY 4
#define MPI_RECEIVE_SERVICE_TASK_PRIORITY 4

// SPI Parameters
#define CHARACTER_LENGTH 15
#define NUM_CHARACTERS_IN_FLIT 9
#define ARBITRATION_DELAY_SCALE_FACTOR 350
#define GPIO_SETTLE_TIME 20
#define TRANSMIT_DELAY 1600
#define SPI_HOLD_IN_RESET_TIME 10
#define PACKET_BUFFER_SIZE 4
#define MAX_ARBITRATION_ATTEMPTS 5
#define MAX_DATA_PAYLOAD_SIZE 256
#define NUM_PORTS 4

// Processing parameters
#define INBOUND_FLIT_BUFFER_SIZE 128
#define OUTBOUND_FLIT_BUFFER_SIZE 8
#define NUM_VIRTUAL_DATA_CHANNELS 15
#define NUM_DATA_PACKET_LOOKUPS 15
#define TIME_TO_LIVE_DATA_TRANSFER 20
#define TIME_TO_LIVE_DIRECT_TRANSFER 24
#define TIME_TO_LIVE_DIRECT_BUFFER 24
#define VIRTUAL_CHANNEL_INITIAL_BLOCKED_COUNT 30
#define RESERVE_TIMEOUT 8
#define MAX_GRAPH_PARTITION_ITERATIONS 20

// Protocol parameters
#define MAX_NUM_NODES 9	// Note: set to actual num nodes + 1. increasing this above 9 will produce erratic behavior due to bit-field size limitations in structs and the header
#define TREE_NODE_UNDEFINED 63
#define CURRENT_PACKET_VERSION 1
#define ROOT_ADDRESS 1
#define PACKET_TIME_TO_LIVE 10
#define ROUTING_TABLE_TIME_TO_LIVE 300
#define NUM_PACKET_SNIFFERS 2
#define MAX_COMM_FAILURE_LEVEL 3
#define MAX_PATH_LENGTH 8

// Follow-up parameters
#define NUM_FOLLOW_UP_ITEMS 32
#define NEIGHBOR_FOLLOW_UP_RATE 25
#define NEIGHBOR_FOLLOW_UP_RATE_WITHOUT_ADDRESS 1
#define ADDRESS_REQUEST_FOLLOW_UP_TIME 1
#define REPORT_NEIGHBORS_FOLLOW_UP_RATE 1
#define DIRECT_DATA_CLEANUP_RATE 1
#define ENSURE_ROUTING_TABLE_DISTRIBUTION_FOLLOW_UP_RATE 5
#define ENSURE_BARRIER_REACHED_FOLLOW_UP_RATE 1

#endif
