// Project Identifier: 1761414855B69983BD8035097EFBD312EB0527F0
#include <string>
#include <iostream>
#include <iomanip>
#include "drone.h"

using namespace std;

class Driver{
private:
    uint32_t option = 0;

    Drones drone;

public:
    void get_mode(int argc, char *argv[]){
        int choice = 0, option_index = 0;
        opterr = false;
        struct option long_opts[] = {
            {"mode", required_argument, nullptr, 'm'},
            {"help,", no_argument, nullptr, 'h'},
            {nullptr, 0, nullptr, '\0'}
        };

        while ((choice = getopt_long(argc, argv, "m:h", long_opts, &option_index)) != -1){
            switch (choice){
                case 'm':{
                    if (strcmp(optarg, "MST") == 0){
                        // std::cout << "THIS IS MST\n===============\n";
                        option = 1;
                    }
                    else if (strcmp(optarg, "FASTTSP") == 0){
                        // std::cout << "THIS IS FASTTSP\n===============\n";
                        option = 2;
                    }
                    else if(strcmp(optarg, "OPTTSP") == 0){
                        // std::cout << "THIS IS OPTTSP\n===============\n";
                        option = 3;
                    }
                    else{
                        cerr << "Invalid mode type\n";
                        exit(1);
                    }
                    break;
                }

                case 'h':{
                    std::cout << "This is the help\n";
                    exit(0); break;
                }

                default:{
                    cerr << "This command don't work\n";
                    exit(1);
                }
            }
        }
        if (option == 0){
            cerr << "There is nothing or invalid in the command line\n";
            exit(1);
        }
    }

    void run(){
        // std::cout << "This is the option: " << option << "\n=====================\n\n";
        drone.deliver(option);
    }
};


int main(int argc, char *argv[]){
    //some recommended things
    cout << std::setprecision(2);
    cout << std::fixed;

    Driver driver;
    driver.get_mode(argc, argv);
    driver.run();

    return 0;
}