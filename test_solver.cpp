#include "solver.h"

void test1(){
    if(N!=6){
        cout<<"Skipped test1: N!=6"<<endl;
        return;
    }
    puzzle p;
    p.parse_constraint("thermo 0,0 1,0 2,0 3,1");
    p.parse_constraint("thermo 0,1 1,1 2,1 3,2 3,3");
    p.parse_constraint("thermo 3,4 2,4 1,4 0,3");
    p.parse_constraint("thermo 4,5 3,5 2,5 1,5 0,4");
    // p.parse_constraint("thermo 4,1 5,1");
    p.parse_constraint("thermo 5,2 5,3 5,4 5,5");

    solver s;
    s.set_solution_count(10);
    s.set_random(true);
    s.target(p);
    bool result = s.solve();
    cout<<"Test1: result="<<result<<", solution.size()="<<s.solutions.size()<<endl;
    for(auto& sol: s.solutions){
        sol.print();
    }
    cout<<"Test1 "<<(s.solutions.size()==2?"passed":"failed")<<endl;
}

void test2(){
    if(N!=9){
        cout<<"Skipped test2: N!=9"<<endl;
        return;
    }
    puzzle p;

    vector<string> constraints;
    constraints.push_back("thermo 2,3 1,2 0,1 0,0");
    constraints.push_back("thermo 2,5 2,6 2,7 1,8");
    constraints.push_back("thermo 4,4 3,4 2,4 1,4 0,4");
    constraints.push_back("given 4,7 1");
    constraints.push_back("given 5,7 2");
    constraints.push_back("given 6,6 3");
    constraints.push_back("given 7,5 4");
    constraints.push_back("given 7,4 5");
    constraints.push_back("given 7,3 6");
    constraints.push_back("given 6,2 7");
    constraints.push_back("given 5,1 8");
    constraints.push_back("given 4,1 9");
    constraints.push_back("given 5,5 1");

    for(auto& c: constraints){
        p.parse_constraint(c);
    }

    solver s;
    s.set_solution_count(10);
    s.target(p);
    bool result = s.solve();
    cout << "Test2 result: " << result << ", solution.size()=" << s.solutions.size() << endl;
}


int main(){
    test1();
    test2();
}