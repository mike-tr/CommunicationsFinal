#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "src/headers/select.hpp"

int main(int argc, char *argv[]) {
    int listenfd = 0, listenfd2 = 0;
    struct sockaddr_in serv_addr;
    int ret;
    int r_port1 = 5000, r_port2 = 5001;
    char buff[1025];
    //time_t ticks;

    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    listenfd2 = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(r_port1);

    // This piece of code here binds the server ip to the address specified below and the port!
    bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_port = htons(r_port2);
    bind(listenfd2, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    // This fuctiong is given by the martze idk how it actually working
    // but the fact is that it creates a way to listen to data from all the sockets and the user input
    // at the same time.
    // You really dont need to know how it works to use that
    // simply for any connection/socket you want to listen too
    // you call
    // add_fd_to_monitoring(socket_i_want_to_listen_to);
    printf("adding fd1(%d) to monitoring\n", listenfd);
    add_fd_to_monitoring(listenfd);
    printf("adding fd2(%d) to monitoring\n", listenfd2);
    add_fd_to_monitoring(listenfd2);

    // Not sure what this function does google it
    // but it probably enables listening to socket fd and fd2. i.e port 5000 and port 5001.
    // THis fucking is not blocking anything!
    listen(listenfd, 10);
    listen(listenfd2, 10);

    bool listen = true;
    // run this fucntion to see if that working :
    // echo "This is my data" > /dev/udp/127.0.0.1/5001
    while (listen) {
        // this part is w8ing for any input from user or a UDP message to port2 or port1.
        // i.e in this case if you send udp message to 127.0.0.1:5000 or 127.0.0.1:5001 this will show the message
        // and it will print the "socket" that has recieved that message.
        printf("waiting for input...\n");

        // Notice : wait for input uses "recv msg", so this function is on while loop,
        // untill it gets input you will see that until there is no input nothing happens.
        // think of this as a realllly long sleep and the program wakes up when new input is recieved.
        ret = wait_for_input();
        printf("fd: %d is ready. reading...\n", ret);
        // this simply reads the bytes send from the right socket, and droppes the message into bugg
        // Notice : that you want to remove any garbage from buff.
        // so i ADD here buff = 0
        memset(buff, 1025, sizeof(char));
        read(ret, buff, 1025);
        printf("%s", buff);
    }
}
