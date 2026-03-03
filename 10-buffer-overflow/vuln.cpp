// g++ -m32 -fno-stack-protector -z execstack vuln.cpp -o vuln
#include <iostream>
#include <cstring>
#include <cstdio>   // for gets
#include <cstdlib>  // for system
using namespace std;

void secret() {
    cout << "💥 Buffer overflow succeeded! Accessing secret() function..." << endl;
    system("/bin/sh"); // spawns a shell
}

void vulnerable() {
    char buffer[64];
    cout << "Enter some text: ";
    gets(buffer);  // vulnerable function (removed in modern C/C++)
    cout << "You entered: " << buffer << endl;
}

int main() {
    vulnerable();
    return 0;
}
