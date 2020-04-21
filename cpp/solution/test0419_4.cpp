//
// Created by syj on 2020/4/19.
// 修改了图的存储结构
// 在commit0418_2的基础上修改了图的结构，顶点不用哈希。。。,事实证明定点数280000可行
// 6.8813
//
#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <set>
#include <unordered_map>

#pragma comment(linker, "/STACK:1000000000")

using namespace std;

class FindCycleSolution {
public:
    FindCycleSolution() {
        cycle_num = 0;
        result[0].resize(3 * 500000);
        result[1].resize(4 * 500000);
        result[2].resize(5 * 1000000);
        result[3].resize(6 * 2000000);
        result[4].resize(7 * 3000000);
        graph = new set<int>[300000];
        reverse_graph = new vector<int>[300000];
        reverse_graph2 = new unordered_map<int, vector<int>>[300000];
        visited = new bool[300000]{false};
    }

    ~FindCycleSolution() = default;

    void generate_graph(string &file_path) {
        FILE *file = fopen(file_path.c_str(), "r");
        unsigned int from, to, weight;
        while (fscanf(file, "%u,%u,%u", &from, &to, &weight) != EOF) {
            vertex_set.insert(to);
            vertex_set.insert(from);
            graph[from].insert(to);
            reverse_graph[to].push_back(from);
        }
    }

    void findCyclesInScc() {
        for (auto &i : vertex_set) {
            findAllSimpleCycles(i, i, 0);
        }
    }

    void generate_reverse() {
        for (auto &i : vertex_set) {
            unordered_map<int, vector<int>> temp;
            // j is adj of i
            for (auto &j : reverse_graph[i]) {
                for (auto &k : reverse_graph[j]) {
                    temp[k].push_back(j);
                }
            }
            reverse_graph2[i] = temp;
        }
    }


    void findAllSimpleCycles(int start, int current, int depth) {
        pointStack[depth] = current;
        visited[current] = true;

        for (auto &adjacent : graph[current]) {
            if (adjacent == start && depth > 1) {
                for (int i = 0; i < depth + 1; ++i) {
                    result[depth - 2][((depth + 1) * result_index[depth - 2]) + i] = pointStack[i];
                }
                ++result_index[depth - 2];
                ++cycle_num;
            } else if (!visited[adjacent] && adjacent > start) {
                if (depth < 4) {
                    findAllSimpleCycles(start, adjacent, depth + 1);
                } else if (depth == 4) {
                    bool isSecond = false;
                    for (auto &v:reverse_graph[start]) {
                        if (v == adjacent) {
                            pointStack[depth + 1] = adjacent;
                            for (int i = 0; i < depth + 2; ++i) {
                                result[depth - 1][((depth + 2) * result_index[depth - 1]) + i] = pointStack[i];
                            }
                            ++result_index[depth - 1];
                            ++cycle_num;
                            isSecond = true;
                            break;
                        }
                    }
                    if (!isSecond) {
                        for (pair<int, vector<int>> m : reverse_graph2[start]) {
                            int third_vertex = m.first;
                            if (third_vertex == adjacent) {
                                for (auto &v : m.second) {
                                    if (v > start && !visited[v]) {
                                        pointStack[depth + 1] = adjacent;
                                        pointStack[depth + 2] = v;
                                        for (int i = 0; i < depth + 3; ++i) {
                                            result[depth][((depth + 3) * result_index[depth]) + i] = pointStack[i];
                                        }
                                        ++result_index[depth];
                                        ++cycle_num;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        visited[current] = false;
    }

    void output(string &path) {
        FILE *file = fopen(path.c_str(), "w");
        fprintf(file, "%d\n", cycle_num);
        int i = 0;
        for (auto &cycles: result) {
            for (int j = 0; j < (result_index[i] * (i + 3)); j += i + 3) {
                fprintf(file, "%u", cycles[j]);
                for (int k = j + 1; k < j + i + 3; ++k) {
                    fprintf(file, ",%u", cycles[k]);
                }
                fprintf(file, "\n");
            }
            ++i;
        }
        fclose(file);
    }

private:
    bool *visited;
    int pointStack[7];
    vector<int> result[5];
    int result_index[5]{};

    set<int> vertex_set;
    set<int> *graph;
    vector<int> *reverse_graph;
    unordered_map<int, vector<int>> *reverse_graph2;
    int cycle_num;
};

int main() {
//    string data_path = R"(D:\hw\data\test_data.txt)";
//    string linux_path = R"(/home/syj/Documents/hw/data/test_data.txt)";
//    string huawei_path = R"(/root/hw/data/test_data.txt)";
//    string oPath = "result.txt";
    string oPath = "/projects/student/result.txt";
    string iPath = "/data/test_data.txt";
    FindCycleSolution solution;
    solution.generate_graph(iPath);
    solution.generate_reverse();
    solution.findCyclesInScc();
    solution.output(oPath);

    return 0;
}
