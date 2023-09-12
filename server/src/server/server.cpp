#include <sys/epoll.h>
#include <netdb.h>

#include <string.h> // memset
#include <unistd.h>
#include <fcntl.h>

#include <iostream>
#include <cstdint>
#include <string>

constexpr std::size_t EPOLL_SIZE = 1000;
constexpr std::size_t MAX_BUF = 512;


template<typename T>
struct scope_guard {
  scope_guard(const T &t) : t_(t) {};
  ~scope_guard() { close(t_); }
  const T &t_;
};


void process_connection(int fd) {
  std::string result;

  while (true) {
    char buf[MAX_BUF];
    ssize_t readed = read(fd, buf, MAX_BUF - 1);
    if (readed == 0)
      break; // connection closed

    if (readed == -1) {
      if (errno == EAGAIN || errno == EWOULDBLOCK)
        break; // I/O buffer emptied

      perror("read()");
      return;
    }

    buf[readed] = '\0';
    result += buf;
  }

  // * send response
  auto response = result;
  ssize_t writed = send(fd, response.c_str(), response.size(), 0);
  if (writed == -1) {
    perror("send()");
  }

  if (static_cast<std::size_t>(writed) != response.size())
    std::cout << "Incomplete write" << std::endl;
}


void finish_connection(int epfd, epoll_event& ev) {
  int fd = ev.data.fd;

  // Explicitly deregistering file descriptors before close
  if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev) == -1) {
    perror("epoll_ctl() del fd");
    throw std::runtime_error("epoll_ctl() del fd for listen_sock");
  }

  if (shutdown(fd, SHUT_RDWR) == -1)
    perror("shutdown()");

  if (close(fd) == -1)
    perror("close()");
}


void setnonblocking(int socket) {
  int flags = fcntl(socket, F_GETFL);
  if (flags == -1) {
    perror("fcntl() get flags");
    throw std::runtime_error("fcntl() get flags");
  }
  if (fcntl(socket, F_SETFL, flags | O_NONBLOCK) == -1) {
    perror("fcntl() set nonblocking");
    throw std::runtime_error("fcntl() set nonblocking");
  }
}


// TODO: always inline
inline sockaddr_in create_sockaddr(std::uint16_t port) {
  sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof serv_addr);

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(port);
  serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

  return serv_addr;
}


inline void init_listen_socket(int epfd, int listen_socket) {
  epoll_event ev;

  ev.events = EPOLLIN;
  ev.data.fd = listen_socket;

  if (epoll_ctl(epfd, EPOLL_CTL_ADD, listen_socket, &ev) == -1) {
    perror("epoll_ctl() add listen_sock");
    throw std::runtime_error("epoll_ctl() listen_sock");
  }
}


inline void accept_client_socket(int epfd, int listen_socket) {
  epoll_event ev;

  int client_socket;
  sockaddr_storage their_addr;
  socklen_t addr_size = sizeof their_addr;

  if ((client_socket = accept(listen_socket,
                              (sockaddr *) &their_addr,
                              &addr_size)) == -1) {
    perror("accept()");
    throw std::runtime_error("accept() client socket");
  }

  setnonblocking(client_socket);

  // default - level-triggered
  // EPOLLET - edge-triggered
  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = client_socket;

  if (epoll_ctl(epfd, EPOLL_CTL_ADD, client_socket, &ev) == -1) {
    perror("epoll_ctl() client_socket");
    throw std::runtime_error("epoll_ctl() add client socket");
  }
}


int main(int argc, char** argv) {
  if(argc < 2) {
    std::cout << "usgae: " << argv[0] << " <port>" << std::endl;
    return -1;
  }

  // port from arg to number
  std::uint16_t port = std::stoul(argv[1]);

  // create socket
  int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_socket == -1) {
    perror("socket() error");
    return -1;
  }

  // close on socket fd
  scope_guard<int> listen_socket_guard(listen_socket);

  // by default, system doesn't close socket,
  // but set it's status to TIME_WAIT; allow socket reuse
  int yes = 1;
  if (setsockopt(listen_socket,
                 SOL_SOCKET, SO_REUSEADDR,
                 &yes, sizeof(int)) == -1) {

    perror("setsockopt()");
    return -1;
  }

  // init struct
  sockaddr_in serv_addr = create_sockaddr(port);

  // bind socket to address
  if (bind(listen_socket,
           (const sockaddr*) &serv_addr,
           sizeof serv_addr) == -1) {

    perror("bind() error");
    return -1;
  }

  // start listening on port
  if (listen(listen_socket, 1) == -1) {
    perror("listen() error");
    return -1;
  }

  // create epoll instance
  int epfd = epoll_create(EPOLL_SIZE);
  if (epfd == -1) {
    perror("epoll_create()");
    return -1;
  }

  scope_guard<int> epoll_guard(epfd);

  init_listen_socket(epfd, listen_socket);

  std::cout << "Server starts to listen on port: " << port << std::endl;

  epoll_event evlist[EPOLL_SIZE];
  while(true) {
    int num_of_fds = epoll_wait(epfd, evlist, EPOLL_SIZE, -1);
    if (num_of_fds == -1) {
      perror("epoll_wait()");
      return -1;
    }

    // iterate over ready events
    for (int n = 0; n < num_of_fds; ++n) {
      if (evlist[n].data.fd == listen_socket) {
        accept_client_socket(epfd, listen_socket);
      }
      else {
        process_connection(evlist[n].data.fd);
        finish_connection(epfd, evlist[n]);
      }
    }
  }

  return EXIT_SUCCESS;
}

// TODO: signal processing for correct termination.

