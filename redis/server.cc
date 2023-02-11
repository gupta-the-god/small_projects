#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// Binds and listens to a socket. Returns the file descriptor
int SetupSocket() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        std::cout << "Error creating a socket: " << errno << std::endl;
        return -1;
    }

    int reused_address = 1;
    int result = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reused_address, sizeof(reused_address));
    if (result < 0) {
        std::cout << "Error setting socket options: " << errno << std::endl;
        return -1;
    }

    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = ntohs(8080);
    sock_addr.sin_addr.s_addr = INADDR_ANY;

    result = bind(fd, (struct sockaddr *) &sock_addr, sizeof(sock_addr));
    if (result < 0) {
        std::cout << "Problem binding addr to socket: " << errno << std::endl;
        return -1;
    }
    result = listen(fd, SOMAXCONN);
    if (result < 0) {
        std::cout << "Problem listening on socket" << std::endl;
        return -1;
    }    
 
    std::cout << "connection is ready..." << std::endl;
    return fd;	
}

void Loop(int connection_fd) {
    char msg[64];
    ssize_t bytes_read = read(connection_fd, msg, sizeof(msg));
    if (bytes_read <= 0) {
        //flag an error
	    std::cout << "No bytes were read." << std::endl;
        return;
    }    

    std::cout << "Recevied message: " << msg << std::endl;

    char out[] = "right back atcha";
    ssize_t bytes_written = write(connection_fd, out, sizeof(out));
    if (bytes_written <= 0) {
        //flag an error
        std::cout << "No bytes were written" << std::endl;
        return;
    }
}

int Communicate(int fd) {
    while (true) {
        struct sockaddr_in client_sock_addr;
        socklen_t sock_len = sizeof(client_sock_addr);
        int cfd = accept(fd, (struct sockaddr *) &client_sock_addr, &sock_len);
        if (cfd < 0) {
            continue;
        }
        std::cout << "Accepted connection!" << std::endl;

        Loop(cfd);
        close(cfd);
    }
}

int main() {
    int fd = SetupSocket();
    if (fd == -1) {
        return 0;
    }
    Communicate(fd);
    return 0;
}


