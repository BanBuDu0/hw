//
// Created by syj on 2020/4/13.
// 修改了result的结构，做cutGraph
// 11.15
//
#include <iostream>
#include <list>
#include <vector>
#include <string>
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
    for (int i = 0; i < a.size(); i++) {
        if (a[i] == b[i]) {
            continue;
        } else {
            return a[i] < b[i];
        }
    }
}

class FindCycleSolution {
public:
    FindCycleSolution() {
        time = 0;
        total_vertex = 0;
        result1.resize(5);
        result2.resize(5);
        cycle_num1 = 0;
        cycle_num2 = 0;
    }

    ~FindCycleSolution() = default;

    void generate_graph(string &file_path) {
        FILE *file = fopen(file_path.c_str(), "r");
        unsigned int from, to, weight;
        vector<pair<unsigned int, unsigned int>> input_pair;
        while (fscanf(file, "%u,%u,%u", &from, &to, &weight) != EOF) {
            input_pair.emplace_back(from, to);
            vertex_set.push_back(to);
            vertex_set.push_back(from);
        }
        sort(vertex_set.begin(), vertex_set.end());
        vertex_set.erase(unique(vertex_set.begin(), vertex_set.end()), vertex_set.end());

        for (auto &vertex : vertex_set) {
            vertex_hash[vertex] = total_vertex++;
        }

        int f, t;
        graph.resize(total_vertex);
        reverse_graph.resize(total_vertex);
        for (int i = 0; i < total_vertex; i++) {
            reverse_graph[i].resize(total_vertex, false);
        }

        for (auto &pair : input_pair) {
            f = vertex_hash[pair.first], t = vertex_hash[pair.second];
            graph[f].push_back(t);
            reverse_graph[t][f] = true;
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
        for (auto &i : scc_result) {
            for (auto &vertex : i) {
                vector<int> temp_adj;
                for (auto &adj : graph[vertex]) {
                    if (i.count(adj) != 0) {
                        temp_adj.push_back(adj);
                    }
                }
                graph[vertex] = temp_adj;
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

        for (auto &adjacent : graph[vertex]) {
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
                scc_result.emplace_back(stronglyConnectedComponent);
            }
        }
    }

    void findCyclesInScc() {
        visited1.resize(total_vertex);
        visited2.resize(total_vertex);
        thread thread1(&FindCycleSolution::find1, this);
        thread thread2(&FindCycleSolution::find2, this);
        thread1.join();
        thread2.join();
    }

    void find1() {
        int i = 0;
        for (int a : scc_result[0]) {
            if (i % 2 == 0) {
                findAllSimpleCycles1(a, a, 0);
            }
            i++;
        }
    }


    void find2() {
        int j = 0;
        for (int a : scc_result[0]) {
            if (j % 2 == 1) {
                findAllSimpleCycles1(a, a, 0);
            }
            j++;
        }
    }

    void findAllSimpleCycles1(int start, int current, int depth) {
        pointStack1.push_front(current);
        visited1[current] = true;
        for (auto &adjacent : graph[current]) {
            if (adjacent == start) {
                if (depth > 1) {
                    cout << endl;
                    vector<unsigned int> cycle;
                    int i = 0;
                    for (auto it = pointStack1.rbegin(); it != pointStack1.rend(); ++it) {
                        cycle.push_back(vertex_set[*it]);
                        ++i;
                    }
                    result1[depth - 2].push_back(cycle);
                    ++cycle_num1;
                }
            } else if (visited1[adjacent] == false && adjacent > start) {
                if (depth < 5) {
                    findAllSimpleCycles1(start, adjacent, depth + 1);
                } else if (depth == 5) {
                    if (reverse_graph[start][adjacent]) {
                        vector<unsigned int> cycle = vector<unsigned int>(depth + 2, 0);
                        int i = 0;
                        for (auto it = pointStack1.rbegin(); it != pointStack1.rend(); ++it) {
                            cycle.push_back(vertex_set[*it]);
                            ++i;
                        }
                        cycle.push_back(adjacent);
                        result1[depth - 1].push_back(cycle);
                        ++cycle_num1;
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
        for (auto &adjacent : graph[current]) {
            if (adjacent == start) {
                if (depth > 1) {
                    vector<unsigned int> cycle;
                    int i = 0;
                    for (auto it = pointStack2.rbegin(); it != pointStack2.rend(); ++it) {
                        cycle.push_back(vertex_set[*it]);
                        ++i;
                    }
                    result2[depth - 2].push_back(cycle);
                    ++cycle_num2;
                }
            } else if (visited2[adjacent] == false && adjacent > start) {
                if (depth < 5) {
                    findAllSimpleCycles2(start, adjacent, depth + 1);
                } else if (depth == 5) {
                    if (reverse_graph[start][adjacent]) {
                        vector<unsigned int> cycle;
                        int i = 0;
                        for (auto it = pointStack2.rbegin(); it != pointStack2.rend(); ++it) {
                            cycle.push_back(vertex_set[*it]);
                            ++i;
                        }
                        cycle.push_back(adjacent);
                        result2[depth - 1].push_back(cycle);
                        ++cycle_num2;
                    }
                }
            }
        }
        visited2[current] = false;
        pointStack2.pop_front();
    }

    inline static void sort_result(vector<vector<unsigned int>> *sub_result) {
        sort(sub_result->begin(), sub_result->end(), cmp);
    }


    void output(string &path) {
        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < result2[i].size(); ++j) {
                result1[i].push_back(result2[i][j]);
            }
        }

        FILE *file = fopen(path.c_str(), "w");
        fprintf(file, "%d\n", cycle_num1 + cycle_num2);

        thread thread1(sort_result, &result1[0]);
        thread thread2(sort_result, &result1[1]);
        thread thread3(sort_result, &result1[2]);
        thread thread4(sort_result, &result1[3]);
        thread thread5(sort_result, &result1[4]);

        thread1.join();
        thread2.join();
        thread3.join();
        thread4.join();
        thread5.join();

        for (auto i : result1) {
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
    vector<set<int>> scc_result;
    int time;

    vector<bool> visited1, visited2;
    deque<int> pointStack1, pointStack2;
    vector<vector<vector<unsigned int>>> result1, result2;
    int cycle_num1, cycle_num2;

    vector<unsigned int> vertex_set;
    unordered_map<unsigned int, int> vertex_hash;
    vector<vector<int>> graph;
    vector<vector<bool>> reverse_graph;
    int total_vertex;

};

int main() {

#ifdef DEBUG
    clock_t start, finish;
    start = clock();
    string data_path = R"(D:\hw\data\test_data.txt)";
    string linux_path = R"(/home/syj/Documents/hw/data/test_data2.txt)";
    string huawei_path = R"(/root/hw/data/test_data.txt)";
    string o = "result.txt";
#endif
    string iPath = "/data/test_data.txt";
    string oPath = "/projects/student/result.txt";

    FindCycleSolution solution;
    solution.generate_graph(linux_path);
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
    solution.findCyclesInScc();
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
