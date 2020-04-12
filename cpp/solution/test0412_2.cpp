//
// Created by syj on 2020/4/12.
// 先做一层深搜，再判断，这个应该很慢
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
        time = 0;
        total_vertex = 0;
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
        reverse_graph2.resize(total_vertex);

        for (auto &pair : input_pair) {
            f = vertex_hash[pair.first], t = vertex_hash[pair.second];
            graph[f].push_back(t);
            reverse_graph[t].push_back(f);
        }
    }

    //能和scc并行
    void generate_reverse() {
        vector<bool> h;
        for (int i = 0; i < total_vertex; ++i) {
            unordered_map<int, vector<bool>> temp;
            // j is adj of i
            h = vector<bool>(total_vertex, false);
            for (auto &j : reverse_graph[i]) {
                vector<int> j_adj = reverse_graph[j];
                for (auto &k : j_adj) {
                    h[k] = true;
                }
                temp[j] = h;
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

    void findCyclesInScc() {
        visited.resize(total_vertex);
        for (auto &i : scc_result) {
            for (auto &vertex : i) {
                findAllSimpleCycles(vertex, vertex, 0);
            }
        }
    }

    void findAllSimpleCycles(int start, int current, int depth) {
        pointStack.push_front(current);
        visited[current] = true;

        for (auto &adjacent : graph[current]) {
            if (adjacent == start) {
                if (depth > 1) {
                    vector<unsigned int> cycle;
                    for (auto it = pointStack.rbegin(); it != pointStack.rend(); ++it) {
                        cycle.push_back(vertex_set[*it]);
                    }
                    result.push_back(cycle);
                }
            } else if (visited[adjacent] == false && adjacent > start) {
                if (depth < 5) {
                    findAllSimpleCycles(start, adjacent, depth + 1);
                } else if (depth == 5) {
                    auto a = reverse_graph2[start][adjacent];
                    if(!a.empty()){
                        if(a[current]){
                            vector<unsigned int> cycle;
                            for (auto it = pointStack.rbegin(); it != pointStack.rend(); ++it) {
                                cycle.push_back(vertex_set[*it]);
                            }
                            cycle.push_back(adjacent);
                            result.push_back(cycle);
                        }
                    }
                }
            }
        }
        visited[current] = false;
        pointStack.pop_front();
    }

    void output(string &path) {
        ofstream outfile(path, ios::trunc);
        vector<vector<unsigned int>> all_res;
        outfile << result.size() << endl;
        sort(result.begin(), result.end(), cmp);
        for (auto &cycle : result) {
            for (auto &vertex : cycle) {
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
    unordered_map<int, int> visitedTime;
    unordered_map<int, int> lowTime;
    vector<bool> onStack;
    deque<int> stack;
    vector<bool> scc_visited;
    vector<set<int>> scc_result;
    int time;

    vector<bool> visited;
    deque<int> pointStack;
    vector<vector<unsigned int>> result;

    vector<unsigned int> vertex_set;
    unordered_map<unsigned int, int> vertex_hash;
    vector<vector<int>> graph, reverse_graph;
    vector<unordered_map<int, vector<bool>>> reverse_graph2;
    int total_vertex;
};

int main() {
    clock_t start, finish;
    start = clock();
    string data_path = R"(D:\hw\data\test_data.txt)";
    string linux_path = R"(/home/syj/Documents/hw/data/test_data.txt)";
    string huawei_path = R"(/root/hw/data/test_data.txt)";
    string iPath = "/data/test_data.txt";
    string oPath = "/projects/student/result.txt";
    string o = "result1.txt";

    //生成数据
    FindCycleSolution solution;
    solution.generate_graph(data_path);
    finish = clock();
    printf("generate_graph: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    start = clock();
    solution.generate_reverse();
    finish = clock();
    printf("reverse search: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    start = clock();

    //split scc
    solution.scc();
    solution.cutGraph();
    finish = clock();
    printf("splic scc: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    start = clock();

    //find cycle
    solution.findCyclesInScc();
    finish = clock();
    printf("findCycles: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    start = clock();

    //output
    solution.output(o);
    finish = clock();
    printf("output: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
    //    system("pause");
    return 0;
}
