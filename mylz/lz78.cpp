#include <vector>
#include <fstream>
#include <iostream>
#include <map>
#include <cstdlib>
using namespace std;

vector<pair<int, unsigned char> > lz78(ifstream & f) {
    vector<pair<int, unsigned char> > buf;
    map<string, int> index;
    vector<string> table;
    table.push_back("");
    char a = f.get();
    string s="";

    while (f.good()) {
        s+=a;
        char nexta = f.get();
        map<string, int>::iterator idxit = index.find(s);
        if (idxit == index.end() || f.eof()) {
            int newindex = table.size();
            string prevs = s.substr(0,s.length()-1);
            int oldindex = index[prevs]; //inserts zeroes if not found - thats all right.
//            cout << oldindex << " " << a << " | " << newindex << " " << prevs << " " << s << endl;
/*          for (string s : table) {
                cout <<"'"<< s << "' ";
            }
*/
            cout << endl;
            index[s] = newindex;
            table.push_back(s);
            buf.push_back(make_pair(oldindex,a));
            s="";
        }
        a=nexta;
    }

    return buf;
}

string unlz78(vector<pair<int, unsigned char> > z) {
    string res;
    vector<string> table;
    table.push_back("");
    map<int, int> histogram;
    for (pair<int, unsigned char> p : z) {
        string newword = table[p.first] + char(p.second);
        histogram[p.first]++;
        table.push_back(newword);
        res+=newword;
    }
    cout << "Stats:" << endl;

    for (pair<int, int> p : histogram) {
        int gyak = p.second;
        int hossz = table[p.first].length() ;
        int sporolas = gyak*hossz - 3*gyak;
        if (sporolas > 10) {
            cout << gyak << " * " << hossz << " = " << gyak*hossz << " -> " << sporolas << endl;
        }
    }
    return res;
}


int main() {
    ifstream f("short.txt",ios::binary);
    if (!f.good()) {
        cerr << "nincs meg a file" << endl;
        exit(1);
    }
    vector<pair<int, unsigned char> > buf = lz78(f);
    for (pair<int, unsigned char> p : buf) {
//        cout << p.first << "-> " << char(p.second) << endl;
    }
    string unlzed = unlz78(buf);
//    cout << unlzed << endl;
    cout << unlzed.length() << " " << buf.size() << "(" << buf.size()*3 << ")" << endl;
    return 0;
}
