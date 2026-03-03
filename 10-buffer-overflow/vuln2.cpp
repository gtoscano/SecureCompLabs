
// Compile with: g++ -m32 -fno-stack-protector -z execstack vuln.cpp -o vuln
// You can use: objdump -d vuln | grep secret to identify the addresses?

#include <iostream>
#include <cstring>
#include <cstdio>   // for gets
#include <cstdlib>  // for system

using namespace std;

void secret1() {
    cout << "Buffer overflow succeeded! Accessing secret1() function..." << endl;
    system("/bin/sh");  // spawns a shell
}

void secret2() {
    cout << "Buffer overflow succeeded! Accessing secret2() function..." << endl;
    // Just as a demo, run 'id' or something else
    system("id");
}

void vulnerable() {
    char buffer[64];
    cout << "Enter some text: ";
    gets(buffer);  // unsafe (removed in modern standards)
    cout << "You entered: " << buffer << endl;
}

int main() {
    vulnerable();
    return 0;
}
