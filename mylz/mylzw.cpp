#include <vector>
#include <fstream>
#include <iostream>
#include <map>
#include <cstdlib>
using namespace std;

string prettyprint(string s) {
	string res="'";
	for (size_t i=0;i<s.length();i++) {
		if (s[i]>=32) res+=s[i];
		if (s[i]==10) res+="(enter)";
	}
	return res+"'";
}

vector<int > lzw(ifstream & f) {
    vector<int > buf;
    map<string, int> dic;
    for (int i=0;i<=255;i++) {
		char c = i;
		dic[string(1,c)]=i;
    }
    vector<string> table;
    char a = f.get();
    string s="";

    while (f.good()) {
        s+=a;
        char nexta = f.get();
        map<string, int>::iterator idxit = dic.find(s);
        if (idxit == dic.end() || f.eof()) {
            int newindex = dic.size();
            string prevs = s.substr(0,s.length()-1);
            if (prevs=="") return buf;
            int oldindex = dic.at(prevs);
            cout << oldindex << " " << prettyprint(string(1,a)) << " | " << newindex << " " << prettyprint(prevs) << " " << prettyprint(s) << endl;
            dic[s] = newindex;
            buf.push_back(oldindex);
            s=a;
        }
        a=nexta;
    }

    return buf;
}

int bitcount(size_t tablesize) {
	int sum = 8; // first item is 8 bit always
	int step = 9;
	for (size_t i = 257; i<tablesize+257 ;i++) {
		if ( (i^(i-1))==(i*2-1)) {  //i is power of 2
			step++;
		}
		sum += step;
	}
	return sum;
}

string unlzw(vector<int> z) {
    string res;
    map<string, int> dic;
    vector<string> table;
    for (int i=0;i<=255;i++) {
		char c = i;
		dic[string(1,c)]=i;
		table.push_back(string(1,c));
    }
    map<int, int> histogram;
    for (size_t i=0;i<z.size();i++) {
        string newword = table[z[i]];
        size_t next_item = z[i+1];
        string nextfirstletter;
        if (next_item<table.size()) {
        	nextfirstletter = table[next_item][0];
        } else {
            cout << "!!!";
        	nextfirstletter = table[z[i]][0];
        }
        cout << table.size() << " " << z[i] << " " << z[i+1] << " " << prettyprint(table[z[i]]) << endl;
        histogram[z[i]]++;
        newword += nextfirstletter;
        table.push_back(newword);
        res+=newword.substr(0,newword.length()-1);
    }
    cout << "Stats:" << endl;

    for (pair<int, int> p : histogram) {
        int gyak = p.second;
        int hossz = table[p.first].length() ;
        int sporolas = gyak*hossz - 1.5*gyak; // 12 bit for everything as estimation
        if (sporolas > 10) {
            cout << gyak << " * " << hossz << " = " << gyak*hossz << " -> " << sporolas << endl;
        }
    }
    return res;
}


int main() {
    ifstream f("mylzw.cpp",ios::binary);
    if (!f.good()) {
        cerr << "nincs meg a file" << endl;
        exit(1);
    }
    vector<int > buf = lzw(f);
//    for (int p : buf) {
//        cout << p << endl;
//    }
    string unlzed = unlzw(buf);
    cout << unlzed << endl;
    cout << unlzed.length() << " " << buf.size() << "(" << bitcount(buf.size())/8+1 << ")" << endl;

    return 0;
}

