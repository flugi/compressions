#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <algorithm>
#include <set>
#include "../../ITK_graphicslib/graphics.hpp"
using namespace genv;
using namespace std;

const int Y = 720;
const int X = Y*16/9;

struct Stat {
protected:
    map<char, int> freq;
    int sum;
public:
    Stat() : sum(0) {
        
    }
    int alphabet_size() const {return freq.size();}
    string alphabet() const {
        string res;
        for (pair<char, int> p : freq) {
            res+=p.first;
        }
        return res;
    }
    void reset() {
        freq.clear();
        sum=0;
    }
    void add(char c) {
        freq[c]++;
        sum++;
    }
    void remove(char c) {
        if (freq.find(c)==freq.end()) 
            return;
        if (freq.size()==1) return;
        freq[c]--;
        if (freq.at(c)==0) {
            freq.erase(c);
        }
        sum--;
    }
    void add(string s) {
        for (char c : s) {
            add(c);
        }
    }
    int operator[](char c) const {
        return freq.at(c);
    }
    int denominator() const {return sum;}
    double p(string s) {
        double res=1;
        for (char c : s) {
            float pc = float(freq.at(c))/sum;
            res *= pc;
        }
        return res;
    }
    void dump() {
        for (pair<char,int> p : freq) {
            cout << p.first << " " << p.second << endl;
        }
    }
} defaultstat;



struct ANS {
protected:
    Stat s;
    vector<vector<int> > I;
    vector<int> decodeoffset;
    int L;
    int offset = 1;
    string decodalpha;
public:
    ANS(Stat ps = defaultstat) : s(ps), I(s.alphabet_size()), decodeoffset(s.denominator()), L(s.denominator()) {
        int N = s.alphabet_size();
        string alpha = s.alphabet();
        {
            int p=0;
            for (int i = 0; i < L; i++) {
                while( I[p].size() >= s[alpha[p]]) ++p%=N;
                decodeoffset[i] = I[p].size();
                I[p].push_back(i);
                decodalpha += alpha[p];
                ++p%=N;
            }
        }
    }
    int get_offset() const {return offset;}
    int enc(int st, int a) {
        int Is = I[a].size();
        int per = st/Is;
        int news = L*per+I[a][st%Is];
        return news+offset;
    }
    int enc_symbol(int st, char c) {
        int a = s.alphabet().find(c);
        return enc(st,a);
    }
    bool is_valid_symbol(char c) {
        return s.alphabet().find(c) != string::npos;
    }
    int dec(int st) {
        if (st < offset) return -1;
        char currsymbol = decodalpha[(st-offset) % L];
        int a = s.alphabet().find(currsymbol);
        int Is = I[a].size();
        int period = (st-offset)/L;
        int period_offset = decodeoffset[(st-offset)%L];
//        cout <<"["<< a << " " << Is << " " << period << " " << offset <<"]" << endl;
        return period*Is+period_offset;
    }
    string decode_state(int st) {
        string res;
        if (st==0) return "";
        while( st >= offset) {
            res = which_symbol(st) + res;
            st = dec(st);
//            cout << st << flush;
        };
        return res;
    }
    char which_symbol(int state) {
        if (state < offset) return ' ';
        return decodalpha[(state-offset) % L];
    }
};

enum class Help {
    ENCODING_TABLE, PARENT_STATE, RULE1, RULE2, RULE3, RULE_FILL1, RATIOS
};

const vector<int> FKey({key_f1, key_f2, key_f3, key_f4, key_f5, key_f6, key_f7, key_f8, key_f9, key_f10, key_f11});


struct centertext {
    string s;
    int sx, sy;
    centertext(string ps, int pw, int ph) : s(ps), sx(pw), sy(ph) {}
    void operator()(genv::canvas& out) {
        if (s=="") return;
        int w = out.twidth(s);
        int h = out.cascent()+gout.cdescent();
        out << genv::move(+sx/2-w/2, sy/2-h/2) << text(s);
    }
};

struct bordered_centertext {
    string s;
    int sx, sy, b;
    color bc,fc;
    bordered_centertext(string ps, int pw, int ph, color pfc, color pbc, int pb=2) : s(ps), sx(pw), sy(ph), bc(pbc), fc(pfc), b(pb) {}
    void operator()(genv::canvas& out) {
        out << bc;
        int x=out.x();
        int y=out.y();
        for (int dx=-b;dx<=b;dx++) {
            for (int dy=-b;dy<=b;dy++) {
                out << move_to(x+dx, y+dy);
                out<<centertext(s,sx,sy);
            }
        }
        out << fc << move_to(x,y);
        out << centertext(s,sx,sy);
    }
};

struct ANSscreen {
    int state;
    int tableheight, tablex,tabley;
    int cellwidth;
    int rulex, ruley, rulesx, rulesy;
    bool removemode = false;
    genv::color bgcol, txcol, activecol, highcol, helpcol;
    int titlemode=0;
    vector<int> highlight;
    vector<int> helps;
    Stat s;
    ANS a;
    ANSscreen() : bgcol(50,50,50), txcol(255,180,128), activecol(255,255,255), highcol(50,150,50), helpcol(255,255,180), helps(12,0) {
        highlight = vector<int>(9,0);
        state = 0;
        tableheight = Y/6*5;
        tablex = 10;
        tabley = (Y-tableheight)/2;
        s.add("abc");
        cellwidth = 120;
        rulesx = 400;
        rulex = X-rulesx- 10;
        ruley = 20;
        rulesy = gout.cascent()+gout.cdescent()+10;
        a = ANS(s);
    }
    void title() {
        gout << bgcol << move_to(0,0) << box(X,Y);
        gout.load_font("../../ITK_graphicslib/LiberationSans-Bold.ttf",50);
        gout << helpcol;
        gout << move_to(0,0) << centertext("ANS - Asymmetric Numeral System", X,Y/3);
        gout.load_font("../../ITK_graphicslib/LiberationSans-Bold.ttf",30);
        gout << move_to(0,Y/3) << centertext("Episode 1 : Basics", X,Y/3);
        gout.load_font("../../ITK_graphicslib/LiberationSans-Bold.ttf",24);
        gout << move_to(0,2*Y/3) << centertext("a video by Gergely Feldhoffer", X,Y/4);
        
    }
    void draw() {
        if (titlemode) {
            title();
            return;
        }
        gout << move_to(0,0) << bgcol << box(X,Y);
        int fonth = gout.cascent()+gout.cdescent();
        int cellheight = fonth+2;
        int th = tableheight/cellheight;
        int minst = state - th/2;
        if (minst < 0) minst = 0;
        for (int j=0;j<s.alphabet_size();j++) {
            stringstream ss;
            char c = s.alphabet()[j];
            ss << c << " " << s[c] <<"/" << s.denominator();
            gout << move_to(tablex + (1+j) * cellwidth , tabley - fonth) ;
            gout <<txcol << centertext(ss.str(),cellwidth, cellheight);
        }
        gout << txcol << move_to(tablex, tabley-cellheight) << centertext("state", cellwidth, cellheight);
        gout << txcol << move_to(tablex+(s.alphabet_size()+1)*cellwidth, tabley-cellheight) << centertext("parent", cellwidth, cellheight);
        gout << txcol << move_to(tablex+(s.alphabet_size()+2)*cellwidth, tabley-cellheight) << text("encoded content");
        for (int i=0; i<=th; i++) {
            {
                stringstream ss;
                ss << i+minst;
                gout << move_to(tablex, tabley + i*fonth) ;
                if (i!=state-minst) {
                    gout <<txcol << centertext(ss.str(), cellwidth, cellheight);
                } else {
                    gout << activecol << centertext(ss.str(), cellwidth, cellheight);
                }
            }
            {
                for (int j=0;j<s.alphabet_size();j++) {
                    int cellvalue = a.enc(i+minst, j);
                    int period = (cellvalue-a.get_offset()) / s.denominator();
                    if (period < highlight.size() && highlight[period]) {
                        gout << move_to(tablex + (1+j) * cellwidth , tabley + i*fonth) << highcol << box(cellwidth, fonth);

                    }
                    stringstream ss;
                    ss << a.enc(i+minst, j);
                    gout << move_to(tablex + (1+j) * cellwidth , tabley + i*fonth) ;
                    if (i!=state-minst) {
                        gout <<txcol << centertext(ss.str(), cellwidth, cellheight);
                    } else {
                        gout << activecol << centertext(ss.str(), cellwidth, cellheight);
                    }
                }
            }
            {
                stringstream ss;
                if (a.dec(i+minst) != -1)
                ss << a.dec(i+minst);
                ss << " ";
                gout << move_to(tablex + (s.alphabet_size()+1)*cellwidth , tabley + i*fonth) ;
                if (i!=state-minst) {
                    gout <<txcol << centertext(ss.str(), cellwidth, cellheight);
                } else {
                    gout << activecol << centertext(ss.str(), cellwidth, cellheight);
                }
            }
            {
                stringstream ss;
                ss << a.decode_state(i+minst) << " ";
                gout << move_to(tablex + (s.alphabet_size()+2)*cellwidth , tabley + i*fonth) ;
                if (i!=state-minst) {
                    gout <<txcol ;
                } else {
                    gout << activecol ;
                }
                gout << text(ss.str());
            }
        }
        if (helps[int(Help::ENCODING_TABLE)]) {
            int tx = tablex+cellwidth;
            int ty = tabley-cellheight;
            int tsx = s.alphabet_size()*cellwidth;
            int tsy = (th+1)* cellheight;
            for (int i=0;i<3;i++) {
                gout << helpcol ;
                gout << move_to(tx-i, ty-i) << line(0,tsy+i*2);
                gout << line(tsx+i*2,0) << line(0,-tsy-i*2) << line(-tsx-i*2,0);
            }
            
            gout << move_to(tx,ty) << bordered_centertext("Encoding table",tsx, tsy, helpcol, bgcol);
        }
        if (helps[int(Help::PARENT_STATE)]) {
            int tx = tablex+cellwidth+s.alphabet_size()*cellwidth;
            int ty = tabley-cellheight;
            int tsx = cellwidth;
            int tsy = (th+1)* cellheight;
            for (int i=0;i<3;i++) {
                gout << helpcol ;
                gout << move_to(tx-i, ty-i) << line(0,tsy+i*2);
                gout << line(tsx+i*2,0) << line(0,-tsy-i*2) << line(-tsx-i*2,0);
            }
            
            gout << move_to(tx,ty) << bordered_centertext("Parents",tsx, tsy, helpcol, bgcol);
        }
        if (helps[int(Help::RULE1)]) {
            gout << move_to(rulex,ruley) << bordered_centertext("1: cell count = denominator",rulesx, rulesy, helpcol, bgcol);
        }
        if (helps[int(Help::RULE2)]) {
            gout << move_to(rulex,ruley+rulesy) << bordered_centertext("2: column height = frequency",rulesx, rulesy, helpcol, bgcol);
        }
        if (helps[int(Help::RULE3)]) {
            gout << move_to(rulex,ruley+2*rulesy) << bordered_centertext("3: use all numbers 1..denominator",rulesx, rulesy, helpcol, bgcol);
        }
        if (helps[int(Help::RULE_FILL1)]) {
            gout << move_to(rulex,ruley+3*rulesy) << bordered_centertext("Fill: Iterate through the numbers",rulesx, rulesy, helpcol, bgcol);
            gout << move_to(rulex,ruley+4*rulesy) << bordered_centertext("and put the value if the column is",rulesx, rulesy, helpcol, bgcol);
            gout << move_to(rulex,ruley+5*rulesy) << bordered_centertext("long enough or jump to the next",rulesx, rulesy, helpcol, bgcol);
        }
        if (helps[int(Help::RATIOS)]) {
            for (int i=0;i<s.alphabet_size();i++) {
                stringstream ss;
                int es = a.enc(state, i);
                ss <<s.alphabet()[i] <<":" << state << "/" << es << "=" << float(state)/es;
                gout << move_to(rulex,ruley+(6+i)*rulesy) << bordered_centertext(ss.str(),rulesx, rulesy, helpcol, bgcol);
                
            }
        }
    }
    void handle(genv::event ev) {
        if (ev.keycode >= '1' && ev.keycode <= '9') {
            int h = ev.keycode - '1';
            highlight[h]=1-highlight[h];
        }
        if (find(FKey.begin(), FKey.end(), ev.keycode) != FKey.end()) {
            vector<int>::const_iterator found = find(FKey.begin(), FKey.end(), ev.keycode);
            int offset = found - FKey.begin();
            helps[offset] = 1-helps[offset];
        }
        if (ev.button == btn_wheelup) {
            if (state > 0) state--;
        }
        if (ev.button == btn_wheeldown) {
            state++;
        }
        if (ev.button == btn_left && ev.pos_x > tablex + cellwidth && ev.pos_y > tabley && ev.pos_x < tablex + (s.alphabet_size()+1)*cellwidth && ev.pos_y < tabley + tableheight)  {
            int fonth = gout.cascent()+gout.cdescent();
            int th = tableheight/fonth;
            int minst = state - th/2;
            if (minst < 0) minst = 0;
            int sym = (ev.pos_x - tablex - cellwidth) / cellwidth;
            int st = (ev.pos_y-tabley)/fonth + minst;
            state = a.enc(st,sym);
        }
        if (ev.keycode == key_lctrl) {
            removemode = true;
        }
        if (ev.keycode == -key_lctrl) {
            removemode = false;
        }
        if (removemode && ev.keycode >=1 && ev.keycode <= 26) {
            char c = ev.keycode + 'a' - 1;
            s.remove(c);
            a = ANS(s);
            state = 0;
        }
        if (ev.keycode >= 'A' && ev.keycode <= 'Z') {
            char c = tolower(char(ev.keycode));
            s.add(c);
            a = ANS(s);
            state = 0;
        }
        if (ev.keycode >= 'a' && ev.keycode <= 'z') {
            if (a.is_valid_symbol(char(ev.keycode))) {
                state = a.enc_symbol(state, char(ev.keycode) );
            }
        }
        if (ev.keycode == key_backspace) {
            if (state != 0) {
                state = a.dec(state);
            }
        }
        if (ev.keycode == key_pgdn) {
            state += 10;
        }
        if (ev.keycode == key_pgup) {
            state -= 10;
            if (state <0 ) state = 0;
        }
        if (ev.keycode == key_tab) {
            titlemode = 1-titlemode;
        }
    }
    
};

void interactive() {
    gout.open(X,Y);
    gout.load_font("../../ITK_graphicslib/LiberationSans-Bold.ttf",24);
    gout.load_font("../../ITK_graphicslib/LiberationSans-Bold.ttf",24);
    ANSscreen anss;
    anss.draw();
    gout << refresh;
    event ev;
    while(gin >> ev && ev.keycode != key_escape) {
        anss.handle(ev);
        anss.draw();
        gout << refresh;
    }
}

void test() {
    Stat s;
    s.add("aaaaaabbbbcc");
    cout <<"aa:" << s.p("aa") << endl;;
    cout << s.alphabet() << endl;
    ANS a(s);
    int L = s.denominator();
    for (int i=0;i<1500;i++) {
        cout << i << " (" << a.which_symbol(i) <<"): " ;
        for (int j=0;j<s.alphabet_size();j++) {
            cout << a.enc(i,j) << " ";
        }
        cout <<"->" << a.dec(i) << " : " <<a.decode_state(i) << "(" << float(s.p(a.decode_state(i))*100) <<"%)"  ;
        cout << endl;
    }
}

int main(int argc, char * argv[]) {
//    test();
    interactive();
}
