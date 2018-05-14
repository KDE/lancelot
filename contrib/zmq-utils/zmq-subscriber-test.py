#!/usr/bin/python3

import zmq
import time

context = zmq.Context()
socket = context.socket(zmq.SUB)
socket.connect("ipc:///tmp/ivan-zmq-voy-socket-out")
socket.setsockopt_string(zmq.SUBSCRIBE, '')

while True:
    msg = socket.recv()
    # print(str(msg, 'utf-8'))
    print(msg)

