#include <arpa/inet.h>
#include <fcntl.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include "iomanager/ioscheduler.h"

using namespace cppCoroutine;

char recv_data[4096];

const char data[] = "GET / HTTP/1.0\r\n\r\n";

int sock;

void func() {
  // Return 4096 bytes of data to recv_data from the socket
  // recv(sock, recv_data, 4096, 0);
  // fmt::print("recv_data is {}\n\n", recv_data);

  int bytes_received = recv(sock, recv_data, sizeof(recv_data) - 1, 0);
  if (bytes_received >= 0) {
    recv_data[bytes_received] = '\0';  // 确保 recv_data 作为字符串以 '\0' 结尾
  } else if (bytes_received == 0) {
    fmt::print("Connection closed by the server.\n");
  } else {
    fmt::print("recv failed with error: {}\n", strerror(errno));
  }
  fmt::print("recv_data is {}\n\n", recv_data);
}
// Send the data to the socket
void func2() { send(sock, data, sizeof(data), 0); }

int main(int argc, char const *argv[]) {
  // Create an IOManager object with 2 threads
  IOManager manager(2);
  // Create a socket
  sock = socket(AF_INET, SOCK_STREAM, 0);
  // Set the server address
  sockaddr_in server;
  server.sin_family = AF_INET;
  // Set the port to 80, this is the port of the HTTP server
  server.sin_port = htons(80);
  // server.sin_addr.s_addr = inet_addr("103.235.46.96");
  server.sin_addr.s_addr = inet_addr("142.251.32.46");
  // Set the socket to non-blocking mode
  fcntl(sock, F_SETFL, O_NONBLOCK);
  // Connect to the server
  connect(sock, (struct sockaddr *)&server, sizeof(server));
  // Add the event to the IOManager
  manager.addEvent(sock, IOManager::WRITE, &func2);
  manager.addEvent(sock, IOManager::READ, &func);

  fmt::print("event has been posted\n\n");
  return 0;
}