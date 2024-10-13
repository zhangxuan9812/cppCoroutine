#include <arpa/inet.h>
#include <fcntl.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "hook/hook.h"
#include "iomanager/ioscheduler.h"

using namespace cppCoroutine;

void func() {
  // Create a socket
  int sock = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(80);  // Http server port
  server.sin_addr.s_addr = inet_addr("103.235.46.96");

  int rt = connect(sock, (struct sockaddr *)&server, sizeof(server));
  if (rt) {
    fmt::print("connect() failed: {}\n", strerror(errno));
    return;
  }
  fmt::print("connected\n");

  const char data[] = "GET / HTTP/1.0\r\n\r\n";
  rt = send(sock, data, sizeof(data), 0);
  if (rt <= 0) {
    fmt::print("send() failed\n");
  }
  fmt::print("send success\n");

  std::string buff;
  buff.resize(4096);
  rt = recv(sock, &buff[0], sizeof(buff) - 1, 0);
  if (rt <= 0) {
    fmt::print("recv failed\n");
  }
  fmt::print("recv success\n");

  buff.resize(10);
  fmt::print("recv data: {}\n", buff);
}

int main(int argc, char const *argv[]) {
  IOManager manager(2);
  for (int i = 0; i < 4; i++) {
    manager.scheduleLock(&func);
  }
  return 0;
}