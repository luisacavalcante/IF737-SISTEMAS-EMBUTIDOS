# Copyright Amazon.com, Inc. or its affiliates. All Rights Reserved.
# SPDX-License-Identifier: MIT-0

from awscrt import io, mqtt, auth, http
from awsiot import mqtt_connection_builder
from paho.mqtt import client as mqtt_client
import time
import json

# Define ENDPOINT, CLIENT_ID, PATH_TO_CERTIFICATE, PATH_TO_PRIVATE_KEY, PATH_TO_AMAZON_ROOT_CA_1, MESSAGE, TOPIC, and RANGE
ENDPOINT = "asytcz80wa5hw-ats.iot.us-east-2.amazonaws.com"
CLIENT_ID = "testDevice"
PATH_TO_CERTIFICATE = "certificates/cfc74c0cd8b1588ca575fda1e58190d1952900f03ded726def1b188649340c43-certificate.pem.crt"
PATH_TO_PRIVATE_KEY = "certificates/cfc74c0cd8b1588ca575fda1e58190d1952900f03ded726def1b188649340c43-private.pem.key"
PATH_TO_AMAZON_ROOT_CA_1 = "certificates/root.pem"
MESSAGE = "Hello World"
TOPIC = "test/testing"
TOPIC_TO_SUBSCRIBE = "test/subscribe"
RANGE = 20

def on_cloud_message_received(topic, payload, **kwargs):
    print("Received message from topic '{}': {}".format(topic, payload))
    mqtt_connection.publish(topic=TOPIC, payload=json.loads(payload), qos=mqtt.QoS.AT_LEAST_ONCE)
    print("Published: '" + payload + "' to the topic: " + "'test/testing'")

# Spin up resources
event_loop_group = io.EventLoopGroup(1)
host_resolver = io.DefaultHostResolver(event_loop_group)
client_bootstrap = io.ClientBootstrap(event_loop_group, host_resolver)
mqtt_connection = mqtt_connection_builder.mtls_from_path(
            endpoint=ENDPOINT,
            cert_filepath=PATH_TO_CERTIFICATE,
            pri_key_filepath=PATH_TO_PRIVATE_KEY,
            client_bootstrap=client_bootstrap,
            ca_filepath=PATH_TO_AMAZON_ROOT_CA_1,
            client_id=CLIENT_ID,
            clean_session=False,
            keep_alive_secs=6
            )
print("Connecting to {} with client ID '{}'...".format(
        ENDPOINT, CLIENT_ID))
# Make the connect() call
connect_future = mqtt_connection.connect()
# Future.result() waits until a result is available
connect_future.result()
print("Connected!")

subscribe_future, packet_id = mqtt_connection.subscribe(
    topic=TOPIC_TO_SUBSCRIBE,
    qos=mqtt.QoS.AT_LEAST_ONCE,
    callback=on_cloud_message_received
)

print(f'Subscribed to {TOPIC_TO_SUBSCRIBE}')

# Publish message to server desired number of times.

while True:
  a = 1

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