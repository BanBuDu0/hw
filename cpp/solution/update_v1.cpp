//
// Created by Sunyu on 2020/4/6.
// 指定了递归调用的深度最大为7层，同时优化了一些递归时图查找
//

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
        unsigned int from, to, weight;

        while (fscanf(file, "%u,%u,%u", &from, &to, &weight) != EOF) {
            graph[from].push_back(make_pair(to, weight));
        }
        end = graph.end();
    }

    void findCycles() {
        int i = 0;
        int size = graph.size();
        for (auto &it : graph) {
            if (i % 100 == 0) {
                printf("%d/%d\n", i, size);
            }
            ++i;
            unsigned int vertex = it.first;
            findAllSimpleCycles(vertex, vertex, 0);
//            marked.insert(vertex);
        }
    }

    void findAllSimpleCycles(unsigned int start, unsigned int current, int depth) {
        pointStack.push_front(current);
        visited.insert(current);

        //if current have adjacent
        auto adjacent_list = graph.find(current);
        if (adjacent_list != end) {
            for (auto &p : adjacent_list->second) {
                unsigned int adjacent = p.first;
//                if (marked.count(adjacent) == 0) {
                if (adjacent == start) {
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
                } else if (depth < 7 && visited.count(adjacent) == 0 && adjacent > start) {
                    findAllSimpleCycles(start, adjacent, depth + 1);
                }
//                }
            }
        }
        visited.erase(current);
        pointStack.pop_front();
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
    set<unsigned int> visited, marked;
    deque<unsigned int> pointStack;
    map<unsigned int, list<pair<unsigned int, int>>> graph;
    map<unsigned int, list<pair<unsigned int, int>>>::iterator end;
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

