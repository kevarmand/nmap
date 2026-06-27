# Packet layer README

This document describes the packet formats used by the scanner and the fields read from replies.

Supported scan packet families:

```txt
TCP:
  SYN
  ACK
  NULL
  FIN
  XMAS

UDP:
  UDP
```

---

# Global packet layouts

## Sent TCP probe: raw IPv4 + TCP

The raw socket send buffer contains no Ethernet header.

```txt
Raw buffer passed to sendto()

byte offset
0x00  ┌──────────────────────────────────────────────┐
      │ IPv4 header                                  │ 20 bytes
0x14  ├──────────────────────────────────────────────┤
      │ TCP header                                   │ 20 bytes
0x28  └──────────────────────────────────────────────┘
```

Example:

```txt
0000  45 00 00 28 9c 40 00 00 40 06 86 4c ac 1c 00 01
0010  ac 1c 00 0a 9c 40 00 15 10 00 00 00 00 00 00 00
0020  50 02 04 00 a7 49 00 00
```

Boundaries:

```txt
0000    45 00 00 28 9c 40 00 00 40 06 86 4c ac 1c 00 01    IPv4
0010    ac 1c 00 0a                                        IPv4
0014    9c 40 00 15 10 00 00 00 00 00 00 00 50 02 04 00    TCP
0024    a7 49 00 00                                        TCP
```

---

## Sent UDP probe: raw IPv4 + UDP

The raw socket send buffer contains no Ethernet header.

```txt
Raw buffer passed to sendto()

byte offset
0x00  ┌──────────────────────────────────────────────┐
      │ IPv4 header                                  │ 20 bytes
0x14  ├──────────────────────────────────────────────┤
      │ UDP header                                   │ 8 bytes
0x1c  └──────────────────────────────────────────────┘
```

Example probe:

```txt
source IP        = 172.28.0.1
target IP        = 172.28.0.10
source port      = 40000
target port      = 53
payload length   = 0
```

Same values in hex:

```txt
172.28.0.1   = ac 1c 00 01
172.28.0.10  = ac 1c 00 0a
40000        = 9c 40
53           = 00 35
UDP length   = 00 08
```

Example UDP packet:

```txt
0000  45 00 00 1c 9c 40 00 00 40 11 86 49 ac 1c 00 01
0010  ac 1c 00 0a 9c 40 00 35 00 08 b1 2c
```

Boundaries:

```txt
0000    45 00 00 1c 9c 40 00 00 40 11 86 49 ac 1c 00 01    IPv4
0010    ac 1c 00 0a                                        IPv4
0014    9c 40 00 35 00 08 b1 2c                            UDP
```

Important bytes:

```txt
45             -> IPv4, 20-byte IPv4 header
00 1c          -> total length = 28 bytes
40             -> TTL 64
11             -> protocol UDP
ac 1c 00 01    -> source IP 172.28.0.1
ac 1c 00 0a    -> target IP 172.28.0.10

9c 40          -> UDP source port 40000
00 35          -> UDP destination port 53
00 08          -> UDP length = 8 bytes
b1 2c          -> UDP checksum
```

---

## Captured direct TCP reply: Ethernet + IPv4 + TCP

Packets received with pcap include the Ethernet header.

```txt
Pcap captured packet, DLT_EN10MB

byte offset
0x00  ┌──────────────────────────────────────────────┐
      │ Ethernet header                              │ 14 bytes
0x0e  ├──────────────────────────────────────────────┤
      │ IPv4 header                                  │ ip->ihl * 4 bytes
0x22  ├──────────────────────────────────────────────┤
      │ TCP header                                   │ tcp->doff * 4 bytes
      └──────────────────────────────────────────────┘
```

Example:

```txt
0000  02 42 ac 1c 00 01 8a c3 a6 4b 93 8a 08 00 45 00
0010  00 28 00 00 40 00 40 06 22 89 ac 1c 00 0a ac 1c
0020  00 01 00 15 9c 40 22 11 33 44 10 00 00 01 50 12
0030  fa f0 12 34 00 00
```

Boundaries:

```txt
0000    02 42 ac 1c 00 01 8a c3 a6 4b 93 8a 08 00          Ethernet
000e    45 00 00 28 00 00 40 00 40 06 22 89 ac 1c 00 0a    IPv4
001e    ac 1c 00 01                                        IPv4
0022    00 15 9c 40 22 11 33 44 10 00 00 01 50 12 fa f0    TCP
0032    12 34 00 00                                        TCP
```

---

## Captured direct UDP reply: Ethernet + IPv4 + UDP

```txt
Pcap captured packet, DLT_EN10MB

byte offset
0x00  ┌──────────────────────────────────────────────┐
      │ Ethernet header                              │ 14 bytes
0x0e  ├──────────────────────────────────────────────┤
      │ IPv4 header                                  │ ip->ihl * 4 bytes
0x22  ├──────────────────────────────────────────────┤
      │ UDP header                                   │ 8 bytes
0x2a  ├──────────────────────────────────────────────┤
      │ UDP payload                                  │ optional
      └──────────────────────────────────────────────┘
```

Example direct UDP reply:

```txt
0000  02 42 ac 1c 00 01 8a c3 a6 4b 93 8a 08 00 45 00
0010  00 1c 00 00 40 00 40 11 22 7c ac 1c 00 0a ac 1c
0020  00 01 00 35 9c 40 00 08 7a b2
```

Boundaries:

```txt
0000    02 42 ac 1c 00 01 8a c3 a6 4b 93 8a 08 00          Ethernet
000e    45 00 00 1c 00 00 40 00 40 11 22 7c ac 1c 00 0a    IPv4
001e    ac 1c 00 01                                        IPv4
0022    00 35 9c 40 00 08 7a b2                            UDP
```

Read fields:

```txt
IPv4:
  11             -> protocol UDP
  ac 1c 00 0a    -> reply source IP = target IP
  ac 1c 00 01    -> reply destination IP = local IP

UDP:
  00 35          -> reply source port = 53
  9c 40          -> reply destination port = 40000
  00 08          -> UDP length = 8
```

Meaning:

```txt
UDP reply from 172.28.0.10:53 to 172.28.0.1:40000
```

Classification:

```txt
UDP scan + direct UDP reply -> open
```

---

## Captured ICMP error reply: Ethernet + outer IPv4 + ICMP + inner IPv4 + inner transport

ICMP errors contain part of the packet that caused the error.

```txt
Pcap captured ICMP error packet

byte offset
0x00  ┌──────────────────────────────────────────────┐
      │ Ethernet header                              │ 14 bytes
0x0e  ├──────────────────────────────────────────────┤
      │ Outer IPv4 header                            │ router/filter -> local host
0x22  ├──────────────────────────────────────────────┤
      │ ICMP header                                  │ 8 bytes
0x2a  ├──────────────────────────────────────────────┤
      │ Inner IPv4 header                            │ original packet we sent
0x3e  ├──────────────────────────────────────────────┤
      │ Inner TCP or UDP header beginning            │ original ports
      └──────────────────────────────────────────────┘
```

Example ICMP error for a TCP probe:

```txt
0000  02 42 ac 1c 00 01 8a c3 a6 4b 93 8a 08 00 45 00
0010  00 38 12 34 00 00 40 01 aa bb ac 1c 00 0a ac 1c
0020  00 01 03 0d cc dd 00 00 45 00 00 28 9c 40 00 00
0030  40 06 86 4c ac 1c 00 01 ac 1c 00 0a 9c 40 00 25
0040  10 00 00 00 00 00 00 00
```

Boundaries:

```txt
0000    02 42 ac 1c 00 01 8a c3 a6 4b 93 8a 08 00          Ethernet
000e    45 00 00 38 12 34 00 00 40 01 aa bb ac 1c 00 0a    Outer IPv4
001e    ac 1c 00 01                                        Outer IPv4
0022    03 0d cc dd 00 00 00 00                            ICMP
002a    45 00 00 28 9c 40 00 00 40 06 86 4c ac 1c 00 01    Inner IPv4
003a    ac 1c 00 0a                                        Inner IPv4
003e    9c 40 00 25 10 00 00 00                            Inner TCP beginning
```

Example ICMP error for a UDP probe:

```txt
0000  02 42 ac 1c 00 01 8a c3 a6 4b 93 8a 08 00 45 00
0010  00 30 12 34 00 00 40 01 aa c3 ac 1c 00 0a ac 1c
0020  00 01 03 03 cc dd 00 00 45 00 00 1c 9c 40 00 00
0030  40 11 86 49 ac 1c 00 01 ac 1c 00 0a 9c 40 00 35
0040  00 08 b1 2c
```

Boundaries:

```txt
0000    02 42 ac 1c 00 01 8a c3 a6 4b 93 8a 08 00          Ethernet
000e    45 00 00 30 12 34 00 00 40 01 aa c3 ac 1c 00 0a    Outer IPv4
001e    ac 1c 00 01                                        Outer IPv4
0022    03 03 cc dd 00 00 00 00                            ICMP
002a    45 00 00 1c 9c 40 00 00 40 11 86 49 ac 1c 00 01    Inner IPv4
003a    ac 1c 00 0a                                        Inner IPv4
003e    9c 40 00 35 00 08 b1 2c                            Inner UDP
```

Read fields:

```txt
Outer IPv4:
  01             -> protocol ICMP
  src IP         -> ICMP sender
  dst IP         -> local IP

ICMP:
  first byte     -> type
  second byte    -> code

Inner IPv4:
  06             -> original protocol TCP
  11             -> original protocol UDP
  src IP         -> original local IP
  dst IP         -> original target IP

Inner transport:
  first 2 bytes  -> original source port
  next 2 bytes   -> original destination port
```

---

# IPv4 header details

IPv4 header bytes from the TCP example:

```txt
45 00 00 28 9c 40 00 00 40 06 86 4c ac 1c 00 01 ac 1c 00 0a
```

Layout:

```txt
offset  bytes                  field
────────────────────────────────────────────────────
0x00    45                     version + IHL
0x01    00                     TOS
0x02    00 28                  total length
0x04    9c 40                  identification
0x06    00 00                  flags + fragment offset
0x08    40                     TTL
0x09    06                     protocol
0x0a    86 4c                  IPv4 checksum
0x0c    ac 1c 00 01            source IP
0x10    ac 1c 00 0a            destination IP
```

## IPv4 byte 0: version + IHL

Raw byte:

```txt
45
```

Binary:

```txt
0x45 = 0100 0101
```

Split:

```txt
0100 0101
^^^^ ^^^^
│    │
│    └── IHL     = 0101 = 5
└─────── version = 0100 = 4
```

Meaning:

```txt
version = 4              -> IPv4
IHL     = 5              -> 5 * 4 = 20 bytes IPv4 header
```

## IPv4 protocol byte

Raw byte for TCP:

```txt
06
```

Meaning:

```txt
0x06 = TCP
```

Raw byte for UDP:

```txt
11
```

Meaning:

```txt
0x11 = UDP
```

Raw byte for ICMP:

```txt
01
```

Meaning:

```txt
0x01 = ICMP
```

---

# TCP header details

TCP header bytes from the SYN example:

```txt
9c 40 00 15 10 00 00 00 00 00 00 00 50 02 04 00 a7 49 00 00
```

Layout:

```txt
offset  bytes                  field
────────────────────────────────────────────────────
0x00    9c 40                  source port
0x02    00 15                  destination port
0x04    10 00 00 00            sequence number
0x08    00 00 00 00            acknowledgement number
0x0c    50                     data offset + reserved
0x0d    02                     flags
0x0e    04 00                  window size
0x10    a7 49                  TCP checksum
0x12    00 00                  urgent pointer
```

## TCP byte 12: data offset

Raw byte:

```txt
50
```

Binary:

```txt
0x50 = 0101 0000
```

Split:

```txt
0101 0000
^^^^ ^^^^
│    │
│    └── reserved = 0000
└─────── data offset = 0101 = 5
```

Meaning:

```txt
TCP header length = 5 * 4 = 20 bytes
```

## TCP byte 13: flags

Flag masks:

```txt
FIN = 0x01 = 0000 0001
SYN = 0x02 = 0000 0010
RST = 0x04 = 0000 0100
PSH = 0x08 = 0000 1000
ACK = 0x10 = 0001 0000
URG = 0x20 = 0010 0000
```

Flags sent by scan type:

```txt
scan   flags byte   binary       meaning
────────────────────────────────────────────
SYN    0x02         0000 0010    SYN
NULL   0x00         0000 0000    no flag
FIN    0x01         0000 0001    FIN
XMAS   0x29         0010 1001    FIN + PSH + URG
ACK    0x10         0001 0000    ACK
```

XMAS calculation:

```txt
FIN = 0x01
PSH = 0x08
URG = 0x20

0x01 | 0x08 | 0x20 = 0x29
```

---

# UDP header details

UDP header bytes from the UDP example:

```txt
9c 40 00 35 00 08 b1 2c
```

Layout:

```txt
offset  bytes                  field
────────────────────────────────────────────────────
0x00    9c 40                  source port
0x02    00 35                  destination port
0x04    00 08                  UDP length
0x06    b1 2c                  UDP checksum
```

## UDP bytes 0-1: source port

Raw bytes:

```txt
9c 40
```

Value:

```txt
0x9c40 = 40000
```

Meaning:

```txt
source port = probe->src_port
```

## UDP bytes 2-3: destination port

Raw bytes:

```txt
00 35
```

Value:

```txt
0x0035 = 53
```

Meaning:

```txt
destination port = probe->dst_port
```

## UDP bytes 4-5: UDP length

Raw bytes:

```txt
00 08
```

Value:

```txt
0x0008 = 8
```

Meaning:

```txt
UDP header length = 8 bytes
UDP payload length = 0 bytes
```

If payload is added later:

```txt
UDP length = 8 + payload_size
```

## UDP bytes 6-7: UDP checksum

Raw bytes:

```txt
b1 2c
```

Meaning:

```txt
UDP checksum
```

UDP checksum input:

```txt
UDP pseudo-header + UDP header + UDP payload
```

Pseudo-header:

```txt
source IP
destination IP
zero byte
protocol = 0x11
UDP length
```

---

# Direct TCP reply parsing

The parser reads:

```txt
outer IPv4 source      -> reply.src_ip
outer IPv4 destination -> reply.dst_ip
TCP source port        -> reply.src_port
TCP destination port   -> reply.dst_port
TCP flags byte         -> reply.tcp_flags
```

Useful direct TCP reply must match:

```txt
reply.src_ip    == probe->target_ip
reply.dst_ip    == local source IP
reply.src_port  == probe->dst_port
reply.dst_port  == probe->src_port
probe.state     == PROBE_IN_FLIGHT
```

## SYN/ACK reply

TCP flags byte:

```txt
12
```

Binary:

```txt
0x12 = 0001 0010
```

Split:

```txt
0001 0010
|||| ||||
|||| |||└── FIN = 0
|||| ||└─── SYN = 1
|||| |└──── RST = 0
|||| └───── PSH = 0
|||└─────── ACK = 1
||└──────── URG = 0
|└───────── ECE = 0
└────────── CWR = 0
```

Classification:

```txt
SYN scan + SYN/ACK -> open
```

## RST reply

TCP flags byte:

```txt
04
```

Binary:

```txt
0x04 = 0000 0100
```

Classification:

```txt
SYN scan  + RST -> closed
NULL scan + RST -> closed
FIN scan  + RST -> closed
XMAS scan + RST -> closed
ACK scan  + RST -> unfiltered
```

---

# Direct UDP reply parsing

The parser reads:

```txt
outer IPv4 source      -> reply.src_ip
outer IPv4 destination -> reply.dst_ip
UDP source port        -> reply.src_port
UDP destination port   -> reply.dst_port
```

Useful direct UDP reply must match:

```txt
reply.src_ip    == probe->target_ip
reply.dst_ip    == local source IP
reply.src_port  == probe->dst_port
reply.dst_port  == probe->src_port
probe.state     == PROBE_IN_FLIGHT
```

Classification:

```txt
UDP scan + direct UDP reply -> open
```

---

# ICMP error parsing

ICMP error replies are used for both TCP and UDP scans.

The parser reads:

```txt
Outer IPv4:
  protocol      -> must be ICMP
  source IP     -> reply.src_ip
  destination IP -> reply.dst_ip

ICMP:
  type          -> reply.icmp_type
  code          -> reply.icmp_code

Inner IPv4:
  protocol      -> reply.original_protocol
  source IP     -> reply.original_src_ip
  destination IP -> reply.original_dst_ip

Inner TCP/UDP:
  source port   -> reply.original_src_port
  destination port -> reply.original_dst_port
```

Useful ICMP reply must match:

```txt
reply.original_src_ip   == local source IP
reply.original_dst_ip   == probe->target_ip
reply.original_src_port == probe->src_port
reply.original_dst_port == probe->dst_port
probe.state             == PROBE_IN_FLIGHT
```

For TCP scans:

```txt
reply.original_protocol == TCP
```

For UDP scans:

```txt
reply.original_protocol == UDP
```

---

# ICMP destination unreachable codes

ICMP destination unreachable:

```txt
type = 3
```

Codes used as filtered:

```txt
type  code  hex   meaning
────────────────────────────────────────────────────────────
3     0     03 00 network unreachable
3     1     03 01 host unreachable
3     2     03 02 protocol unreachable
3     9     03 09 network administratively prohibited
3     10    03 0a host administratively prohibited
3     13    03 0d communication administratively prohibited
```

UDP port unreachable:

```txt
type  code  hex   meaning
────────────────────────────────────────────────────────────
3     3     03 03 port unreachable
```

Classification:

```txt
TCP scan + ICMP type 3 + filtered code -> filtered

UDP scan + ICMP type 3 code 3        -> closed
UDP scan + ICMP type 3 filtered code -> filtered
```

---

# Timeout classification

Timeout is not read from a packet.

It happens when:

```txt
probe.state == PROBE_IN_FLIGHT
now_ms - probe->sent_at_ms >= config->scan.timeout_ms
```

Timeout result:

```txt
SYN   timeout -> filtered
ACK   timeout -> filtered
NULL  timeout -> open|filtered
FIN   timeout -> open|filtered
XMAS  timeout -> open|filtered
UDP   timeout -> open|filtered
```

---

# Final classification table

```txt
scan   sent packet        useful reply                    timeout
────────────────────────────────────────────────────────────────────────────
SYN    TCP SYN            TCP SYN/ACK -> open             filtered
                         TCP RST     -> closed
                         ICMP filtered -> filtered

ACK    TCP ACK            TCP RST     -> unfiltered       filtered
                         ICMP filtered -> filtered

NULL   TCP no flag        TCP RST     -> closed           open|filtered
                         ICMP filtered -> filtered

FIN    TCP FIN            TCP RST     -> closed           open|filtered
                         ICMP filtered -> filtered

XMAS   TCP FIN+PSH+URG    TCP RST     -> closed           open|filtered
                         ICMP filtered -> filtered

UDP    UDP datagram       UDP reply   -> open             open|filtered
                         ICMP port unreachable -> closed
                         ICMP filtered -> filtered
```

---

# Ignored packets

The parser ignores packets when:

```txt
unsupported datalink
not IPv4
IPv4 header is truncated
IPv4 header length is invalid
TCP header is truncated
UDP header is truncated
ICMP header is truncated
inner IPv4 header is truncated
inner TCP/UDP header is truncated
protocol is unsupported
packet does not match an in-flight probe
matched packet gives no useful classification
```
