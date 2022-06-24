// Project identifier: 0E04A31E0D60C01986ACB20081C9D8722A1899B6
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include <getopt.h>
#include "market.h"

using namespace std;

struct Input{
    int traders = 0, stocks = 0;
    bool mode = true;
};

stringstream randomizer(Input inputs){
    stringstream ss;

    if (inputs.mode) return ss;

    string junk;
    int seed, orders, rate;

    getline(cin, junk);
    cin >> junk >> seed;
    cin >> junk >> orders;
    cin >> junk >> rate;

    P2random::PR_init(ss, seed, inputs.traders, inputs.stocks, orders, rate);

    return ss;
}

class Driver{
private:
    Input theinputs;
    Option theoptions;

public:

    void get_options(int argc, char* argv[]){
        int choice = 0, option_index = 0;
        opterr = false;

        struct option long_opts[] =
        {
            {"verbose", no_argument, nullptr, 'v'},
            {"median", no_argument, nullptr, 'm'},
            {"trader_info", no_argument, nullptr, 'i'},
            {"time_travelers", no_argument, nullptr, 't'},
            {nullptr, 0, nullptr, '\0'}
        };
        
        while ((choice = getopt_long(argc, argv, "vmit", long_opts, &option_index)) != -1){
            switch(choice){
                case 'v':{
                    theoptions.verbose = true;
                    break;
                }
                case 'm':{
                    theoptions.median = true;
                    break;
                }
                case 'i':{
                    theoptions.itrader = true;
                    break;
                }
                case 't':{
                    theoptions.traveler = true;
                    break;
                }
                default:{
                    break;
                }
            }//end switch
        }//end while
    }//end get_options

    void get_inputs(){
        string junk, mode;

        getline(cin, junk);
        cin >> junk >> mode;
        cin >> junk >> theinputs.traders;
        cin >> junk >> theinputs.stocks;

        if (mode == "PR") theinputs.mode = false;
    }// end get_inputs

    void run(){
        char junk;
        string bs; //buyer or seller
        int tempprice, tempquantity;
        int ptime = 0, time = 0, tid = 0, sid = 0, oid = 0, price = 0, quantity = 0;//, counter = 0;
        //tid == trader id, sid == stock id
        //oid == order id, ptime == previous time

        Market market(theinputs.stocks, theinputs.traders);

        stringstream ss = randomizer(theinputs);
        istream &is = theinputs.mode ? cin : ss;

        cout << "Processing orders...\n";

        while (is >> time >> bs >> junk >> tid >> junk >> sid >> junk >> tempprice >> junk >> tempquantity){
            if (time < ptime || time < 0){
                cerr << "Timestamp for an order was less than current time" << endl;
                exit(1);
            }
            if (tid < 0 || tid >= theinputs.traders){
                cerr << "Invalid Trader ID" << endl;
                exit(1);
            }
            if (sid < 0 || sid >= theinputs.stocks){
                cerr << "Invalid Stock ID" << endl;
                exit(1);
            }
            if (tempprice <= 0){
                cerr << "Invalid price" << endl;
                exit(1);
            }
            if (tempquantity <= 0){
                cerr << "Invalid quantity" << endl;
                exit(1);
            }
            
            price = tempprice;
            quantity = tempquantity;

            Order order(bs[0], time, tid, oid, price, quantity);
            oid++;

            if(theoptions.traveler){
                if (bs[0] == 'S')
                    market.stocks[sid].addmin(time, price);
                else
                    market.stocks[sid].addmax(time, price);
            }

            if ((theoptions.median && time != ptime) && oid != 0){
                market.printmedian(ptime);

                time = order.time;
            }
            
            market.place_order(order, sid, bs[0]);

            market.find_trade(order, sid, theoptions);

            ptime = time;
        }// end while
        
        if (theoptions.median == true)
            market.printmedian(ptime);

        cout << "---End of Day---\n";
        cout << "Trades Completed: " << market.getnumtrades() << '\n';

        if (theoptions.itrader == true){
            cout << "---Trader Info---\n";

            for (int i = 0; i < theinputs.traders; i++){
                cout << "Trader " << i << " bought " << market.traders[i].bought << 
                    " and sold " << market.traders[i].sold << " for a net transfer of $" <<
                    market.traders[i].net << '\n';
            }
        }

        if (theoptions.traveler == true){
            cout << "---Time Travelers---\n";
            int btime, stime;
            for (int i = 0; i < theinputs.stocks; i++){
                if (market.stocks[i].currtime.time == -1 || market.stocks[i].max.time == -1){
                    btime = stime = -1;
                }
                else{
                    btime = market.stocks[i].currtime.time;
                    stime = market.stocks[i].max.time;
                }
                if (btime != -1 || stime != -1)
                    cout << "A time traveler would buy Stock " << i << " at time " << btime << " for $" << market.stocks[i].currtime.price << " and sell it at time " << stime << " for $" << market.stocks[i].max.price << endl;
                else
                    cout << "A time traveler could not make a profit on Stock " << i << endl;
            }
        }
    }//end run

}; //end Driver class

int main(int argc, char *argv[]){
    Driver driver;

    driver.get_options(argc, argv);

    driver.get_inputs();

    driver.run();

} //end main