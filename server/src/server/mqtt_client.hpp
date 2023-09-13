#ifndef MY_MQTT_CLIENT
#define MY_MQTT_CLIENT

#include <vector>
#include <string>
#include <map>


struct Session {
  // Epoll fd for write and identification.
  int fd;

  std::vector<std::string> subscriptions;
};


class MQTTClient {
public:

  // Creates session if not exists and add subscription.
  void AddSubscription(int fd, std::string topic);

  // Close session and erase it from opened sessions.
  void CloseSession(int fd);

  void StartListen(int fd);

private:
  // session key is epoll evlist file descriptor as unique ID.
  std::map<int, Session> sessions;
};

#endif // MY_MQTT_CLIENT
