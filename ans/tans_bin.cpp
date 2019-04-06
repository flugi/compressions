#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iterator>
#include <cmath>
#include <set>
#include <unordered_map>
#include <map>

using namespace std;

typedef unsigned char BYTE;

std::vector<BYTE> readFile(string filename)
{
    // open the file:
    std::ifstream file(filename, std::ios::binary);

    // read the data:
    return std::vector<BYTE>((std::istream_iterator<BYTE>(file)),
                              std::istream_iterator<BYTE>());
}

vector<size_t> rescale_freq(vector<size_t> v, int N) {
    int64_t sum = 0;
    for (size_t a : v){
        sum += a;
    }
    //spec: new sum must be N, new value can be zero only if it was zero before
    set<int> zero, lessthanone, normal;
    for (int i=0;i<int(v.size());i++){
        if (v[i]==0) zero.insert(i);
        else if (v[i]/float(sum)*N < 1.0) lessthanone.insert(i); else
            normal.insert(i);
    }
/*    cout << "zeros: ";
    for (int a : zero) {
        cout << a << " ";
    }
    cout << endl;
    cout << "lessthanone: ";
    for (int a : lessthanone) {
        cout << a << " ";
    }
    cout << endl;
    cout << "normal: ";
    for (int a : normal) {
        cout << a << " ";
    }
    cout << endl;*/
    vector<size_t> res(v.size());
    for (int a : zero) {
        res[a]=0;
    }
    int sum_of_lessthanones = 0;
    for (int a : lessthanone) {
        sum_of_lessthanones +=v[a];
        res[a]=1;
    }
    float ref_sum=0;
    int new_sum=0;
    int old_sum = sum-sum_of_lessthanones;
    int target_sum = N-lessthanone.size();
    float diff_from_ref=0;
    for (int a : normal) {
        ref_sum += v[a]/float(old_sum)*target_sum;
        int new_value = round(v[a]/float(old_sum)*target_sum+diff_from_ref);
        new_sum += new_value;
        diff_from_ref = ref_sum - new_sum;
        res[a] = new_value;
//        cout << v[a] << "-> " << new_value << " " << ref_sum << " " << new_sum << " " << target_sum << " " << diff_from_ref << endl;
    }
    
    return res;
}

struct FSM_item {
    int next_state;
    size_t bits;
    int bit_count;
};

struct EncodingTable {
    vector<vector<int> > t;
    int L;
    //unordered_map< size_t, FSM_item> fsm;
    vector<FSM_item> fsm_table;
    EncodingTable(const vector<size_t>& freq, int pL) : t(freq.size()), L(pL) 
    {
        for (size_t i=0;i<freq.size();i++) {
            t[i].reserve(freq[i]);
        }
        cout << "full state machine array size: " << (L*2)*256*sizeof(FSM_item) << endl;
        fsm_table = vector<FSM_item>(256*(L*2), FSM_item({-1,0,0}));
        int c=0;
        for (int i=0;i<L;i++) {
            while(t[c].size() >= freq[c])
                c++;
            t[c].push_back(i+L);
        }
    }
    int safe_get(int state, int symbol) {
        if (state>=int(t[symbol].size()) && state < 2*int(t[symbol].size()))
            return t[symbol][state - t[symbol].size()];
        else return -1;
    }
    inline FSM_item operator()(int state, int symbol) {
        /* // hashmap was slow, switching to vector
        size_t key = state*L+symbol;
        if (fsm.find(key) != fsm.end()) {
            return fsm.at(key);
        }*/
        if (fsm_table[state*256+symbol].next_state!=-1) {
            return fsm_table[state*256+symbol];
        }
        FSM_item item;
        int reduced_state = state;
        item.bits=0;
        item.bit_count=0;
        while (reduced_state >= 2*int(t[symbol].size())) {
            item.bits = (item.bits<<1) + reduced_state%2;
            item.bit_count++;
            reduced_state/=2;
        }
        item.next_state = t[symbol][reduced_state-t[symbol].size()];
        if (item.next_state < 0) throw 0;
//        fsm[key]=item;
        fsm_table[state*256+symbol] = item;
        return item;
    }
    void dump(ostream &out) {
        for (int i=0;i<2*L;i++) {
            for (int j=0;j<int(t.size());j++) {
                out << safe_get(i,j) << " ";
            }
            out << endl;
        }
    }
};


struct TANS {
    vector<size_t> fq;
    EncodingTable * enc;
    int L;
    TANS(const vector<size_t> &freq, int prec=(1<<11)) {
        fq = rescale_freq(freq, prec);
        enc = new EncodingTable(fq,prec);
        L = prec;
    }
    vector<BYTE> encode(const vector<BYTE>& data) {
        vector<BYTE> res(6);
        int state = 2*L-1;
        size_t workspace=0;
        int workspace_bits=0;
        for (size_t i=0;i<data.size();i++) {
            FSM_item item = (*enc)(state, data[data.size()-1-i]);
            workspace = (workspace << item.bit_count) + item.bits;
            workspace_bits += item.bit_count;
            //should be possible to make faster
//            cerr << i << " " << state << " " << data[i] << " " << workspace << " " << workspace_bits << endl;
            while(workspace_bits > 8) {
                size_t mask = 255 << (workspace_bits-8);
                size_t mskval = workspace & mask;
                workspace_bits-=8;
                int to_write = mskval >> workspace_bits;
//                    cerr <<"FLUSH:" <<i<< " "<< workspace << " " <<mask << " "<< mskval<< " " << to_write << " " << workspace-mskval << endl;
                workspace -= mskval;
                res.push_back(to_write);
            }
            state = item.next_state;
//            cout << state << " " << item.bit_count << " " << flush;
        }
        //put on the res: workspace (1 byte), workspace_bits,state (4 byte)
        res[0] = workspace%256;
        res[1] = workspace_bits;
        *((int*)&res[2]) = state ;
        return res;
    }
};

int main(int argc, char *argv[]) {
    string fname;
    int precbit;
    stringstream ss;
    for (int i=1;i<argc;i++) {
        ss << argv[i] << " ";
    }
    ss >> fname;
    if (!ss.good()) {
        cout << argv[0] << " infile" << endl;
        exit(1);
    }
    ss >> precbit;
    if (!ss.good()) precbit = 14;
    vector<BYTE> data = readFile(fname);
    cout << data.size() << " bytes read" << endl;
    vector<size_t> freq(256,0);
    for(BYTE a : data) {
        freq[a]++;
    }
/*    for (size_t a : freq) {
        cout << a << " "; 
    }
    cout << endl;*/
    TANS tans(freq, (1<<precbit));
    vector<BYTE> comp = tans.encode(data);
    string outfname = fname +".tans";
    ofstream of(outfname, ios::binary);
    of.write((const char *)&comp[0],comp.size());
    cout << comp.size() << endl;
}
