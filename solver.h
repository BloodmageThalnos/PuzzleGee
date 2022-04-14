/*
    Implements a solver for extended sudoku puzzle.
*/

#pragma once
#include <bits/stdc++.h>

#include "puzzle.h"
#include "log.h"

using namespace std;

class solver{
    puzzle p;
    bool broken = false;
    int checked_count = 0;
    int solution_count = 1;
    function<int()> random = nullptr;
    function<bool(const puzzle&)> extra_check = 0;
public:
    vector<puzzle> solutions;

    solver(){}

    puzzle get_solution(){
        return solutions[0];
    }

    void set_solution_count(int c){
        solution_count = c;
    }

    void set_random(const function<int()>& r = std::rand){
        random = r;
    }

    void set_extra_check(const function<bool(const puzzle&)>& f){
        extra_check = f;
    }

    void target(const puzzle& _p){
        p = _p;
        checked_count = 0;
        broken = false;
    }

    bool check_rules_for_node(int val, int i, int j){
        for(int k = 0; k < N; k++) if(i!=k){
            node& n2 = p.nodes[k][j];
            if(n2.is_checked() && n2.val == val) {
                return false;
            }
        }
        for(int k = 0; k < N; k++) if(j!=k){
            node& n2 = p.nodes[i][k];
            if(n2.is_checked() && n2.val == val) {
                return false;
            }
        }
        for(int k = 0; k < N; k++){
            int x = i/X*X + k/Y;
            int y = j/Y*Y + k%Y;
            if(x==i && y==j) continue;
            node& n2 = p.nodes[x][y];
            if(n2.is_checked() && n2.val == val) {
                return false;
            }
        }
        for(const thermal& t: p.thermals){
            for(int k = 0; k < t.path.size(); k++){
                int x = t.path[k].first, y = t.path[k].second;
                if(x == i && y == j) {
                    for(int kk = 0; kk < k; kk++){
                        node& n2 = p.nodes[t.path[kk].first][t.path[kk].second];
                        if(n2.is_checked() && n2.val >= val) {
                            return false;
                        }
                    }
                    for(int kk = k+1; kk < t.path.size(); kk++){
                        node& n2 = p.nodes[t.path[kk].first][t.path[kk].second];
                        if(n2.is_checked() && n2.val <= val) {
                            return false;
                        }
                    }
                    break;
                }
            }
        }
        return true;
    }

    void check_node(node& n, int i, int j){
        if(n.empty()){
            broken = true;
            return;
        }
    }

    bool eliminate_candidates(int i, int j){
        bool updated = false;
        node& n = p.nodes[i][j];
        for(int k=0; k<N; k++){
            node& n2 = p.nodes[k][j];
            if(n2.can_be(n.val)){
                n2.exclude(n.val);
                updated = true;
                check_node(n2, k, j);
            }
        }
        for(int k=0; k<N; k++){
            node& n2 = p.nodes[i][k];
            if(n2.can_be(n.val)){
                n2.exclude(n.val);
                updated = true;
                check_node(n2, i, k);
            }
        }
        for(int k=0; k<N; k++){
            int x = i/X*X + k/Y;
            int y = j/Y*Y + k%Y;
            node& n2 = p.nodes[x][y];
            if(n2.can_be(n.val)){
                n2.exclude(n.val);
                updated = true;
                check_node(n2, x, y);
            }
        }

        // Thermal
        for(auto& t: p.thermals){
            for(int k=0; k<t.path.size(); k++) {
                int x = t.path[k].first, y = t.path[k].second;
                if(x == i && y == j){
                    // Nodes before this one should have lower value
                    int now = n.val;
                    for(int kk=k-1; kk>=0; kk--, now--) {
                        if(now<=1){
                            broken = true;
                            break;
                        }
                        char x = t.path[kk].first, y = t.path[kk].second;
                        node& n2 = p.nodes[x][y];
                        if(n2.is_unchecked()){
                            bool local_updated = false;
                            for(int t=now; t<=N; t++) {
                                if(n2.can_be(t)){
                                    local_updated = true;
                                    n2.exclude(t);
                                }
                            }
                            if(local_updated){
                                updated = true;
                                check_node(n2, x, y);
                            }
                        }
                    }
                    // Nodes after this one should have higher value
                    now = n.val;
                    for(int kk=k+1; kk<t.path.size(); kk++, now++) {
                        if(now>=N){
                            broken = true;
                            break;
                        }
                        char x = t.path[kk].first, y = t.path[kk].second;
                        node& n2 = p.nodes[x][y];
                        if(n2.is_unchecked()){
                            bool local_updated = false;
                            for(int t=1; t<=now; t++) {
                                if(n2.can_be(t)){
                                    local_updated = true;
                                    n2.exclude(t);
                                }
                            }
                            if(local_updated){
                                updated = true;
                                check_node(n2, x, y);
                            }
                        }
                    }
                    break;
                }
            }
        }
        return updated;
    }

    bool hidden_singles(){
        bool found = false;
        for(int i=0; i<N; i++) {
            for(int k=1; k<=N; k++){
                int count = 0;
                int pos = -1;
                for(int j=0; j<N; j++){
                    if(p.nodes[i][j].val == k) goto out1;
                    if(p.nodes[i][j].can_be(k)){
                        count++;
                        pos = j;
                    }
                }
                if(count == 1){
                    p.nodes[i][pos].val = k;
                    checked_count++;
                    eliminate_candidates(i, pos);
                    found = true;
                }
            out1:;
            }
        }
        for(int i=0; i<N; i++) {
            for(int k=1; k<=N; k++){
                int count = 0;
                int pos = -1;
                for(int j=0; j<N; j++){
                    if(p.nodes[j][i].val == k) goto out2;
                    if(p.nodes[j][i].can_be(k)){
                        count++;
                        pos = j;
                    }
                }
                if(count == 1){
                    p.nodes[pos][i].val = k;
                    checked_count++;
                    eliminate_candidates(pos, i);
                    found = true;
                }
            out2:;
            }
        }
        for(int i=0; i<Y; i++) {
            for(int j=0; j<X; j++) {
                for(int k=1; k<=N; k++){
                    int count = 0;
                    int pos = -1;
                    for(int o=0; o<N; o++){
                        int x = i*X + o/Y;
                        int y = j*Y + o%Y;
                        if(p.nodes[x][y].val == k) goto out3;
                        if(p.nodes[x][y].can_be(k)){
                            count++;
                            pos = o;
                        }
                    }
                    if(count == 1){
                        int x = i*X + pos/Y;
                        int y = j*Y + pos%Y;
                        p.nodes[x][y].val = k;
                        checked_count++;
                        eliminate_candidates(x, y);
                        found = true;
                    }
                out3:;
                }
            }
        }
        return found;
    }

    bool update_therometer(){
        bool updated = false;

        for(auto& t: p.thermals){
            int cnt = t.path.size();
            for(int i=0; i<cnt-1; i++){
                int x1 = t.path[i].first, y1 = t.path[i].second;
                if(p.nodes[x1][y1].is_checked()){
                    continue;
                }
                int j = i+1;
                int x2 = t.path[j].first, y2 = t.path[j].second;
                if(p.nodes[x2][y2].is_checked()){
                    i++;
                    continue;
                }
                unsigned int& val1 = p.nodes[x1][y1].val, &val2 = p.nodes[x2][y2].val;
                bool local_updated = false;
                int mi_left = ffs(val1);
                for(int i=mi_left-1; i>=0; i--){
                    if(val2 & (1<<i)){
                        val2 ^= (1<<i);
                        local_updated = true;
                    }
                }
                if(local_updated){
                    updated = true;
                    check_node(p.nodes[x2][y2], x2, y2);
                    if(broken) return false;
                }
                local_updated = false;
                int ma_right = 32 - __builtin_clz(val2 ^ UNCHECKED_MASK);
                for(int i=ma_right-1; i<N; i++){
                    if(val1 & (1<<i)){
                        val1 ^= (1<<i);
                        local_updated = true;
                    }
                }
                if(local_updated){
                    updated = true;
                    check_node(p.nodes[x1][y1], x1, y1);
                    if(broken) return false;
                }
            }
        }

        return updated;
    }

    void brute_force(){
        int si = -1, sj = -1, sval = 1000;
        for(int i=0; i<N; i++) {
            for(int j=0; j<N; j++) {
                if(p.nodes[i][j].is_unchecked()){
                    int cnt = __popcount(p.nodes[i][j].val);
                    if(sval > cnt) sval = cnt, si = i, sj = j;
                }
            }
        }
        int i = si, j = sj;
        int random_val = random ? random() : 0;
        char values[N] = {0}, Z = 0;
        for(int k=0; k<N; k++){
            int value = (k+random_val)%N+1;
            if(p.nodes[i][j].can_be(value) && check_rules_for_node(value, i, j)){
                values[Z++] = value;
            }
        }
        if(!Z) {
            broken = true;
            return;
        }
        for(int _=0; _<Z-1; _++){
            puzzle tmp = p;
            int tmp_c = checked_count++;
            p.nodes[i][j].val = values[_];
            eliminate_candidates(i, j);
            if(!broken && solve_recursive()){
                return;
            }
            p = tmp;
            broken = false;
            checked_count = tmp_c;
        }
        p.nodes[i][j].val = values[Z-1];
        checked_count++;
        eliminate_candidates(i, j);
        if(!broken && solve_recursive()){
            return;
        }
        broken = true;
    }

    bool solve(){
        solutions.clear();
        for(int i=0; i<N; i++) {
            for(int j=0; j<N; j++) {
                if(p.nodes[i][j].is_checked()){
                    checked_count++;
                    eliminate_candidates(i, j);
                }
            }
        }
        solve_recursive();
        return solutions.size() > 0;
    }

    bool solve_recursive(){
        #ifdef DEBUG
        cout << "Solving, count=" << checked_count << endl;
        p.print();
        #endif
        while(true){
            bool updated = false;
            // 1. Eliminate all given candidates
            for(int i=0; i<N; i++) {
                for(int j=0; j<N; j++) {
                    if(p.nodes[i][j].check()){
                        checked_count++;
                        updated = true;
                        eliminate_candidates(i, j);
                        if(broken) return false;
                    }
                }
            }
            // 1.1 Check if all nodes are determinated.
            if(checked_count == N*N) {
                if(extra_check && !extra_check(p)){
                    return false;
                }

                solutions.push_back(p);
                
                if(solutions.size() >= solution_count){
                    // Return true to stop recursion
                    return true;
                }
                return false;
            }
            // 1.2 If some nodes are updated, return to step 1.
            if(updated) continue;

            // 2. Find hidden singles
            updated = hidden_singles();
            // 2.2 If some nodes are updated, return to step 1.
            if(broken) return false;
            if(updated) continue;

            // 3. Update therometer logic
            updated = update_therometer();
            if(broken) return false;
            if(updated) continue;

            // 4. TODO: Find naked pairs/triples
            // 5. TODO: Find hidden pairs/triples
            
            // Finally. Try brute force
            brute_force();
            if(broken) return false;
            break;
        }

        return true;
    }
};