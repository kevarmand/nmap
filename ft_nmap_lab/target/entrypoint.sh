#!/bin/sh
set -eu

iptables -F INPUT
iptables -P INPUT ACCEPT

for p in 30 81 88 111 135 139 515 587 631 1021; do
    iptables -A INPUT -p tcp --dport "$p" -j DROP
done

for p in 37 42 113 119 389; do
    iptables -A INPUT -p tcp --dport "$p" -j REJECT --reject-with icmp-host-prohibited
done

for p in 69 111 520 631 1021; do
    iptables -A INPUT -p udp --dport "$p" -j DROP
done

for p in 137 138 162 389 450; do
    iptables -A INPUT -p udp --dport "$p" -j REJECT --reject-with icmp-host-prohibited
done

echo "[target] TCP open ports:      21 22 25 53 80 110 143 443 445 993"
echo "[target] UDP open ports:      53 123 161 500 514"
echo "[target] TCP DROP filtered:   30 81 88 111 135 139 515 587 631 1021"
echo "[target] TCP REJECT filtered: 37 42 113 119 389"
echo "[target] UDP DROP filtered:   69 111 520 631 1021"
echo "[target] UDP REJECT filtered: 137 138 162 389 450"
echo "[target] Static IP:           172.28.0.10"

exec python3 /service_lab.py
