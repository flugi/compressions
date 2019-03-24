#include <iostream>
#include <vector>
#include <set>
#include <map>
#include <fstream>
#include <sstream>
#include <cmath>
using namespace std;

#define assert(a) {if (!(a)) {cerr << "ASSERT failed at line " <<__LINE__ << endl; exit(1);}}

bool is_reverse(string a, string b) {
    if (a.length()!=b.length()) return false;
    for (size_t i=0;i<a.length();i++) {
        if (a[i] != b[a.length()-1-i]) return false;
    }
    return true;
}


struct tANS
{
    vector<vector<int> > t;         // encoding table
    set<char> alphabet;             
    string symbol;                  // s mapping
    map<char, int> char2alphabetindex;      // s mapping inverse
    vector<set<int> > indices;      // capital I in the paper, state indices for a given symbol
    string tANSmap;                 // string, where tANSmap[i] is the decoded symbol for state i
    vector<int> D;                  // decoding table
    int L;                          // Lower boundary of the states actually used while encoding, L..2L-1 is the used interval, also all probabilities are k/L rationals 
    int max_I_size;                 // maximum value of indices, for the table size (no higher state will be needed for the encoding table)
    map<char, int> freq;            // frequency of a symbol, occurence count in the setup string
    ostream & out;                  // for logging or being verbose
    int preferred_start_state;      // which state will be the start (and decoding end)
    
    tANS(ostream & log=cout) : out(log) {}
    void setup(string s) {
        int M = s.length();
        out << "M: " << M << endl;
        for (char c : s) {
            alphabet.insert(c);
            freq[c]++;
        }
        for (char c : alphabet) {
            char2alphabetindex[c]=symbol.length();
            symbol += c;
        }
        max_I_size = 0;
        indices=vector<set<int> >(alphabet.size());
        for (char c : alphabet) {
            int s = char2alphabetindex.at(c);
            for (int j=freq.at(c);j<2*freq.at(c);j++) {
                indices[s].insert(j);
                if (j>max_I_size) max_I_size = j;
            }
        }
        for (size_t i=0;i<indices.size();i++) {
            out << "I of " << symbol[i] <<" :" ;
            for (int a : indices[i]) {
                out <<a << " ";
            }
            out << endl;
        }
        tANSmap="  ";
        t = vector<vector<int> >(max_I_size+1, vector<int>(alphabet.size(), -1));
        vector<int> fillcount(alphabet.size(),0);
        vector<int> fillpos(alphabet.size());
        for (size_t i=0;i<fillpos.size();i++) {
            fillpos[i] = freq.at(symbol[i]);
        }
        int nexttofill = alphabet.size()-1;
        for (int i=M;i<2*M;i++) {
            do {
                ++nexttofill%=alphabet.size();
  //              out << nexttofill << " " << fillcount[nexttofill] << " " << freq.at(symbol[nexttofill]) << endl;
            } while (fillcount[nexttofill] >= freq.at(symbol[nexttofill]));
//            out << nexttofill << endl;
            t[fillpos[nexttofill]][nexttofill] = i;
            fillpos[nexttofill]++;
            fillcount[nexttofill]++;
            tANSmap+=symbol[nexttofill];
        }
        out << "Encoding table: " << endl;
        for (size_t i=0;i<t.size();i++) {
            out << i << ": " ;
            for (int a : t[i]) {
                out << a << " ";
            }
            out << endl;
        }
        D = vector<int>(2*M,-1);
        for (size_t i=0;i<t.size();i++) {
            for (size_t j=0;j<t[i].size();j++) {
                if (t[i][j]!=-1 && i!=0)
                    D[t[i][j]] = i;
            }
        }
        out << "Decoding state transitions: ";
        for (size_t i=0;i<D.size();i++) {
            out << i << "->" << D[i] << "   ";
        }
        out << endl;
        L=M;
        preferred_start_state = L*2-1;
    }
    pair< vector<bool>, size_t > encode(string data) const {
        out << "Lets encode '" << data <<"'"<<endl;
        
        size_t state = preferred_start_state;
        vector<bool> res;
        for (size_t i=0; i<data.length(); i++) {
            int s = char2alphabetindex.at(data[i]);
            out << "s:" <<s << " so state should be " << indices[s].size()*2-1 << " or below. state : " << state <<  " -> [";
            while (state > indices[s].size()*2-1) {
                res.push_back(state & 1);
                out << (state & 1);
                state /= 2;
            }
            out << "] -> " << state << ", so the encoding table for " <<s<< " leads to ";
            int next_state = t[state][s];
            out << next_state << endl;
            state = next_state;

        }
        return make_pair(res, state);
    }
    string decode(int start_state, vector<bool> data) const {
        out << "Lets decode the bit sequence <";
        for (bool b : data) out <<b;
        out << "> from state=" << start_state << endl;
        string res="";
        int state = start_state;
        do {
            char c = tANSmap[state-L+2];
            int s = char2alphabetindex.at(c);
            out <<"symbol for state "<<state << " :'" << c << "' (s=" << s;
            res+=c;
            out << ") ";
            int next_state = D[state];
            out << " decoding table[" << state <<"] => " <<  next_state << ", ";
            state = next_state;
            if (state < L) 
                out << state << " < " << L << ": ";
            else 
                out << "so the state is";
            while(state < L) {
                out <<"[";
                out << state <<"x2+";
                assert(!data.empty());
                state = 2*state + data.back();
                out << data.back() <<"]";
                data.pop_back();
            }
            out <<" " << state << endl;
        } while (!(data.empty() && state == preferred_start_state));
        return res;
    }
    
    
    void draw_state_machine() {
        ofstream out("a.dot");
        out << "digraph ans {" << endl;
        out << " node [shape=box]" << endl;
        for (int state=L;state<2*L;state++) {
            for (unsigned int s=0; s<alphabet.size();s++) {
                int rstate = state;
                stringstream bits;
                while (rstate > int(indices[s].size()*2-1)) {
                    bits << (rstate & 1);
                    rstate /= 2;
                }
                int next_state = t[rstate][s];
//                out << " rstate: " << rstate;
//                out << "state:" << state << " s:" << s << " ";
 //               out << " next state:" << next_state << endl;
                out <<""<< state << " -> " << next_state <<" [label=\"" << symbol.at(s) <<  " [" << bits.str() << "]\"];" << endl;
                }
        }
        out << "}" << endl;
    }
};

void test_random() {
    ofstream logfile("tans.log");
    for (int t=0;t<1000;t++) {
    int N = rand()%200+26;
    string cbloommap = "abcdefghijklmnopqrstuvwxyz";
    for (int i=0;i<N-26;i++) {
        cbloommap += 'a'+rand()%26;
    }
    tANS tans(logfile);
    tans.setup(cbloommap);
    string to_encode = "";
    for (int i=0;i<10+rand()%100;i++) {
        to_encode += 'a'+rand()%26;
    }
    pair<vector<bool>, int> compressed = tans.encode(to_encode);
    string decoded = tans.decode(compressed.second, compressed.first);
    if (is_reverse(to_encode, decoded)) {
        cout << "result OK" << endl;
    } else {
        cout << "result fails" << endl;
        cout << to_encode << endl << decoded << endl;
    }
    }
}



void test_compression_ratio() {
    ofstream logfile("tans_compratio.log");
    int N = 100000;
    int TN = 20;
    for (int test=0;test<TN;test++) {
        float p = float(test+1)/(TN+2);
        string vocab, test_str;
        for (int i=0;i<N;i++) {
            vocab+='a' + (rand()%1000>p*1000);
            test_str+='a' + (rand()%1000>p*1000);
        }
        int measuredfreq = 0;
        for (char c : vocab) {
            measuredfreq += (c=='a');
        }
//        cout << measuredfreq << endl;
        p = float(measuredfreq)/N;
        tANS tans(logfile);
        tans.setup(vocab);
        pair<vector<bool>, int> compressed = tans.encode(test_str);
        double shannon = 0;
        for (size_t i=0;i<test_str.length();i++) {
            float cp = (test_str[i]=='a')?p:(1.0-p);
            shannon += -log(cp)/log(2.0);
        }
        cout <<"shannon: "<< shannon << " ANS:" << compressed.first.size() <<" (without state stored) with p=" << p << endl;
    }
}

void test_file(string fname) {
    ofstream logfile("tans.log");
    string data,line;
    ifstream f(fname);
    getline(f,line);
    while(f.good()) {
        data += line;
        getline(f,line);
    }
    f.close();
    tANS tans(logfile);
    tans.setup(data);
    pair<vector<bool>, int> compressed = tans.encode(data);
    string decoded = tans.decode(compressed.second, compressed.first);
    if (is_reverse(data, decoded)) {
        cout << "result OK" << endl;
        cout << compressed.first.size() << " bits (" << int(ceil(compressed.first.size()/8.0)) << " bytes) vs " << data.length() << " bytes" << endl;
    } else {
        cout << "result fails" << endl;
        cout << data << endl << decoded << endl;
    }
    
}

void test_simple() {
    string vocabulary = "aaabcc";
    tANS tans;
    tans.setup(vocabulary);
    pair<vector<bool>, int> compressed = tans.encode("caab");
    string decoded = tans.decode(compressed.second, compressed.first);
}

void test_statemachine() {
    tANS tans;
    tans.setup("aab");
    tans.draw_state_machine();
}

int main()
{
//    test_simple();
//    test_file("tans.cpp");
//    test_random();
//    test_compression_ratio();
    test_statemachine();
    return 0;
}
