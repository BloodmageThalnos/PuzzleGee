#define N 6
#define X 2
#define Y 3
#include "bits/stdc++.h"
#include "solver.h"
#include "puzzle.h"

using namespace std;

const int primes[] = {163, 241, 251, 263, 431, 461, 463, 521, 523, 541, 563, 613, 631, 641, 643, 653};

bool check(const puzzle& p){
    bool result = true;

    for(int x: primes){
        bool has = false;

        for(int i=0; i<N; i++){
            for(int j=0; j<N-2; j++){
                if(p.nodes[i][j].val * 100 + p.nodes[i][j+1].val * 10 + p.nodes[i][j+2].val == x){
                    has = true;
                    break;
                }
                if(p.nodes[j][i].val * 100 + p.nodes[j+1][i].val * 10 + p.nodes[j+2][i].val == x){
                    has = true;
                    break;
                }
                if(i<N-2 && p.nodes[i][j].val * 100 + p.nodes[i+1][j+1].val * 10 + p.nodes[i+2][j+2].val == x){
                    has = true;
                    break;
                }
                if(i>1 && p.nodes[i][j].val * 100 + p.nodes[i-1][j+1].val * 10 + p.nodes[i-2][j+2].val == x){
                    has = true;
                    break;
                }
            }
        }

        if(!has) {
            result = false; break;
        }
    }

    return result;
}

void solve(){
    puzzle p;
    vector<thread> threads;
    vector<puzzle> solutions;
    mutex _mutex;
    for(int i=1; i<=N; i++){
        p.nodes[0][0].val = i;
        threads.emplace_back([p, &solutions, &_mutex](){
            _mutex.lock();
            cout << "Thread " << p.nodes[0][0].val << " started" << endl;
            _mutex.unlock();
            solver s;
            s.target(p);
            s.set_extra_check(check);
            s.set_solution_count(5);
            bool result = s.solve();
            if(result){
                cout << "Thread " << p.nodes[0][0].val << " got " << s.solutions.size() << " solutions." << endl;
                _mutex.lock();
                for(auto& sol: s.solutions){
                    solutions.push_back(sol);
                }
                _mutex.unlock();
            }
        });
    }
    for(auto& thread: threads) {
        thread.join();
    }
    cout << "Solution.size()=" << solutions.size() << endl;
    for(auto& sol: solutions){
        sol.print();
    }
}

int main(){
    solve();
    return 0;
}