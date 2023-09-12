#include "mqtt_client.hpp"

#include <iostream>


void MQTTClient::AddSubscription(int fd, std::string topic) {
  sessions[fd].subscriptions.push_back(std::move(topic));
  std::cout << "Add Subscription: " << fd
            << ", topic: " << topic << std::endl;
}


void MQTTClient::CloseSession(int fd) {
  sessions.erase(fd);
  std::cout << "Close session: " << fd << std::endl;
}


void MQTTClient::StartListen(int fd) {
  std::cout << "Start listen: " << fd << std::endl;
}
