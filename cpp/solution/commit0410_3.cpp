//
// Created by syj on 2020/4/10.
// 在commit0410_2的基础上修改 insert ->emplace, push_back -> emplace_back, push_front -> emplace_front
// 13.12，比commit0410_2慢了
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
    }

    ~FindCycleSolution() = default;

    void generate_graph(string &file_path) {
        FILE *file = fopen(file_path.c_str(), "r");
        unsigned int from, to, weight;
        while (fscanf(file, "%u,%u,%u", &from, &to, &weight) != EOF) {
            graph[from].emplace_back(to);
            reverse_graph[to].emplace_back(from);
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
        scc_visited.emplace(vertex);
        visitedTime[vertex] = time;
        lowTime[vertex] = time;
        time++;
        stack.emplace_front(vertex);
        onStack.emplace(vertex);

        for (auto &adjacent : graph[vertex]) {
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
            set<unsigned int> stronglyConnectedComponent;
            unsigned int v;
            do {
                v = stack.front();
                stack.pop_front();
                onStack.erase(v);
                stronglyConnectedComponent.insert(v);
            } while (vertex != v);
            if (stronglyConnectedComponent.size() > 2) {
                scc_result.emplace_back(stronglyConnectedComponent);
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
            unordered_map<unsigned int, list<unsigned int>> temp_graph, temp_r_graph;
            for (auto &vertex : sub_vertex) {
                list<unsigned int> temp_adj, temp_r_adj;
                for (auto &adj : graph[vertex]) {
                    if (sub_vertex.count(adj) != 0) {
                        temp_adj.emplace_back(adj);
                    }
                }
                for (auto &r_adj : reverse_graph[vertex]) {
                    if (sub_vertex.count(r_adj) != 0) {
                        temp_r_adj.emplace_back(r_adj);
                    }
                }
                temp_graph[vertex] = temp_adj;
                temp_r_graph[vertex] = temp_r_adj;
            }
            sub_graphs.emplace_back(temp_graph);
            sub_reverse_graph.emplace_back(temp_r_graph);
        }
    }

    void test_graph() {
        for (auto &g : sub_graphs) {
            cout << "graph-----------------------------------------" << endl;
            for (auto &it : g) {
                unsigned int key = it.first;
                cout << "vex: " << key << " ";
                for (auto &l : it.second) {
                    cout << "a " << l << " ";
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
                }
                ++j;
            }
            ++i;
        }
    }


    void findAllSimpleCycles(unsigned int start, unsigned int current, int depth, int subGraph_index) {
        pointStack.emplace_front(current);
        visited.emplace(current);

        for (auto &adjacent : sub_graphs[subGraph_index][current]) {
            //when depth=6, there 7 points in the pointStack
            //so just take care about the adjacent is start or not
            if (adjacent == start) {
                if (pointStack.size() > 2 && pointStack.size() < 8) {
                    string temp;
                    vector<unsigned int> cycle;
                    for (auto it = pointStack.rbegin(); it != pointStack.rend(); ++it) {
                        cycle.emplace_back(*it);
                    }
                    result.emplace_back(cycle);
                }
            } else if (visited.count(adjacent) == 0 && adjacent > start) {
                if (depth < 5) {
                    findAllSimpleCycles(start, adjacent, depth + 1, subGraph_index);
                } else if (depth == 5) {
                    for (auto &v : sub_reverse_graph[subGraph_index][start]) {
                        if (v == adjacent) {
                            string temp;
                            vector<unsigned int> cycle;
                            for (auto it = pointStack.rbegin(); it != pointStack.rend(); ++it) {
                                cycle.emplace_back(*it);
                            }
                            cycle.emplace_back(adjacent);
                            result.emplace_back(cycle);
                        }
                    }
                }
            }
        }
        visited.erase(current);
        pointStack.pop_front();
    }


    void output(string &path) {
        ofstream outfile(path, ios::trunc);
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
    unordered_map<unsigned int, list<unsigned int>> graph, reverse_graph;
    vector<unordered_map<unsigned int, list<unsigned int>>> sub_graphs, sub_reverse_graph;
    vector<vector<unsigned int>> result;
};

int main() {
//    clock_t start, finish;
//    start = clock();
//    string data_path = R"(D:\hw\data\test_data.txt)";
//    string linux_path = R"(/home/syj/Documents/hw/data/test_data.txt)";
//    string huawei_path = R"(/root/hw/data/test_data.txt)";
    string iPath = "/data/test_data.txt";
    string oPath = "/projects/student/result.txt";
    string o = "result.txt";

    //生成数据
    FindCycleSolution solution;
    solution.generate_graph(iPath);
//    finish = clock();
//    printf("generate_graph: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
//    start = clock();

    //split scc
    solution.scc();
    solution.generate_sub_graph();
//    finish = clock();
//    printf("splic scc: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
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

