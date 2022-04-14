#pragma once
#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

// size of the matrix and value range
#ifndef N
const int N = 9;
#endif

// size of each box
#ifndef X
const int X = 3;
#endif
#ifndef Y
const int Y = 3;
#endif

const unsigned int UNCHECKED_MASK = 1u<<30;

// A node represents a cell in the puzzle.
struct node{
    unsigned int val;
    node(): val(0){}
    node(int v): val(v){}
    void init(int v){
        val = v;
    }
    void init_unchecked(){
        val = UNCHECKED_MASK | ((1u<<N)-1);
    }
    bool can_be(int v){
        return (val & UNCHECKED_MASK) && (val & (1u<<v-1));
    }
    bool is_checked(){
        return !(val & UNCHECKED_MASK);
    }
    bool is_initial(){
        return val == (UNCHECKED_MASK | ((1u<<N)-1));
    }
    bool is_unchecked(){
        return val & UNCHECKED_MASK;
    }
    bool check(){
        if((val & UNCHECKED_MASK) && __builtin_popcount(val) == 2){
            val = ffs(val);
            return true;
        }
        return false;
    }
    bool empty(){
        return val == UNCHECKED_MASK;
    }
    void exclude(int v){
        val &= ~(1u<<v-1);
    }
};

// A thermal is a constraint of node that should be connected & unrepeated.
// The value on a thermal should increase strictly.
struct thermal{
    vector<pair<char, char>> path;
    thermal(): path(){}
};

struct puzzle{
    node nodes[N][N];
    vector<thermal> thermals;

    puzzle(): nodes(), thermals(){
        for(int i=0; i<N; i++)
            for(int j=0; j<N; j++)
                nodes[i][j].init_unchecked();
    }

    puzzle(const puzzle& p): nodes(), thermals(p.thermals){
        for(int i=0; i<N; i++)
            for(int j=0; j<N; j++)
                nodes[i][j] = p.nodes[i][j];
    }

    string to_string(){
        stringstream ss;
        for(int i=0; i<N; i++){
            for(int j=0; j<N; j++){
                node& n = nodes[i][j];
                if(n.is_checked()) {
                    ss << "given " << i << "," << j << " " << n.val << endl;
                }
            }
        }
        for(auto& t: thermals){
            ss << "thermo ";
            for(auto& p: t.path){
                ss << (int)p.first << "," << (int)p.second << " ";
            }
            ss << endl;
        }
        ss << "--------------" << endl;
        for(int i=0; i<N; i++){
            for(int j=0; j<N; j++){
                node& n = nodes[i][j];
                if(n.is_checked())
                    ss << n.val << "  ";
                else {
                    ss << "?  ";
                }
            }
            ss << endl;
        }
        return ss.str();
    }

    void print(){
        // output the nodes
        for(int i=0; i<N; i++){
            for(int j=0; j<N; j++){
                node& n = nodes[i][j];
                if(n.is_checked())
                    cout << n.val << "  ";
                else if(n.is_initial()){
                    cout << "?  ";
                }
                else {
                    cout << "(";
                    for(int i=1; i<=N; i++){
                        if(n.can_be(i)){
                            cout << i;
                        }
                    }
                    cout << ") ";
                }
            }
            cout << endl;
        }
        cout << endl;
    }

    bool parse_constraint(const string& s){
        if(s.substr(0, 6) == "thermo"){
            thermal t;
            for(int i=6; i<s.size(); i++){
                if (s[i] == ' ') continue;
                else if (s[i]>='0' && s[i]<='9'){
                    int now = s[i] - '0';
                    while(i+1<s.size() && s[i+1]>='0' && s[i+1]<='9'){
                        now = now*10 + s[i+1] - '0';
                        i++;
                    }
                    if(++i == s.size() || s[i++]!=','){
                        cerr << "Thermal path error:\n" << s << endl;
                        return false;
                    }
                    int now2 = s[i] - '0';
                    while(i+1<s.size() && s[i+1]>='0' && s[i+1]<='9'){
                        now2 = now2*10 + s[i+1] - '0';
                        i++;
                    }
                    t.path.push_back({now, now2});
                }
                else{
                    cerr << "Unknown character in char " << i << ":\n" << s << endl;
                    return false;
                }
            }
            if(t.path.size()<2) {
                cerr << "Thermal path must have at least two points:\n" << s << endl;
                return false;
            }
            thermals.push_back(t);
        }
        else if(s.substr(0, 5) == "given"){
            // Check sixth eighth and tenth character is numerical
            if(s[6]<'0' || s[6]>'9' || s[8]<'0' || s[8]>'9' || s[10]<'0' || s[10]>'9'){
                cerr << "Given command error:\n" << s << endl;
                return false;
            }
            int i = s[6] - '0';
            int j = s[8] - '0';
            int k = s[10] - '0';
            nodes[i][j].init(k);
        }
        else{
            cerr << "Unknown command:\n" << s << endl;
            return false;
        }
        return true;
    }

};
