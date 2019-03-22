#include <iostream>
#include <vector>
#include <set>
#include <map>
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
    vector<set<int> > indices;      // capital I in the paper
    string tANSmap;
    vector<int> D;
    int L;
    int max_I_size;
    map<char, int> freq;
    void set_cbloom(string s) {
        int M = s.length();
        cout << "M: " << M << endl;
        for (char c : s) {
            alphabet.insert(c);
            freq[c]++;
        }
        for (char c : alphabet) {
            char2alphabetindex[c]=symbol.length();
            symbol += c;
        }
        indices=vector<set<int> >(alphabet.size());
        for (char c : alphabet) {
            int s = char2alphabetindex.at(c);
            for (int j=freq.at(c);j<2*freq.at(c);j++) {
                indices[s].insert(j);
            }
            cout << endl;
        }
        for (size_t i=0;i<indices.size();i++) {
            cout << "I of " << symbol[i] <<" :" ;
            for (int a : indices[i]) {
                cout <<a << " ";
            }
            cout << endl;
        }
        tANSmap="  ";
        t = vector<vector<int> >(M*2, vector<int>(alphabet.size(), -1));
        vector<int> fillcount(alphabet.size(),0);
        vector<int> fillpos(alphabet.size());
        for (size_t i=0;i<fillpos.size();i++) {
            fillpos[i] = freq.at(symbol[i]);
        }
        int nexttofill = alphabet.size()-1;
        for (int i=M;i<2*M;i++) {
            do {
                ++nexttofill%=alphabet.size();
                cout << nexttofill << " " << fillcount[nexttofill] << " " << freq.at(symbol[nexttofill]) << endl;
            } while (fillcount[nexttofill] >= freq.at(symbol[nexttofill]));
            cout << nexttofill << endl;
            t[fillpos[nexttofill]][nexttofill] = i;
            fillpos[nexttofill]++;
            fillcount[nexttofill]++;
            tANSmap+=symbol[nexttofill];
        }
        cout << "Encoding table: " << endl;
        for (size_t i=0;i<t.size();i++) {
            cout << i << ": " ;
            for (int a : t[i]) {
                cout << a << " ";
            }
            cout << endl;
        }
        D = vector<int>(2*M);
        for (size_t i=0;i<t.size();i++) {
            for (size_t j=0;j<t[i].size();j++) {
                if (t[i][j]!=-1)
                    D[t[i][j]] = i;
            }
        }
        cout << "Decoding state transitions: ";
        for (size_t i=0;i<D.size();i++) {
            cout << i << "->" << D[i] << "   ";
        }
        cout << endl;
        L=M;
    }
    void set_AndrewPolar(string s) {
        tANSmap = string("  ")+s; //2 spaces concatenated to shift the first character to index 2
        int ml = s.length()+1;
        assert((ml & (ml+1)) == 0);
        L = (ml+1)/2;
        cout << L << endl;
        for (char c : s) {
            alphabet.insert(c);
            freq[c]++;
        }
        for (char c : alphabet) {
            char2alphabetindex[c]=symbol.length();
            symbol += c;
        }
        cout <<"Symbols: " << symbol << endl;
        for (pair<char,int> p:freq) {
            cout << p.first << ":" << p.second;
        }
        indices = vector<set<int> >(symbol.length());
        for (size_t i=0;i<s.length();i++) {
            indices[char2alphabetindex.at(s[i])].insert(i+2); //encoding table should start with 2 in order to avoid infinite 1->1->1 chain
        }
        max_I_size = indices[0].size();
        for (size_t i=1;i<indices.size();i++) {
            if (indices[i].size() > max_I_size)
                max_I_size = indices[i].size();
        }
        cout << "Encoding table length: " << max_I_size << endl;
        for (size_t i=0;i<indices.size();i++) {
            cout << "I of " << i <<" :" ;
            for (int a : indices[i]) {
                cout <<a << " ";
            }
            cout << endl;
        }
        t = vector<vector<int> > (max_I_size+1, vector<int>(alphabet.size(), -1));
        for (size_t i=0;i<indices.size();i++) {
            int counter=1;
            for (int index : indices[i]) {
                t[counter++][i] = index;
            }
        }
        
        cout << "Encoding table: " << endl;
        for (size_t i=0;i<t.size();i++) {
            cout << i << ": " ;
            for (int a : t[i]) {
                cout << a << " ";
            }
            cout << endl;
        }
        D = vector<int>(2*L);
        for (size_t i=0;i<t.size();i++) {
            for (size_t j=0;j<t[i].size();j++) {
                if (t[i][j]!=-1)
                    D[t[i][j]] = i;
            }
        }
        cout << "Decoding state transitions: ";
        for (size_t i=0;i<D.size();i++) {
            cout << i << "->" << D[i] << "   ";
        }
        cout << endl;
    }
    pair< vector<bool>, size_t > encode(string data) const {
        cout << "Lets encode '" << data <<"'"<<endl;
        size_t state = 2*L-1;
        vector<bool> res;
        for (size_t i=0; i<data.length(); i++) {
            int s = char2alphabetindex.at(data[i]);
            cout << "state : " << state << " before bit output" << endl;
            cout << "output bits: <";
            while (state > indices[s].size()*2-1) {
                res.push_back(state & 1);
                cout << (state & 1);
                state /= 2;
            }
            cout << ">" << endl;
            cout <<" s:" <<s << " state:" << state << endl;
            int next_state = t[state][s];
            cout << "next state: " << next_state << endl;
            state = next_state;

        }
        return make_pair(res, state);
    }
    string decode(int start_state, vector<bool> data) const {
        cout << "Lets decode the bit sequence <";
        for (bool b : data) cout <<b;
        cout << "> from state=" << start_state << endl;
        string res="";
        size_t state = start_state;
        do {
            cout << state << " " << L << endl;
            char c = tANSmap[state-L+2];
            cout << c << endl;
            int s = char2alphabetindex.at(c);
            int highes_state_for_s = indices.at(s).size()*2+1;
            cout <<"decoded symbol for state "<<state << " :" << c << ", s=" << s <<" threshold:" <<highes_state_for_s<< endl;
            res+=c;
            cout << "decoded string to far: '" << res <<"'" << endl;
            int next_state = D[state];
            cout << "state transition: " << state <<"-> " <<  next_state << endl;
            state = next_state;
            cout << "reading bits if needed:";
            while(state < L) {
                cout <<"[" << state <<"->";
                assert(!data.empty());
                state = 2*state + data.back();
                cout << state <<"]";
                data.pop_back();
            }
            cout << endl;
        } while (!(data.empty() && state == 2*L-1));
        return res;
    }
};

void test() {
    for (int t=0;t<1000;t++) {
    int N = rand()%200+26;
    string cbloommap = "abcdefghijklmnopqrstuvwxyz";
    for (int i=0;i<N-26;i++) {
        cbloommap += 'a'+rand()%26;
    }
    tANS tans;
    tans.set_cbloom(cbloommap);
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

int main()
{
    test();
    return 0;
}
