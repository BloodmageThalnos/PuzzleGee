#include "solver.h"

void test1(){
    if(N!=4){
        cout<<"Skipped test1: N!=4"<<endl;
        return;
    }
    puzzle p;
    p.parse_constraint("thermo 0,3 0,2 0,1 0,0");
    p.parse_constraint("thermo 2,0 2,1 2,2 2,3");

    solver s;
    s.target(p);
    bool result = s.solve();
    cout<<"Test1: "<<(result?"pass":"fail")<<endl;
    s.get_solution().print();
}

void test2(){
    if(N!=4){
        cout<<"Skipped test2: N!=4"<<endl;
        return;
    }
    puzzle p;
    p.parse_constraint("thermo 0,1 0,2");
    p.parse_constraint("thermo 1,1 1,0 0,0");
    p.parse_constraint("thermo 2,3 2,2 2,1");
    p.parse_constraint("thermo 3,0 2,0");

    solver s;
    s.target(p);
    bool result = s.solve();
    cout<<"Test2: "<<(result?"pass":"fail")<<endl;
    s.get_solution().print();
}

void test3(){
    if(N!=4){
        cout<<"Skipped test3: N!=4"<<endl;
        return;
    }
    puzzle p;
    p.parse_constraint("thermo 0,1 1,2");
    p.parse_constraint("thermo 2,3 1,3");
    p.parse_constraint("given 0,1 2");
    p.parse_constraint("given 3,0 3");

    solver s;
    s.target(p);
    bool result = s.solve();
    cout<<"Test3: "<<(result?"pass":"fail")<<endl;
    s.get_solution().print();
}

int main(){
    test1();
    test2();
    test3();

}