# Scan types README

This document explains what each scan type sends, what replies are expected, and how the scanner classifies the result.

For byte-level packet layouts, header offsets, flags and hexadecimal examples, see the packet protocol README.

---

# 1. Global idea

For each target port, the scanner creates one probe per requested scan type.

Example with port `80` and five TCP scans:

```txt
80/SYN
80/ACK
80/NULL
80/FIN
80/XMAS
```

Each probe has its own source port, so replies can be matched back to the correct probe.

The runtime loop does:

```txt
send pending probes
read available replies with pcap
match replies to in-flight probes
classify matched replies
expire probes that timed out
print final report
```

A probe reaches a final state in one of two ways:

```txt
1. a useful reply is received
2. no useful reply is received before timeout
```

---

# 2. Matching replies to probes

A reply is not trusted just because it is TCP, UDP or ICMP.

It must match an in-flight probe.

## Direct TCP or UDP reply

For a direct reply, the scanner checks:

```txt
reply source IP      == target IP
reply destination IP == local source IP
reply source port    == target port
reply destination port == probe source port
probe state          == in flight
```

Example:

```txt
probe:
  local  172.28.0.1:40000
  target 172.28.0.10:80

reply:
  source      172.28.0.10:80
  destination 172.28.0.1:40000
```

This reply matches the probe.

## ICMP error reply

ICMP errors contain the beginning of the original packet that caused the error.

The scanner checks the embedded original packet:

```txt
original source IP      == local source IP
original destination IP == target IP
original source port    == probe source port
original destination port == target port
original protocol       == TCP or UDP, depending on the scan
probe state             == in flight
```

This is how ICMP errors are matched to the original probe.

---

# 3. SYN scan

## Request sent

The scanner sends a TCP packet with the SYN flag set.

```txt
TCP flags: SYN
```

Equivalent Nmap scan type:

```txt
-sS
```

## Expected replies

```txt
SYN/ACK reply -> port is open
RST reply     -> port is closed
ICMP filtered -> port is filtered
no reply      -> port is filtered
```

## Classification table

```txt
reply received             result
────────────────────────────────────
TCP SYN/ACK                open
TCP RST                    closed
ICMP filtered error        filtered
timeout                    filtered
```

## Meaning

SYN scan starts a TCP connection but does not complete it.

If the target replies with `SYN/ACK`, the port accepts TCP connections.

If the target replies with `RST`, the port is reachable but closed.

If there is no reply, or a filtering ICMP error is received, the probe is considered filtered.

---

# 4. ACK scan

## Request sent

The scanner sends a TCP packet with the ACK flag set.

```txt
TCP flags: ACK
```

Equivalent Nmap scan type:

```txt
-sA
```

## Expected replies

```txt
RST reply     -> port is unfiltered
ICMP filtered -> port is filtered
no reply      -> port is filtered
```

## Classification table

```txt
reply received             result
────────────────────────────────────
TCP RST                    unfiltered
ICMP filtered error        filtered
timeout                    filtered
```

## Meaning

ACK scan does not determine whether the port is open or closed.

It is used to determine whether the port is filtered by a firewall.

A `RST` means the packet reached the target stack, so the port is marked `unfiltered`.

No reply, or a filtering ICMP error, means the probe was probably filtered.

---

# 5. NULL scan

## Request sent

The scanner sends a TCP packet with no flags set.

```txt
TCP flags: none
```

Equivalent Nmap scan type:

```txt
-sN
```

## Expected replies

```txt
RST reply     -> port is closed
ICMP filtered -> port is filtered
no reply      -> port is open|filtered
```

## Classification table

```txt
reply received             result
────────────────────────────────────
TCP RST                    closed
ICMP filtered error        filtered
timeout                    open|filtered
```

## Meaning

According to classic TCP scan behavior, closed ports should answer unexpected flag combinations with `RST`.

Open ports often ignore NULL packets.

So if a `RST` is received, the port is closed.

If there is no reply, the scanner cannot distinguish between:

```txt
open port
filtered probe
```

So the result is:

```txt
open|filtered
```

---

# 6. FIN scan

## Request sent

The scanner sends a TCP packet with the FIN flag set.

```txt
TCP flags: FIN
```

Equivalent Nmap scan type:

```txt
-sF
```

## Expected replies

```txt
RST reply     -> port is closed
ICMP filtered -> port is filtered
no reply      -> port is open|filtered
```

## Classification table

```txt
reply received             result
────────────────────────────────────
TCP RST                    closed
ICMP filtered error        filtered
timeout                    open|filtered
```

## Meaning

FIN scan uses the same classification logic as NULL scan.

Closed ports normally answer with `RST`.

Open ports often ignore the packet.

No reply is ambiguous, so the scanner reports:

```txt
open|filtered
```

---

# 7. XMAS scan

## Request sent

The scanner sends a TCP packet with FIN, PSH and URG flags set.

```txt
TCP flags: FIN + PSH + URG
```

Equivalent Nmap scan type:

```txt
-sX
```

## Expected replies

```txt
RST reply     -> port is closed
ICMP filtered -> port is filtered
no reply      -> port is open|filtered
```

## Classification table

```txt
reply received             result
────────────────────────────────────
TCP RST                    closed
ICMP filtered error        filtered
timeout                    open|filtered
```

## Meaning

XMAS scan sends an unusual TCP packet with several flags enabled.

Closed ports normally answer with `RST`.

Open ports often ignore the packet.

No reply is ambiguous, so the scanner reports:

```txt
open|filtered
```

---

# 8. UDP scan

## Request sent

The scanner sends a UDP datagram to the target port.

```txt
IP protocol: UDP
UDP source port: probe source port
UDP destination port: target port
```

Equivalent Nmap scan type:

```txt
-sU
```

The UDP payload may be empty or protocol-specific.

For the generic scanner behavior:

```txt
empty UDP payload is valid
```

## Expected replies

```txt
direct UDP reply             -> port is open
ICMP port unreachable        -> port is closed
ICMP filtered error          -> port is filtered
no reply                     -> port is open|filtered
```

## Classification table

```txt
reply received                    result
──────────────────────────────────────────
UDP reply                         open
ICMP destination unreachable,
code port unreachable             closed
ICMP filtered error               filtered
timeout                           open|filtered
```

## Meaning

UDP has no handshake.

A direct UDP reply proves that something is listening on the port.

An ICMP port unreachable error means the target host actively says that no UDP service is listening on that port.

No reply is ambiguous, because UDP services are not required to answer and firewalls can silently drop packets.

So no reply means:

```txt
open|filtered
```

---

# 9. ICMP errors used by the scanner

The scanner treats these ICMP destination unreachable errors as filtered:

```txt
ICMP type 3 code 0   network unreachable
ICMP type 3 code 1   host unreachable
ICMP type 3 code 2   protocol unreachable
ICMP type 3 code 9   network administratively prohibited
ICMP type 3 code 10  host administratively prohibited
ICMP type 3 code 13  communication administratively prohibited
```

For UDP only:

```txt
ICMP type 3 code 3   port unreachable -> closed
```

---

# 10. Final result summary

```txt
scan   request sent        useful reply                  timeout
──────────────────────────────────────────────────────────────────────
SYN    TCP SYN             SYN/ACK -> open               filtered
                            RST     -> closed
                            ICMP filtered -> filtered

ACK    TCP ACK             RST -> unfiltered             filtered
                            ICMP filtered -> filtered

NULL   TCP no flags         RST -> closed                 open|filtered
                            ICMP filtered -> filtered

FIN    TCP FIN              RST -> closed                 open|filtered
                            ICMP filtered -> filtered

XMAS   TCP FIN+PSH+URG      RST -> closed                 open|filtered
                            ICMP filtered -> filtered

UDP    UDP datagram         UDP reply -> open             open|filtered
                            ICMP port unreachable -> closed
                            ICMP filtered -> filtered
```

---

# 11. What the scanner ignores

Packets are ignored when they do not prove anything about an in-flight probe.

Ignored cases include:

```txt
packet is not IPv4
protocol is unsupported
packet is truncated
TCP/UDP ports do not match any in-flight probe
ICMP embedded packet does not match any in-flight probe
reply matches a probe but has no useful classification
probe is already done
```

The scanner never updates a probe result from an unrelated packet.

---

# 12. Why source ports matter

Each probe uses a source port controlled by the scanner.

Example:

```txt
probe 0:
  target port = 21
  source port = 40000

probe 1:
  target port = 22
  source port = 40001

probe 2:
  target port = 23
  source port = 40002
```

When a reply comes back, the destination port of the reply is the source port of the original probe.

This allows the scanner to match replies even when many probes are in flight at the same time.

Example direct TCP reply:

```txt
target 172.28.0.10:22 -> local 172.28.0.1:40001
```

This matches the probe whose source port was `40001`.

For ICMP errors, the source port is read from the embedded original TCP or UDP header.

---

# 13. Why timeouts are part of the result

Some scan types rely on the absence of a reply.

For example:

```txt
NULL / FIN / XMAS:
  no reply -> open|filtered
```

So timeout is not just an error path.

It is part of the scan logic.

The runtime marks a probe as timed out when:

```txt
probe is still in flight
configured timeout has elapsed
no useful reply was matched
```

Then the timeout result depends on the scan type.

---

# 14. What the report displays

The report does not perform network logic.

It only displays the final result already stored in each probe.

The report reads:

```txt
probe destination port
probe scan type
probe state
probe result
```

Then it prints one line per port and one column per enabled scan type.

Example:

```txt
PORT    SYN       NULL           FIN            XMAS           ACK          UDP
21      open      open|filtered  open|filtered  open|filtered  unfiltered   open|filtered
22      open      open|filtered  open|filtered  open|filtered  unfiltered   closed
23      closed    closed         closed         closed         unfiltered   closed
```
