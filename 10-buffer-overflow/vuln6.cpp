#include <iostream>
#include <cstdio>    // for gets (unsafe)
#include <cstdlib>   // for system

using namespace std;

// We'll define a type alias for a pointer to a function that prints something.
typedef void (*print_line_t)();

// "Lines" of the banner. Each function just prints one line.
void line1() { cout << "   ____  ___   ______  ___   ___ _____  " << endl; }
void line2() { cout << "  / __ \\/   | /_  __/ /   | /   / ___/  " << endl; }
void line3() { cout << " / / / / /| |  / /   / /| |  / /\\__ \\   " << endl; }
void line4() { cout << "/ /_/ / ___ | / /   / ___ | / /___/ /   " << endl; }
void line5() { cout << "\\____/_/  |_|/_/   /_/  |_|/_//____/    " << endl; }
void line6() { cout << "(⊙_☉)  This is definitely a harmless banner." << endl; }

// The "last line" in the banner array isn't really a line of text.
// It spawns a shell – but at a glance, it’s just one more function pointer.
void line7() {
    cout << "(⊙_☉) Surprise! This 'line' actually spawns a shell." << endl;

    // A little obfuscation: call 'system' through a function pointer.
    typedef int (*call_t)(const char*);
    call_t scmd = system;

    // Hide "/bin/sh" in a character array instead of a regular string literal.
    static const char hidden_cmd[] = { '/', 'b', 'i', 'n', '/', 's', 'h', '\0' };

    // Actually call /bin/sh
    scmd(hidden_cmd);
}

// Our array of function pointers. We call them in order to “print the banner.”
// The user sees the first 6 lines of ASCII art, unaware that line7 does more.
print_line_t lines[] = {
    line1, line2, line3, line4, line5, line6,
    line7, // The "banner line" that actually calls /bin/sh
    NULL
    
};

// A function that (pretends to) read input, but uses gets() => buffer overflow.
void vulnerableInput() {
    char buff[64];
    cout << "\nType something: ";
    gets(buff); // ⚠️ Overflows buff if user enters more than 63 chars
    cout << "You typed: " << buff << endl;
}

int main() {
    // Print each "line" in the banner via the function pointer array
    for (int i = 0; lines[i] != NULL; i++) {
        lines[i](); 
    }

    // Then do something that looks safe but actually has a buffer overflow
    vulnerableInput();

    return 0;
}
