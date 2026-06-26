#!/usr/bin/env python3
import socket
import threading
import time
import signal
import sys

TCP_OPEN = {
    21: "fake ftp",
    22: "fake ssh",
    25: "fake smtp",
    53: "fake domain",
    80: "fake http",
    110: "fake pop3",
    143: "fake imap",
    443: "fake https",
    445: "fake microsoft-ds",
    993: "fake imaps",
}

UDP_OPEN = {
    53: "fake domain",
    123: "fake ntp",
    161: "fake snmp",
    500: "fake isakmp",
    514: "fake syslog",
}

stop = threading.Event()

def tcp_server(port: int, label: str) -> None:
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind(("0.0.0.0", port))
    sock.listen(64)
    sock.settimeout(1.0)
    print(f"[tcp] listening on {port} ({label})", flush=True)

    while not stop.is_set():
        try:
            conn, addr = sock.accept()
        except socket.timeout:
            continue
        except OSError:
            break

        with conn:
            try:
                conn.sendall(f"{label} on tcp/{port}\r\n".encode())
                conn.settimeout(0.2)
                try:
                    data = conn.recv(512)
                    if data:
                        conn.sendall(b"ok\r\n")
                except socket.timeout:
                    pass
            except OSError:
                pass

    sock.close()

def udp_server(port: int, label: str) -> None:
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("0.0.0.0", port))
    sock.settimeout(1.0)
    print(f"[udp] listening on {port} ({label})", flush=True)

    while not stop.is_set():
        try:
            data, addr = sock.recvfrom(2048)
        except socket.timeout:
            continue
        except OSError:
            break

        try:
            sock.sendto(f"{label} on udp/{port}\n".encode(), addr)
        except OSError:
            pass

    sock.close()

def shutdown(signum, frame) -> None:
    stop.set()

def main() -> int:
    signal.signal(signal.SIGTERM, shutdown)
    signal.signal(signal.SIGINT, shutdown)

    for port, label in TCP_OPEN.items():
        threading.Thread(target=tcp_server, args=(port, label), daemon=True).start()

    for port, label in UDP_OPEN.items():
        threading.Thread(target=udp_server, args=(port, label), daemon=True).start()

    while not stop.is_set():
        time.sleep(0.5)

    return 0

if __name__ == "__main__":
    sys.exit(main())
