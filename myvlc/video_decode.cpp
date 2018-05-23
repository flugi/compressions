#include "vlc.hpp"

#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char*argv[]) {
    string fname;
    if (argc == 2) {
        fname = argv[1];
    } else {
        cerr << "batch mode not implemented yet";
    }
    return 0;
}
