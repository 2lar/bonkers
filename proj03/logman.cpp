//Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3E83350

#include <iostream>
#include <string>
#include <cstring>
#include <sstream>
#include "logman.h"


using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2){
        cerr << "Lol there was some error in the command line\n";
        exit(1);
    }

    string help = "-h", helplong = "--h";
    if (argv[1] == help || argv[1] == helplong){
        cout << "This is the help message for LOGMAN\n";
        exit(0);
    }

    //open logfile
    ifstream logfile;
    //this makes logfile as the contents of argv[1] if not help
    try{
        logfile.open(argv[1]);
        if (!logfile.is_open()){
            throw (101);
        }
    }catch(int e){
        cerr << "The LOGFILE did not open\n";
        exit(1);
    }

    Collection thelogs;
    thelogs.make_data(logfile);
    thelogs.runlogman();
    return 0;
}