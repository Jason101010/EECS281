//Project identifier: 292F24D17A4455C1B5133EDD8C7CEAA0C9570A98

#include <string>
#include <getopt.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <cmath>
#include <fstream>


using namespace std;

struct Options{
    bool verbose = false;
    string fileName = "";
};



struct Transaction{
    long long unsigned int time;
    string IP;
    string sender;
    string recipent;
    int amount;
    long long unsigned int dateExe;
    bool split;
    uint32_t id;
    int32_t fee;

    Transaction(int64_t t, string i, string s, string r, int a, int64_t d, bool b, uint32_t count)
    : time(t), IP(i), sender(s), recipent(r), amount(a), dateExe(d), split(b), id(count){};
};

struct User{
    long long unsigned int registration_date;
    uint32_t pin;
    int balance;
    unordered_set<string> ip_list;
    vector<Transaction*> incoming;
    uint32_t incoming_count = 0;
    vector<Transaction*> outgoing;
    uint32_t outgoing_count = 0;
};

class TransactionComp{
    public:
    bool operator()(Transaction* a, Transaction* b){
        if(a->dateExe > b->dateExe){
            return true;
        }
        if(a->dateExe == b->dateExe){
            if(a->id > b->id){
                return true;
            }
        }
        return false;
    }
};

class Bank{
    public:
        Bank(){

        }

        void fileRead(Options& option){
            ifstream ifs;
            ifs.open(option.fileName);
            string temp;
            uint32_t count = 0;
            while(!ifs.eof()){
                string s;
                string name;
                long long unsigned int temp;
                long long unsigned int time = 0;
                getline(ifs, s, ':');
                if(s == ""){
                    break;
                }
                temp = stoull(s) * 10000000000ULL;
                time += temp;
                getline(ifs, s, ':');
                temp = stoull(s) * 100000000ULL;
                time += temp;
                getline(ifs, s, ':');
                temp = stoull(s) * 1000000ULL;
                time += temp;
                getline(ifs, s, ':');
                temp = stoull(s) * 10000ULL;
                time += temp;
                getline(ifs, s, ':');
                temp = stoull(s) * 100ULL;
                time += temp;
                getline(ifs, s, '|');
                temp = stoull(s);
                time += temp;
                User u;
                u.registration_date = time;
                getline(ifs, s, '|');
                name = s;
                getline(ifs, s, '|');
                u.pin = stoi(s);
                ifs >> s;
                u.balance = stoi(s);
                users[name] = u;
                getline(ifs, s);
            }
            char c;
            cin >> temp;
            //while loop for commands
            while(temp != "$$$"){
                c = temp[0];
                if(c == '#'){
                    getline(cin, temp);
                }else if(c == 'l'){
                    string name, ip;
                    uint32_t password;
                    cin >> name;
                    cin >> password;
                    cin >> ip;

                    auto it = users.find(name);
                    if(it != users.end() && it->second.pin == password){
                        it->second.ip_list.insert(ip);
                        if(option.verbose){
                            cout << "User " << name << " logged in.\n";
                        }
                    }else{
                        if(option.verbose){
                            cout << "Failed to log in " << name << ".\n";
                        }
                    }
                }else if(c == 'p'){
                    string i, s, r;
                    long long unsigned int t, d;
                    char b;
                    int a;
                    t = get_time_int();
                    
                    
                    cin >> i;
                    cin >> s;
                    cin >> r;
                    cin >> a;
                    d = get_time_int();
                    cin >> b;
                    bool split_cost = true;
                    if(b == 'o'){
                        split_cost = false;
                    }
                    auto it = users.find(s);
                    auto it2 = users.find(r);
                    
                    if(d - t > 3000000ULL){
                        if(option.verbose){
                            cout << "Select a time less than three days in the future.\n";
                        }
                    }else if(it == users.end()){
                        if(option.verbose){
                            cout << "Sender "<< s << " does not exist.\n";
                        }
                    }else if(it2 == users.end()){
                        if(option.verbose){
                            cout << "Recipient "<< r << " does not exist.\n";
                        }
                    }else if(it->second.registration_date > d || it2->second.registration_date > d){
                        if(option.verbose){
                            cout << "At the time of execution, sender and/or recipient have not registered.\n";
                        }   
                    }else if(it->second.ip_list.empty()){
                        if(option.verbose){
                            cout << "Sender " << s << " is not logged in.\n";
                        }
                    }else if(it->second.ip_list.find(i) == it->second.ip_list.end()){
                        if(option.verbose){
                            cout << "Fraudulent transaction detected, aborting request.\n";
                        }
                    }else{
                        if(!transactions.empty()){
                            while(t >= transactions.top()->dateExe){
                                make_transaction(option);
                                if(transactions.empty()){
                                    break;
                                }
                            }
                        } 
                        Transaction * tempTrans = new Transaction(t, i, s, r, a, d, split_cost, count);
                        transactions.push(tempTrans);
                        if(option.verbose){
                            cout << "Transaction placed at " << t << ": $" << a << " from " << s << " to " << r << " at " << d << ".\n";
                        }
                        ++count;                        
                    }
                }else{
                    //for if temp == "out"
                    string name, ip;
                    cin >> name;
                    cin >> ip;
                    auto it = users.find(name);
                    if(it != users.end()){
                        auto it2  = it->second.ip_list.find(ip);
                        if(it2 != it->second.ip_list.end()){
                            it->second.ip_list.erase(it2);
                            if(option.verbose){
                                cout << "User " << name << " logged out.\n";
                            }
                        }else{
                            if(option.verbose){
                                cout << "Failed to log out " << name << ".\n";
                            }
                        }
                    }else{
                        if(option.verbose){
                            cout << "Failed to log out " << name << ".\n";
                        }   
                    }
                }
                cin >> temp;

            }
            while(!transactions.empty()){
                make_transaction(option);
            }
        
            //while loop for query
            while(cin >> temp){
                if(temp[0] == 'l'){
                    long long unsigned int time1, time2;
                    uint32_t counter = 0;
                    time1 = get_time_int();
                    time2 = get_time_int();
                    for(auto t : transactions_finished){
                        if(t->dateExe >= time1){
                            if(t->dateExe < time2){
                                cout << t->id << ": " << t->sender << " sent " << t->amount;
                                if(t->amount == 1){
                                    cout << " dollar to ";
                                }else{
                                    cout << " dollars to ";
                                }
                                cout << t->recipent << " at " << t->dateExe << ".\n";
                                ++counter;
                            }else{
                                break;
                            }
                        }
                    }
                    if(counter == 1){
                        cout << "There was ";
                    }else{
                        cout << "There were ";
                    }
                    cout << counter;
                    if(counter == 1){
                        cout << " transaction ";
                    }else{
                        cout << " transactions ";
                    }
                    cout << "that";
                    if(counter == 1){
                        cout << " was ";
                    }else{
                        cout << " were ";
                    }
                    cout << "placed between time " << time1 <<  " to " << time2 << ".\n";

                }else if(temp[0] == 'r'){
                    long long unsigned int time1, time2;
                    uint32_t revenue = 0;
                    time1 = get_time_int();
                    time2 = get_time_int();
                    for(auto t : transactions_finished){
                        if(t->dateExe >= time1){
                            if(t->dateExe < time2){
                               revenue += t->fee;
                            }else{
                                break;
                            }
                        }
                    }
                    int64_t time_passed = time2 - time1;
                    int64_t seconds = time_passed % 100;
                    time_passed /= 100;
                    int64_t minutes = time_passed % 100;
                    time_passed /= 100;
                    int64_t hours = time_passed % 100;
                    time_passed /= 100;
                    int64_t days = time_passed % 100;
                    time_passed /= 100;
                    int64_t months = time_passed % 100;
                    time_passed /= 100;
                    int64_t years = time_passed % 100;

                    cout << "281Bank has collected " << revenue  << " dollars in fees over";

                    if(years != 0){
                        if(years == 1){
                            cout << " " << years << " year";
                        }else{
                            cout << " " << years << " years";
                        }
                    }
                    if(months != 0){
                        if(months == 1){
                            cout << " " << months << " month";
                        }else{
                            cout << " " << months << " months";
                        }
                    }
                    if(days != 0){
                        if(days == 1){
                            cout << " " << days << " day";
                        }else{
                            cout << " " << days << " days";
                        }
                    }
                    if(hours != 0){
                        if(hours == 1){
                            cout << " " << hours << " hour";
                        }else{
                            cout << " " << hours << " hours";
                        }
                    }
                    if(minutes != 0){
                        if(minutes == 1){
                            cout << " " << minutes << " minute";
                        }else{
                            cout << " " << minutes << " minutes";
                        }
                    }
                    if(seconds != 0){
                        if(seconds == 1){
                            cout << " " << seconds << " second";
                        }else{
                            cout << " " << seconds << " seconds";
                        }
                    }
                    if(years + months + days + hours + minutes + seconds == 0){
                        cout << " ";
                    }
                    cout << ".\n";
                }else if(temp[0] == 'h'){
                    string user_id;
                    cin >> user_id;
                    auto it = users.find(user_id);
                    if(it == users.end()){
                        cout << "User " << user_id << " does not exist.\n";
                    }else{
                        cout << "Customer " << user_id << " account summary:\n";
                        cout << "Balance: $" << it->second.balance << "\n";
                        cout << "Total # of transactions: " << (it->second.incoming_count + it->second.outgoing_count) << "\n";
                        cout << "Incoming " << it->second.incoming_count << ":\n";
                        for(auto trans : it->second.incoming){
                            cout << trans->id << ": " << trans->sender << " sent " << trans->amount;
                            if(trans->amount == 1){
                                cout << " dollar "; 
                            }else{
                                cout << " dollars ";
                            }
                            cout << "to " << trans->recipent << " at " << trans->dateExe << ".\n";
                        }
                        cout << "Outgoing " << it->second.outgoing_count << ":\n";
                        for(auto trans : it->second.outgoing){
                            cout << trans->id << ": " << trans->sender << " sent " << trans->amount;
                            if(trans->amount == 1){
                                cout << " dollar "; 
                            }else{
                                cout << " dollars ";
                            }
                            cout << "to " << trans->recipent << " at " << trans->dateExe << ".\n";
                        }
                    }
                }else{
                    //for summarize day (s)
                    long long unsigned int timeS, timeF;
                    timeS = get_time_int() / 1000000ULL * 1000000ULL;
                    timeF = timeS + 1000000ULL;
                    int32_t numTrans = 0;
                    uint64_t revenue = 0;
                    cout << "Summary of [" << timeS << ", "  << timeF << "):\n";
                    for(auto trans : transactions_finished){
                        if(trans->dateExe >= timeS){
                            if(trans->dateExe < timeF){
                                cout << trans->id << ": " << trans->sender << " sent " << trans->amount;
                                if(trans->amount == 1){
                                    cout << " dollar "; 
                                }else{
                                    cout << " dollars ";
                                }
                                cout << "to " << trans->recipent << " at " << trans->dateExe << ".\n";
                                revenue += trans->fee;
                                ++numTrans;
                            }else{
                                break;
                            }
                        }
                    }
                    cout << "There ";
                    if(numTrans != 1){
                        cout << "were";
                    }else{
                        cout << "was";
                    }
                    cout << " a total of " << numTrans;
                    if(numTrans != 1){
                        cout << " transactions";
                    }else{
                        cout << " transaction";
                    }
                    cout << ", 281Bank has collected " << revenue << " dollars in fees.\n";
                }
            }
            for(auto i : transactions_finished){
                delete i;
            }
        }

    private:
    //string is the user's id/name, user is user
    unordered_map<string, User> users;
    //Reminder to creata a priority queue for transactions/queury list
    priority_queue<Transaction*, deque<Transaction*>, TransactionComp> transactions;
    deque<Transaction*> transactions_finished;

    long long unsigned int get_time_int(){
        string s;
        long long unsigned int temp;
        long long unsigned int time = 0;
        getline(cin, s, ':');
        temp = stoull(s) * 10000000000ULL;
        time += temp;
        getline(cin, s, ':');
        temp = stoull(s) * 100000000ULL;
        time += temp;
        getline(cin, s, ':');
        temp = stoull(s) * 1000000ULL;
        time += temp;
        getline(cin, s, ':');
        temp = stoull(s) * 10000ULL;
        time += temp;
        getline(cin, s, ':');
        temp = stoull(s) * 100ULL;
        time += temp;
        cin >> s;
        temp = stoull(s);
        time += temp;
        return time;
    }

    void make_transaction(Options& option){
        Transaction* temp;
        int costS, costR, fees;
        temp = transactions.top();
        costR = 0;
        if((temp->amount) / 100 < 10){
            fees = 10;
        }else if((temp->amount) / 100 > 450){
            fees = 450;
        }else{
            fees = temp->amount / 100;
        }
        if(temp->dateExe - users[temp->sender].registration_date > 50000000000ULL){
            fees = (fees * 3) / 4;
        }
        if(temp->split){
            costS = static_cast<int>(ceil(static_cast<double>(fees) / 2.0));
            costR = fees - costS;
        }else{
            costS = fees;
            costR = 0;
        }
        costS += temp->amount;
        if(users[temp->sender].balance < costS || users[temp->recipent].balance < costR){
            if(option.verbose){
                cout << "Insufficient funds to process transaction " << temp->id << ".\n";
            }
            transactions.pop();
            delete temp;
        }else{
            users[temp->sender].balance -= costS;
            users[temp->recipent].balance += temp->amount;
            users[temp->recipent].balance -= costR;
            if(option.verbose){
                cout << "Transaction executed at " << temp->dateExe << ": $" << temp->amount << " from " << temp->sender << " to " << temp->recipent << ".\n";
            }
            temp->fee = fees;
            users[temp->sender].outgoing_count += 1;
            users[temp->recipent].incoming_count += 1;
            if(users[temp->sender].outgoing.size() == 10){
                users[temp->sender].outgoing.erase(users[temp->sender].outgoing.begin());
            }
            users[temp->sender].outgoing.push_back(temp);
            if(users[temp->recipent].incoming.size() == 10){
                users[temp->recipent].incoming.erase(users[temp->recipent].incoming.begin());
            }
            users[temp->recipent].incoming.push_back(temp);
            transactions_finished.push_back(temp);
            transactions.pop();
        }
    }
};

void printHelp(){
    cout << "Include -v or --verbose to print out certain log messages.\n";
    cout << "Include -f <string> or --file <string> to use the included registration file\n"; 
    cout << "Include -h to get help\n";
}

void getMode(int argc, char * argv[], Options &options) {
    opterr = false;
    int choice;
    int index = 0;
    option long_options[] = {
        { "help", no_argument, nullptr, 'h'},
        { "verbose", no_argument, nullptr, 'v'},
        { "file", required_argument, nullptr, 'f'},
        { nullptr, 0, nullptr, '\0' }
    }; 
    while ((choice = getopt_long(argc, argv, "hvf:", long_options, &index)) != -1) {
        switch (choice) {
            case 'h':
                printHelp();
                exit(1);
            case 'v':
                options.verbose = true;
                break;
            case 'f':{
                string arg{optarg};
                if(arg == ""){
                    cerr << "No file included after including -f \n";
                    exit(1);
                }
                options.fileName = arg;
                break;
            }
            default:
                cerr << choice << endl;
                cerr << "Error: invalid option" << endl;
                exit(1);
        } 
    }
}  

int main(int argc, char *argv[]){
    ios_base::sync_with_stdio(false);
    Options option;
    getMode(argc, argv, option);
    Bank b;
    b.fileRead(option);
}

    
    