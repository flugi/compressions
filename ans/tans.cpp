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
    tANS(string s) {
        tANSmap = string("  ")+s; //2 spaces concatenated to shift the first character to index 2
        int ml = s.length()+1;
        assert((ml & (ml+1)) == 0);
        L = (ml+1)/2;
        cout << L << endl;
        for (char c : s) {
            alphabet.insert(c);
        }
        for (char c : alphabet) {
            char2alphabetindex[c]=symbol.length();
            symbol += c;
        }
        cout <<"Symbols: " << symbol << endl;
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
            while (state > indices[s].size()) {
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
            char c = tANSmap[state];
            cout <<"decoded symbol for state "<<state << " :" << c << endl;
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

int main()
{
    string tansmap = "abacbabacbaabcabaacbbaacbabaca";
//    string tansmap = "aaaabaaacbaaaaabaacbaaaabaaaca";
    //                2345...                      31
    tANS tans(tansmap); 
//    string to_encode = "abac";
    string to_encode = "abaccbcbcbaabb";
    pair<vector<bool>, int> compressed = tans.encode(to_encode);
    string decoded = tans.decode(compressed.second, compressed.first);
    if (is_reverse(to_encode, decoded)) {
        cout << "result OK" << endl;
    } else {
        cout << "result fails" << endl;
        cout << to_encode << endl << decoded << endl;
    }
    return 0;
}
