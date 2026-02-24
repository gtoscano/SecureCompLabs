**Task 1:** Detect HTTP access to admin pages

```bash
alert tcp any any -> any 80 \
  (msg:"Admin page access detected"; \
   content:"/admin"; nocase; \
   http_uri; \
   classtype:web-application-activity; \
   sid:1000002; rev:1;)
```

**Task 2:** Detect potential SQL injection in URL parameters

```bash
alert tcp any any -> any 80 \
  (msg:"Possible SQL injection attempt"; \
   content:"SELECT"; nocase; \
   content:"FROM"; nocase; distance:0; \
   http_uri; \
   classtype:web-application-attack; \
   sid:1000003; rev:1;)
```

**Testing (offline, safe):**
```bash
snort -r traffic.pcap -c custom.rules -A console
```

**Note:** Offline analysis only — no live network testing without authorization
