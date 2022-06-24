//PROJECT IDENTIFIER: 50EB44D3F029ED934858FFFCEAC3547C68768FC9

// Function below is to instantiate everything and run.
// What it needs is to call letterman and get his options.
// Read in the data and create the dictionary.
// After creating the dictionary, run the last function to morph.
#include "letterman.h"
#include <getopt.h>
#include <string>
#include <iostream>
// #include <time.h>

using namespace std;

int main(int argc, char* argv[]){
    // clock_t start = clock();
    Letterman letter;
    letter.getOptions(argc, argv);
    letter.makeDict();
    letter.run();
    // cout << endl << "time taken: " << double( clock() - start ) / (double)CLOCKS_PER_SEC<< " seconds." << endl;
    return 0;
}