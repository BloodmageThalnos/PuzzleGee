/*
    The first puzzle generating system by littledva.
    Includes extended puzzle solving & puzzle generating.

    This program focuses on thermo puzzle.
*/

#include <filesystem>

#include "parser.h"
#include "puzzle.h"
#include "solver.h"

using namespace std;

filesystem::path output_dir = "./puzzles";
vector<string> constraints;
vector<string> hidden_constraints;
vector<pair<int, int>> possible_givens;
string output_prefix;

int num_workers = 1;
int min_given_count = 30;

void parse_config(fstream& fin){
    string line;
    while(getline(fin, line)){
        if(line.length() == 0 || line[0] != '[') continue;
        if(line == "[constraint]"){
            while(getline(fin, line)){
                if(line.length() == 0) break;
                constraints.push_back(line);
                cout << "Got constraint: [" << line << "]\n";
            }
        }
        else if(line == "[hidden]"){
            while(getline(fin, line)){
                if(line.length() == 0) break;
                hidden_constraints.push_back(line);
                cout << "Got hidden constraint: [" << line << "]\n";
            }
        }
        else if(line == "[possible_given]"){
            while(getline(fin, line)){
                if(line.length() == 0) break;
                int sx, sy, ex, ey;
                sx = line[0] - '0';
                if(line[1] == '-') {
                    sy = line[2] - '0';
                    ex = line[4] - '0';
                    if(line[5] == '-') ey = line[6] - '0';
                    else ey = ex;
                }
                else {
                    sy = sx;
                    ex = line[2] - '0';
                    if(line[3] == '-') ey = line[4] - '0';
                    else ey = ex;
                }
                for(int i=sx; i<=sy; i++){
                    for(int j=ex; j<=ey; j++){
                        possible_givens.push_back({i-1, j-1});
                        cout << "Got possible_given: [" << i-1 << ',' << j-1 << "]\n";
                    }
                }
            }
        }
        else if(line == "[output]"){
            getline(fin, line);
            output_dir = line;
            cout << "output_dir: [" << output_dir << "]\n";
        }
        else if(line == "[prefix]"){
            getline(fin, output_prefix);
            cout << "output_prefix: [" << output_prefix << "]\n";
        }
        else if(line == "[num_workers]"){
            int c;
            num_workers = 0;
            while(c = fin.get(), c != EOF && c != '\n'){
                num_workers = num_workers * 10 + (c - '0');
            }
            cout << "num_workers: [" << num_workers << "]\n";
        }
        else if(line == "[min_given_count]"){
            int c;
            min_given_count = 0;
            while(c = fin.get(), c != EOF && c != '\n'){
                min_given_count = min_given_count * 10 + (c - '0');
            }
            cout << "min_given_count: [" << min_given_count << "]\n";
        }
    }
}

void initialize(){
    // Initialize input & output stream.
    cin.tie(0);
    cout.tie(0);
    ios::sync_with_stdio(false);

    // Initialize random
    srand(time(0));
    
    // Initialize configs
    fstream fin("./config", ios::in);
    if(fin.is_open()){
        parse_config(fin);
    }
    fin.close();
}

bool check_environment(){
    // Checks output dir exists and writable
    if(!(mkdir(output_dir.c_str(), 0777) || errno == EEXIST)){
        if(access(output_dir.c_str(), W_OK) == -1){
            cerr << "Output directory is not writable." << endl;
            return false;
        }
    }

    puts("Check environment ok.");
    return true;
}

void generate_puzzle(){
    // Generate the initial puzzle with constraints.
    puzzle initial;
    for(const string& s: constraints){
        if(!initial.parse_constraint(s)){
            cerr << "Failed to parse constraint." << endl;
            return;
        }
    }
    puzzle initial_hidden = initial;
    for(const string& s: hidden_constraints){
        if(!initial_hidden.parse_constraint(s)){
            cerr << "Failed to parse constraint." << endl;
            return;
        }
    }

    // target: minimize given count
    atomic<int> min_given_count = ::min_given_count;
    atomic<int> id = 0;

    vector<thread> threads;
    for(int tid = 1; tid <= num_workers; tid++){
        threads.emplace_back([&, tid](){
            solver s;
            uniform_int_distribution<int> distribution(0, (1<<16)-1);
            random_device rd;
            mt19937 random_number_engine(rd()); // pseudorandom number generator
            auto rnd = bind(distribution, random_number_engine);
            s.set_random(rnd);
            int tried_times = 0;
            while(true){
                // Get a copy of the initial puzzle
                puzzle p = initial;
                int given_count = 0;
                int one_to_six_count = 0;

                // Randomly solve the puzzle.
                s.target(initial_hidden);
                s.set_solution_count(1);
                bool result = s.solve();
                if(!result){
                    cerr << "Failed to solve the initial (hidden) puzzle." << endl;
                    return;
                }
                puzzle solved = s.solutions[0];
                //cout << "Solved the initial puzzle." << endl;
                //solved.print();

                // Add random constraints based on the solved puzzle.
                for(int _=0; _<300; _++){
                    // Randomly pick a possible given.
                    int x = rnd() % possible_givens.size();
                    int y = possible_givens[x].first;
                    int z = possible_givens[x].second;
                    if(p.nodes[y][z].is_checked()){
                        continue;
                    }
                    if(solved.nodes[y][z].val >= 1 && solved.nodes[y][z].val <= 6){
                        if(++one_to_six_count >= 4) continue;
                    }
                    p.nodes[y][z] = solved.nodes[y][z];

                    given_count++;
                    // Skip early solving to save time.
                    if(given_count < min_given_count){
                        continue;
                    }
                    // Resolve the puzzle and check if there is single solution.
                    s.target(p);
                    // s.set_random(false);
                    s.set_solution_count(2);
                    bool result = s.solve();
                    if(s.solutions.size() == 1){
                        break;
                    }
                    // solution not optimal, restart progress.
                    if(given_count > min_given_count + 6){
                        break;
                    }
                }

                if(s.solutions.size() != 1){
                    continue;
                }

                for(int i=0; i<N; i++)
                for(int j=0; j<N; j++){
                    // Pick a possible given, remove it.
                    if(!p.nodes[i][j].is_checked()){
                        continue;
                    }
                    int tmp = p.nodes[i][j].val;
                    p.nodes[i][j].init_unchecked();

                    // Resolve the puzzle and check if there is single solution.
                    s.target(p);
                    s.set_solution_count(2);
                    bool result = s.solve();
                    if(s.solutions.size() == 1){
                        given_count--;
                    }
                    else{
                        p.nodes[i][j] = tmp;
                    }
                }

                if(given_count > min_given_count){
                    continue;
                }

                min_given_count = given_count;

                // If there is a single solution, write the puzzle to file.
                
                string filename = output_prefix + "_" + to_string(++id) + "_" + to_string(given_count) + ".txt";
                ofstream fout(output_dir / filename);
                if(!fout.is_open()){
                    cerr << "Failed to open file: " << filename << endl;
                    return;
                }
                fout << p.to_string();
                fout.close();
                cout << "Generated puzzle: " << filename << endl;
            }
        });
    }
    
    for(auto& thread: threads) {
        thread.join();
    }
}

int main(){
    initialize();
    if(!check_environment()) return -1;
    generate_puzzle();
    return 0;
}

