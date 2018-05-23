#ifndef MY_VLC_INCLUDED
#define MY_VLC_INCLUDED

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <queue>

#include <iostream>

namespace vlc {

typedef unsigned char byte;

/// array of bits, buf may be longer than sufficient
struct chunk {
private:
    std::vector<vlc::byte> buf= {0};
    size_t size_bits=0;
public:
    size_t get_size_bits() const {return size_bits; }
    void put_bit(bool newbit) {
        buf[size_bits/8] |= newbit << (7-size_bits%8);
        size_bits++;
        if (size_bits%8==0) {
            buf.push_back(0);
        }
    }
    bool get_bit(size_t index) const {
        vlc::byte a = buf[index/8];
        a <<= index%8;
        return a&128;
    }

    void put_chunk(const chunk& c) {
        if (c.size_bits==0) return;
        for (size_t i=0;i<c.size_bits;i++) {
            put_bit(c.get_bit(i));
        }
    }

    void numericprint(std::ostream& out) const {
        for (size_t i=0; i<=size_bits/8; i++) {
            out << int(buf[i]) << " ";
        }
    }

    void debugprint(std::ostream &out, bool perbyte=false) const {
        if (size_bits==0) return;
        for (int i=0; i<=int((size_bits-1)/8); i++) {
            vlc::byte a = buf.at(i);
            for (int j=0;j<std::min(8,signed(size_bits)-i*8);j++) {

//                out << "(" <<i << " " << j << " " << int(a) << ")";
                out << !!int(a & 128);
                a <<=1;
            }
            if (perbyte) out<<" ";
        }

    }
    void debugput(std::string s) {
        for (char a : s) {
            put_bit(a=='1');
        }
    }
    void save(std::ofstream& out) {
        out.write((char *)&size_bits, sizeof(size_bits));
        if (size_bits==0) return;
        out.write((char *)&(buf[0]),size_bits/8+1);
    }

    void save_file(std::string fname) {
        std::ofstream f(fname, std::ios::binary);
        save(f);
    }
    bool load(std::ifstream &f) {
        size_t sizebits;
        f.read((char*)&sizebits, sizeof(sizebits));
        if (!f.good()) return false;
        buf.resize(sizebits/8+1, 0);
        f.read((char*)&(buf[0]), sizebits/8+1);
        if (!f.good()) return false;
        size_bits = sizebits;
    }

    bool load_file(std::string fname) {
        std::ifstream f(fname, std::ios::binary);
        return load(f);
    }
};

static std::ostream& operator<<(std::ostream &out, chunk c) {
    c.debugprint(out);
    return out;
}

static unsigned int zigzag(int x) {
    if (x<0) {
        return -x*2;
    } else return x*2+1;
}

static int unzigzag(unsigned int x) {
    if (x % 2) {
        return x/2;
    } else return -int(x)/2;
}



template<typename T>
class PrefixCode {
public:
    virtual void debugdump(std::ostream& out) { out << "dump is unsupported"; }
    virtual bool encode(const std::vector<T>& v, chunk& c) = 0;
    virtual bool decode(const chunk& c, std::vector<T> &v) = 0;

};

template<int K>
class NumericSemiBlockCode : public PrefixCode<int> {
public:
    bool put_element(unsigned int x, chunk &c) {
        chunk a;
        while (x >= ((1<<K)-1)) {
            for (int k=0;k<K;k++) {
                a.put_bit(1);
            }
            x -= ((1<<K)-1);
        }
        for (int i=K-1; i>=0;i--) {
            a.put_bit( x & (1<<i) );
        }
        c.put_chunk(a);
    }

    virtual bool encode(const std::vector<int>& v, chunk& c) {
        for (int a : v) {
            put_element(zigzag(a), c);
        }
        return true;
    }
    virtual bool decode(const chunk& c, std::vector<int> &v) {
        int a = 0;
        std::vector<int> curr(K,0);
        for (size_t i=0;i<c.get_size_bits()/K;i++) {
            for (int b=0;b<K;b++) {
                curr[b] = c.get_bit(i*K+b);
            }
            if (curr == std::vector<int>(K,1)) {
                a += ((1<<K) - 1);
            } else {
                for (int j=0; j<K; j++) {
                    a += (1<<(K-j-1))*(!!curr[j]);
                }
                v.push_back(unzigzag(a));
                a=0;
            }
        }
        return false;
    }
};

struct HC {
    int num, data;
    chunk bits;
    HC *p,*l,*r;
    HC(int d,int n) {
        data=d;
        num=n;
        p=0;
        l=0;
        r=0;
    }
};

struct HCCompare {
bool operator ()(const HC*a, const HC*b) {
        return a->num > b->num;
    }
};

bool kisebb(HC*a, HC*b) {
    return a->num < b->num;
}

void preorder(HC* a, std::string s, std::map<int, chunk>& dict) {
//    std::cout << s << " ";
    if (!a->l && !a->r) {
        chunk c;
        c.debugput(s);
        dict[a->data] = c;
    }
    if (a->l) preorder(a->l, s+'0', dict);
    if (a->r) preorder(a->r, s+'1', dict);
}

class HuffmanCode : public PrefixCode<int> {
    std::map<int, chunk> dictionary;
public:
    bool put_element(unsigned int x, chunk &c) {
        c.put_chunk(dictionary.at(x));
    }


    void build(const std::vector<int>& v) {
        std::map<int, int> hist;
        for (int a : v) {
            hist[a]++;
        }
        std::priority_queue<HC*, std::vector<HC*>, HCCompare> table;
        for (std::pair<int,int>p : hist) {
            table.push(new HC(p.first, p.second));
        }
        while(table.size()>1) {
            HC* a = table.top();
            table.pop();
            HC* b = table.top();
            table.pop();
            HC* parent = new HC(a->data, a->num+b->num);
            parent->l = a;
            parent->r = b;
            a->p = parent;
            b->p = parent;
            table.push(parent);
            //std::cout << a->num << " " << b->num << " -> " << parent->num << " ";
        }
        preorder( table.top(), "", dictionary);
/*
        std::cout << " dict ready " << std::endl;
        for (std::pair<int, chunk> a : dictionary) {
            std::cout << a.first << " ";
        }
        std::cout << " dict ready " << std::endl;
*/
    }

    virtual bool encode(const std::vector<int>& v, chunk& c) {
        for (int a : v) {
            put_element(a, c);
        }
        return true;
    }
    virtual bool decode(const chunk& c, std::vector<int> &v) {
        return false;
    }
};


class VLC {
    public:
    VLC() {}

};

}

#endif
