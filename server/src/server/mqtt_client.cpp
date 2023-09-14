#include "mqtt_client.hpp"

#include <mqtt/client.h>

#include <iostream>
#include <thread>
#include <chrono>

#include <netdb.h>


const std::string SERVER_ADDRESS{ "tcp://mosquitto:1883" };
const std::string CLIENT_ID{ "sync_consume" };


void MQTTClient::AddSubscription(int fd, std::string topic) {
  std::cout << "Add Subscription: " << fd
            << ", topic: " << topic << std::endl;

  sessions[fd].fd = fd;
  sessions[fd].subscriptions.push_back(std::move(topic));
}


void MQTTClient::CloseSession(int fd) {
  sessions.erase(fd);
  std::cout << "Close session: " << fd << std::endl;
}


// TODO: random
std::string generate_client_id(const Session& session) {
  return CLIENT_ID + "_" + std::to_string(session.fd);
}


bool write_to_subscriber(int fd, const std::string& response) {
  bool fail_to_write = false;

  ssize_t writed = send(fd, response.c_str(), response.size(), 0);
  if (writed == -1) {
    perror("send()");
  }

  // When session killed - write will be unavailable
  // TODO(wisp): better aproach on epoll mechanics.
  if (static_cast<std::size_t>(writed) != response.size())
    fail_to_write = true;

  return fail_to_write;
}


void MQTTSession(Session session) noexcept {
  try {
    mqtt::client cli(SERVER_ADDRESS, generate_client_id(session));

    auto connOpts = mqtt::connect_options_builder()
      .keep_alive_interval(std::chrono::seconds(30))
      .automatic_reconnect(std::chrono::seconds(2), std::chrono::seconds(30))
      .clean_session(false)
      .finalize();

    std::cout << "Connecting to the MQTT server..." << std::endl;
    mqtt::connect_response rsp = cli.connect(connOpts);
    std::cout << "Connecting to the MQTT server done" << std::endl;

    if (!rsp.is_session_present()) {
      std::cout << "Subscribing to topics..." << std::endl;
      cli.subscribe(session.subscriptions);
      std::cout << "Subscribing to topics OK" << std::endl;
    }
    else {
      std::cout << "Session already present. Skipping subscribe." << std::endl;
    }

    while (true) {
      auto msg = cli.consume_message();

      if (msg) {
        // Debug command sections:
        if (msg->get_topic() == "command" &&
            msg->to_string() == "exit") {
          std::cout << "Exit command received" << std::endl;
          break;
        }

        // Subscribers response:
        std::string response = msg->get_topic() + ": " + msg->to_string();
        bool fail_to_write = write_to_subscriber(session.fd, response);
        if (fail_to_write)
          break;
      }
      else if (!cli.is_connected()) {
        // Reconet:
        std::cout << "Lost connection" << std::endl;
        while (!cli.is_connected()) {
          std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
        std::cout << "Re-established connection" << std::endl;
      }
    }

    std::cout << "\nDisconnecting from the MQTT server..." << std::endl;
    cli.disconnect();

  }
  catch (const mqtt::exception& ex) {
    std::cout << "mqtt exception: " << ex.what() << std::endl;
  }
  catch (const std::exception& ex) {
    std::cout << "std exception: " << ex.what() << std::endl;
  }
  catch(...) {
    std::cout << "Unknown exception" << std::endl;
  }

}


void MQTTClient::StartListen(int fd) {
  std::cout << "Start listen: " << fd << std::endl;

  if (sessions.count(fd)) {
    // copy session to thread
    std::thread t(MQTTSession, sessions[fd]);
    t.detach();
  } else {
    write_to_subscriber(fd, "No subscriptions\n");
  }
}

