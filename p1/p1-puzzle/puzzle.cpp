// Project Identifier: A8A3A33EF075ACEF9B08F5B9845569ECCB423725

#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <tuple>

using namespace std;

enum class Mode {
  queue_mode,
  stack_mode,
  map_mode,
  list_mode,
}; 



struct Options {
  Mode mode_container;
  Mode mode_output = Mode::map_mode;
};

void printHelp(){
    cout << "Use --queue or -q to direct the program to use a search container that behaves like a queue, performing a breadth first search\n";
    cout << "Use --stack or -s to direct the program to use a search container that behaves like a qlike a stack, performing a depth first search\n";
    cout << "Use --output map|list or -o map|list to direct the program to print solution output in the specified format" << endl;
}

void getMode(int argc, char * argv[], Options &options) {
  opterr = false;
  int choice;
  int index = 0;
  option long_options[] = {
    { "help", no_argument, nullptr, 'h'},
    { "queue", no_argument, nullptr, 'q'},
    { "stack", no_argument, nullptr, 's'},
    { "output", required_argument, nullptr, 'o'},
    { nullptr, 0, nullptr, '\0' }
  }; 
  int exists = 0;
  while ((choice = getopt_long(argc, argv, "hqso:", long_options, &index)) != -1) {
    switch (choice) {
      case 'h':
        printHelp();
        exit(0);

      case 'q':
        exists += 1;
        if(exists != 1){
            cerr << "Error: only one of -q and -s should exist" << endl;
            exit(1);
        }

        options.mode_container = Mode::queue_mode;
        break;

      case 's':
        exists += 1;
        if(exists != 1){
            cerr << "Error: only one of -q and -s should exist" << endl;
            exit(1);
        }
  
        options.mode_container = Mode::map_mode;
        break;

      case 'o':{
        string arg{optarg};
        if (arg != "map" && arg != "list") {
            cerr << "Error: invalid mode\n";
            cerr << " I don't recognize: " << arg << endl;
            exit(1);
        } 

        if (arg == "map") {
            options.mode_output = Mode::map_mode;
        } else {
            options.mode_output = Mode::list_mode;
        }
        break;
      }

      default:
        cerr << choice << endl;
        cerr << "Error: invalid option" << endl;
        exit(1);
    }
  }
   if(exists != 1){
        cerr << "Error: one of -q and -s should exist" << endl;
        exit(1);
    }
}  

void check_entry(uint32_t num_color, char input){
    if(input >= 'A' && input <= 'Z'){
        if(num_color < static_cast<uint32_t>(input - 'A') + 1){
            cerr << input << endl;
            cerr << "Error: invalid map entry (color)" << endl;
            exit(1);
        }
    }else if(input >= 'a' && input <= 'z'){
        if(num_color < static_cast<uint32_t>(input - 'a') + 1){
            cerr << input << endl;
            cerr << "Error: invalid map entry (button)" << endl;
            exit(1);
        }
    }else if(input != '^' && input != '.' && input != '@' && input != '?' && input != '#'){
        cerr << input << endl;
        cerr << "Error: invalid map entry (other)" << endl;
    }
}

int class_to_int(char c){
    if(c == '^'){
        return 0;
    }
    return static_cast<int>(c - 'A') + 1;
}

char int_to_class(int i){
    if(i == 0){
        return '^';
    }
    return static_cast<char>(i - 1) + 'A';
}

bool check(int cl, int r, int c, vector<vector<char>> &map){  
    if(map[r][c] == '?'){
        return true;
    }
    if(map[r][c] == '.'){
        return true;
    }
    char temp = int_to_class(cl);

    if(map[r][c] == '^'){
        return true;
    }

    if(map[r][c] == '@' && cl != 0){
        return true;
    }

    if(map[r][c] >= 97 && map[r][c] <= 122){
        return true;
    }
    if(map[r][c] == temp || map[r][c] == temp + ' '){
        return true;
    }

    
    return false;
    
}



void look(deque<tuple<int, int, int>> &search, tuple<int, int, int> &current, vector<vector<char>> &map, vector<vector<vector<char>>> &backtrace, bool isButton,
        vector<vector<char>> &map2){
    int cl, r, c;
    cl = get<0>(current);
    r = get<1>(current);
    c = get<2>(current);
    if(isButton){
        char temp = map[r][c];
        if(temp != '^'){
            temp = temp - ' ';
        }
        if(backtrace[class_to_int(temp)][r][c] != '.'){
            return;
        }else{
            search.push_back({class_to_int(temp), r, c});
            backtrace[class_to_int(temp)][r][c] = int_to_class(cl) + ' ';
            map2[r][c] = '.';
            return;
        }
    }
    if(r > 0){
        if(!check(cl, r - 1, c, map)){
            //backtrace[cl][r - 1][c] = '#';
        }
        if(map[r - 1][c] == '?'){
            current = {cl, r - 1, c};
            backtrace[cl][r - 1][c] = 'S';
            return;
        }
        if(backtrace[cl][r - 1][c] == '.' && check(cl, r - 1, c, map)){
            //cout << backtrace[cl][r - 1][c] << endl;
            //cout << cl << " " << r - 1 << " " << c << endl;
            search.push_back({cl, r - 1, c});
            backtrace[cl][r - 1][c] = 'S';
            map2[r - 1][c] = '.';
        }
    }
    if(static_cast<size_t>(c + 1) < map[0].size()){
        if(!check(cl, r, c + 1, map)){
            //backtrace[cl][r][c + 1] = '#';
        }
        if(map[r][c + 1] == '?'){
            current = {cl, r, c + 1};
            backtrace[cl][r][c + 1] = 'W';
            return;
        }
        if(backtrace[cl][r][c + 1] == '.' && check(cl, r, c + 1, map)){
            //cout << backtrace[cl][r][c + 1] << endl;
            //cout << cl << " " << r << " " << c +1 << endl;
            search.push_back({cl, r, c + 1});
            backtrace[cl][r][c + 1] = 'W';
            map2[r][c + 1] = '.';
        }
    }

    if(static_cast<size_t>(r + 1) < map.size()){
        if(!check(cl, r + 1, c, map)){
            //backtrace[cl][r + 1][c] = '#';
        }
        if(map[r + 1][c] == '?'){
            current = {cl, r + 1, c};
            backtrace[cl][r + 1][c] = 'N';
            return;
        }
        if(backtrace[cl][r + 1][c] == '.' && check(cl, r + 1, c, map)){
            //cout << backtrace[cl][r + 1][c] << endl;
           // cout << cl << " " << r + 1 << " " << c << endl;
            search.push_back({cl, r + 1, c});
            backtrace[cl][r + 1][c] = 'N';
            map2[r + 1][c] = '.';
        }
    }
    if(c > 0){
        if(!check(cl, r, c - 1, map)){
            //backtrace[cl][r][c - 1] = '#';
        }
        if(map[r][c - 1] == '?'){
            current = {cl, r, c - 1};
            backtrace[cl][r][c - 1] = 'E';
            return;
        }
        if(backtrace[cl][r][c - 1] == '.' && check(cl, r, c - 1, map)){
           // cout << backtrace[cl][r][c - 1] << endl;
           // cout << cl << " " << r << " " << c -1 << endl;
            search.push_back({cl, r, c - 1});
            backtrace[cl][r][c - 1] = 'E';
            map2[r][c - 1] = '.';
        }
    }
}

bool isButton(tuple<int, int, int> &current, vector<vector<char>> &map){
    char temp = map[get<1>(current)][get<2>(current)];
    if((temp >= 97) && (temp <= 122) && (int_to_class(get<0>(current)) != temp - ' ')){
        return true;
    }else if(temp == '^' && get<0>(current) != 0){
        return true;
    }else{
        return false;
    }
}

void print_output(bool isMap, vector<vector<char>> &map, vector<vector<vector<char>>> &backtrace, tuple<int, int, int> &end){
    if(isMap){
        tuple<int, int, int> curr = end;
        char c = backtrace[get<0>(end)][get<1>(end)][get<2>(end)];
        backtrace[get<0>(end)][get<1>(end)][get<2>(end)] = '?';
        while(c != '@'){
            if(c == 'N'){
                curr = {get<0>(curr), get<1>(curr) - 1, get<2>(curr)};
                c = backtrace[get<0>(curr)][get<1>(curr)][get<2>(curr)]; 
                backtrace[get<0>(curr)][get<1>(curr)][get<2>(curr)] = '+';
            }else if(c == 'E'){
                curr = {get<0>(curr), get<1>(curr), get<2>(curr) + 1};
                c = backtrace[get<0>(curr)][get<1>(curr)][get<2>(curr)];
                backtrace[get<0>(curr)][get<1>(curr)][get<2>(curr)] = '+';
            }else if(c == 'S'){
                curr = {get<0>(curr), get<1>(curr) + 1, get<2>(curr)};
                c = backtrace[get<0>(curr)][get<1>(curr)][get<2>(curr)];
                backtrace[get<0>(curr)][get<1>(curr)][get<2>(curr)] = '+';
            }else if(c == 'W'){
                curr = {get<0>(curr), get<1>(curr), get<2>(curr) - 1};
                c = backtrace[get<0>(curr)][get<1>(curr)][get<2>(curr)];
                backtrace[get<0>(curr)][get<1>(curr)][get<2>(curr)] = '+';
            }else if(c == '^'){
                backtrace[get<0>(curr)][get<1>(curr)][get<2>(curr)] = '@';
                curr = {class_to_int(c), get<1>(curr), get<2>(curr)};
                c = backtrace[get<0>(curr)][get<1>(curr)][get<2>(curr)];
                backtrace[get<0>(curr)][get<1>(curr)][get<2>(curr)] = '%';
            }
            else{
                backtrace[get<0>(curr)][get<1>(curr)][get<2>(curr)] = '@';
                curr = {class_to_int(c - ' '), get<1>(curr), get<2>(curr)};
                c = backtrace[get<0>(curr)][get<1>(curr)][get<2>(curr)];
                backtrace[get<0>(curr)][get<1>(curr)][get<2>(curr)] = '%';
            }
        }
        backtrace[get<0>(curr)][get<1>(curr)][get<2>(curr)] = '@';

        for(int i = 0; static_cast<size_t>(i) < backtrace.size(); ++i){
            if(i == 0){
                cout << "// color " << int_to_class(i) << "\n";
            }else{
                cout << "// color " << static_cast<char>(int_to_class(i) + ' ') << "\n";
            }
            for(int j = 0; static_cast<size_t>(j) < backtrace[0].size(); ++j){
                for(int k = 0; static_cast<size_t>(k) < backtrace[0][0].size(); ++k){
                    if(backtrace[i][j][k] == '+'|| backtrace[i][j][k] == '?'|| backtrace[i][j][k] == '@'|| backtrace[i][j][k] == '%'){
                        cout << backtrace[i][j][k];
                    }else{
                        if(i != 0 && map[j][k] == '@'){
                            cout << '.';
                        }else if((class_to_int(map[j][k]) == i) || (class_to_int(map[j][k] - ' ') == i)){
                            cout << '.';
                        }else{
                            cout << map[j][k];
                        }
                    }
                }
            cout << '\n';
            }
        }
    }else{
        tuple<int, int, int> curr = end;
        stack<tuple<int, int, int>> output;
        output.push(end);
        char c = backtrace[get<0>(end)][get<1>(end)][get<2>(end)];
        backtrace[get<0>(end)][get<1>(end)][get<2>(end)] = '?';
        while(c != '@'){
            if(c == 'N'){
                curr = {get<0>(curr), get<1>(curr) - 1, get<2>(curr)};
            }else if(c == 'E'){
                curr = {get<0>(curr), get<1>(curr), get<2>(curr) + 1};
            }else if(c == 'S'){
                curr = {get<0>(curr), get<1>(curr) + 1, get<2>(curr)};
            }else if(c == 'W'){
                curr = {get<0>(curr), get<1>(curr), get<2>(curr) - 1};
            }else if(c == '^'){
                curr = {class_to_int(c), get<1>(curr), get<2>(curr)};
            }
            else{
                curr = {class_to_int(c - ' '), get<1>(curr), get<2>(curr)};
            }
            c = backtrace[get<0>(curr)][get<1>(curr)][get<2>(curr)];
            output.push(curr);
        }
        tuple<int, int, int> temp;
        while(output.size() != 0){
            temp = output.top();
            output.pop();
            if(get<0>(temp) == 0){
                cout << "(" << int_to_class(get<0>(temp)) << ", (" << get<1>(temp) << ", " << get<2>(temp) << "))\n";
            }else{
                cout << "(" << int_to_class(get<0>(temp) + ' ') << ", (" << get<1>(temp) << ", " << get<2>(temp) << "))\n";
            }
        }
    }
}

void no_solution(vector<vector<char>> &map, vector<vector<char>> &map2){
    cout << "No solution.\n";
    cout << "Discovered:\n";
    for(size_t i = 0; i < map.size(); ++i){
        for(size_t j = 0; j < map[0].size(); ++j){
            if(map2[i][j] == '.'){
                cout << map[i][j];
            }else{
                cout << '#';
            }
        }
        cout << "\n";
    }
}

void solve(bool isQueue, bool isMap, pair<int, int> &start, vector<vector<char>> &map, vector<vector<vector<char>>> &backtrace){
    vector<vector<char>> map2;
    map2.resize(map.size(), vector<char>(map[0].size(), '!'));
    deque<tuple<int, int, int>> search;
    tuple<int, int, int> current;
    search.push_back({0, start.first, start.second});
    map2[start.first][start.second] = '.';
    bool found = false;
    while(static_cast<int>(search.size()) > 0 && !found){
        if(isQueue){
            current = search.front();
            search.pop_front();
        }else{
            current = search.back();
            search.pop_back();
        }
        
        if(isButton(current, map)){
            look(search, current, map, backtrace, true, map2);
        }else{
            look(search, current, map, backtrace, false, map2);
        }
        if(map[get<1>(current)][get<2>(current)] == '?'){
            found = true;
        }
    }
    backtrace[0][start.first][start.second] = '@';
    if(!found){
        no_solution(map, map2);
    }else{
        print_output(isMap, map, backtrace, current);
    }
}



int main(int argc, char *argv[]){
    ios_base::sync_with_stdio(false);

    
    vector<vector<vector<char>>> backtrace;
    vector<vector<char>> map;

    uint32_t color, row, col;
    cin >> color >> row >> col;

    if(static_cast<int>(color) > 26 || static_cast<int>(color) < 0){
        cerr << color << "\n";
        cerr << "Error: invalid color entry" << endl;
        exit(1);
    }
    if(static_cast<int>(row) < 1){
        cerr << row << "\n";
        cerr << "Error: invalid row entry" << endl;
        exit(1);
    }
    if(static_cast<int>(col) < 1){
        cerr << col << "\n";
        cerr << "Error: invalid column entry" << endl;
        exit(1);
    }

    
    backtrace.resize(color + 1, vector<vector<char>>(row, vector<char>(col, '.')));
    map.resize(row, vector<char>(col, '!'));

    string junk, line;
    getline(cin, junk);
    int row_count = 0;
    int num1 = 0; //count of @
    int num2 = 0; //count of ?
    pair<int, int> start;
    while(getline(cin, line)){
        if(line[0] != '/' && line[1] != '/' ){
            int count = 0;
            for(char c : line){
                check_entry(color, c);
                if(c == '@'){
                    ++num1;
                    start = {row_count, count};
                }
                if(c == '?'){
                    ++num2;
                }
                map[row_count][count] = c;
                ++count;
            }
            ++row_count;
        }
    }
    if(num1 != 1){
        cerr << "Error: " << num1 << " of @ in maze" << endl;
        exit(1);
    }
    if(num2 != 1){
        cerr << "Error: " << num2 << " of ? in maze" << endl;
        exit(1);
    }

    Options options;
    getMode(argc, argv, options);

    bool isQueue;
    if (options.mode_container == Mode::queue_mode){
        isQueue = true;
    }else{
        isQueue = false;
    }

    bool isMap;
    if(options.mode_output == Mode::map_mode){
        isMap = true;
    }else{
        isMap = false;
    }


    solve(isQueue, isMap, start, map, backtrace);
}