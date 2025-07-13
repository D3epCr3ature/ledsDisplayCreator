
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
        #define DBG(FMT, ...)
#endif

/* Format is: !C[3-4]N([A-F]|[a-f]|[0-9]){4},(.{4})+$
 *            !       : Transaction's start
 *            C<comp> : Component prefix with <comp> between 3 & 4
 *            N<data> : Number of data prefix with <data> its value in HEXA
 *            ,       : Splitter between "pre-amble" and actual data
 *            <dataX> : Data value in RGB[W] format <=> uint32_t
 *            $       : Transaction's end                                    */
#define PREAMBLE_LEN    (  9u)
#define LEDS_LEN        (940u)
#define EPILOGUE_LEN    (  1u)
#define BUFFER_LEN      (PREAMBLE_LEN + LEDS_LEN*sizeof(uint32_t) + EPILOGUE_LEN)

#define COLOR_OFF	0xAAAAAAAA

#define FPS 1

union color {
        struct {
                uint8_t r;
                uint8_t g;
                uint8_t b;
                uint8_t w;
        } rgbw;
        uint32_t word;
} C_OFF = { .word = COLOR_OFF };

struct branch {
        uint16_t start;
        uint16_t end;
};

#define BIT_NO_BR       (0)
#define BIT_BR_01       (1 <<  0)
#define BIT_BR_02       (1 <<  1)
#define BIT_BR_03       (1 <<  2)
#define BIT_BR_04       (1 <<  3)
#define BIT_BR_05       (1 <<  4)
#define BIT_BR_06       (1 <<  5)
#define BIT_BR_07       (1 <<  6)
#define BIT_BR_08       (1 <<  7)
#define BIT_BR_09       (1 <<  8)
#define BIT_BR_10       (1 <<  9)
#define BIT_BR_11       (1 << 10)
#define BIT_BR_12       (1 << 11)
#define BIT_ALL_BR      ( 0x0FFF)

#define BUFF_ACCESS(BUF, OFST) (*(uint32_t *)((BUF) + \
                                 PREAMBLE_LEN + (OFST)*sizeof(uint32_t)))

struct state {
       uint16_t activeBranches;
       useconds_t duration;
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
 * @brief  Read response from server and detect "Leaving" msg.
 * @return 0xDEADBEEF: Client leaves server,
 *                  0: Server disconnects itself an make client leave
 *               NULL: Allocation has failed
 *************************************************************************** */
void *receiveServerMsg(void *arg) {
        int  socketFd = *((int *)arg);
        char buff[5+1] = {0};
        int  streamLen;
        uint32_t dataLenLE;
        /* Avoid dynamic allocation in threaded function,
         * by having the return code var. as static */
        static int exitRc = 0;
        exitRc = 0xDEADBEEF;

        while (running) {
                streamLen = recv(socketFd, (void *)buff,
                                 sizeof(buff)-1, MSG_DONTWAIT);

                dataLenLE = __cpu_to_le32p((uint32_t *)buff);
                #if defined(ENA_DBG)
                printf("CLT-DBG: dataLenLE[%d] | Msg[%s]\n",
                       dataLenLE, buffRecvData);
                /* Tips: From threaded fn(), flush output to
                 *       actually print it */
                fflush(stdout);
                #endif

                /* Full stream is 4 bytes of stream length +
                 * 1 for server's shut down routing value
                 * So compare both value for safety and avoid case like:
                 * - 4 random bytes before actual data */
                if (streamLen != 5 && dataLenLE != 1)
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
        char buff[BUFFER_LEN+1] = {0};
        char *buffRecvData = buff+4;    /* buff+4 to skip stream length info. */
        char *buffSendData = buff+PREAMBLE_LEN;
        /* Data treatment */
        int streamLen = 0;
        uint32_t streamLenLE = 0, dataLenLE = 0;
        //union color c = { .rgbw = { .w = 0, .r = 0, .g = 0xBB, .b = 0xFF } };
        union color c = { .rgbw = { .w = 0, .r = 0, .g = 0x88, .b = 0xCC } };
        int iBranch = 0;
        struct branch branches[12] = {
                {  0,  90}, { 91, 162}, {163, 236}, {237, 310},
                {311, 382}, {383, 473}, {474, 547}, {548, 621},
                {622, 687}, {688, 780}, {781, 873}, {874, 939}
        };
        uint8_t s = 0;
        struct state sequence[] = {
                {BIT_BR_06, 1000000 / 4}, {BIT_BR_10, 1000000 / 4},
                {BIT_BR_11, 1000000 / 4}, {BIT_BR_03, 1000000 / 4},
                {BIT_BR_04, 1000000 / 4}, {BIT_BR_02, 1000000 / 4},
                {BIT_BR_05, 1000000 / 4}, {BIT_BR_09, 1000000 / 4},
                {BIT_NO_BR, 1000000 / 4}, {BIT_BR_09, 1000000 / 4},
                {BIT_BR_12, 1000000 / 4}, {BIT_BR_03, 1000000 / 4},
                {BIT_BR_04, 1000000 / 4}, {BIT_BR_03, 1000000 / 4},
                {BIT_BR_12, 1000000 / 4}, {BIT_NO_BR, 1000000 / 4},
                {BIT_BR_03, 1000000 / 4}, {BIT_BR_04, 1000000 / 4},
                {BIT_NO_BR, 1000000 / 4}, {BIT_ALL_BR, 1000000},
                {BIT_BR_06, 1000000 / 4}, {BIT_ALL_BR, 1000000}
        };
        /* Thread about detecting server leaving */
        pthread_t svrLeavingThread;
        int *threadRc;
        /* Others */
        int rc = 0, i = 0;

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

        /* Fully initiate srv_addr struct to 0 */
        memset(&srv_addr, 0, sizeof(srv_addr));

        /* Connect signal to handler */
        signal(SIGINT, sigint_handler);

        printf("~~~ WELCOME TO THE PROGRAM ~~~\n");
        printf("CLT: Wait for connection validation...\n");
        while ( ! connectionSuccessful && running ) {
                /* Use MSG_DONTWAIT as MSG_WAITALL wait for buffer
                 * to be filled with "size" elements (if no error occurs).
                 * Other flags, like MSG_TRUNC, are in evaluation
                 * as possibilities. */
                streamLenLE = recv(socketFd, (void *) buff,
                                   BUFFER_LEN, MSG_DONTWAIT);

                /* 4 bytes of stream length + 1 for routing value of
                 * acknowledge value: CLIENT_CONNECTION_ACK_AND_WAITING_NEW_DATA */
                if (streamLenLE != 5)   continue;

                dataLenLE = __cpu_to_le32p((uint32_t *)buff);
                DBG("Server's response (length: %d, actual data: %d):\n",
                    streamLenLE, dataLenLE);

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
               "drive LEDs display!\n");
        while (running) {
                /* Build custom preamble */
                snprintf(buff, BUFFER_LEN, "!C%dN%04x,", 3, LEDS_LEN);
                buff[PREAMBLE_LEN] = 0;

                streamLen = strlen(buff) + LEDS_LEN * sizeof(uint32_t) + 1;
                DBG("CLT-DBG: Built string(len[%d])\n", streamLen);

                /* Fill with color data */
                for (i = 0; i < sizeof(branches)/sizeof(*branches); i++) {
                        if (sequence[s].activeBranches & (1 << i)) {
                                /* Set color */
                                for (iBranch  = branches[i].start;
                                     iBranch <= branches[i].end  ; iBranch++) {
                                     BUFF_ACCESS(buff, iBranch) = c.word;
                                }
                        } else {
                                /* Set OFF color */
                                for (iBranch  = branches[i].start;
                                     iBranch <= branches[i].end  ; iBranch++) {
                                     BUFF_ACCESS(buff, iBranch) = C_OFF.word;
                                }
                        }
                }

                /* Fill with epilogue */
                buff[BUFFER_LEN-1] = '$';

                streamLenLE = __cpu_to_le32p((uint32_t *)&streamLen);
                if (send(socketFd, &streamLenLE,
                         sizeof(streamLenLE), 0) != sizeof(streamLenLE))
                        printf("CLT: Packet's preamble could not "
                               "be sent properly\n");

                if (send(socketFd, (void *) buff,
                         BUFFER_LEN, 0) != BUFFER_LEN)
                        printf("CLT: Data could not "
                               "be sent properly\n");

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

                usleep(sequence[s].duration);

                if (++s >= sizeof(sequence)/sizeof(*sequence))
                        s = 0;
        }

        pthread_join(svrLeavingThread, (void **) &threadRc);
        if (threadRc && *threadRc) {
                /* 1st 4 bytes are stream length info. in Little Endian */
                if (send(socketFd, (void *) "\x07\0\0\0Leaving", 11, 0) != 11)
                        fprintf(stderr, "Sending failed\n");

                printf("CLT: Leaving server\n");

        } else {
                printf("SVR: Forced client to leave\n");
        }

        /* Resources cleaning */
        close(socketFd);

        return 0;
}
