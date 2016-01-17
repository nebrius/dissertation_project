// Definitions of various symbols

#ifndef SYMBOLS_H
#define SYMBOLS_H

// Test cases
#define TEST_SPI 1
#define TEST_PROTOCOL 2
#define TEST_MPI 3

// Addressing
#define ADDRESS_UNDEFINED 0

// Miscellaneous system declarations
#define true 1
#define false 0
#define UNCHANGED -1
#define LSPCLK_SPEED 100000000
#define PACKET_ID_UNDEFINED 0

// Service Tags
#define SERVICE_TAG_UNDEFINED 0
#define TEST_SERVICE_TAG 1
#define MPI_RECEIVE_SERVICE_TAG 2

// Service provider declarations
#define SERVICE_PROVIDER_AVAILABLE 0
#define SERVICE_PROVIDER_PENDING 255

// Seven Segment values
#define SEVENSEG_BLANK -1
#define SEVENSEG_BLANK_DIGIT 0x10
#define SEVENSEG_1DASH 0x11
#define SEVENSEG_2DASH 0x12
#define SEVENSEG_3DASH 0x13
#define SEVENSEG_FINAL 0x14

// Outbound processing states
#define PORT_STATE_IDLE 0
#define PORT_STATE_WAITING_ON_NEIGHBOR 1
#define PORT_STATE_ARBITRATING 2
#define PORT_STATE_READY_TO_TRANSMIT 3
#define PORT_STATE_TRANSMITTED 4
#define PORT_STATE_COLLISION 5

// Port declarations
#define PORTA 0
#define PORTA_SHIFTED 0x0000
#define PORTB 1
#define PORTB_SHIFTED 0x1000
#define PORTC 2
#define PORTC_SHIFTED 0x2000
#define PORTD 3
#define PORTD_SHIFTED 0x3000
#define PORT_UNDEFINED 4

// Errors
#define SUCCESS 0
#define ERROR 0xFFFF

// Virtual channel declarations
#define VIRTUAL_CHANNEL_CONTROL NUM_VIRTUAL_DATA_CHANNELS
#define VIRTUAL_CHANNEL_UNDEFINED NUM_VIRTUAL_DATA_CHANNELS + 1
#define VIRTUAL_CHANNEL_AVAILABLE 0

// Flit type declarations (note that we pre-shift them here to avoid having to do it in processing)
#define FLIT_TYPE_HEADER 0x0010
#define FLIT_TYPE_DATA_PAYLOAD 0x0020
#define FLIT_TYPE_DATA_PAYLOAD_END 0x0030
#define FLIT_TYPE_UNDEFINED 0x0040

// Node types
#define NODE_TYPE_UNDEFINED 0
#define NODE_TYPE_ROOT_ROUTER 1
#define NODE_TYPE_NON_ROOT_ROUTER 2
#define NODE_TYPE_HOST 3

// Comm types
#define COMM_TYPE_UNDEFINED 0
#define COMM_TYPE_ADDRESSLESS 1
#define COMM_TYPE_UNICAST 2
#define COMM_TYPE_MULTICAST 3
#define COMM_TYPE_BROADCAST 4

// Mailbox subjects
#define MAIL_NO_MESSAGE 0
#define MAIL_DATA_TRANSFER_SUCCESS 1
#define MAIL_DATA_TRANSFER_FAILED 2
#define MAIL_NEW_DATA_TRANSFER_AVAILABLE 3
#define MAIL_SEND_NEW_TRANSFER 4
#define MAIL_BARRIER_ACKED 5
#define MAIL_BARRIER_COMPLETE 6
#define MAIL_ANY_SUBJECT 7

// MPI Defines
#define MPI_COMM_WORLD 1
#define MPI_ANY_SOURCE 0xFF
#define MPI_ANY_DESTINATION 0xFF
#define MPI_REQUEST_NONE 0
#define MPI_REQUEST_BLOCKING_SEND 1
#define MPI_REQUEST_NON_BLOCKING_SEND 2
#define MPI_REQUEST_BLOCKING_RECEIVE 3
#define MPI_REQUEST_NON_BLOCKING_RECEIVE 4
#define MPI_REQUEST_BARRIER 5

// Generic commands, sequence steps, and errors
#define COMMAND_UNDEFINED 0
#define SEQUENCE_UNDEFINED 0
#define SEQUENCE_ERROR 15
#define PROTOCOL_ERROR_TRANSMISSION_ERROR 251
#define PROTOCOL_ERROR_NO_BUFFERS_AVAILABLE 252
#define PROTOCOL_ERROR_INVALID_CRC 253
#define PROTOCOL_ERROR_PACKET_TTL_EXPIRED 254
#define PROTOCOL_ERROR_COULD_NOT_DELIVER_PACKET 255

// Discover neighbors command, sequence steps, and errors
#define COMMAND_DISCOVER_NEIGHBORS 1
#define SEQUENCE_DISCOVER_NEIGHBORS_REQUEST_NEIGHBOR 1
#define SEQUENCE_DISCOVER_NEIGHBORS_NEIGHBOR_RESPONSE 2
#define SEQUENCE_DISCOVER_NEIGHBORS_ERROR 15

// Request address command, sequence steps, and errors
#define COMMAND_REQUEST_ADDRESS 2
#define SEQUENCE_REQUEST_ADDRESS_REQUEST 1
#define SEQUENCE_REQUEST_ADDRESS_REQUEST_FORWARD 2
#define SEQUENCE_REQUEST_ADDRESS_GRANT_FORWARD 3
#define SEQUENCE_REQUEST_ADDRESS_GRANT 4
#define SEQUENCE_REQUEST_ADDRESS_ERROR 15

// Report neighbors command, sequence steps, and errors
#define COMMAND_REPORT_NEIGHBORS 3
#define SEQUENCE_REPORT_NEIGHBORS 1
#define SEQUENCE_REPORT_NEIGHBORS_RECEIVED 2
#define SEQUENCE_REPORT_NEIGHBORS_ERROR 15

// Request routing table command, sequence steps, and errors
#define COMMAND_REQUEST_ROUTING_TABLE 4
#define SEQUENCE_REQUEST_ROUTING_TABLE_REQUEST 1
#define SEQUENCE_REQUEST_ROUTING_TABLE_REQUEST_TRANSFER 2
#define SEQUENCE_REQUEST_ROUTING_TABLE_REQUEST_ACCEPTED 3
#define SEQUENCE_REQUEST_ROUTING_TABLE_TRANSFER_RECEIVED 4
#define SEQUENCE_REQUEST_ROUTING_TABLE_ERROR 15
#define PROTOCOL_ERROR_REQUEST_ROUTING_TABLE_REJECT_TRANSFER 1
#define PROTOCOL_ERROR_REQUEST_ROUTING_TABLE_INVALID_CRC 2

// Communication failure command, sequence steps, and errors
#define COMMAND_COMM_FAILURE 5
#define SEQUENCE_COMM_FAILURE 1
#define SEQUENCE_COMM_FAILURE_ERROR 15

// Communication failure command, sequence steps, and errors
#define COMMAND_NEW_ROUTING_TABLE_AVAILABLE 6
#define SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE 1
#define SEQUENCE_NEW_ROUTING_TABLE_AVAILABLE_ERROR 15

// Data transfer command, sequence steps, and errors
#define COMMAND_DATA_TRANSFER 7
#define SEQUENCE_DATA_TRANSFER_REQUEST_TRANSFER 1
#define SEQUENCE_DATA_TRANSFER_REQUEST_ACCEPTED 2
#define SEQUENCE_DATA_TRANSFER_TRANSFER_RECEIVED 3
#define SEQUENCE_DATA_TRANSFER_ERROR 15
#define PROTOCOL_ERROR_DATA_TRANSFER_REJECT_TRANSFER 1
#define PROTOCOL_ERROR_DATA_TRANSFER_INVALID_CRC 2

// MPI Control command, sequence steps, and errors
#define COMMAND_MPI_CONTROL 8
#define SEQUENCE_MPI_CONTROL 1
#define SEQUENCE_MPI_CONTROL_ACK 2
#define SEQUENCE_MPI_CONTROL_ERROR 15

#endif
