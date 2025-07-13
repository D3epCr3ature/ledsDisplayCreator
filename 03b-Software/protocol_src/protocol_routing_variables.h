
#ifndef __PROTOCOL_ROUTING_VARIABLES__
#define __PROTOCOL_ROUTING_VARIABLES__

enum PROTOCOL_SERVER_RESPONSE {
        CLIENT_CONNECTION_ACK_AND_WAITING_DATA =   0,
        DATA_RECEIVED_ACK                      =   1,
};

enum PROTOCOL_DATA_RECEIVED_INFO {
        DATA_FILLING_DISPLAY                   = 0,
        /* More data than LEDs on display */
        DATA_TRUNCATED                         = 1,
        /* Less data than LEDs on display */
        DATA_NOT_FILLING_DISPLAY               = 2,
};

/* Start @ "100" to give PROTOCOL_SERVER_RESPONSE extension possibility */
enum PROTOCOL_COMMON_ACTION {
        /* Leave from CLIENT side | Shut down from SERVER side */
        LEAVE_SHUTDOWN                         = 100,
};

#endif /* __PROTOCOL_ROUTING_VARIABLES__ */
