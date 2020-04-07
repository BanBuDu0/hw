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

bool cmp(vector<unsigned int> a, vector<unsigned int> b) {
    if (a.size() == b.size()) {
        for (int i = 0; i < a.size(); i++) {
            if (a[i] == b[i]) {
                continue;
            } else {
                return a[i] < b[i];
            }
        }
    } else {
        return a.size() < b.size();
    }
    return a.size() < b.size();
}


class FindCycleSolution {
public:
    FindCycleSolution() {
        time = 0;
    }

    ~FindCycleSolution() = default;

    void generate_graph(string &file_path) {
        FILE *file = fopen(file_path.c_str(), "r");
        unsigned int from, to, weight;
        while (fscanf(file, "%u,%u,%u", &from, &to, &weight) != EOF) {
            graph[from].push_back(make_pair(to, weight));
            all_vertex.insert(from);
            all_vertex.insert(to);
        }
    }

    void scc() {
        for (auto &it : graph) {
            unsigned int vertex = it.first;
            if (scc_visited.count(vertex) != 0) {
                continue;
            }
            sccUtil(vertex);
        }
    }

    void sccUtil(unsigned int vertex) {
        scc_visited.insert(vertex);
        visitedTime[vertex] = time;
        lowTime[vertex] = time;
        time++;
        stack.push_front(vertex);
        onStack.insert(vertex);

        for (auto &p : graph[vertex]) {
            unsigned int adjacent = p.first;
            if (scc_visited.count(adjacent) == 0) {
                sccUtil(adjacent);
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
            if (stronglyConnectedComponenet.size() > 2) {
                scc_result.push_back(stronglyConnectedComponenet);
            }
        }
    }

    void print_scc() {
        for (auto &s:scc_result) {
            if (s.size() > 2) {
                for (auto &v:s) {
                    printf("%u ", v);
                }
                printf("\n");
            }
        }
    }

    void generate_sub_graph() {
        for (auto &sub_vertex : scc_result) {
            unordered_map<unsigned int, list<pair<unsigned int, int>>> temp_graph;
            for (auto &vertex : sub_vertex) {
                temp_graph[vertex] = graph[vertex];
            }
            subGraphs.push_back(temp_graph);
        }
    }

    void test_graph() {
        for (auto &g : subGraphs) {
            cout << "graph-----------------------------------------" << endl;
            for (auto &it : g) {
                unsigned int key = it.first;
                list<pair<unsigned int, int>> temp = it.second;
                cout << "vex: " << key << " ";
                for (auto &l : temp) {
                    cout << "a " << l.first << " " << "w " << l.second << " ";
                }
                cout << endl;
            }
        }
    }

    void findCyclesInScc() {
        int i = 0;
        for (auto &scc_vertex: scc_result) {
            int j = 0;
            for (auto &vertex: scc_vertex) {
                if (j < (scc_vertex.size() - 2)) {
                    findAllSimpleCycles(vertex, vertex, 0, i);
//                marked.insert(vertex);
                }
                ++j;
            }
            ++i;
        }
    }


    void findAllSimpleCycles(unsigned int start, unsigned int current, int depth, int subGraph_index) {
        pointStack.push_front(current);
        visited.insert(current);

        for (auto &p : subGraphs[subGraph_index][current]) {
            //when depth=6, there 7 points in the pointStack
            //so just take care about the adjacent is start or not
            unsigned int adjacent = p.first;
            if (adjacent == start) {
                if (pointStack.size() > 2 && pointStack.size() < 8) {
                    string temp;
                    vector<unsigned int> cycle;
                    for (auto it = pointStack.rbegin(); it != pointStack.rend(); ++it) {
                        cycle.push_back(*it);
                    }
                    result.push_back(cycle);
                }
            } else if (visited.count(adjacent) == 0 && depth < 6 && adjacent > start) {
                findAllSimpleCycles(start, adjacent, depth + 1, subGraph_index);
            }
        }
        visited.erase(current);
        pointStack.pop_front();
    }


    void output() {
        ofstream outfile("out.txt", ios::trunc);
        vector<vector<unsigned int>> all_res;
        outfile << result.size() << endl;
        sort(result.begin(), result.end(), cmp);
        for (auto &cycle:result) {
            for (auto &vertex: cycle) {
                if (vertex == cycle.back()) {
                    outfile << vertex;
                } else {
                    outfile << vertex << ",";
                }
            }
            outfile << "\n";
        }
        outfile.close();
    }

private:
    unordered_map<unsigned int, int> visitedTime;
    unordered_map<unsigned int, int> lowTime;
    set<unsigned int> onStack;
    deque<unsigned int> stack;
    set<unsigned int> scc_visited;
    vector<set<unsigned int>> scc_result;
    int time;

    set<unsigned int> visited;
    deque<unsigned int> pointStack;
    set<unsigned int> all_vertex;
    unordered_map<unsigned int, list<pair<unsigned int, int>>> graph;
    vector<unordered_map<unsigned int, list<pair<unsigned int, int>>>> subGraphs;
    vector<vector<unsigned int>> result;
};

int main() {
    clock_t start, finish;
    start = clock();
    string data_path = R"(D:\hw\data\test_data.txt)";
    string linux_path = R"(/home/syj/Documents/hw/data/test_data.txt)";
    string huawei_path = R"(/root/hw/data/test_data.txt)";

    //生成数据
    FindCycleSolution solution;
    solution.generate_graph(data_path);
    finish = clock();
    printf("generate_graph: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    start = clock();

    //split scc
    solution.scc();
    solution.generate_sub_graph();
    finish = clock();
    printf("splic scc: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    start = clock();

    //find cycle
    solution.findCyclesInScc();
    finish = clock();
    printf("findCycles: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    start = clock();

    //output
    solution.output();
    finish = clock();
    printf("output: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);

    return 0;
}

