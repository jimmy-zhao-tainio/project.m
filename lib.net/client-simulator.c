#include <lib.net/client-simulator.h>
#include <lib.core/error.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <strings.h>

bool client_simulator_connect (const char *ip, uint16_t port)
{
        int file_descriptor;
        struct sockaddr_in address;

        if ((file_descriptor = socket (AF_INET,SOCK_STREAM, 0)) == -1) {
                error_code (SystemCall, 1);
                return false;
        }
        bzero (&address, sizeof (address));
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr (ip);
        address.sin_port = htons (port);
        if (connect (file_descriptor, (struct sockaddr *)&address, sizeof (address)) == -1) {
                close (file_descriptor);
                error_code (SystemCall, 2);
                return false;
        }
        close (file_descriptor);
        return true;
}
