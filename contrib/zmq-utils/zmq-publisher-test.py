#!/usr/bin/python3

import zmq
import time
import random

context = zmq.Context()
socket = context.socket(zmq.PUB)
socket.bind("ipc:///tmp/ivan-zmq-voy-socket-in")

while True:
    val = random.randrange(1, 1000)
    print("Message:%d" % (val))
    socket.send_string("Message:%d" % (val))
    time.sleep(1)

