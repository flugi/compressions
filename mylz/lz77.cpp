#include <iostream>
#include <vector>
#include <fstream>

using namespace std;

string readfile(string fname) {
    string res, line;
    ifstream f(fname);
    if (!f.good()) {
        cerr << "invalid file" << endl;
    }
    getline (f,line);
    while(f.good()) {
        res+=line+"\n";
        getline (f,line);
    }
    return res;
}

const int max_p = 4095; //12 bit
const int max_l = 15;   //4 bit

struct item {
    int p,l;    //12+4 = 16 bit
    char c;     //8 bit
};

ostream & operator<<(ostream& out, const item& t) {
    if (t.c ==10) {
        out <<"[" << t.p << " " << t.l << " (newline)] ";
    } else
    out <<"[" << t.p << " " << t.l << " '" << t.c <<"'] ";


    return out;
}

vector<item> lz77(string s) {
    vector<item> res;
    int curr = 0;
    while (curr<s.length()) {
        int lprfx = 0;
        int len = 0;
        bool inmatch = false;
        int clen = 0;
        int startpos = 0;
        if (curr-startpos > max_p) startpos = curr-max_p;
        for (int i=startpos;i<curr || inmatch;i++) {
            if (s[i]==s[curr+clen] && clen < max_l) {
                inmatch = true;
                clen++;
            } else {
                inmatch = false;
                if (len < clen) {
                    len = clen;
                    lprfx = curr-(i-len);
                }
                clen=0;
            }
        }
        item ci;
        ci.p = lprfx;
        ci.l = len;
        ci.c = s[curr+len];
        cout << ci << endl;
        res.push_back(ci);
        curr += len+1;
    }
    return res;
}

string unlz77(vector<item> v) {
    string res;
    for (item ci : v) {
        int p = res.length();
        for (int i=0;i<ci.l;i++) {
            res += res[p-ci.p+i];
        }
        if (ci.c) //HACK
            res += ci.c;
    }
    return res;
}

int main() {
    string s1 = "test text for lz77 for testing with long test text matches";
    string s2 = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    string s3 = "On the other hand, we denounce with righteous indignation and dislike men who are so beguiled and demoralized by the charms of pleasure of the moment, so blinded by desire, that they cannot foresee the pain and trouble that are bound to ensue; and equal blame belongs to those who fail in their duty through weakness of will, which is the same as saying through shrinking from toil and pain. These cases are perfectly simple and easy to distinguish. In a free hour, when our power of choice is untrammelled and when nothing prevents our being able to do what we like best, every pleasure is to be welcomed and every pain avoided. But in certain circumstances and owing to the claims of duty or the obligations of business it will frequently occur that pleasures have to be repudiated and annoyances accepted. The wise man therefore always holds in these matters to this principle of selection: he rejects pleasures to secure other greater pleasures, or else he endures pains to avoid worse pains";
    string sf = readfile("test.txt");
    string s = s2;
    vector<item> v = lz77(s);
    string us = unlz77(v);
    if (s==us) {
        cout << "OK" << endl;
    } else {
        cout << s.size() << " " << us.size() << endl;
        cout << s.compare(us) << endl;
        cout << "ERROR" << endl << "'"<< s << "'"<< endl << endl << "'"<< us << "'"<< endl;
    }
    cout << s.length() << " item count:" << v.size()  << " (" <<v.size()*3 << ")"<< endl;
}
