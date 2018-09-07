#!/usr/bin/env python3

import sys
import errno
import select
import socket
import threading
import collections

"""
SelectClient:

An asynchronous TCP client implemented by running an event loop in a second
thread. The event loop calls select() on two sockets, the client socket and an
indicator which is used to break out of the call for writing.


Members:

sock - main internet socket
read_ind - unix socket used to break out of select()
write_ind - unix socket written to when the main thread wants to write data
write_queue - enqueued messages accessible from both threads
write_queue_mutex - synchronize access to write_queue

on_read - read callback
on_write - write callback
"""

def tcp4_socket(block=True):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)

    if not block:
        sock.setblocking(0)

    return sock

def tcp4_server(ip, port, rebind=False, backlog=20):
    server = tcp4_socket()

    if rebind:
        server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    server.bind((ip, port))
    server.listen(backlog)
    return server

def server_accept(server, block=True):
    sock, addr = server.accept()

    if not block:
        sock.setblocking(0)

    return sock, addr

def create_ind_pair():
    server = tcp4_server("127.0.0.1", 0, rebind=True, backlog=1)
    addr = server.getsockname()

    sock1 = tcp4_socket(block=False)
    try:
        sock1.connect(addr)
        print(e)
    except OSError as e:
        if e.errno != errno.EINPROGRESS:
            raise

    sock2, _ = server_accept(server, block=False)
    server.close()
    return sock1, sock2

# indicator
read_ind, write_ind = create_ind_pair()

# socket
sock = socket.create_connection(("127.0.0.1", 8000))
print("sock: ", sock)

# shared queue
write_queue = collections.deque()
write_queue_mutex = threading.Lock()

rlist = None
wlist = None
elist = None

def loop_forever():
    while True:
        rlist = [sock, read_ind]

        with write_queue_mutex:
            if len(write_queue) > 0:
                wlist = [sock]
            else:
                wlist = []

        elist = []

        selected = select.select(rlist, wlist, elist)

        if sock in selected[0]:
            blk = sock.recv(1024)
            print("recv: ", blk)

        if read_ind in selected[0] and sock not in selected[1]:
            read_ind.recv(1)
            selected[1].append(sock)

        if sock in selected[1]:

            with write_queue_mutex:
                while len(write_queue) > 0:
                    msg = write_queue.popleft()
                    sock.sendall(msg)

            if sock in wlist:
                del wlist[0]


thread = threading.Thread(target=loop_forever, daemon=True)
thread.start()

while True:
    try:
        msg = input("send: ")
    except (EOFError, KeyboardInterrupt):
        print()
        sys.exit(0)

    try:
        with write_queue_mutex:
            write_queue.append(msg.encode())

        write_ind.send(b"0")
    except OSError as e:
        if e.errno not in (errno.EAGAIN, errno.EWOULDBLOCK):
            raise
        else:
            print("write_ind.send: blocked")
