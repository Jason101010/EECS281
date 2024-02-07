// Project identifier: 9504853406CBAC39EE89AA3AD238AA12CA198043

#include "P2random.h"
#include <string>
#include <queue>
#include <deque>
#include <vector>
#include <getopt.h>
#include <iostream>

using namespace std;

struct Options{
    bool v_present = false;
    bool s_present = false;
    bool m_present = false;
    int stats = 0;
};

void printHelp(){
    cout << "Include -v or --verbose to print out extra messages during the operation of the program about zombies being created, moved, or destroyed.\n";
    cout << "Include -m or --median to print out extra messages during the program indicating the median time that zombies have been active before being destroyed\n ";
    cout << "Include -s <num> or --statistics <num> to print out <num> entries for each type of statistic\n"; 
    cout << "Include -h to get help\n";
}

void getMode(int argc, char * argv[], Options &options) {
    opterr = false;
    int choice;
    int index = 0;
    option long_options[] = {
        { "help", no_argument, nullptr, 'h'},
        { "verbose", no_argument, nullptr, 'v'},
        { "median", no_argument, nullptr, 'm'},
        { "statistics", required_argument, nullptr, 's'},
        { nullptr, 0, nullptr, '\0' }
    }; 
    while ((choice = getopt_long(argc, argv, "hvms:", long_options, &index)) != -1) {
        switch (choice) {
            case 'h':
                printHelp();
                exit(1);
            case 'v':
                options.v_present = true;
                break;
            case 'm':
                options.m_present = true;
                break;
            case 's':{
                string arg{optarg};
                options.s_present = true;
                options.stats = static_cast<int>(stoul(arg));
                break;
            }
            default:
                cerr << choice << endl;
                cerr << "Error: invalid option" << endl;
                exit(1);
        } 
    }
}  

struct Zombie{
    string name = "";
    uint32_t health = 0;
    uint32_t speed = 0;
    uint32_t distance = 0;
    uint32_t round = 0;
    uint32_t active = 0;
    uint32_t dofA = 0;

    Zombie(string n, uint32_t h, uint32_t s, uint32_t d, uint32_t r)
    : name{n}, health{h}, speed{s}, distance{d}, round{r}, active{1}{
        dofA = distance / speed + round;
    }
};

class ZombComp{
    //True when b has a greater priority than a
    public:
    bool operator()(Zombie* a, Zombie* b){
        uint32_t esta = a->dofA;
        uint32_t estb = b->dofA;
        if(esta > estb){
            return true;
        }   
        if(esta < estb){
            return false;
        }
        if(a->health > b->health){
            return true;
            }
        if(a->health < b->health){
            return false;
        }
        if(a->name > b->name){
            return true;
        }else{
            return false;
        }
    }
};

class LeastActive{
    public:
    bool operator()(Zombie* a, Zombie* b){
        if(a->active > b->active){
            return true;
        }   
        if(a->active < b->active){
            return false;
        }
        if(a->name > b->name){
            return true;
        }else{
            return false;
        }
    }
};

class MostActive{
    public:
    bool operator()(Zombie* b, Zombie* a){
        if(a->active > b->active){
            return true;
        }   
        if(a->active < b->active){
            return false;
        }
        if(a->name < b->name){
            return true;
        }else{
            return false;
        }
    }
};

void setup(uint32_t &capacity){
    string temp;
    getline(cin, temp);
    cin >> temp;
    cin >> temp;
    capacity = static_cast<uint32_t>(stoul(temp));
    cin >> temp;
    cin >> temp;
    uint32_t seed = static_cast<uint32_t>(stoul(temp));
    cin >> temp;
    cin >> temp;
    uint32_t distance = static_cast<uint32_t>(stoul(temp));
    cin >> temp;
    cin >> temp;
    uint32_t speed = static_cast<uint32_t>(stoul(temp));
    cin >> temp;
    cin >> temp;
    uint32_t health = static_cast<uint32_t>(stoul(temp));
    P2random::initialize(seed, distance, speed, health);
}

class ActCompareMax{
    public:
    bool operator()(uint32_t a, uint32_t b){
        if(b > a){
            return true;
        }
        return false;
    }
};

class ActCompareMin{
    public:
    bool operator()(uint32_t a, uint32_t b){
        if(a > b){
            return true;
        }
        return false;
    }
};

void add_to_med(uint32_t act, priority_queue<uint32_t, deque<uint32_t>, ActCompareMin> &medL, priority_queue<uint32_t, deque<uint32_t>, ActCompareMax> &meds){
    if(meds.size() == 0 || act < meds.top()){
        meds.push(act);
    }else{
        medL.push(act);
    }
    size_t sizeS = meds.size();
    size_t sizeL = medL.size();
    if(sizeS > sizeL + 1){
        medL.push(meds.top());
        meds.pop();
    }
    if(sizeL > sizeS + 1){
        meds.push(medL.top());
        medL.pop();
    }
}

void play(Options &options, uint32_t capacity){
    uint32_t arrows;
    deque<Zombie*> mz;
    deque<Zombie*> dead;
    priority_queue<Zombie*, deque<Zombie*>, ZombComp> pq;

    string temp;
    cin >> temp; //---
    cin >> temp; //round: 
    cin >> temp;
    uint32_t r_creation = static_cast<uint32_t>(stoul(temp));
    uint32_t round = 1;
    uint32_t zombies_random;
    uint32_t zombies_named;
    string name;
    uint32_t distance, speed, health;
    Zombie * z;
    Zombie * y;
    bool end = false;
    int reached = -1;
    int mz_num = 0;
    int stats = 0;
    uint32_t stats_2 = 0;
    priority_queue<uint32_t, deque<uint32_t>, ActCompareMin> medL;
    priority_queue<uint32_t, deque<uint32_t>, ActCompareMax> meds;

    while(!end || !pq.empty()){
        if(options.v_present){
            cout << "Round: " << round << "\n";
        }
        mz_num = 0;
        if(mz.size() != 0){
            for(auto i : mz){
                if(i->health != 0){
                    if(i->distance < i->speed){
                        i->distance = 0;
                    }else{
                        i->distance = i->distance - i->speed;
                    }
                    if(options.v_present){
                        cout << "Moved: " << i->name << " (distance: " << i->distance << ", speed: " << i->speed << ", health: " << i->health << ")\n"; 
                    }
                    i->active += 1;
                }
                if(i->distance == 0 && reached == -1){
                    reached = mz_num;
                }
                ++mz_num;
            }
        }
        if(mz.size() != 0 && reached != -1){
            Zombie* i = mz[reached];
            
            cout << "DEFEAT IN ROUND " << round << "! " << i->name << " ate your brains!\n";
            
            if(options.s_present){
                cout << "Zombies still active: " << pq.size() <<"\n";
                cout << "First zombies killed:\n";
                size_t siz = dead.size();
                for(size_t i = 0; i < min(siz,static_cast<size_t>(options.stats)); ++i){
                    cout << dead[i]->name << " " << i + 1 << "\n";
                }
                cout << "Last zombies killed:\n";
                if(static_cast<size_t>(options.stats) > siz){
                    stats = 0;
                    stats_2 = static_cast<uint32_t>(siz);
                }else{
                    stats = static_cast<int>(siz) - options.stats;
                    stats_2 = static_cast<uint32_t>(options.stats);
                }
                for(int i = static_cast<int>(siz) - 1; i >= stats; --i){
                    cout << dead[i]->name << " " << stats_2 << "\n";
                    --stats_2;
                }
                cout << "Most active zombies:\n";
                auto it1 = mz.begin();
                auto it2 = mz.end();
                priority_queue<Zombie*, deque<Zombie*>, MostActive> actH(it1, it2);
                int loops = min(static_cast<int>(actH.size()), options.stats);
                for(int i = 0; i < loops; ++i){
                        cout << actH.top()->name << " " << actH.top()->active << "\n";
                        actH.pop();
                }
                cout << "Least active zombies:\n";
                auto it3 = mz.begin();
                auto it4 = mz.end();
                priority_queue<Zombie*, deque<Zombie*>, LeastActive> actL(it3, it4);
                for(int i = 0; i < loops; ++i){
                    cout << actL.top()->name << " " << actL.top()->active << "\n";
                    actL.pop();
                }
            }
            for(auto i : mz){
                delete i;
            }
            return;
        }
        if(round == r_creation && !end){
            cin >> temp; // random-Zombies:
            cin >> temp;
            zombies_random = static_cast<uint32_t>(stoul(temp));
            for(uint32_t i = 0; i < zombies_random; ++i){
                name  = P2random::getNextZombieName();
	            distance = P2random::getNextZombieDistance();
	            speed    = P2random::getNextZombieSpeed();
	            health   = P2random::getNextZombieHealth();
                z =  new Zombie(name, health, speed, distance, round);
                if(options.v_present){
                    cout << "Created: " << name << " (distance: " << distance << ", speed: " << speed << ", health: " << health << ")\n";
                }
                mz.push_back(z);
                pq.push(z);
            }
            cin >> temp; // named-zombies:
            cin >> temp;
            zombies_named = static_cast<uint32_t>(stoul(temp));
            for(uint32_t i = 0; i < zombies_named; ++i){
                cin >> name;
                cin >> temp;
                cin >> distance;
                cin >> temp;
                cin >> speed;
                cin >> temp;
                cin >> health;
                z = new Zombie(name, health, speed, distance, round);
                if(options.v_present){
                    cout << "Created: " << name << " (distance: " << distance << ", speed: " << speed << ", health: " << health << ")\n";
                }
                mz.push_back(z);
                pq.push(z);
            }
            cin >> temp; //---
            if(temp != "---"){
                end = true;
            }
            cin >> temp; //round:
            cin >> r_creation;
        }
        
        arrows = capacity;
        while(arrows > 0 && !pq.empty()){
            if(!pq.empty()){
                y = pq.top();
                if(arrows >= y->health && y->health != 0){
                    arrows -= y->health;
                    y->health = 0;
                    dead.push_back(y);
                    add_to_med(y->active, medL, meds);
                    if(options.v_present){
                        cout << "Destroyed: " << y->name << " (distance: " << y->distance << ", speed: " << y->speed << ", health: " << y->health << ")\n"; 
                    }
                    pq.pop();
                }else{
                    y->health -= arrows;
                    arrows = 0;
                }
            }
            if(pq.empty() && end){
                if(options.m_present){
                    uint32_t median;
                    if(medL.size() > meds.size()){
                        median = medL.top();
                    }else if(meds.size() < medL.size()){
                        median = meds.top();
                    }else{
                        median = (meds.top() + medL.top()) / 2;
                    }
                    cout << "At the end of round " << round << ", the median zombie lifetime is " << median << "\n";
                }
                cout << "VICTORY IN ROUND " << round << "! " << dead.back()->name << " was the last zombie.\n";
                
                if(options.s_present){
                    cout << "Zombies still active: " << pq.size() <<"\n";
                    cout << "First zombies killed:\n";
                    size_t siz = dead.size();
                    for(size_t i = 0; i < min(siz,static_cast<size_t>(options.stats)); ++i){
                        cout << dead[i]->name << " " << i + 1 << "\n";
                    }
                    cout << "Last zombies killed:\n";
                    if(static_cast<size_t>(options.stats) > siz){
                        stats = 0;
                        stats_2 = static_cast<uint32_t>(siz);
                    }else{
                        stats = static_cast<int>(siz) - options.stats;
                        stats_2 = static_cast<uint32_t>(options.stats);
                    }
                    for(int i = static_cast<int>(siz) - 1; i >= stats; --i){
                        cout << dead[i]->name << " " << stats_2 << "\n";
                        --stats_2;
                    }
                    cout << "Most active zombies:\n";
                    auto it1 = mz.begin();
                    auto it2 = mz.end();
                    priority_queue<Zombie*, deque<Zombie*>, MostActive> actH(it1, it2);
                    int loops = min(static_cast<int>(actH.size()), options.stats);
                    for(int i = 0; i < loops; ++i){
                        cout << actH.top()->name << " " << actH.top()->active << "\n";
                        actH.pop();
                    }
                    cout << "Least active zombies:\n";
                    auto it3 = mz.begin();
                    auto it4 = mz.end();
                    priority_queue<Zombie*, deque<Zombie*>, LeastActive> actL(it3, it4);
                    for(int i = 0; i < loops; ++i){
                        cout << actL.top()->name << " " << actL.top()->active << "\n";
                        actL.pop();
                    }
                }
                for(auto i : mz){
                    delete i;
                }
                return;
            }
            
        }
        if(!dead.empty() && options.m_present){
            uint32_t median;
            if(medL.size() > meds.size()){
                median = medL.top();
            }else if(meds.size() < medL.size()){
                median = meds.top();
            }else{
                median = (meds.top() + medL.top()) / 2;
            }
            cout << "At the end of round " << round << ", the median zombie lifetime is " << median << "\n";

        }
    ++round; 
    }
}

int main(int argc, char *argv[]){
    ios_base::sync_with_stdio(false);
    Options options;
    getMode(argc, argv, options);
    uint32_t capacity;
    setup(capacity);
    play(options, capacity);
}
