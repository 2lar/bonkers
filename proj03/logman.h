//Project Identifier: 01BD41C3BF016AD7E8B6F837DF18926EC3E83350

#include <string>
#include <cstring>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cstring>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <time.h>

using namespace std;

typedef long long int longint;

string stringlow(string stringer){
    transform(stringer.begin(), stringer.end(), stringer.begin(), ::tolower);
    return stringer;
}

string timenocolon(string coloned){
    coloned.erase(remove(coloned.begin(), coloned.end(), ':'), coloned.end());
    return coloned;
}

bool anbool(char c){ //made for not alphanumonly
    if (!isalnum(c) && c != ' ') return true;
    return false;
}
string alphanumonly(string s){
    replace_if(s.begin(), s.end(), anbool, ' ');
    return s;
}

struct Log{
    string timestamp, category, message;
    uint32_t id;
    longint rtime;
};

struct timecomp{
    bool operator()(Log &t1, Log &t2){
        if (t1.rtime < t2.rtime) return true;
        else if (t1.rtime > t2.rtime) return false;
        else{
            if (strcasecmp(t1.category.c_str(),t2.category.c_str()) < 0) return true;
            else if (strcasecmp(t1.category.c_str(),t2.category.c_str()) > 0) return false;
            else{
                return t1.id < t2.id;
            }
        }
    }
};

struct upcomp{
    bool operator()(longint time, Log &thelog){
        if (thelog.rtime > time) return true;
        return false;
    }
};

struct lowcomp{
    bool operator()(Log &thelog, longint time){
        if (thelog.rtime < time) return true;
        return false;
    }
};

class Collection{
private:
    vector<Log> masterlog;
    unordered_map<string, vector<uint32_t>> categories;
    unordered_map<string, vector<uint32_t>> messages;
    vector<uint32_t> excerptlist, holding, idlist;

    //FUNCTIONS

    bool checkrange(int val = -1){
        if (val == -1) return true;
        if (size_t(val) > excerptlist.size() || excerptlist.size() == 0){
            cerr << "range out of bounds\n";
            return true;
        }return false;
    }

    void keymaker(string &msg, string &cmsg, uint32_t i){
        string word, othercat = cmsg;

        stringstream iss(msg);
        while (iss >> word){
            if (find(messages[word].begin(), messages[word].end(), i) == messages[word].end())
                messages[word].emplace_back(i);
        }

        othercat = alphanumonly(othercat);
        stringstream css(othercat);
        while (css >> word){
            if (find(messages[word].begin(), messages[word].end(), i) == messages[word].end())
                messages[word].emplace_back(i);
        }
    }

    void timesearch(longint &t1, longint &t2){
        auto lower = lower_bound(masterlog.begin(), masterlog.end(), t1, lowcomp());
        auto upper = upper_bound(masterlog.begin(), masterlog.end(), t2, upcomp());

        uint32_t low = signed(lower - masterlog.begin()), high = signed(upper - masterlog.begin());
        
        while (low < high){
            holding.emplace_back(low);
            low++;
        }
        cout << "Timestamps search: " << upper - lower << " entries found\n";
    }

    void matchsearch(longint &t){
        uint32_t left = 0, right = signed(masterlog.size()), counter = 0;

        while (left < right){
            uint32_t mid = (left + right)/2;
            if (t <= masterlog[mid].rtime)right = mid;
            else left = mid + 1;
        }

        while (left < masterlog.size() && masterlog[left].rtime == t){
            holding.emplace_back(masterlog[left].id);
            left++; counter++;
        }
        cout << "Timestamp search: " << counter << " entries found\n";
    }

    void categorysearch(string &in){ //PERFECTLY FINE
        holding = categories[in];
        cout << "Category search: " << categories[in].size() << " entries found\n";
    }

    void keysearch(string &keys){
        vector<string> keywords;

        keys = stringlow(alphanumonly(keys));

        string word;
        stringstream iss(keys);
        while (iss >> word)
            keywords.emplace_back(word);
        
        for (auto itr : keywords){
            vector<uint32_t> aux = messages[itr];
            if (aux.empty()){
                holding.clear();
                break;
            }
            else if (holding.empty()){
                holding = aux;
            }
            else{
                vector<uint32_t>::iterator si = set_intersection(holding.begin(), holding.end(), aux.begin(), aux.end(), holding.begin());
                holding.erase(si, holding.end());
            }
            if (holding.size() == 0) break;
        }
        cout << "Keyword search: " << holding.size() << " entries found\n";
    }

public:
    //will most likely need some more functions
    // clock_t start = clock();
    void make_data(ifstream &logfile){
        Log alog;
        uint32_t nlogs = 0; //a reference to index
        
        while (getline(logfile, alog.timestamp, '|')){
            alog.rtime = stoll(timenocolon(alog.timestamp));

            getline(logfile, alog.category, '|');
            getline(logfile, alog.message);

            alog.id = nlogs;
            nlogs++;

            //make master log
            masterlog.emplace_back(alog);
        }sort(masterlog.begin(), masterlog.end(), timecomp());
        idlist.resize(masterlog.size());

        for (uint32_t i = 0; i < masterlog.size(); i++){
            idlist[masterlog[i].id] = i;
            string catmsg = stringlow(masterlog[i].category);
            string msgmsg = stringlow(alphanumonly(masterlog[i].message));

            categories[catmsg].emplace_back(i);
            keymaker(msgmsg, catmsg, i);
        }
        cout << nlogs << " entries read\n";

        //TESTING
        cout << "This is MESSAGES LIST:\n";
        for (auto e : messages){
            cout << e.first << ": ";
            for (auto a : e.second)
                cout << a << " ";
            cout << endl;
        }cout << endl << endl;
        
        // cout << "This is CATEGORIES LIST:\n";
        // for (auto e : categories){
        //     cout << e.first << ": ";
        //     for (auto a : e.second)
        //         cout << a << " ";
        // }cout << endl;
    }

    void runlogman(){
        char cmd;
        do {
            cout << "% ";
            cin >> cmd;
            switch(cmd){
                case 't':{ //timestamp search
                    holding.clear();
                    string stringer;
                    longint li1, li2;
                    cin >> std::ws;
                    getline(cin, stringer);

                    if (stringer.length() != 29 || stringer[14] != '|'){
                        cerr << "invalid length of times\n";
                        break;
                    }

                    stringer = timenocolon(stringer);
                    try{
                        li1 = stoll(stringer.substr(0,15));
                        li2 = stoll(stringer.substr(11));
                    }catch (exception &invalid_argument){
                        cerr << "timestamp is not given in time error\n";
                        break;
                    }
                    timesearch(li1, li2);
                    break;
                }
                case 'm':{ //matching search
                    holding.clear();
                    string in;
                    longint thetime;
                    cin >> std::ws;
                    try{
                        getline(cin, in);
                        in = timenocolon(in);
                        thetime = stoll(in);
                    }catch (exception &invalid_argument){
                        cerr << "invalid argument error\n";
                        break;
                    }
                    matchsearch(thetime);
                    break;
                }
                case 'c':{ //category search
                    holding.clear();
                    string in;
                    cin >> std::ws;
                    getline(cin, in);
                    transform(in.begin(), in.end(), in.begin(), ::tolower);
                    categorysearch(in);
                    break;
                }
                case 'k':{ //keyword search
                    holding.clear();
                    string words;
                    getline(cin, words);
                    keysearch(words);
                    break;
                }
                case 'a':{ //append into excerpt
                    size_t inte;
                    cin >> inte;
                    if (inte >= masterlog.size()){
                        cerr << "range out of bounds\n";
                    }else{
                        excerptlist.emplace_back(idlist[inte]);
                        cout << "log entry " << inte << " appended\n";
                    }
                    break;
                }
                case 'r':{ //append the search results
                    for (auto e : holding)
                        excerptlist.emplace_back(e);
                    
                    cout << holding.size() << " log entries appended\n";
                    break;
                }
                case 'd':{ //delete log entry
                    int deleter;
                    cin >> deleter;
                    if (checkrange(deleter)) break;
                    excerptlist.erase(excerptlist.begin() + deleter);
                    cout << "Deleted excerpt list entry " << deleter << '\n';
                    break;
                }
                case 'b':{ //move to beginning
                    int beginner;
                    cin >> beginner;
                    if (checkrange(beginner)) break;
                    auto itr = excerptlist.begin() + beginner;
                    rotate(excerptlist.begin(), itr, itr+1);
                    cout << "Moved excerpt list entry " << beginner << '\n';
                    break;
                }
                case 'e':{ //move to end
                    int ender;
                    cin >> ender;
                    if (checkrange(ender)) break;
                    auto itr = excerptlist.begin() + ender;
                    rotate(itr, itr + 1, excerptlist.end());
                    cout << "Moved excerpt list entry " << ender << '\n';
                    break;
                }
                case 's':{ //sort to excerpt list
                    uint32_t size = signed(excerptlist.size())-1;
                    if (excerptlist.size() == 0){
                        cout << "excerpt list sorted\n(previously empty)\n";
                    }else{
                        cout << "excerpt list sorted\nprevious ordering:\n";
                        cout << 0 << '|' << masterlog[excerptlist[0]].id << '|' << masterlog[excerptlist[0]].timestamp <<  '|' << masterlog[excerptlist[0]].category << '|' << masterlog[excerptlist[0]].message << "\n...\n";
                        cout << size << '|' << masterlog[excerptlist[size]].id << '|' << masterlog[excerptlist[size]].timestamp <<  '|' << masterlog[excerptlist[size]].category << '|' << masterlog[excerptlist[size]].message << '\n';
                        sort(excerptlist.begin(), excerptlist.end());
                        cout << "new ordering:\n";
                        cout << 0 << '|' << masterlog[excerptlist[0]].id << '|' << masterlog[excerptlist[0]].timestamp <<  '|' << masterlog[excerptlist[0]].category << '|' << masterlog[excerptlist[0]].message << "\n...\n";
                        cout << size << '|' << masterlog[excerptlist[size]].id << '|' << masterlog[excerptlist[size]].timestamp <<  '|' << masterlog[excerptlist[size]].category << '|' << masterlog[excerptlist[size]].message << '\n';
                    }
                    break;
                }
                case 'l':{ //clear excerpt list
                    uint32_t size = signed(excerptlist.size())-1;
                    if (excerptlist.size() == 0){
                        cout << "excerpt list cleared\n(previously empty)\n";
                    }else{
                        cout << "excerpt list cleared\nprevious contents:\n";
                        cout << 0 << '|' << masterlog[excerptlist[0]].id << '|' << masterlog[excerptlist[0]].timestamp <<  '|' << masterlog[excerptlist[0]].category << '|' << masterlog[excerptlist[0]].message << "\n...\n";
                        cout << size << '|' << masterlog[excerptlist[size]].id << '|' << masterlog[excerptlist[size]].timestamp <<  '|' << masterlog[excerptlist[size]].category << '|' << masterlog[excerptlist[size]].message << '\n';
                        excerptlist.clear();
                    }
                    break;
                }
                case 'g':{ //print most recent search results
                    if (holding.size() == 0) break;

                    for (uint32_t i : holding){
                        cout << masterlog[i].id << '|' << masterlog[i].timestamp << "|" << masterlog[i].category << '|' << masterlog[i].message << '\n';
                    }
                    break;
                }
                case 'p':{ //print excerpt list
                    for (uint32_t e = 0; e < excerptlist.size(); e++){
                        cout << e << '|' << masterlog[excerptlist[e]].id << '|' << masterlog[excerptlist[e]].timestamp << '|' << masterlog[excerptlist[e]].category << '|' << masterlog[excerptlist[e]].message << endl;
                    }
                    break;
                }
                case 'q':{
                    // cout << endl << endl << "time taken: " << double( clock() - start ) / (double)CLOCKS_PER_SEC<< " seconds." << endl;
                    exit(0);
                }
                case '#':{ //No operation. Could add a comment...
                    string comm;
                    getline(cin, comm);
                    break;
                }
                default:{
                    cerr << "invalid command\n";
                    string comm;
                    getline(cin, comm);
                }
            }
        }while (cmd != 'q');
    }
};
