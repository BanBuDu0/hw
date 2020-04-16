//
// Created by syj on 2020/4/11.
// 修改了图的存储结构
// 在commit0416_2的基础上
// graph, reverse_graph, visited, result;
//
#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <set>
#include <deque>
#include <unordered_map>
#include <algorithm>
#include <ctime>
#include <thread>

#pragma comment(linker, "/STACK:10000000")

using namespace std;

class FindCycleSolution {
public:
    FindCycleSolution() {
        cycle_num1 = 0, cycle_num2 = 0, total_vertex = 0;
        for (int i = 0; i < 5; ++i) {
            tempStack1[i].resize(i + 3);
            tempStack2[i].resize(i + 3);
        }
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
        visited1 = new bool[total_vertex]{false};
        visited2 = new bool[total_vertex]{false};
        int f, t;
        for (int i = 0; i < total_vertex; i++) {
            reverse_graph[i].resize(total_vertex, false);
        }

        for (auto &pair : input_pair) {
            f = vertex_hash[pair.first], t = vertex_hash[pair.second];
            graph[f].insert(t);
            reverse_graph[t][f] = true;
        }
    }

    void finCycle() {
        thread thread1(&FindCycleSolution::findCyclesInScc1, this);
        thread thread2(&FindCycleSolution::findCyclesInScc2, this);
        thread1.join();
        thread2.join();
    }


    void findCyclesInScc1() {
        for (int i = 0; i < total_vertex / 2; ++i) {
            findAllSimpleCycles1(i, i, 0);
        }
    }

    void findCyclesInScc2() {
        for (int i = total_vertex / 2; i < total_vertex; ++i) {
            findAllSimpleCycles2(i, i, 0);
        }
    }

    void findAllSimpleCycles1(int start, int current, int depth) {
        pointStack1[depth] = current;
        visited1[current] = true;

        for (auto &adjacent : graph[current]) {
            if (adjacent == start && depth > 1) {
                for (int i = 0; i < depth + 1; ++i) {
                    tempStack1[depth - 2][i] = pointStack1[i];
                }
                result1[depth - 2].push_back(tempStack1[depth - 2]);
                ++cycle_num1;
            } else if (!visited1[adjacent] && adjacent > start) {
                if (depth < 5) {
                    findAllSimpleCycles1(start, adjacent, depth + 1);
                } else if (depth == 5) {
                    if (reverse_graph[start][adjacent]) {
                        pointStack1[depth + 1] = adjacent;
                        for (int i = 0; i < depth + 2; ++i) {
                            tempStack1[depth - 1][i] = pointStack1[i];
                        }
                        result1[depth - 1].push_back(tempStack1[depth - 1]);
                        ++cycle_num1;
                    }
                }
            }
        }
        visited1[current] = false;
    }

    void findAllSimpleCycles2(int start, int current, int depth) {
        pointStack2[depth] = current;
        visited2[current] = true;

        for (auto &adjacent : graph[current]) {
            if (adjacent == start && depth > 1) {
                for (int i = 0; i < depth + 1; ++i) {
                    tempStack2[depth - 2][i] = pointStack2[i];
                }
                result2[depth - 2].push_back(tempStack2[depth - 2]);
                ++cycle_num2;
            } else if (!visited2[adjacent] && adjacent > start) {
                if (depth < 5) {
                    findAllSimpleCycles2(start, adjacent, depth + 1);
                } else if (depth == 5) {
                    if (reverse_graph[start][adjacent]) {
                        pointStack2[depth + 1] = adjacent;
                        for (int i = 0; i < depth + 2; ++i) {
                            tempStack2[depth - 1][i] = pointStack2[i];
                        }
                        result2[depth - 1].push_back(tempStack2[depth - 1]);
                        ++cycle_num2;
                    }
                }
            }
        }
        visited2[current] = false;
    }

    void output(string &path) {
        FILE *file = fopen(path.c_str(), "w");
        fprintf(file, "%d\n", cycle_num1 + cycle_num2);
        for(int i = 0; i < 5; ++i){
            for (auto &cycle : result1[i]) {
                fprintf(file, "%u", cycle[0]);
                for (int j = 1; j < cycle.size(); ++j) {
                    fprintf(file, ",%u", cycle[j]);
                }
                fprintf(file, "\n");
            }

            for (auto &cycle : result2[i]) {
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
    bool *visited1, *visited2;
    int pointStack1[7], pointStack2[7];
    vector<unsigned int> tempStack1[5], tempStack2[5];
    vector<vector<unsigned int>> result1[5], result2[5];

    vector<unsigned int> vertex_set;
    vector<set<int>> graph;
    vector<vector<bool>> reverse_graph;
    int total_vertex;
    int cycle_num1, cycle_num2;
};

int main() {
//    clock_t start, finish;
//    start = clock();
//    string data_path = R"(D:\hw\data\test_data.txt)";
//    string linux_path = R"(/home/syj/Documents/hw/data/test_data.txt)";
//    string huawei_path = R"(/root/hw/data/test_data.txt)";
//    string o = "result.txt";
    string iPath = "/data/test_data.txt";
    string oPath = "/projects/student/result.txt";
    FindCycleSolution solution;
    solution.generate_graph(iPath);
    solution.finCycle();
    solution.output(oPath);

    return 0;
}
