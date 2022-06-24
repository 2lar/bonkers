// Project identifier: 0E04A31E0D60C01986ACB20081C9D8722A1899B6

#include <iostream>
#include <sstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <string>
#include "P2random.h"

using namespace std;

struct Option{
    bool verbose = false, median = false,
        itrader = false, traveler = false;
};


class Trader{ //prob can turn this into a struct
public:
    int id, bought, sold, net;

public:
    Trader(): id(0), bought(0), sold(0), net(0) {}
    Trader(int theid): id(theid){}

};

class Order{
public:
    char type = '\n'; // will be b or s
    int time = 0, trader = 0, oid = 0,
        price = 0, quantity = 0;

    Order() : type(0), trader(0), oid(0), price(0), quantity(0){}
    Order(
        char ty, //buy or sell
        int t, //time
        int tr, //trader
        int i, //order ID
        int p, //price
        int q //quantity
    ): type(ty), time(t), trader(tr), oid(i), price(p), quantity(q){}

    void newquant(Order other, int quant){
        type = other.type;
        time = other.time;
        trader = other.trader;
        oid = other.oid + 1;
        price = other.price;
        quantity = quant;
    }
};
struct bcompare{
    bool operator() (Order &b1, Order &b2){
        if (b1.price != b2.price)
            return b1.price < b2.price;
        else
            return b1.oid > b2.oid;
    }
};

struct scompare{
    bool operator() (Order &s1, Order &s2){
        if (s1.price != s2.price)
            return s1.price > s2.price;
        else
            return s1.oid > s2.oid;
    }
};

class Stock{
private:
    struct Travel{
        int time = 0, price = 0;
    };
    int stockid = 0;
    
public:
    Travel currtime;
    Travel nexttime;
    Travel max;
    Stock() : stockid(0){
        currtime.time = nexttime.time = max.time = -1;
        currtime.price = nexttime.price = max.price = 0;
    };
    Stock(int theid) : stockid(theid){
        currtime.time = nexttime.time = max.time = -1;
        currtime.price = nexttime.price = max.price = 0;
    }

    //when deciding trades
    priority_queue<Order, vector<Order>, bcompare> buyer;
    priority_queue<Order, vector<Order>, scompare> seller;

    //when finding the median
    priority_queue<int, vector<int>, greater<int>> upper;
    priority_queue<int, vector<int>, less<int>> lower;

    bool checktrade(){
        if (buyer.empty() || seller.empty()) return false;

        if (buyer.top().price >= seller.top().price){
            return true;
        }else return false;

    }

    void addmin(int time, int price){
        if (currtime.time == -1){
            currtime.price = price;
            currtime.time = time;
        }
        else if(price < currtime.price){
            if (max.time == -1){
                currtime.price = price;
                currtime.time = time;
            }
            else if (nexttime.time == -1){
                nexttime.price = price;
                nexttime.time = time;
            }
            else if(price < nexttime.price){
                nexttime.price = price;
                nexttime.time = time;
            }
        }
        return;
    }

    void addmax(int time, int price){
        if (currtime.time != -1) {
            if (((nexttime.time != -1) && (price > nexttime.price)) || (price > currtime.price)) {
                if (price == max.price) {
                    if (nexttime.time != -1) {
                        max.time = time;
                        max.price = price;

                        currtime.time = nexttime.time;
                        currtime.price = nexttime.price;
                        nexttime.time = -1;
                    }
                }
                else if (price > max.price) {

                    max.time = time;
                    max.price = price;

                    if (nexttime.time != -1) {
                        currtime.time = nexttime.time;
                        currtime.price = nexttime.price;
                        nexttime.time = -1;
                    }
                }
            }
	    }
	    return;
    }

    int getmedian(){
        if (!lower.empty() && !upper.empty()){
            if (lower.size() == upper.size())
                return ((lower.top()+upper.top())/2);
            else
                return (lower.size() > upper.size() ? lower.top() : upper.top());
        }
        else if (!lower.empty())
            return lower.top();
        else return -1;
    }

    int getid() {return stockid;}
};

class Market{
private:
    uint32_t numtrades = 0;

public:
    vector<Trader> traders;
    vector<Stock> stocks;

    Market(){};
    Market(int nstocks, int ntraders){
        stocks.resize(nstocks);
        for (int i = 0; i < nstocks; i++) stocks[i] = Stock(i);

        traders.resize(ntraders);
        for (int i = 0; i < ntraders; i++) traders[i] = Trader(i);
    }

    void printmedian(int time){
        for (Stock stock : stocks){
            int median = stock.getmedian();
            if (median != -1)
                cout << "Median match price of Stock " << stock.getid() << 
                    " at time " << time << " is $" << median << endl;
        }
    }

    void place_order(Order order, int sid, char type){
        //type as in order type, tid = trader ID, sid = stock ID
        if (type == 'B'){
            stocks[sid].buyer.push(order);
        }
        else{ // 'S' for sell
            stocks[sid].seller.push(order);
        }
    }

    void find_trade(Order &order, int sid, Option option){
        int stocksbought = 0, tradeprice = 0, totalspent = 0;
        bool trading = stocks[sid].checktrade();
        if (!trading) return;

        bool b = (order.type == 'B');

        priority_queue<Order, vector<Order>, scompare> &sells = stocks[sid].seller;
        priority_queue<Order, vector<Order>, bcompare> &buys = stocks[sid].buyer;

        while (trading){
            Order temp;
            if (b){
                stocksbought = order.quantity <= sells.top().quantity ? 
                    order.quantity : sells.top().quantity;

                tradeprice = sells.top().price;
            }else{
                stocksbought = order.quantity <= buys.top().quantity ? 
                    order.quantity : buys.top().quantity;

                tradeprice = buys.top().price;
            }totalspent = stocksbought * tradeprice;

            order.quantity -= stocksbought;

            int nquant;
            if (b){
                nquant = sells.top().quantity - stocksbought;
                temp.newquant(sells.top(), nquant);

                traders[order.trader].bought += stocksbought;
                traders[order.trader].net -= totalspent;
                traders[sells.top().trader].sold += stocksbought;
                traders[sells.top().trader].net += totalspent;

                if (option.verbose){
                    cout << "Trader " << order.trader << " purchased " << stocksbought << 
                        " shares of Stock " << sid << " from Trader " << sells.top().trader 
                        << " for $" << tradeprice << "/share\n";
                }
                sells.pop();
                stocks[sid].buyer.pop();
                if (temp.quantity != 0) sells.push(temp);
                if (order.quantity != 0) buys.push(order);
            }else{
                nquant = buys.top().quantity - stocksbought;
                temp.newquant(buys.top(), nquant);

                traders[order.trader].sold += stocksbought;
                traders[order.trader].net += totalspent;
                traders[buys.top().trader].bought += stocksbought;
                traders[buys.top().trader].net -= totalspent;

                if (option.verbose){
                    cout << "Trader " << buys.top().trader << " purchased " << 
                    stocksbought << " shares of Stock " << sid << " from Trader " << 
                    order.trader << " for $" << tradeprice << "/share\n";

                }
                buys.pop();
                stocks[sid].seller.pop();
                if (temp.quantity != 0) buys.push(temp);
                if (order.quantity != 0) sells.push(order);
            }

            if (option.median){
                if (stocks[sid].upper.empty() && stocks[sid].lower.empty()){
                    stocks[sid].lower.push(tradeprice);
                }else if (tradeprice > stocks[sid].lower.top()){
                    stocks[sid].upper.push(tradeprice);
                }else{
                    stocks[sid].lower.push(tradeprice);
                }

                int diff = (signed(stocks[sid].lower.size()) - 
                    signed(stocks[sid].upper.size()));
                
                if (diff > 1){
                    int temp = stocks[sid].lower.top();
                    stocks[sid].lower.pop();
                    stocks[sid].upper.push(temp);
                }else if (diff < -1){
                    int temp = stocks[sid].upper.top();
                    stocks[sid].upper.pop();
                    stocks[sid].lower.push(temp);
                }
            }
            trading = stocks[sid].checktrade();
            numtrades++;
        }
    }// end find trade

    int getnumtrades(){
        return numtrades;
    }

};

