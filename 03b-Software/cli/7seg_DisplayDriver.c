
/* Include for .. */
#include <inttypes.h>   /* .. normalized type, like "uint32_t" */
#include <pthread.h>    /* .. pthread_[create|join]() */
#include <signal.h>     /* .. signal() */
#include <stdio.h>
#include <stdlib.h>     /* .. exit(), atoi(), malloc() */
#include <string.h>     /* .. memset() */
#include <unistd.h>     /* .. usleep(), close() */

#include <asm/byteorder.h>      /* .. __cpu_to_be32p() */
#include <arpa/inet.h>          /* .. inet_pton() */
#include <netinet/in.h>         /* .. htons(), struct sockaddr_in */
#include <sys/socket.h>         /* .. socket(), connect(), recv(), send() */

#include "../protocol_src/protocol_routing_variables.h"

/* Uncomment to enable debug prints */
//#define ENA_DBG 1
#if defined(ENA_DBG)
        #define DBG(FMT, ...)	printf(FMT, ##__VA_ARGS__)
#else
        #define DBG(FMT, ...)   (void)0
#endif

#define BUFFER_LEN      300

#define COLOR_OFF	0xAAAAAAAA

#define FPS 3

#define SEGMENTS        7
#define LEDS_PER_SEG    3

union color {
        struct {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                uint8_t w;
        } rgbw;
        uint32_t word;
} C_OFF = { .word = COLOR_OFF };

struct digit {
        union color segments[SEGMENTS][LEDS_PER_SEG];
};

/* Running states */
int running = 1;

/** **************************************************************************
 * @brief Signal SIGINT handler
 *        Receive SIGINT to properly end program and close socket
 *************************************************************************** */
void sigint_handler(int sig) {
        printf(" SIGINT handler w/ code: %d\n", sig);
        running = 0;
}

/** **************************************************************************
 * @brief Regarding the segment's status, put an "ON" color or the OFF color
 *        in the digits array
 *************************************************************************** */
void prepareSegment(int digit, int segment, int status,
                    struct digit *arr, union color c) {
        int i = 0;
        if (status) {
                for ( ; i < LEDS_PER_SEG; i++) {
                        arr[digit].segments[segment][i] = c;
                }
        } else {
                for ( ; i < LEDS_PER_SEG; i++) {
                        arr[digit].segments[segment][i] = C_OFF;
                }
        }
}

/** **************************************************************************
 * @brief Prepare digits array to drive Qt application
 *        that will simulate a 7segments display
 * @note  7segments is coded based on the WS2811 chip behavior.
 *        That's why it's an array that is sent and not a simple uint8.
 *************************************************************************** */
void prepareDigits(struct digit *arr, union color c) {
        int j = 0;
        uint16_t segments = 0;

        /* ****** 0 ****** */
        for (segments = 0b00111111; j < SEGMENTS; j++) {
                prepareSegment( 0, j, segments & (1 << j), arr, c);
        }

        /* ****** 1 ****** */
        for (segments = 0b00000110, j = 0; j < SEGMENTS; j++) {
                prepareSegment( 1, j, segments & (1 << j), arr, c);
        }

        /* ****** 2 ****** */
        for (segments = 0b01011011, j = 0; j < SEGMENTS; j++) {
                prepareSegment( 2, j, segments & (1 << j), arr, c);
        }

        /* ****** 3 ****** */
        for (segments = 0b01001111, j = 0; j < SEGMENTS; j++) {
                prepareSegment( 3, j, segments & (1 << j), arr, c);
        }

        /* ****** 4 ****** */
        for (segments = 0b01100110, j = 0; j < SEGMENTS; j++) {
                prepareSegment( 4, j, segments & (1 << j), arr, c);
        }

        /* ****** 5 ****** */
        for (segments = 0b01101101, j = 0; j < SEGMENTS; j++) {
                prepareSegment( 5, j, segments & (1 << j), arr, c);
        }

        /* ****** 6 ****** */
        for (segments = 0b01111101, j = 0; j < SEGMENTS; j++) {
                prepareSegment( 6, j, segments & (1 << j), arr, c);
        }

        /* ****** 7 ****** */
        for (segments = 0b00000111, j = 0; j < SEGMENTS; j++) {
                prepareSegment( 7, j, segments & (1 << j), arr, c);
        }

        /* ****** 8 ****** */
        for (segments = 0b01111111, j = 0; j < SEGMENTS; j++) {
                prepareSegment( 8, j, segments & (1 << j), arr, c);
        }

        /* ****** 9 ****** */
        for (segments = 0b01101111, j = 0; j < SEGMENTS; j++) {
                prepareSegment( 9, j, segments & (1 << j), arr, c);
        }

        /* ****** A ****** */
        for (segments = 0b01110111, j = 0; j < SEGMENTS; j++) {
                prepareSegment(10, j, segments & (1 << j), arr, c);
        }

        /* ****** b ****** */
        for (segments = 0b01111100, j = 0; j < SEGMENTS; j++) {
                prepareSegment(11, j, segments & (1 << j), arr, c);
        }

        /* ****** C ****** */
        for (segments = 0b00111001, j = 0; j < SEGMENTS; j++) {
                prepareSegment(12, j, segments & (1 << j), arr, c);
        }

        /* ****** d ****** */
        for (segments = 0b01011110, j = 0; j < SEGMENTS; j++) {
                prepareSegment(13, j, segments & (1 << j), arr, c);
        }

        /* ****** E ****** */
        for (segments = 0b01111001, j = 0; j < SEGMENTS; j++) {
                prepareSegment(14, j, segments & (1 << j), arr, c);
        }

        /* ****** F ****** */
        for (segments = 0b01110001, j = 0; j < SEGMENTS; j++) {
                prepareSegment(15, j, segments & (1 << j), arr, c);
        }
}

/** **************************************************************************
 * @brief  Read response from server and detect "Leaving" msg.
 * @return 0xDEADBEEF: Client leaves server,
 *                  0: Server disconnects itself and forced client to leave
 *************************************************************************** */
void *receiveServerMsg(void *arg) {
        int  socketFd = *((int *)arg);
        char buff[5+1] = {0};
        uint32_t streamLen, dataLen;
        /* Avoid dynamic allocation in threaded function,
         * by having the return code var. as static */
        static int exitRc = 0;
        exitRc = 0xDEADBEEF;

        while (running) {
                streamLen = recv(socketFd, (void *)buff,
                                 sizeof(buff)-1, MSG_DONTWAIT);

                dataLen   = __cpu_to_le32p((uint32_t *)buff);

                /* Full stream is 4 bytes of stream length +
                 * 1 for server's shut down routing value
                 * So compare both value for safety and avoid case like:
                 * - 4 random bytes before actual data */
                if (streamLen != 5 && dataLen != 1)
                        continue;

                if (buff[4] == LEAVE_SHUTDOWN) {
                        exitRc  = 0;
                        running = 0;
                }
        }

        return (void *)&exitRc;
}

/** **************************************************************************
 * @brief Main application function
 *************************************************************************** */
int main(int argc, char **argv) {
        /* Socket parameters */
        int socketFd = 0, port = 0;
        struct sockaddr_in srv_addr = {};
        uint8_t connectionSuccessful = 0;
        /* Exchanging buffer */
        char buff[BUFFER_LEN+1] = {};
        /* buff+4 to skip stream length info. */
        char *buffRecvData = buff+4;
        /* Data treatment */
        int streamLen = 0;
        uint32_t streamLenLE = 0, dataLenLE = 0;
        union color c = { .rgbw = { .w = 0, .r = 0, .g = 0xBB, .b = 0xFF } };
        struct digit digits[16] = {};
        /* Thread about detecting server leaving msg */
        pthread_t svrLeavingThread = {};
        int *threadRc = NULL;
        /* Others */
        int rc = 0, state = 0;

        if (argc != 3) {
                fprintf(stderr, "usage: %s <SERVER_IP> <PORT>\n", argv[0]);
                fprintf(stderr, "\tSERVER_IP: 10's base for IPv4\n");
                fprintf(stderr, "\t           16's base for IPv6\n");
                fprintf(stderr, "\tPORT     : Port's communication\n");
                exit(EXIT_FAILURE);
        }

        /* Parsing server's IP */
        if (inet_pton(AF_INET, argv[1], &srv_addr.sin_addr) <= 0) {
                fprintf(stderr, "Converting server's IP failed\n");
                exit(EXIT_FAILURE);
        }

        /* Parsing server's port */
        port = atoi(argv[2]);

        /* Socket's creation */
        socketFd = socket(AF_INET, SOCK_STREAM, 0);
        if (socketFd < 0) {
                fprintf(stderr, "Could not create socket client side\n");
                exit(EXIT_FAILURE);
        }
        DBG("CLT-DBG: Step passed[socket]\n");

        /* Fill server's infos */
        srv_addr.sin_family = AF_INET;
        srv_addr.sin_port = htons(port);

        printf("CLT: Connecting to server at %s:%d...\n", argv[1], port);
        if (connect(socketFd, (struct sockaddr *) &srv_addr,
                                sizeof(srv_addr)) < 0) {
                fprintf(stderr, "FAILURE\n");
                /* Clean ressource(s) that could be used */
                close(socketFd);
                exit(EXIT_FAILURE);
        }
        DBG("CLT-DBG: Step passed[connect]\n");

        /* Prepare digits & Fully initiate srv_addr struct to 0 */
        prepareDigits(digits, c);
        memset(&srv_addr, 0, sizeof(srv_addr));

        /* Connect signal to handler */
        signal(SIGINT, sigint_handler);

        printf("~~~ WELCOME TO THE PROGRAM ~~~\n");
        printf("CLT: Wait for connection validation...\n");
        while ( ! connectionSuccessful && running ) {
                /* Use MSG_DONTWAIT as MSG_WAITALL wait for buffer
                 * to be filled with BUFFER_LEN elements (if no error occurs).
                 * Other flags, like MSG_TRUNC, are in evaluation
                 * as possibilities. */
                streamLenLE = recv(socketFd, (void *) buff,
                                   BUFFER_LEN, MSG_DONTWAIT);

                /* 4 bytes of stream length + 1 for routing value of
                 * acknowledge value: CLIENT_CONNECTION_ACK_AND_WAITING_DATA */
                if (streamLenLE !=  5)    continue;

                dataLenLE = __cpu_to_le32p((uint32_t *)buff);
                DBG("Server's response (length: %d, actual data: %d):\n",
                    streamLenLE, dataLenLE);

                printf("SVR: %s\n", buffRecvData);

                if (*((uint8_t *)buffRecvData) ==
                    CLIENT_CONNECTION_ACK_AND_WAITING_DATA) {
                        connectionSuccessful = 1;
                }
        }

        printf("CLT: Launching task to detect if server kills itself...\n");
        rc = pthread_create(&svrLeavingThread, NULL,
                            receiveServerMsg, &socketFd);
        if (rc) {
                printf("CLT: Thread could not be created. Program aborted!\n");
                /* 1st 4 bytes are stream length info. in Little Endian */
                if (send(socketFd, (void *) "\x07\0\0\0Leaving", 11, 0) != 11)
                        fprintf(stderr, "Sending failed\n");
                /* Clean ressource(s) that could be used */
                close(socketFd);
                exit(EXIT_FAILURE);
        }

        printf("CLT: Start communication with server to "
               "drive 7Segments display!\n");
        while (running) {
                /* Prepare custom protocol's header */
                snprintf(buff, BUFFER_LEN, "!C%dN%04x,",
                         3, SEGMENTS * LEDS_PER_SEG);
                buff[9] = 0;

                /* Set full len of data to send */
                streamLen = strlen(buff) + SEGMENTS * LEDS_PER_SEG * \
                            sizeof(digits->segments[0]->word) + 1;
                DBG("CLT-DBG: Built string(len[%d])\n", streamLen);

                /* Get len of data in Little Endian and send it */
                streamLenLE = __cpu_to_le32p((uint32_t *)&streamLen);
                if (send(socketFd, &streamLenLE,
                         sizeof(streamLenLE), 0) != sizeof(streamLenLE))
                        printf("CLT: Packet's preamble could not "
                               "be sent properly\n");

                /* Send custom protocol's header */
                if (send(socketFd, (void *) buff,
                         strlen(buff), 0) != strlen(buff))
                        printf("CLT: Data's header could not "
                               "be sent properly\n");

                /* Send actual data */
                /* Works because host machine operates with Little Endian  */
                /* TODO/IDEA: Only change endianess treatment Qt side
                 *            Otherwise, a loop would be needed to
                 *            transform and ensure data's endianess, like: */
                /*for (uint32_t i = 0; i < 7; i++) {
                        uint32_t tmp = __cpu_to_le32p(
                                        &(digits[state].segments+i)->word);
                        if (send(socketFd, (void *) &tmp, sizeof(tmp), 0)
                            != sizeof(tmp))
                                printf("CLT: Data[state=%d] segment[%d] could "
                                       "not be sent properly\n", state, i);
                }*/
                if (send(socketFd, (void *) digits[state].segments,
                          SEGMENTS * LEDS_PER_SEG \
                          *sizeof(digits[state].segments[0]->word), 0)
                          != SEGMENTS * LEDS_PER_SEG *sizeof(digits[state].segments[0]->word))
                        printf("CLT: Data[state=%d] could not "
                               "be sent properly\n", state);

                /* Send protocol's epilogue */
                if (send(socketFd, (void *) "$", 1, 0) != 1)
                        printf("CLT: Data's terminating char could not "
                               "be sent properly\n");

                state = (state + 1) % 16;

                usleep(1000000 / FPS);
        }

        pthread_join(svrLeavingThread, (void **) &threadRc);
        if (threadRc && *threadRc) {
                /* 1st 4 bytes are stream length info. in Little Endian */
                if (send(socketFd, (void *) "\x07\0\0\0Leaving", 11, 0) != 11)
                        fprintf(stderr, "Sending failed\n");

                printf("CLT: Leaving server\n");

        } else {
                printf("SVR: Forcing client to leave\n");
        }

        /* Resources cleaning */
        close(socketFd);

        return 0;
}
