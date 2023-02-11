#include <errno.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

// Connects to a server socket. Returns the file descriptor
int Connect() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        std::cout << "Error creating a socket: " << errno << std::endl;
        return -1;
    }
    
    struct sockaddr_in sock_addr;
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = ntohs(8080);
    sock_addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);
    int result = connect(fd, (struct sockaddr *) &sock_addr, sizeof(sock_addr));
    if (result < 0) {
        std::cout << "Error connecting: " << errno << std::endl;
        return -1;
    }
 
    return fd;	
}

void Loop(int connection_fd) {
    while (true) {
        char out[] = "sup fool";
        ssize_t bytes_written = write(connection_fd, out, sizeof(out));
        if (bytes_written <= 0) {
            std::cout << "Write failed: " << errno << std::endl;
            return;
        }

        char resp[64];
        ssize_t bytes_read = read(connection_fd, resp, sizeof(resp) - 1);
        if (bytes_read < 0) {
            std::cout << "Read failed: " << errno << std::endl;            
            return;
        } 

        std::cout << "Received message from server: " << resp << std::endl;   
    }
}

void Communicate(int fd) {
    Loop(fd);
    close(fd);
}

int main() {
    int fd = Connect();
    Communicate(fd);
}
