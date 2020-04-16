//
// Created by syj on 2020/4/11.
// 修改了图的存储结构
// 在commit0411_2的基础上
// 11.0561
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

#pragma comment(linker, "/STACK:10000000")

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
        time = 0, cycle_num = 0,total_vertex = 0;
        tempStack.resize(5);
        for(int i = 0; i < 5; ++i){
            tempStack[i].resize(i + 3);
        }
        result.resize(5);
    }

    ~FindCycleSolution() = default;

    void generate_graph(string &file_path) {
        unordered_map<unsigned int, int> vertex_hash;
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

        graph.resize(total_vertex);
        reverse_graph.resize(total_vertex);
        visited.resize(total_vertex);
        scc_visited.resize(total_vertex);
        onStack.resize(total_vertex);

        int f, t;
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
        for (auto &i : scc_result) {
            for (auto &vertex : i) {
                findAllSimpleCycles(vertex, vertex, 0);
            }
        }
    }

    void findAllSimpleCycles(int start, int current, int depth) {
        pointStack.push_back(current);
        visited[current] = true;

        for (auto &adjacent : graph[current]) {
            if (adjacent == start && depth > 1) {
                if (depth > 1) {
                    for(int i = 0; i < depth + 1; ++i){
                        tempStack[depth - 2][i] = vertex_set[pointStack[i]];
                    }
                    result[depth - 2].push_back(tempStack[depth - 2]);
                    ++cycle_num;
                }
            } else if (visited[adjacent] == false && adjacent > start) {
                if (depth < 5) {
                    findAllSimpleCycles(start, adjacent, depth + 1);
                } else if (depth == 5) {
                    if (reverse_graph[start][adjacent]) {
                        pointStack.push_back(adjacent);
                        for(int i = 0; i < depth + 2; ++i){
                            tempStack[depth - 1][i] = vertex_set[pointStack[i]];
                        }
                        pointStack.pop_back();
                        result[depth - 1].push_back(tempStack[depth - 1]);
                        ++cycle_num;
                    }
                }
            }
        }
        visited[current] = false;
        pointStack.pop_back();
    }

    void output(string &path) {
        FILE *file = fopen(path.c_str(), "w");
        fprintf(file, "%d\n", cycle_num);
        for (auto i : result) {
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
    vector<set<int>> scc_result;
    int time;

    vector<bool> visited;
    vector<int> pointStack;
    vector<vector<unsigned int>> tempStack;
    vector<vector<vector<unsigned int>>> result;

    vector<unsigned int> vertex_set;
    vector<vector<int>> graph;
    vector<vector<bool>> reverse_graph;
    int total_vertex;
    int cycle_num;
};

int main() {
//    clock_t start, finish;
//    start = clock();
//    string data_path = R"(D:\hw\data\test_data.txt)";
//    string linux_path = R"(/home/syj/Documents/hw/data/test_data2.txt)";
//    string huawei_path = R"(/root/hw/data/test_data.txt)";
//    string o = "result.txt";
    string iPath = "/data/test_data.txt";
    string oPath = "/projects/student/result.txt";


    //生成数据
    FindCycleSolution solution;
    solution.generate_graph(iPath);
//    finish = clock();
//    printf("generate_graph: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
//    start = clock();

    //split scc
    solution.scc();
//    finish = clock();
//    printf("split scc: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
//    start = clock();

    //find cycle
    solution.findCyclesInScc();
//    finish = clock();
//    printf("findCycles: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
//    start = clock();

    //output
    solution.output(oPath);
//    finish = clock();
//    printf("output: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    return 0;
}
