//
// Created by syj on 2020/4/11.
// 改成了marked，修改了result的结构
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
#include <thread>

#pragma comment(linker, "/STACK:10000000")

using namespace std;

#define DEBUG

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
        total_subGraph = 0;
        total_vertex = 0;
        cycle_num1 = 0, cycle_num2 = 0;
    }

    ~FindCycleSolution() = default;

    void generate_graph(string &file_path) {
        FILE *file = fopen(file_path.c_str(), "r");
        unsigned int from, to, weight;
        vector<pair<unsigned int, unsigned int>> input_pair;
        while (fscanf(file, "%u,%u,%u", &from, &to, &weight) != EOF) {
            input_pair.emplace_back(from, to);
            vertex_set1.push_back(to);
            vertex_set1.push_back(from);
        }
        sort(vertex_set1.begin(), vertex_set1.end());
        vertex_set1.erase(unique(vertex_set1.begin(), vertex_set1.end()), vertex_set1.end());

        for (auto &vertex : vertex_set1) {
            vertex_hash[vertex] = total_vertex++;
        }

        vertex_set2.resize(total_vertex);
        copy(vertex_set1.begin(), vertex_set1.end(), vertex_set2.begin());

        int f, t;
        graph1.resize(total_vertex);
        graph2.resize(total_vertex);
        reverse_graph1.resize(total_vertex);
        reverse_graph2.resize(total_vertex);
        for (int i = 0; i < total_vertex; i++) {
            reverse_graph1[i].resize(total_vertex, false);
            reverse_graph2[i].resize(total_vertex, false);
        }

        for (auto &pair : input_pair) {
            f = vertex_hash[pair.first], t = vertex_hash[pair.second];
            graph1[f].push_back(t);
            graph2[f].push_back(t);
            reverse_graph1[t][f] = true;
            reverse_graph2[t][f] = true;
        }
    }

    void scc() {
        scc_visited.resize(total_vertex);
        onStack.resize(total_vertex);
        for (int vertex = 0; vertex < total_vertex; ++vertex) {
            if (scc_visited[vertex] == true) {
                continue;
            }
            sccUtil(vertex);
        }
    }

    void cutGraph() {
        for (auto &i : scc_result1) {
            for (auto &vertex : i) {
                vector<int> temp_adj;
                for (auto &adj : graph1[vertex]) {
                    if (i.count(adj) != 0) {
                        temp_adj.push_back(adj);
                    }
                }
                graph1[vertex] = temp_adj;
                for (auto &adj : graph2[vertex]) {
                    if (i.count(adj) != 0) {
                        temp_adj.push_back(adj);
                    }
                }
                graph2[vertex] = temp_adj;
            }
        }
        for (auto &i : scc_result2) {
            for (auto &vertex : i) {
                vector<int> temp_adj;
                for (auto &adj : graph1[vertex]) {
                    if (i.count(adj) != 0) {
                        temp_adj.push_back(adj);
                    }
                }

                graph1[vertex] = temp_adj;
                for (auto &adj : graph2[vertex]) {
                    if (i.count(adj) != 0) {
                        temp_adj.push_back(adj);
                    }
                }
                graph2[vertex] = temp_adj;
            }
        }
    }

    void sccUtil(int vertex) {
        scc_visited[vertex] = true;
        visitedTime[vertex] = time;
        lowTime[vertex] = time;
        time++;
        stack.emplace_front(vertex);
        onStack[vertex] = true;

        for (auto &adjacent : graph1[vertex]) {
            if (scc_visited[adjacent] == false) {
                sccUtil(adjacent);
                if (lowTime[adjacent] < lowTime[vertex]) {
                    lowTime[vertex] = lowTime[adjacent];
                }
            } else if (onStack[adjacent] == true) {
                if (visitedTime[adjacent] < lowTime[vertex]) {
                    lowTime[vertex] = visitedTime[adjacent];
                }
            }
        }

        if (visitedTime[vertex] == lowTime[vertex]) {
            set<int> stronglyConnectedComponent;
            int v;
            do {
                v = stack.front();
                stack.pop_front();
                onStack[v] = false;
                stronglyConnectedComponent.insert(v);
            } while (vertex != v);
            if (stronglyConnectedComponent.size() > 2) {
                if (total_subGraph % 2 == 0) {
                    scc_result1.emplace_back(stronglyConnectedComponent);
                } else {
                    scc_result2.emplace_back(stronglyConnectedComponent);
                }
                total_subGraph++;
            }
        }
    }

    void findCycles() {
        thread thread1(&FindCycleSolution::findCycleInSccThread1, this);
        thread thread2(&FindCycleSolution::findCycleInSccThread2, this);
        thread1.join();
        thread1.join();
    }

    void findCycleInSccThread1() {
        for (auto &sub : scc_result1) {
            for (auto &vertex : sub) {
                findAllSimpleCycles1(vertex, vertex, 0);
            }
        }
    }

    void findCycleInSccThread2() {
        for (auto &sub : scc_result2) {
            for (auto &vertex : sub) {
                findAllSimpleCycles2(vertex, vertex, 0);
            }
        }
    }

    void findAllSimpleCycles1(int start, int current, int depth) {
        pointStack1.push_front(current);
        visited1[current] = true;

        for (auto &adjacent : graph1[current]) {
            if (adjacent == start) {
                if (depth > 1) {
                    vector<unsigned int> cycle = vector<unsigned int>(depth + 1);
                    int i = 0;
                    for (auto it = pointStack1.rbegin(); it != pointStack1.rend(); ++it) {
                        cycle[i] = vertex_set1[*it];
                        ++i;
                    }
                    result1[depth - 2].push_back(cycle);
                    cycle_num1++;
                }
            } else if (visited1[adjacent] == false && adjacent > start) {
                if (depth < 5) {
                    findAllSimpleCycles1(start, adjacent, depth + 1);
                } else if (depth == 5) {
                    if (reverse_graph1[start][adjacent]) {
                        vector<unsigned int> cycle = vector<unsigned int>(depth + 2);
                        int i = 0;
                        for (auto it = pointStack1.rbegin(); it != pointStack1.rend(); ++it) {
                            cycle[i] = vertex_set1[*it];
                            ++i;
                        }
                        cycle[i] = adjacent;
                        result1[depth - 1].push_back(cycle);
                        cycle_num1++;
                    }
                }
            }
        }
        visited1[current] = false;
        pointStack1.pop_front();
    }

    void findAllSimpleCycles2(int start, int current, int depth) {
        pointStack2.push_front(current);
        visited2[current] = true;

        for (auto &adjacent : graph2[current]) {
            if (adjacent == start) {
                if (depth > 1) {
                    vector<unsigned int> cycle = vector<unsigned int>(depth + 1);
                    int i = 0;
                    for (auto it = pointStack2.rbegin(); it != pointStack2.rend(); ++it) {
                        cycle[i] = vertex_set2[*it];
                        ++i;
                    }
                    result2[depth - 2].push_back(cycle);
                    cycle_num2++;
                }
            } else if (visited2[adjacent] == false && adjacent > start) {
                if (depth < 5) {
                    findAllSimpleCycles2(start, adjacent, depth + 1);
                } else if (depth == 5) {
                    if (reverse_graph2[start][adjacent]) {
                        vector<unsigned int> cycle = vector<unsigned int>(depth + 2);
                        int i = 0;
                        for (auto it = pointStack2.rbegin(); it != pointStack2.rend(); ++it) {
                            cycle[i] = vertex_set2[*it];
                            ++i;
                        }
                        cycle[i] = adjacent;
                        result2[depth - 1].push_back(cycle);
                        cycle_num2++;
                    }
                }
            }
        }
        visited2[current] = false;
        pointStack2.pop_front();
    }

    void output(string &path) {
        FILE *file = fopen(path.c_str(), "w");
        fprintf(file, "%d\n", (cycle_num1 + cycle_num2));
        for (auto i : result1) {
            sort(i.begin(), i.end(), cmp);
            for (auto &cycle : i) {
                fprintf(file, "%u", cycle[0]);
                for (int j = 1; j < cycle.size(); ++j) {
                    fprintf(file, ",%u", cycle[j]);
                }
                fprintf(file, "\n");
            }
        }
        fclose(file);
    }

private:
    unordered_map<int, int> visitedTime;
    unordered_map<int, int> lowTime;
    vector<bool> onStack;
    deque<int> stack;
    vector<bool> scc_visited;
    int time, total_subGraph;

    vector<set<int>> scc_result1, scc_result2;
    vector<bool> visited1, visited2;
    deque<int> pointStack1, pointStack2;
    vector<vector<vector<unsigned int>>> result1, result2;
    int cycle_num1, cycle_num2;

    vector<unsigned int> vertex_set1, vertex_set2;
    unordered_map<unsigned int, int> vertex_hash;
    vector<vector<int>> graph1, graph2;
    vector<vector<bool>> reverse_graph1, reverse_graph2;
    int total_vertex;
};

int main() {
#ifdef DEBUG
    clock_t start, finish;
    start = clock();
    string data_path = R"(D:\hw\data\test_data.txt)";
    string linux_path = R"(/home/syj/Documents/hw/data/test_data.txt)";
    string huawei_path = R"(/root/hw/data/test_data2.txt)";
    string o = "result.txt";
#endif
    string iPath = "/data/test_data.txt";
    string oPath = "/projects/student/result.txt";

    //生成数据
    FindCycleSolution solution;
    solution.generate_graph(data_path);
#ifdef DEBUG
    finish = clock();
    printf("generate_graph: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    start = clock();
#endif

    //split scc
    solution.scc();
    solution.cutGraph();
#ifdef DEBUG
    finish = clock();
    printf("splic scc: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    start = clock();
#endif

    //find cycle
    solution.findCycles();
#ifdef DEBUG
    finish = clock();
    printf("findCycles: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    start = clock();
#endif
    //output
    solution.output(o);
#ifdef DEBUG
    finish = clock();
    printf("output: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
//    system("pause");
#endif
    return 0;
}
