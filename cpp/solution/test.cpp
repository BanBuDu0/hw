//
// Created by Sunyu on 2020/4/6.
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
#include <unordered_map>
#include <algorithm>

using namespace std;

class StronglyConnectedComponent {
public:
    StronglyConnectedComponent() {
        time = 0;
    }

    ~StronglyConnectedComponent() = default;

    void scc(map<unsigned int, list<pair<unsigned int, int>>> &graph) {
        for (auto &it : graph) {
            unsigned int vertex = it.first;
            if (visited.find(vertex) != visited.end()) {
                continue;
            }
            sccUtil(vertex, graph);
        }
    }

    void sccUtil(unsigned int vertex, map<unsigned int, list<pair<unsigned int, int>>> &graph) {
        visited.insert(vertex);
        visitedTime[vertex] = time;
        lowTime[vertex] = time;
        time++;
        stack.push_front(vertex);
        onStack.insert(vertex);

        for (auto &p : graph[vertex]) {
            unsigned int adjacent = p.first;
            if (visited.count(adjacent) != 0) {
                sccUtil(adjacent, graph);
                if (lowTime[adjacent] < lowTime[vertex]) {
                    lowTime[vertex] = lowTime[adjacent];
                }
            } else if (onStack.count(adjacent) != 0) {
                if (visitedTime[adjacent] < lowTime[vertex]) {
                    lowTime[vertex] = visitedTime[adjacent];
                }
            }
        }

        if (visitedTime[vertex] == lowTime[vertex]) {
            set<unsigned int> stronglyConnectedComponenet;
            unsigned int v;
            do {
                v = stack.front();
                stack.pop_front();
                onStack.erase(v);
                stronglyConnectedComponenet.insert(v);
            } while (vertex != v);
            result.push_back(stronglyConnectedComponenet);
        }

    }


    void output() {
        for (auto &s:result) {
            for (auto &v:s) {
                printf("%u ", v);
            }
            printf("\n");
        }
    }

private:
    map<unsigned int, int> visitedTime;
    map<unsigned int, int> lowTime;
    set<unsigned int> onStack;
    deque<unsigned int> stack;
    set<unsigned int> visited;
    vector<set<unsigned int>> result;
    int time;
};


class Tarjan {
public:
    Tarjan() {
        len = 0;
        result.resize(5);
    };

    ~Tarjan() = default;

    void generate_graph(string &file_path) {
        map<unsigned int, list<pair<unsigned int, int>>> g;
        FILE *file = fopen(file_path.c_str(), "r");
        unsigned int from, to, weight;
        while (fscanf(file, "%u,%u,%u", &from, &to, &weight) != EOF) {
            g[from].push_back(make_pair(to, weight));
        }
        graph = unordered_map<unsigned int, list<pair<unsigned int, int>>>(std::make_move_iterator(g.begin()),
                                                                           std::make_move_iterator(g.end()));
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
            marked.insert(vertex);
        }
    }

    void findAllSimpleCycles(unsigned int start, unsigned int current, int depth) {
        pointStack.push_front(current);
        visited.insert(current);

        for (auto &p : graph[current]) {
            //when depth=6, there 7 points in the pointStack
            //so just take care about the adjacent is start or not
            unsigned int adjacent = p.first;
            if (adjacent == start) {
                if (pointStack.size() > 2 && pointStack.size() < 8) {
                    map<unsigned int, vector<unsigned int>> cycle;
                    vector<unsigned int> sub_cycle;
                    //now pointStack is begin: 56 197 18 end:
                    //rbegin = 18, rend = 56 + 1
                    for (auto it = pointStack.rbegin(); it != pointStack.rend(); ++it) {
                        sub_cycle.push_back(*it);
                    }
                    cycle[pointStack.back()] = sub_cycle;
                    result[sub_cycle.size() - 3].push_back(cycle);
                    ++len;
                }
            } else if (visited.count(adjacent) == 0 && depth < 6 && adjacent > start) {
                findAllSimpleCycles(start, adjacent, depth + 1);
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
    unordered_map<unsigned int, list<pair<unsigned int, int>>> graph;
    vector<vector<map<unsigned int, vector<unsigned int>>>> result;
    int len;
};

int main() {
    clock_t start, finish;
    start = clock();
    string data_path = R"(D:\hw\data\test_data2.txt)";
    string linux_path = R"(/home/syj/Documents/hw/data/test_data.txt)";
    string huawei_path = R"(/root/hw/data/test_data.txt)";

    Tarjan tarjan;
    tarjan.generate_graph(data_path);
    finish = clock();
    printf("generate_graph: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    start = clock();
//    tarjan.test_graph();
    tarjan.findCycles();
    finish = clock();
    printf("findCycles: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    start = clock();
    tarjan.output();
//    map<unsigned int, list<pair<unsigned int, int>>> g = tarjan.getGraph();
//    StronglyConnectedComponent scc;
//    scc.scc(g);
//    scc.output();

    finish = clock();
    printf("output: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    return 0;
}

