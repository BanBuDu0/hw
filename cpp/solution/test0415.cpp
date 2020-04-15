//
// Created by syj on 2020/4/13.
// 不做cutGraph
// 10.8122
//
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <ctime>
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <thread>

#pragma comment(linker, "/STACK:1000000000")

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
        reverse_graph_vertex.resize(total_vertex);
        reverse_graph_thid.resize(total_vertex);
        reverse_graph2.resize(total_vertex);
        for (int i = 0; i < total_vertex; i++) {
            reverse_graph[i].resize(total_vertex, false);
            reverse_graph_thid[i].resize(total_vertex, false);
        }

        for (auto &pair : input_pair) {
            f = vertex_hash[pair.first], t = vertex_hash[pair.second];
            graph[f].push_back(t);
            reverse_graph[t][f] = true;
            reverse_graph_vertex[t].push_back(f);
        }
    }


    void reverse_scc(){
        thread thread1(&FindCycleSolution::generate_reverse, this);
        thread thread2(&FindCycleSolution::scc, this);
        thread1.join();
        thread2.join();
    }

    //能和scc并行
    void generate_reverse() {
        for (int i = 0; i < total_vertex; ++i) {
            unordered_map<int, vector<int>> temp;
            // j is adj of i
            for (auto &j : reverse_graph_vertex[i]) {
                vector<int> j_adj = reverse_graph_vertex[j];
                for (auto &k : j_adj) {
                    temp[k].push_back(j);
                    reverse_graph_thid[i][k] = true;
                }
            }
            reverse_graph2[i] = temp;
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
//        for (auto &i : scc_result) {
//            for (auto &vertex : i) {
//                findAllSimpleCycles(vertex, vertex, 0);
//            }
//        }
        thread thread1(&FindCycleSolution::find1, this);
        thread thread2(&FindCycleSolution::find2, this);
        thread1.join();
        thread2.join();
    }

    void find1() {
        for (int i = 0; i < scc_result.size(); i += 2) {
            for (auto &vertex : scc_result[i]) {
//                if(vertex == 18){
                findAllSimpleCycles1(vertex, vertex, 0);
//                }
            }
        }
    }

    void find2() {
        for (int i = 1; i < scc_result.size(); i += 2) {
            for (auto &vertex : scc_result[i]) {
                findAllSimpleCycles2(vertex, vertex, 0);
            }
        }
    }

    void findAllSimpleCycles1(int start, int current, int depth) {
        pointStack1.push_back(current);
        visited1[current] = true;
        for (auto &adjacent : graph[current]) {
            if (adjacent == start) {
                if (depth > 1) {
                    result1[depth - 2].push_back(pointStack1);
                    ++cycle_num1;
                }
            } else if (visited1[adjacent] == false && adjacent > start) {
                if (depth < 4) {
                    findAllSimpleCycles1(start, adjacent, depth + 1);
                } else if (depth == 4) {
                    if (reverse_graph[start][adjacent]) {
                        pointStack1.push_back(adjacent);
                        result1[depth - 1].push_back(pointStack1);
                        ++cycle_num1;
                        pointStack1.pop_back();
                    }
                    if (reverse_graph_thid[start][adjacent]) {
                        for (auto second : reverse_graph2[start][adjacent]) {
                            if(second > start && visited1[second] == false){
                                pointStack1.push_back(adjacent);
                                pointStack1.push_back(second);
                                result1[depth].push_back(pointStack1);
                                ++cycle_num1;
                                pointStack1.pop_back();
                                pointStack1.pop_back();
                            }
                        }
                    }
                }
            }
        }
        visited1[current] = false;
        pointStack1.pop_back();
    }

    void findAllSimpleCycles2(int start, int current, int depth) {
        pointStack2.push_back(current);
        visited2[current] = true;
        for (auto &adjacent : graph[current]) {
            if (adjacent == start) {
                if (depth > 1) {
                    result2[depth - 2].push_back(pointStack2);
                    ++cycle_num2;
                }
            } else if (visited2[adjacent] == false && adjacent > start) {
                if (depth < 4) {
                    findAllSimpleCycles2(start, adjacent, depth + 1);
                } else if (depth == 4) {
                    if (reverse_graph[start][adjacent]) {
                        pointStack2.push_back(adjacent);
                        result2[depth - 1].push_back(pointStack2);
                        ++cycle_num2;
                        pointStack2.pop_back();
                    }
                    if (reverse_graph_thid[start][adjacent]) {
                        for (auto second : reverse_graph2[start][adjacent]) {
                            if(second > start && visited2[second] == false){
                                pointStack2.push_back(adjacent);
                                pointStack2.push_back(second);
                                result2[depth].push_back(pointStack2);
                                ++cycle_num2;
                                pointStack2.pop_back();
                                pointStack2.pop_back();
                            }
                        }
                    }
                }
            }
        }
        visited2[current] = false;
        pointStack2.pop_back();
    }

    void sort_result(vector<vector<unsigned int>> *sub_result) {
        sort(sub_result->begin(), sub_result->end(), cmp);
    }

    void output(string &path) {

        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < result1[i].size(); ++j) {
                vector<unsigned int> temp;
                for (int k : result1[i][j]) {
                    temp.push_back(vertex_set[k]);
                }
                result[i].push_back(temp);
            }
        }

        for (int i = 0; i < 5; ++i) {
            for (int j = 0; j < result2[i].size(); ++j) {
                vector<unsigned int> temp;
                for (int k : result2[i][j]) {
                    temp.push_back(vertex_set[k]);
                }
                result[i].push_back(temp);
            }
        }

        thread thread1(&FindCycleSolution::sort_result, this, &result[0]);
        thread thread2(&FindCycleSolution::sort_result, this, &result[1]);
        thread thread3(&FindCycleSolution::sort_result, this, &result[2]);
        thread thread4(&FindCycleSolution::sort_result, this, &result[3]);
        thread thread5(&FindCycleSolution::sort_result, this, &result[4]);
        thread1.join();
        thread2.join();
        thread3.join();
        thread4.join();
        thread5.join();


        FILE *file = fopen(path.c_str(), "w");
        fprintf(file, "%d\n", cycle_num1 + cycle_num2);

        for (auto i : result) {
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
    vector<int> pointStack1, pointStack2;
    vector<vector<int>> result1[5], result2[5];
    vector<vector<unsigned int>> result[5];
    int cycle_num1, cycle_num2;

    vector<unsigned int> vertex_set;
    unordered_map<unsigned int, int> vertex_hash;
    vector<vector<int>> graph, reverse_graph_vertex;
    vector<vector<bool>> reverse_graph, reverse_graph_thid;
    vector<unordered_map<int, vector<int>>> reverse_graph2;
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
    solution.reverse_scc();
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
