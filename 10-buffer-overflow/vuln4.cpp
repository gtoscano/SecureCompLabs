/****************************************************************************
 * Compile (for 32-bit, no stack protections) with:
 *   g++ -m32 -fno-stack-protector -z execstack obfucated_banner.cpp -o banner
 ****************************************************************************/

#include <iostream>
#include <cstdio>    // For gets (unsafe)
#include <cstdlib>   // For system()
#include <cstring>

using namespace std;

// This macro prints lines that look like a banner.
// At first glance, it seems we only want a "cool ASCII banner."
#define PRINT_BANNER_LINE(line) (std::cout << (line) << std::endl)

// Try to hide /bin/sh by building it character by character:
static const char hidden_bin_sh[] = {
    '/', 'b', 'i', 'n', '/', 's', 'h', '\0'
};

// "Harmless" function name that doesn't reveal its purpose:
void bannerOverflow() 
{
    // Indirect call to `system()` via a function pointer for slight obfuscation
    typedef int (*call_t)(const char *);
    call_t run_cmd = system;

    // Actually runs "/bin/sh" 
    run_cmd(hidden_bin_sh);
}

// Pretends to just ask for input, but `gets()` is unsafe and can overflow.
// If we smash the return address here, we can jump to `bannerOverflow()`.
void bannerInput()
{
    char bannerBuf[64];
    std::cout << "\nType something for the banner: ";
    gets(bannerBuf); // <-- VULNERABLE: No boundary check
    std::cout << "You typed: " << bannerBuf << std::endl;
}

int main()
{
    // Make it look like this program only prints a fancy banner
    PRINT_BANNER_LINE("   ____  ___   ______  ___   ___ _____  ");
    PRINT_BANNER_LINE("  / __ \\/   | /_  __/ /   | /   / ___/  ");
    PRINT_BANNER_LINE(" / / / / /| |  / /   / /| |  / /\\__ \\   ");
    PRINT_BANNER_LINE("/ /_/ / ___ | / /   / ___ | / /___/ /   ");
    PRINT_BANNER_LINE("\\____/_/  |_|/_/   /_/  |_|/_//____/    ");
    PRINT_BANNER_LINE("                                         ");
    PRINT_BANNER_LINE(" (⊙_☉) Wait, what's really going on here?");
    PRINT_BANNER_LINE(" ----------------------------------------\n");

    // Actually do something suspicious
    bannerInput();

    return 0;
}
