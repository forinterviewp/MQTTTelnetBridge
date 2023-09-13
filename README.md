# MQTTTelnetBridge


## fast dive in:

Use docker compose to fireup services:

`docker-compose up --build`

Create a topic:

`docker exec -it mqtttelnetbridge_mosquitto_1 mosquitto_pub -t 'test/topic' -m 'hello world'`

(Optional) Subscribe via console: 

`docker exec -it mqtttelnetbridge_mosquitto_1 mosquitto_sub -t 'test/topic' -v`

Use telnet:

`telnet 127.0.0.1 1234`

Commands:

`subscribe <topic>` to subscribe a topic

`poll` start listen to topics

`` empty line to drop telnet connection


## MQTT broker

`docker pull eclipse-mosquitto:2.0.17`

`docker run -it --rm --name mosquitto eclipse-mosquitto:2.0.17`

Subscribe: `docker exec -it mosquitto mosquitto_sub -t 'test/topic' -v`

Publish: `docker exec -it mosquitto mosquitto_pub -t 'test/topic' -m 'hello world'`


## Developer Shortcuts

`docker build -t mqttbridge_dev -f ./develop.dockerfile .`

`docker run -it --rm -p1234:1234 -v $(pwd -P)/src:/src:ro --name mqttbridge_dev mqttbridge_dev`

`mkdir ../build && cd ../build`

`cmake ../src && make && ./server/server 1234`

