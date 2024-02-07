//Project identifier: 3E33912F8BAA7542FC4A1585D2DB6FE0312725B9

#include <string>
#include <getopt.h>
#include <iostream>
#include <algorithm>
#include <vector>
#include <limits>
#include <cmath>
#include <iomanip>

using namespace std;

struct Cage{
    double smallest_distance = numeric_limits<double>::infinity();
    int x;
    int y;
    int parent;
    bool included = false;
};

struct Vertice{
    int x;
    int y;
};


void printHelp(){
    cout << "Include -m or --mode {MST|FASTTSP|OPTTSP} to get a minimun spanning tree, a fast solution to TSP or the optimal solution to TSP \n";
    cout << "Include -h to get help\n";
}

void getMode(int argc, char * argv[], string &mode) {
    opterr = false;
    int choice;
    int index = 0;
    option long_options[] = {
        { "help", no_argument, nullptr, 'h'},
        { "mode", required_argument, nullptr, 'm'},
        { nullptr, 0, nullptr, '\0' }
    }; 
    while ((choice = getopt_long(argc, argv, "hm:", long_options, &index)) != -1) {
        switch (choice) {
            case 'h':
                printHelp();
                exit(1);
            case 'm':{
                string arg{optarg};
                if(arg == ""){
                    cerr << "No mode included after including -m \n";
                    exit(1);
                }
                mode = arg;
                break;
            }
            default:
                cerr << choice << endl;
                cerr << "Error: invalid option" << endl;
                exit(1);
        } 
    }
}  

double calcDist(Cage* a, Cage* b, bool isA = false){
    double d;
    if(isA){
        if((a->x < 0 && a->y < 0) && !(b->x <= 0 && b->y <= 0)){
            return numeric_limits<double>::infinity();
        }
        if((b->x < 0 && b->y < 0) && !(a->x <= 0 && a->y <= 0)){
            return numeric_limits<double>::infinity();
        }
    }
    d = sqrt(static_cast<double>(a->x - b->x) * static_cast<double>(a->x - b->x) + static_cast<double>(a->y - b->y) * static_cast<double>(a->y - b->y));
    return d;
}


double getMST(const vector<Cage*>& cages, bool partA = true){
    cages.front()->smallest_distance = 0;
    cages.front()->parent = -1;
    size_t count = 1;
    double weight = 0;
    double min = numeric_limits<double>::infinity();
    int ind = 0;
    int ind2 = 0;
    while(count < cages.size()){
        cages[ind]->included = true;
        ind2 = 0;
        min = numeric_limits<double>::infinity();
        for(size_t i = 0; i < cages.size(); ++i){
            Cage* it = cages[i];
            if(!it->included){
                double dist = 0;
                if(partA){
                    dist = calcDist(cages[ind], it, true);

                }else{
                    dist = calcDist(cages[ind], it);
                }
                if(dist < it->smallest_distance){
                    it->smallest_distance = dist;
                    it->parent = ind;
                }
                if(it->smallest_distance < min){
                    min = it->smallest_distance;
                    ind2 = static_cast<int>(i);
                }
            }
        }
        if(min == numeric_limits<double>::infinity()){
            cerr << "Cannot construct MST";
            for(auto i : cages){
                delete i;
            }
            exit(1);
        }
        weight += min;
        ind = ind2;
        ++count;
    }
    if(partA){
        cout << weight << "\n";
        for(size_t i = 1; i < cages.size(); ++i){
            int first = static_cast<int>(i);
            int second = static_cast<int>(cages[i]->parent);
            if(first <= second){
                cout << first << " " << second << "\n";
            }else{
                cout << second << " " << first << "\n";
            }
        }
    }
    return weight;
}

void getFastTSP(const vector<Cage*>& cages){
    vector<size_t> path(cages.size());
    double weight = 0;
    cages[0]->included = true;
    cages[1]->included = true;
    path[0] = 1; //path[ind] = val; ind = position of vertex in path, value represents the ind of the connected vertex
    path[1] = 0;
    weight += 2 * calcDist(cages[0], cages[1]);
    size_t ind = 2;
    double min_edge;
    size_t min_edge_ind;
    double d1;
    Cage* temp;
    size_t temp2;
    while(ind < cages.size()){
        temp = cages[ind];
        min_edge = numeric_limits<double>::infinity();
        d1 = 0;
        for(size_t i = 0; i < ind; ++i){
            d1 = calcDist(cages[i], temp) + calcDist(temp, cages[path[i]]) - calcDist(cages[i], cages[path[i]]);
            if(d1 < min_edge){
                min_edge = d1;
                min_edge_ind = i;
            }
        }
        temp2 = path[min_edge_ind];
        path[min_edge_ind] = ind;
        path[ind] = temp2;
        weight += min_edge;
        ++ind;
    }
    cout << weight << "\n";
    size_t output = 0;
    cout << output << " ";
    for(size_t i = 0; i < path.size() - 1; ++i){
        cout << path[output] << " ";
        output = path[output];
    }
}

pair<double,vector<int>> getFastTSP2(const vector<Cage*>& cages){
    vector<int> path;
    double weight = 0;
    path.push_back(0);
    cages[0]->included = true;
    path.push_back(1);
    weight += 2 * calcDist(cages[0], cages[1]);
    cages[1]->included = true;
    path.push_back(0);
    int ind = 2;
    double min_edge;
    size_t min_edge_ind;
    double d1;
    Cage* temp;

    while(path.size() < cages.size() + 1){
        temp = cages[ind];
        min_edge = numeric_limits<double>::infinity();
        d1 = 0;
        for(size_t i = 0; i < path.size() - 1; ++i){
            d1 = calcDist(cages[path[i]], temp) + calcDist(temp, cages[path[i + 1]]) - calcDist(cages[path[i]], cages[path[i + 1]]);
            if(d1 < min_edge){
                min_edge = d1;
                min_edge_ind = i + 1;
            }
        }
        auto iter = path.begin() + min_edge_ind;
        path.insert(iter, ind);
        weight += min_edge;
        ++ind;
    }
    path.pop_back();
    return make_pair(weight, path);
}

class OptTSP{
    public:
    OptTSP(vector<Cage*>& cages){
        auto temp = getFastTSP2(cages);
        path = temp.second;
        path_best = temp.second;
        nodes = &cages;
        curCost = 0;
        cost_best = temp.first;
    }

    void genPerms(size_t permLength) {
        double d;
        if (permLength == path.size()) {
            d = calcDist((*nodes)[path[permLength - 1]], (*nodes)[path[0]]);
            curCost += d;
            if(curCost < cost_best){
                cost_best = curCost;
                path_best = path;
            }        
            curCost -= d;
            return;
        }  // if ..complete path

        if (!promising(permLength)) {
            return;
        }  // if ..not promising

        for (size_t i = permLength; i < path.size(); ++i) {
            swap(path[permLength], path[i]);
            d = calcDist((*nodes)[path[permLength - 1]], (*nodes)[path[permLength]]);
            curCost += d;
            genPerms(permLength + 1);
            curCost -= d;
            swap(path[permLength], path[i]);
        }  // for ..unpermuted elements
    }  // genPerms()

    void output(){
        genPerms(1);
        cout << cost_best << "\n";
        for(auto i : path_best){
            cout << i << " ";
        }
    }

    private:
    vector<int> path;
    vector<int> path_best;
    vector<Cage*>* nodes;
    double curCost;
    double cost_best;
    bool promising(size_t curPermLength){
        size_t k = path.size() - curPermLength;
        if(k < 5){
            return true;
        }
        double estimate, dist;
        double edge1 = numeric_limits<double>::infinity();
        double edge2 = numeric_limits<double>::infinity();
        vector<Cage*> temp;
        temp.reserve(k);
        for(size_t i = curPermLength; i < path.size(); ++i){
            Cage* c =  (*nodes)[path[i]];
            c->included = false;
            c->smallest_distance = numeric_limits<double>::infinity();
            temp.push_back(c);
            dist = calcDist((*nodes)[path[0]], c);
            if(dist < edge1){
                edge1 = dist;
            }
            dist = calcDist((*nodes)[path[curPermLength - 1]], c);
            if(dist < edge2){
                edge2 = dist;
            }
        }
        estimate = curCost + edge1 + edge2 + getMST(temp, false);
        if(estimate < cost_best){
            return true;
        }
        return false;
    }

};

void getOptTSP(vector<Cage*>& cages){
    OptTSP temp = OptTSP(cages);
    temp.output();
}

void readRun(string& mode){
    uint64_t size;
    uint64_t count = 0;
    cin >> size;
    vector<Cage*> cages;
    cages.reserve(size);
    int num;
    while(count < size){
        Cage* temp = new Cage();
        cin >> num;
        temp->x = num;
        cin >> num;
        temp->y = num;
        cages.push_back(temp);
        ++count;
    }
    if(mode == "MST"){
        getMST(cages);
    }else if(mode == "FASTTSP"){
        getFastTSP(cages);
    }else{
        getOptTSP(cages);
    }
    for(auto i : cages){
        delete i;
    }
}

int main(int argc, char *argv[]){
    ios_base::sync_with_stdio(false);
    cout << std::setprecision(2); //Always show 2 decimal places
    cout << std::fixed; 
    string mode;
    getMode(argc, argv, mode);
    readRun(mode);
}
