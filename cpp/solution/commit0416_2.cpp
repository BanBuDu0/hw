//
// Created by syj on 2020/4/11.
// 修改了图的存储结构
// 在commit0416_1的基础上
// 8.1374
//
#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <set>
#include <deque>
#include <unordered_map>
#include <algorithm>

#pragma comment(linker, "/STACK:10000000")

using namespace std;


class FindCycleSolution {
public:
    FindCycleSolution() {
        cycle_num = 0, total_vertex = 0;
        tempStack.resize(5);
        for (int i = 0; i < 5; ++i) {
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


    void findCyclesInScc() {
        for (int i = 0; i < total_vertex; ++i) {
            findAllSimpleCycles(i, i, 0);
        }
    }

    void findAllSimpleCycles(int start, int current, int depth) {
        pointStack[depth] = current;
        visited[current] = true;

        for (auto &adjacent : graph[current]) {
            if (adjacent == start && depth > 1) {
                for (int i = 0; i < depth + 1; ++i) {
                    tempStack[depth - 2][i] = vertex_set[pointStack[i]];
                }
                result[depth - 2].push_back(tempStack[depth - 2]);
                ++cycle_num;

            } else if (visited[adjacent] == false && adjacent > start) {
                if (depth < 5) {
                    findAllSimpleCycles(start, adjacent, depth + 1);
                } else if (depth == 5) {
                    if (reverse_graph[start][adjacent]) {
                        pointStack[depth + 1] = adjacent;
                        for (int i = 0; i < depth + 2; ++i) {
                            tempStack[depth - 1][i] = vertex_set[pointStack[i]];
                        }
                        result[depth - 1].push_back(tempStack[depth - 1]);
                        ++cycle_num;
                    }
                }
            }
        }
        visited[current] = false;
    }

    void output(string &path) {
        FILE *file = fopen(path.c_str(), "w");
        fprintf(file, "%d\n", cycle_num);
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
    vector<bool> visited;
    int pointStack[7];
    vector<vector<unsigned int>> tempStack;
    vector<vector<vector<unsigned int>>> result;

    vector<unsigned int> vertex_set;
    vector<set<int>> graph;
    vector<vector<bool>> reverse_graph;
    int total_vertex;
    int cycle_num;
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
    solution.findCyclesInScc();
    solution.output(oPath);

    return 0;
}
