//
// Created by syj on 2020/4/13.
// 11.5388
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
        for (int i = 0; i < total_vertex; i++) {
            reverse_graph[i].resize(total_vertex, false);
        }

        for (auto &pair : input_pair) {
            f = vertex_hash[pair.first], t = vertex_hash[pair.second];
            graph[f].push_back(t);
            reverse_graph[t][f] = true;
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
        for (int i = 0; i < total_vertex; i += 2) {
            findAllSimpleCycles1(i, i, 0);
        }
    }

    void find2() {
        for (int i = 1; i < total_vertex; i += 2) {
            findAllSimpleCycles2(i, i, 0);
        }
    }

    void findAllSimpleCycles1(int start, int current, int depth) {
        pointStack1.push_back(current);
        visited1[current] = true;
        for (auto &adjacent : graph[current]) {
            if (adjacent == start && depth > 1) {
                result1[depth - 2].emplace_back(pointStack1);
                ++cycle_num1;
            } else if (visited1[adjacent] == false && adjacent > start) {
                if (depth < 5) {
                    findAllSimpleCycles1(start, adjacent, depth + 1);
                } else if (depth == 5) {
                    if (reverse_graph[start][adjacent]) {
                        pointStack1.push_back(adjacent);
                        result1[depth - 1].push_back(pointStack1);
                        ++cycle_num1;
                        pointStack1.pop_back();
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
            if (adjacent == start && depth > 1) {
                result2[depth - 2].push_back(pointStack2);
                ++cycle_num2;
            } else if (visited2[adjacent] == false && adjacent > start) {
                if (depth < 5) {
                    findAllSimpleCycles2(start, adjacent, depth + 1);
                } else if (depth == 5) {
                    if (reverse_graph[start][adjacent]) {
                        pointStack2.push_back(adjacent);
                        result2[depth - 1].emplace_back(pointStack2);
                        ++cycle_num2;
                        pointStack2.pop_back();
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
    vector<bool> visited1, visited2;
    vector<int> pointStack1, pointStack2;
    vector<vector<int>> result1[5], result2[5];
    vector<vector<unsigned int>> result[5];
    int cycle_num1, cycle_num2;

    vector<unsigned int> vertex_set;
    unordered_map<unsigned int, int> vertex_hash;
    vector<vector<int>> graph;
    vector<vector<bool>> reverse_graph;
    int total_vertex;
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

    FindCycleSolution solution;
    solution.generate_graph(iPath);
//    finish = clock();
//    printf("generate_graph: %f ms\n", ((double) (finish - start) / CLOCKS_PER_SEC) * 1000);
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
    //    system("pause");
    return 0;
}
