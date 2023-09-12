#include <iostream>
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <netdb.h>
#include <arpa/inet.h>

#include <pthread.h>


constexpr std::size_t BUF_SIZE = 4096;


std::string random_string(int n) {
  static std::vector<char> alphabet = { 'a', 'b', 'c', 'd', 'e', 'f', 'g',
                                        'h', 'i', 'j', 'k', 'l', 'm', 'n',
                                        'o', 'p', 'q', 'r', 's', 't', 'u',
                                        'v', 'w', 'x', 'y', 'z' };
  std::string res;
  for (int i = 0; i < n; i++)
    res += alphabet.at(rand() % alphabet.size());

  return res;
}


void* send_load_data_on_server(void* arg) {
  int port = *((int*)arg);

  int sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    perror("socket() failed");
    return nullptr;
  }

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof serv_addr);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
  serv_addr.sin_port = htons(port);

  if (connect(sock,
              (const sockaddr*)&serv_addr,
              sizeof(serv_addr)) == -1) {
    perror("connect() failed");
    close(sock);
    return nullptr;
  }

  std::string random_data = random_string(BUF_SIZE / 2);
  const auto& expected_response = random_data;

  ssize_t nwrite = write(sock, random_data.c_str(), random_data.size());
  if (nwrite == -1)
    perror("write()");

  if (random_data.size() != static_cast<std::size_t>(nwrite))
    std::cout << "write incomplete\n" << std::endl;

  // read response
  char buf[BUF_SIZE];

  ssize_t nread = read(sock, buf, BUF_SIZE-1);
  if (nread == -1) {
    perror("read error");
  } else {
    buf[nread] = 0;

    if (expected_response == buf)
      std::cout << "Thread request succes." << std::endl;
    else
      std::cout << "Thread request failed." << std::endl;
  }

  if (shutdown(sock, SHUT_RDWR) == -1)
    perror("shutdown()");

  if (close(sock) == -1)
    perror("close()");

  return nullptr;
}


int main(int argc, char ** argv) {
  if (argc < 2) {
    std::cout << "usage: " << argv[0] << " <port>" << std::endl;
    return -1;
  }

  int port = atoi(argv[1]);

  constexpr std::size_t threads_amount = 21;
  std::vector<pthread_t> threads(threads_amount);

  for (std::size_t i=0; i<threads_amount; i++) {
    if(pthread_create(&threads[i], NULL, send_load_data_on_server, &port))
      std::cout << "Error creating thread" << std::endl;
  }

  for (std::size_t i=0; i<threads_amount; i++) {
    if(pthread_join(threads[i], NULL))
      std::cout << "Error joining thread" << std::endl;
  }

  return 0;
}

