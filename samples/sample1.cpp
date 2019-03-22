#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <cmath>
using namespace std;

void sample1() {
    int N = 1000000;
    vector<unsigned char> v(N,0); 
    {
        ofstream f("sample1.data",ios::binary);
        f.write((const char*)&(v[0]),N);
        f.close();    
    }
    v[2019] = 1;
    {
        ofstream f("sample1b.data",ios::binary);
        f.write((const char*)&(v[0]),N);
        f.close();
    }
}

void sample2() {
    int N = 1000000;
    vector<unsigned char> v(N,0);
    for (size_t i=0;i<N;i++) {
        v[i] = i%256;        
    }
    ofstream f("sample2.data",ios::binary);
    f.write((const char*)&(v[0]),N);
    f.close();
}

void sample3() {
    srand(1010);
    int N = 1000000;
    vector<unsigned char> v(N,0);
    for (size_t i=0;i<N;i++) {
        v[i] = rand()%2;
    }
    ofstream f("sample3.data",ios::binary);
    f.write((const char*)&(v[0]),N);
    f.close();
}

void sample4() {
    int N = 1000000;
    vector<unsigned char> v(N,1);
    for (int a = 2; a<N; a++) {
        for (int i=2*a; i<N;i+=a) {
            v[i]=0;
        }
    }
    
    ofstream f("sample4.data",ios::binary);
    f.write((const char*)&(v[0]),N);
    f.close();
}

void sample5() {
    srand(1948);
    int N = 1000000;
    vector<unsigned char> v(N,0);
    for (size_t i=0;i<N;i++) {
        v[i] = rand()%256;
    }
    {
        ofstream f("sample5.data",ios::binary);
        f.write((const char*)&(v[0]),N);
        f.close();
    }
    sort(v.begin(), v.end());
    {
        ofstream f("sample5b.data",ios::binary);
        f.write((const char*)&(v[0]),N);
        f.close();
    }
}

void sample6() {
    srand(1948);
    int N = 1000000;
    vector<unsigned char> v(N,0);
    for (size_t i=0;i<N/2;i++) {
        v[i] = rand()%128;
    }
    for (size_t i=N/2;i<N;i++) {
        v[i] = rand()%128+128;
    }
    {
        ofstream f("sample6.data",ios::binary);
        f.write((const char*)&(v[0]),N);
        f.close();
    }
}


void sample7() {
    srand(1948);
    int N = 1000000;
    vector<unsigned char> v(N,0);
    for (size_t i=0;i<N;i++) {
        v[i] = rand()%256;
    }
    for (size_t i=1;i<N;i++) {
        if (sqrt(rand()%i)<=100) v[i]=0;
    }
    for (size_t i=0;i<N-1;i++) {
        if (sqrt(rand()%(N-i))<=100) v[i]=1;
    }
    {
        ofstream f("sample7.data",ios::binary);
        f.write((const char*)&(v[0]),N);
        f.close();
    }
}


int main() {
    sample1();
    sample2();
    sample3();
    sample4();
    sample5();
    sample6();
    sample7();
    return 0;
}
