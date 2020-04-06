#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <set>
#include <ctime>
#include <deque>

using namespace std;

class Tarjan {
public:
    Tarjan() {
        len = 0;
        result.resize(5);
    };

    ~Tarjan() = default;

    void generate_graph(string &file_path) {
        FILE *file = fopen(file_path.c_str(), "r");
        unsigned int u, v, c;

//        int cnt=0;
        while (fscanf(file, "%u,%u,%u", &u, &v, &c) != EOF) {
            graph[u].push_back(make_pair(v, c));
//            ++cnt;
        }

    }

    void findCycles() {
        int i = 0;
        for (auto &it : graph) {
//            if (i % 100 == 0) {
//            printf("%d/%d\n", i, graph.size());
//            }
            ++i;
            unsigned int vertex = it.first;
            findAllSimpleCycles(vertex, vertex, 0);
            visited.insert(vertex);
            markedStack.clear();
            markedSet.clear();
        }
    }

    bool findAllSimpleCycles(unsigned int start, unsigned int current, int depth) {
        bool hasCycle = false;
        pointStack.push_front(current);
        markedSet.insert(current);
        markedStack.push_front(current);
        bool has_adjacent = false;

        //if current have adjacent
        if (graph.count(current) > 0) {
            list<pair<unsigned int, int>> temp = graph.find(current)->second;
            for (auto &p : temp) {
                unsigned int adjacent = p.first;
                if (visited.count(adjacent) == 0) {
                    if (adjacent == start) {
                        hasCycle = true;
                        if (pointStack.size() > 2 && pointStack.size() < 8) {
                            map<unsigned int, vector<unsigned int>> cycle;
                            vector<unsigned int> sub_cycle;
                            //now pointStack is begin: 56 197 18 endx
                            //rbegin = 18, rend = 56 + 1
                            for (auto it = pointStack.rbegin(); it != pointStack.rend(); ++it) {
                                sub_cycle.push_back(*it);
                            }
                            cycle[pointStack.back()] = sub_cycle;
                            result[sub_cycle.size() - 3].push_back(cycle);
                            ++len;
                        }
                    } else if (markedSet.count(adjacent) == 0) {
                        hasCycle = findAllSimpleCycles(start, adjacent, depth + 1) || hasCycle;
                    }
                }
            }

        }
        if (hasCycle) {
            while (markedStack.front() != current) {
                unsigned int v = markedStack.front();
                markedStack.pop_front();
                markedSet.erase(v);
            }
            unsigned int v = markedStack.front();
            markedStack.pop_front();
            markedSet.erase(v);
        }
        pointStack.pop_front();
        return hasCycle;
    }

    void test_graph() {
        for (auto &it : graph) {
            unsigned int key = it.first;
            list<pair<unsigned int, int>> temp = it.second;
            cout << "vex: " << key << " ";
            for (auto &l : temp) {
                cout << "a " << l.first << " " << "w " << l.second << " ";
            }
            cout << endl;
        }
    }

    void output() {
        ofstream outfile("out.txt", ios::trunc);
        vector<vector<unsigned int>> all_res;
        outfile << len << endl;
        for (vector<map<unsigned int, vector<unsigned int>>> &all_sub_result : result) {
            for (map<unsigned int, vector<unsigned int>> &m_vec : all_sub_result) {
                for (auto &m_map : m_vec) {
//                    vector<unsigned int> temp = m_map.second;
                    for (auto &l : m_map.second) {
                        if (l == m_map.second.back()) {
                            outfile << l;
                        } else {
                            outfile << l << ",";
                        }
                    }
                    outfile << "\n";
                }
            }
        }
        outfile.close();
    }

private:
    set<unsigned int> visited, markedSet;
    deque<unsigned int> pointStack, markedStack;
    map<unsigned int, list<pair<unsigned int, int>>> graph;
    vector<vector<map<unsigned int, vector<unsigned int>>>> result;
    int len;
};

int main() {
    clock_t start, finish;
    start = clock();
    string data_path = R"(D:\hw\data\test_data.txt)";
    string linux_path = R"(/home/syj/Documents/hw/data/test_data.txt)";
    string huawei_path = R"(/root/hw/data/test_data.txt)";

    Tarjan tarjan;
    tarjan.generate_graph(data_path);
    tarjan.findCycles();
    tarjan.output();

    finish = clock();
    printf("%f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    return 0;
}
