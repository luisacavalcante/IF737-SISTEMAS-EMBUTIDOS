from awscrt import io, mqtt, auth, http
from awsiot import mqtt_connection_builder
from paho.mqtt import client as mqtt_client
import time
import json

# AWS IOT Core
AWS_ENDPOINT = "asytcz80wa5hw-ats.iot.us-east-2.amazonaws.com"
CLIENT_ID = "fog-device"
PATH_TO_CERTIFICATE = "certificates/cfc74c0cd8b1588ca575fda1e58190d1952900f03ded726def1b188649340c43-certificate.pem.crt"
PATH_TO_PRIVATE_KEY = "certificates/cfc74c0cd8b1588ca575fda1e58190d1952900f03ded726def1b188649340c43-private.pem.key"
PATH_TO_AMAZON_ROOT_CA_1 = "certificates/root.pem"

MESSAGE = "Hello World"
TOPIC = "pet"
CLOUD_TOPIC_TO_SUBSCRIBE = "config"

# MQTT Local
BROKER_ENDPOINT = '172.22.192.1'
PORT = 1884
LOCAL_BROKER_TOPIC = "pet"
# generate client ID with pub prefix randomly
USERNAME_BROKER_LOCAL = 'teste'
PASSWORD_BROKER_LOCAL = 'teste'

def on_cloud_message_received(topic, payload, **kwargs):
    print("Received message from topic '{}': {}".format(topic, payload))
    mqtt_connection.publish(topic=TOPIC, payload=json.loads(payload), qos=mqtt.QoS.AT_LEAST_ONCE)
    # print("Published: '" + payload + "' to the topic: " + topic)

# Spin up resources
event_loop_group = io.EventLoopGroup(1)
host_resolver = io.DefaultHostResolver(event_loop_group)
client_bootstrap = io.ClientBootstrap(event_loop_group, host_resolver)
mqtt_connection = mqtt_connection_builder.mtls_from_path(
            endpoint=AWS_ENDPOINT,
            cert_filepath=PATH_TO_CERTIFICATE,
            pri_key_filepath=PATH_TO_PRIVATE_KEY,
            client_bootstrap=client_bootstrap,
            ca_filepath=PATH_TO_AMAZON_ROOT_CA_1,
            client_id=CLIENT_ID,
            clean_session=False,
            keep_alive_secs=6
            )
print("Connecting to {} with client ID '{}'...".format(AWS_ENDPOINT, CLIENT_ID))

# Make the connect() call
connect_future = mqtt_connection.connect()
# Future.result() waits until a result is available
connect_future.result()

print("Connected!")

subscribe_future, packet_id = mqtt_connection.subscribe(
    topic=CLOUD_TOPIC_TO_SUBSCRIBE,
    qos=mqtt.QoS.AT_LEAST_ONCE,
    callback=on_cloud_message_received
)

print(f'Subscribed to AWS topic {CLOUD_TOPIC_TO_SUBSCRIBE}')

# Publish message to server desired number of times.

def connect_mqtt() -> mqtt_client:
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(CLIENT_ID)
    # client.tls_set(ca_certs="./mqtt-broker/mosquitto/certs/ca-root-cert.crt")
    client.username_pw_set(USERNAME_BROKER_LOCAL, PASSWORD_BROKER_LOCAL)
    client.on_connect = on_connect
    client.connect(BROKER_ENDPOINT, PORT)
    return client


def subscribe(client: mqtt_client):
    def on_message(client, userdata, msg):
        mqtt_connection.publish(topic=TOPIC, payload=json.dumps(msg.payload.decode()), qos=mqtt.QoS.AT_LEAST_ONCE)
        print(f"Received `{msg.payload.decode()}` from `{msg.topic}` topic")

    client.subscribe(LOCAL_BROKER_TOPIC)
    client.on_message = on_message
    print(f'Subscribed to local MQTT Broker {LOCAL_BROKER_TOPIC}')


def run():
    client = connect_mqtt()
    subscribe(client)
    client.loop_forever()


if __name__ == '__main__':
    run()

# while True:
  # print('Begin Publish')
  # for i in range (RANGE):
  #     data = "{} [{}]".format(MESSAGE, i+1)
  #     message = {"message" : data}
  #     mqtt_connection.publish(topic=TOPIC, payload=json.dumps(message), qos=mqtt.QoS.AT_LEAST_ONCE)
  #     print("Published: '" + json.dumps(message) + "' to the topic: " + "'test/testing'")
  #     t.sleep(0.1)
  # print('Publish End')
  # disconnect_future = mqtt_connection.disconnect()

# disconnect_future.result()