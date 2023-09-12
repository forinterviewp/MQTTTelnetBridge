# MQTTTelnetBridge


## Use telnet

`telnet 127.0.0.1 1234`


## MQTT broker

`docker pull eclipse-mosquitto:2.0.17`

`docker run -it --rm --name mosquitto eclipse-mosquitto:2.0.17`

Subscribe: `docker exec -it mosquitto mosquitto_sub -t 'test/topic' -v`

Publish: `docker exec -it mosquitto mosquitto_pub -t 'test/topic' -m 'hello world'`


## Developer Shortcuts

`docker build -t mqttbridge`

`docker run -it --rm -v $(pwd -P)/src:/src:ro --name mqttbridge mqttbridge`

`mkdir ../build && cd ../build`

`cmake ../src && make && ./server/server 1234`

