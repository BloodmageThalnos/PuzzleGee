#include "bits/stdc++.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace std;

int fls(int x){
    return 32 - __builtin_clz(x);
}
int main(){
    for(int i=1; i<=20; i++){
        cout << i << ' ' << fls(i) << endl;
    }

}