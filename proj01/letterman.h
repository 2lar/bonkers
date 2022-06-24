//PROJECT IDENTIFIER: 50EB44D3F029ED934858FFFCEAC3547C68768FC9

#include <iostream>
#include <algorithm>
#include <getopt.h>
#include <vector>
#include <deque>
#include <string>
#include <cstring>

using namespace std;

struct Word{
    string word = "";
    size_t preidx = 0, modletter_idx = 0;
    bool discovered = false;
    char morpher = '\0', modletter = '\0';
    Word(){}
    Word(string _word) : word(_word) {}
};

class Letterman{
private:
    vector<Word> dict;

    bool change = false, swap = false, length = false;
    char dictType = '\0', deqform = '\0', outform = 'W';
    bool stackmode = false;
    //dictType are for simple (s) / complex (c)
    //deqform: stack or queue
    //outform (output form) is word 'W' or modified 'M'

    string begin = "", end = "";
    size_t s_idx = 0, e_idx = 0, num_words = 0, num_discovered = 0;

    //for the search
    deque<size_t> scontainer = {};
    string curr;
    size_t curr_idx;

    //within the morphs
    size_t modtempidx = 0;
    char modtempchar = '\0';

    // the morphs
    bool changer(const string &cur, const string &aux){
        if (cur.size() != aux.size()) return false;
        uint32_t diff = 0;
        for (uint32_t i = 0; i < cur.size(); i++){
            if (cur[i] != aux[i]){
                diff++;
                if (diff > 1) return false;
                modtempidx = i;
                modtempchar = aux[i];
            }
        }return true;
    }// end changer

    bool swapper(const string &cur, const string &aux){
        if (cur.size() != aux.size()) return false;
        uint32_t diff = 0;
        bool swapcheck = false;
        for (uint32_t i = 0; i < cur.size(); i++){
            if (swapcheck){
                swapcheck = false;
                continue;
            }
            if (cur[i] != aux[i]){
                diff++;
                if (diff > 1) return false;
                if (curr[i] == aux[i+1] && curr[i+1] == aux[i]){
                    swapcheck = true;
                    modtempidx = i;
                }
                else return false;
            }
        }return true;
    }//end swapper

    bool inserter(const string &cur, const string &aux){
        if ((cur.size() + 1) != aux.size()) return false;
        uint32_t diff = 0;
        for (uint32_t i = 0; i < aux.size(); i++){
            if (diff > 0){
                if (cur[i] != aux[i + diff]) return false;
                continue;
            }
            else if (cur[i] != aux[i]){
                diff++;
                if (cur[i] != aux[i+1]) return false;
                modtempidx = i;
                modtempchar = aux[i];
            }
        }return true;
    }// end inserter

    bool deleter(const string &cur, const string &aux){
        if ((cur.size() - 1) != aux.size()) return false;
        uint32_t diff = 0;
        for (uint32_t i = 0; i < aux.size(); i++){
            if (diff > 0){
                if (cur[i+diff] != aux[i]) return false;
                continue;
            }
            else if (cur[i] != aux[i]){
                diff++;
                if (cur[i+1] != aux[i]) return false;
                modtempidx = i;
                modtempchar = cur[i];
            }
        }
		return true;
    }// end deleter

    void discovery(const uint32_t discovery_idx){
        scontainer.push_back(discovery_idx);
        dict[discovery_idx].discovered = true;
        dict[discovery_idx].preidx = curr_idx;
        ++num_discovered;
    }

    bool checkend(){
        if (dict[e_idx].discovered) return true;
        return false;
    }

    bool search(){
        scontainer.push_back(s_idx);
        dict[s_idx].discovered = true;
        num_discovered++;

        // use curr and curr_idx
        while (!scontainer.empty()){
            if (stackmode){
                curr = dict[scontainer.back()].word;
                curr_idx = scontainer.back();
                scontainer.pop_back();
            }else{ //using a queue
                curr = dict[scontainer.front()].word;
                curr_idx = scontainer.front();
                scontainer.pop_front();
            }

            for (uint32_t i = 0; i < dict.size(); i++){
                if (!dict[i].discovered){
                    if (change){
                        if (changer(curr, dict[i].word) && !dict[i].discovered){
                            dict[i].morpher = 'c';
                            discovery(i);
                            dict[i].modletter_idx = modtempidx;
                            dict[i].modletter = modtempchar;
                        }if (checkend()) return true;
                    }
                    if (swap){
                        if (swapper(curr, dict[i].word) && !dict[i].discovered){
                            dict[i].morpher = 's';
                            discovery(i);
                            dict[i].modletter_idx = modtempidx;
                        }if (checkend()) return true;
                    }
                    if (length){
                        if (inserter(curr, dict[i].word) && !dict[i].discovered){
                            dict[i].morpher = 'i';
                            discovery(i);
                            dict[i].modletter_idx = modtempidx;
                            dict[i].modletter = modtempchar;
                        }if (checkend()) return true;
                        if (deleter(curr, dict[i].word) && !dict[i].discovered){
                            dict[i].morpher = 'd';
                            discovery(i);
                            dict[i].modletter_idx = modtempidx;
                            dict[i].modletter = modtempchar;
                        }if (checkend()) return true;
                    }
                }
            }//end for loop for similarity search
        }return false;
    }// end search
    
public:
    void getOptions(int argc, char* argv[]){
        int choice = 0, option_index = 0;
        opterr = false;
        bool modmode = false;
        // bool vbegin = false, vend = false;
        struct option long_opts[] = {
            {"stack", no_argument, nullptr, 's'},
            {"queue", no_argument, nullptr, 'q'},
            {"change", no_argument, nullptr, 'c'},
            {"swap", no_argument, nullptr, 'p'},
            {"length", no_argument, nullptr, 'l'},
            {"output", required_argument, nullptr, 'o'},
            {"begin", required_argument, nullptr, 'b'},
            {"end", required_argument, nullptr, 'e'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, 0, nullptr, '\0'}
        };
        
        while ((choice = getopt_long(argc, argv, "sqcplo:b:e:h", long_opts, &option_index)) != -1){
            switch(choice){
            case 's':{ //modifies deqform and stackmode
                if (deqform == '\0'){
                    deqform = 's';
                    stackmode = true;
                }else{
                    cout << "Conflicting or duplicate stack and queue specified\n";
                    exit(1);
                }break;
            }
            case 'q':
                if (deqform == '\0'){
                    deqform = 'q';
                    stackmode = false;
                }else{
                    cout << "Conflicting or duplicate stack and queue specified\n";
                    exit(1);
                }break;

            case 'c':{
                change = true;
                modmode = true;
                break;
            }
            case 'p':{
                swap = true;
                modmode = true;
                break;
            }
            case 'l':{
                length = true;
                modmode = true;
                break;
            }
            case 'o':{ //modifies outform
                if(*optarg == 'W'){
                    outform ='W';
                    break;
                }else if(*optarg == 'M'){
                    outform = 'M';
                    break;
                }else{
                    cout << "Invalid output mode specified\n";
                    exit(1);
                }break;
            }
            case 'b':{
                if (optarg && begin.empty()){
                    begin = optarg;
                }else{
                    cerr << "Beginning word not specified\n";
                    exit(1);
                }break;
            }
            case 'e':{
                if (optarg && end.empty()){
                    end = optarg;
                }else{
                    cerr << "Ending word not specified\n";
                    exit(1);
                }break;
            }
            case 'h':{
                std::cerr << "Takes in a file to form a dictionary\n"
                << "it also takes in a start and end word\n"
                << "the dictionary searched to morph the\n"
                << "start word into the end word through\n"
                << "the methods that are switched on below\n"
                << "Usage: \'./letter\n\t[--stack | -s]\n"
                << "\t[--queue | -q]\n"
                << "\t[--change | -c]\n"
                << "\t[--swap | -p]\n"
                << "\t[--length | -l]\n"
                << "\t[--output | -o] (W|M)\n"
                << "\t[--begin | -b] <word>\n"
                << "\t[--end | -e] <word>\n"
                << "\t[--help | -h]\n"
                << "\t<Dictionary File>\'" << std::endl;
            exit(0);
            }//help
            }
        }//end while
        if (deqform == '\0'){
            cerr << "Must specify one of stack or queue\n";
            exit(1);
        }
        else if (!modmode){
            cerr << "Must specify at least one modification mode (change length swap)\n";
            exit(1);
        }
        else if(!length){
            if (begin.size() != end.size()){
                cerr << "The first and last words must have the same length when length mode is off\n";
                exit(1);
            }
        }
    }

    void makeDict(){
        cin >> dictType;
        cin >> num_words;

        string line;
        cin >> std::ws;

        while (getline(cin,line)){
            Word aux(line);
            if (line.substr(0,2) != "//"){
                if (dictType == 'S'){ // read simple
                    dict.reserve(num_words);
                    dict.push_back(aux);
                }
                else{ //read complex & [] ! ?
                    if (line.back() == '&'){
                        line.pop_back();
                        dict.push_back(aux.word = line);
                        reverse(line.begin(),line.end());
                        dict.push_back(aux.word = line);
                    }// REVERSAL

                    else if (line.find('[') != string::npos){
                        size_t bleft = line.find('[');
                        size_t bright = line.find(']');
                        for (size_t i=0; i < bright-bleft-1; i++){
                            aux.word = line.substr(0,bleft) + line[bleft+1+i] + line.substr(bright+1, string::npos);
                            dict.push_back(aux);
                        }
                    }// INSERT-EACH

                    else if(line.find('!') != string::npos){
                        size_t swapped = line.find('!');
                        size_t lefter = swapped - 2;
                        size_t righter = swapped - 1;
                        char left = line[lefter];
                        char right = line[righter];

                        line.erase(swapped,1);
                        dict.push_back(aux.word = line);
                        line[lefter] = right;
                        line[righter] = left;
                        dict.push_back(aux.word = line);
                    }// SWAP

                    else if (line.find('?') != string::npos){
						size_t doubler = line.find('?');
						string rletter = line.substr(doubler-1,1);
						line.replace(doubler,1,rletter);
						string doubled = line;
						line.erase(doubler,1);
						dict.push_back(aux.word = line);
						dict.push_back(aux.word = doubled);
                    }//DOUBLER

                    else dict.push_back(aux.word = line);
                    // simple
                }
            }
        }
        // this part finds the index of start and end
        bool yesstart = false, yesend = false;
        for (size_t i = 0; i < dict.size(); i++){
            if (dict[i].word == begin){
                s_idx = i;
                yesstart = true;
            }
            if (dict[i].word == end){
                e_idx = i;
                yesend = true;
            }
            if (yesstart && yesend) {break;}
        }
        if(!yesstart){
            cerr << "Beginning word does not exist in the dictionary\n";
            exit(1);
        }
        else if (!yesend){
            cerr << "Ending word does not exist in the dictionary\n";
            exit(1);
        }
    }// END makeDict

    void run(){
        // for (auto x: dict) cout << x.word << endl;
        if (begin == end){ 
            cout << "Words in morph: 1\n" << end << '\n';
        }
        else if(search()){
            vector<size_t> themorphs;
            themorphs.reserve(num_discovered);
            size_t aux_idx = e_idx;
            themorphs.push_back(aux_idx);

            //this creates the solution list in reverse order, need to reverse later
            while (aux_idx != s_idx){
                aux_idx = dict[aux_idx].preidx;
                themorphs.push_back(aux_idx);
            }

            reverse(themorphs.begin(), themorphs.end()); //reverse the reversed solution order

            cout << "Words in morph: " << themorphs.size() << '\n';
            if (outform == 'W'){
                for (size_t i = 0; i < themorphs.size(); i++){
                    cout << dict[themorphs[i]].word << '\n';
                }
            }
            else{ //outform is 'M'
                cout << dict[themorphs[0]].word << '\n';
                for (auto ele: themorphs){
                    if (ele == s_idx) continue;
                    if (dict[ele].morpher == 'c' || dict[ele].morpher == 'i')
                        cout << dict[ele].morpher << ',' << dict[ele].modletter_idx << ',' << dict[ele].modletter << '\n';
                    else if (dict[ele].morpher == 'd' || dict[ele].morpher == 's')
                        cout << dict[ele].morpher << ',' << dict[ele].modletter_idx << '\n';
                }
            }
        }
        else {
            cout << "No solution, " << num_discovered << " words discovered.\n";
        }
    }//end run
};