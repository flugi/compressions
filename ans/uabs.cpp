#include <iostream>
#include <cmath>
#include <vector>
#include <ctime>

using namespace std;

void binary_decodingtable(double p, int max = 32) {
    for(int i=0;i<max;i++) {
        int x=i;
        int s1 = ceil((x+1)*p) - ceil(x*p);  // 0 if fract(x*p) < 1-p, else 1
        int dec_x;
        if (s1 == 0 ) dec_x = x - ceil(x*p);   // D(x) = (new_x, 0)
        if (s1 == 1 ) dec_x = ceil(x*p);  // D(x) = (new_x, 1)}
        cout << x << " " << (s1?" ":"    ") << dec_x << endl;
    }
}


void binary_encodingtable(double p, int max = 32) {
    for(int i=0;i<max;i++) {
        int x=i;
        int enc_x0, enc_x1;
        enc_x0 = ceil((x+1)/(1-p)) - 1; 
        enc_x1 = floor(x/p); 
        cout << x << " " << enc_x0 << " " << enc_x1 << endl;
    }
}

class UABS {
    int state=1;
    double _p0;
    vector<unsigned char> output;
public:
    int s() {return state;}
    void dump() {
        for (unsigned char p : output) {
            cout << int(p) << " ";
            
        }
        cout << "/" << state << endl;
    }
    UABS(double p=0.3) : _p0(p) {}
    float p(bool bit){
        if (!bit) return _p0;
        return 1-_p0;
    }
    void enc(bool bit) {
        if (!bit) {
            state = ceil((state+1)/(1-_p0))-1;
        } else 
        state = floor(state/_p0); 
        if (state > 255) {
            int to_output = (state & 510)/2;
            int new_state = state & 1;
            cout << state << "->" << new_state << endl;
            output.push_back(to_output);
            state = new_state;
        }
        cout << bit << " " ; dump();
    }
    void dec(vector<bool>&decoded) {
        int s = ceil((state+1)*_p0) - ceil(state*_p0);  // 0 if fract(x*p) < 1-p, else 1
        int dec_x;
        if (s == 0 ) dec_x = state - ceil(state*_p0);   // D(x) = (new_x, 0)
        if (s == 1 ) dec_x = ceil(state*_p0);  // D(x) = (new_x, 1)}
        decoded.push_back(s);
        state = dec_x;
        if (dec_x <2 && !output.empty()) {
            state += 2*output.back();
           output.pop_back();
        }
        cout << s << " "; dump();
    }
};

int main() {
    srand(time(0));
    double p = 0.3;
    binary_decodingtable(p,341);
    binary_encodingtable(p,341);
    UABS codec(p);
    vector<bool> data;//({1,1,0,1,0,0,1,1,1});
    int N = 10;
    for (int i=0;i<N;i++) {
        data.push_back(rand()%100 < p*100);
    }
    for (bool bit : data) {
        codec.enc(bit);
    }
    vector<bool> decoded;
    for (int i=0;i<data.size();i++) {
        codec.dec(decoded);
    }
    bool ok = true;
    for (int i=0;i<data.size();i++) {
        ok &= (data[i] == decoded[decoded.size()-1-i]);
    }
    cout << (ok?"OK":"BAD") << endl;
    return 0;
}
