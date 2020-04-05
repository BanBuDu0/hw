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
#include <deque>

using namespace std;

// #define CLOCKS_PER_SEC  ((clock_t)1000)


inline void split(const string &s, vector<long> &sv, const char flag = ' ') {
    sv.clear();
    istringstream iss(s);
    string temp;

    while (getline(iss, temp, flag)) {
        sv.push_back(stoi(temp));
    }
}

void generate_graph(string &file, map<long, list<pair<long, int>>> &g) {
    ifstream infile(file.data(), ios::in);

    string s;
    while (getline(infile, s)) {
        vector<long> temp;
        split(s, temp, ',');
        //        cout << "read: " << temp[0] << endl;
        g[temp[0]].push_back(make_pair(temp[1], temp[2]));
    }
    infile.close();
}

void test_graph(map<long, list<pair<long, int>>> &graph) {
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

    void findCycles(map<long, list<pair<long, int>>> &graph, vector<vector<map<long, vector<long>>>> &result) {
        for (auto &it : graph) {
            long vertex = it.first;
            findAllSimpleCycles(vertex, vertex, result, graph);
            visited.insert(vertex);
            while (!markedStack.empty()) {
                long v = markedStack.front();
                markedStack.pop_front();
                markedSet.erase(v);
            }
        }
    }

    bool findAllSimpleCycles(long start, long current, vector<vector<map<long, vector<long>>>> &result,
                             map<long, list<pair<long, int>>> &graph) {
        bool hasCycle = false;
        pointStack.push_front(current);
        markedSet.insert(current);
        markedStack.push_front(current);
        if (graph.count(current) > 0) {
            list<pair<long, int>> temp = graph.find(current)->second;
            for (auto &p : temp) {
                long w = p.first;
                if (visited.count(w) == 0) {
                    if (w == start) {
                        hasCycle = true;
                        if (pointStack.size() > 2 && pointStack.size() < 8) {
                            map<long, vector<long>> cycle;
                            vector<long> sub_cycle;
                            //now pointStack is begin: 56 197 18 end
                            //rbegin = 18, rend = 56 + 1
                            for (auto it = pointStack.rbegin(); it != pointStack.rend(); ++it) {
                                sub_cycle.push_back(*it);
                            }
                            cycle[pointStack.back()] = sub_cycle;
                            result[sub_cycle.size() - 3].push_back(cycle);
                        }
                    } else if (markedSet.count(w) == 0) {
                        hasCycle = findAllSimpleCycles(start, w, result, graph) || hasCycle;
                    }
                }
            }
        }

        if (hasCycle) {
            while (markedStack.front() != current) {
                long v = markedStack.front();
                markedStack.pop_front();
                markedSet.erase(v);
            }
            long v = markedStack.front();
            markedStack.pop_front();
            markedSet.erase(v);
        }
        pointStack.pop_front();
        return hasCycle;
    }

private:
    set<long> visited, markedSet;
    deque<long> pointStack, markedStack;
};

int main() {
    clock_t start, finish;
    start = clock();
    map<long, list<pair<long, int>>> graph;
    string data_path = R"(D:\code\leecode\src\com\huawei\data\test_data.txt)";
    string linux_path = R"(/home/syj/Documents/hw/data/test_data.txt)";
    string huawei_path = R"(/root/hw/data/test_data.txt)";
    generate_graph(data_path, graph);

    vector<vector<map<long, vector<long>>>> sub_result;

    sub_result.resize(5);

    Tarjan tarjan;
    tarjan.findCycles(graph, sub_result);

    //output to txt
    int len = 0;
    ofstream outfile("out.txt", ios::trunc);
    vector<vector<long>> all_res;
    for (vector<map<long, vector<long>>> &all_sub_result : sub_result) {
        for (map<long, vector<long>> &m_vec : all_sub_result) {
            for (auto &m_map : m_vec) {
                ++len;
                vector<long> temp = m_map.second;
                all_res.push_back(temp);
            }
        }
    }
    outfile << len << endl;
    for (auto &v:all_res) {
        for (auto &l : v) {
            if (l == v.back()) {
                outfile << l;
            } else {
                outfile << l << ",";
            }
        }
        outfile << "\n";
    }
    outfile.close();


    finish = clock();
    printf("%f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    return 0;
}
