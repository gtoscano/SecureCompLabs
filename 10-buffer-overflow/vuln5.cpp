#include <iostream>
#include <cstring>
#include <cstdlib>  // for system
using namespace std;

void secret() {
    cout << "💥 Buffer overflow succeeded! Accessing secret() function..." << endl;
    system("/bin/sh"); // spawns a shell
}

void vulnerable(char *input) {
    char buffer[64];
    // Danger: strcpy does not check if 'input' is bigger than 'buffer'
    strcpy(buffer, input);
    cout << "You entered: " << buffer << endl;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << "Usage: " << argv[0] << " <data>" << endl;
        return 1;
    }
    vulnerable(argv[1]);
    return 0;
}
