#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <stack>
#include <chrono>
#include <ctime>

using namespace std;

// #define CLOCKS_PER_SEC  ((clock_t)1000)


void split(const string &s, vector<long> &sv, const char flag = ' ') {
    sv.clear();
    istringstream iss(s);
    string temp;

    while (getline(iss, temp, flag)) {
        sv.push_back(stoi(temp));
    }
}

void generate_graph(const string &file, map<long, list<pair<long, int>>> &g) {
    ifstream infile;
    infile.open(file.data());

    string s;
    while (getline(infile, s)) {
        vector<long> temp;
        split(s, temp, ',');
        //        cout << "read: " << temp[0] << endl;
        g[temp[0]].push_back(make_pair(temp[1], temp[2]));
    }
    infile.close();
}

void test_graph(const map<long, list<pair<long, int>>> &graph) {
    for (auto &it : graph) {
        long key = it.first;
        list<pair<long, int>> temp = it.second;
        cout << "vex: " << key << " ";
        for (auto &l : temp) {
            cout << "a " << l.first << " " << "w " << l.second << " ";
        }
        cout << endl;
    }
}

class Tarjan {
public:
    Tarjan() = default;

    ~Tarjan() = default;

    void findCycles(const map<long, list<pair<long, int>>> &graph, vector<vector<long> > &result) {
        for (auto &it : graph) {
            long vertex = it.first;
            findAllSimpleCycles(vertex, vertex, result, graph);
            visited.insert(vertex);
            while (!markedStack.empty()) {
                long v = markedStack.top();
                markedStack.pop();
                markedSet.erase(v);
            }
        }
    }

    bool findAllSimpleCycles(long start, long current, vector<vector<long> > &result,
                             const map<long, list<pair<long, int>>> &graph) {
        bool hasCycle = false;
        pointStack.push(current);
        markedSet.insert(current);
        markedStack.push(current);
        if (graph.count(current) > 0) {
            list<pair<long, int>> temp = graph.find(current)->second;
            for (auto &p : temp) {
                long w = p.first;
                if (visited.count(w) == 0) {
                    if (w == start) {
                        hasCycle = true;
                        if (pointStack.size() > 2 && pointStack.size() < 8) {
                            vector<long> cycle;
                            stack<long> temp_stack;
                            while (!pointStack.empty()) {
                                temp_stack.push(pointStack.top());
                                pointStack.pop();
                            }
                            while (!temp_stack.empty()) {
                                cycle.push_back(temp_stack.top());
                                pointStack.push(temp_stack.top());
                                temp_stack.pop();
                            }
                            result.push_back(cycle);
                        }
                    } else if (markedSet.count(w) == 0) {
                        hasCycle = findAllSimpleCycles(start, w, result, graph) || hasCycle;
                    }
                }
            }
        }

        if (hasCycle) {
            while (markedStack.top() != current) {
                long v = markedStack.top();
                markedStack.pop();
                markedSet.erase(v);
            }
            long v = markedStack.top();
            markedStack.pop();
            markedSet.erase(v);
        }
        pointStack.pop();
        return hasCycle;
    }

private:
    set<long> visited, markedSet;
    stack<long> pointStack, markedStack;
};

void print_result(const vector<vector<long>> &result) {
    int len = result.size();
    printf("%d\n", len);
    for (auto &v : result) {
        for (auto &l : v) {
            printf("%ld ", l);
        }
        printf("\n");
    }
}


int main() {
    clock_t start, finish;
    start = clock();
    map<long, list<pair<long, int>>> graph;
    string data_path = R"(D:\code\leecode\src\com\huawei\data\test_data.txt)";
    string linux_path = R"(/home/syj/Documents/hw/data/test_data.txt)";
    generate_graph(linux_path, graph);
    vector<vector<long>> result;
    Tarjan tarjan;
    tarjan.findCycles(graph, result);
    finish = clock();
    print_result(result);
    printf("%f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    return 0;
}
