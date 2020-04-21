//
// Created by syj on 2020/4/19.
// 修改了图的存储结构
// 在commit0418_2的基础上修改了图的结构，顶点不用哈希。。。,事实证明定点数280000可行
// 用互斥量
// 反正多线程都是负优化
// 7.0560
//
#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <set>
#include <thread>
#include <mutex>

#pragma comment(linker, "/STACK:1000000000")

using namespace std;

class FindCycleSolution {
public:
    FindCycleSolution() {
        cycle_num1 = 0, cycle_num2 = 0;
        result[0].resize(3 * 500000);
        result[1].resize(4 * 500000);
        result[2].resize(5 * 1000000);
        result[3].resize(6 * 2000000);
        result[4].resize(7 * 3000000);

        visited1 = new bool[300000]{false};
        visited2 = new bool[300000]{false};

        graph = new set<int>[300000];
        reverse_graph = new set<int>[300000];
    }

    ~FindCycleSolution() = default;

    void generate_graph(string &file_path) {
        set<int> st;
        FILE *file = fopen(file_path.c_str(), "r");
        unsigned int from, to, weight;
        while (fscanf(file, "%u,%u,%u", &from, &to, &weight) != EOF) {
            st.insert(to);
            st.insert(from);
            graph[from].insert(to);
            reverse_graph[to].insert(from);
        }
        vertex_set.assign(st.begin(), st.end());
        total_vertex = vertex_set.size();
    }

    void findCycles() {
        thread thread1(&FindCycleSolution::findCyclesInScc1, this);
        thread thread2(&FindCycleSolution::findCyclesInScc2, this);
        thread1.join();
        thread2.join();
    }

    void findCyclesInScc1() {
        for (int i = 0; i < (total_vertex / 3); ++i) {
            findAllSimpleCycles1(vertex_set[i], vertex_set[i], 0);
        }
    }

    void findCyclesInScc2() {
        for (int i = (total_vertex  / 3); i < total_vertex; ++i) {
            findAllSimpleCycles2(vertex_set[i], vertex_set[i], 0);
        }
    }

    void findAllSimpleCycles1(int start, int current, int depth) {
        pointStack1[depth] = current;
        visited1[current] = true;
        for (auto &adjacent : graph[current]) {
            if (adjacent == start && depth > 1) {
                mMutex.lock();
                for (int i = 0; i < depth + 1; ++i) {
                    result[depth - 2][((depth + 1) * result_index[depth - 2]) + i] = pointStack1[i];
                }
                ++result_index[depth - 2];
                mMutex.unlock();
                ++cycle_num1;
            } else if (!visited1[adjacent] && adjacent > start) {
                if (depth < 5) {
                    findAllSimpleCycles1(start, adjacent, depth + 1);
                } else if (depth == 5) {
                    for (auto &v : reverse_graph[start]) {
                        if (v == adjacent) {
                            pointStack1[depth + 1] = adjacent;
                            mMutex.lock();
                            for (int i = 0; i < depth + 2; ++i) {
                                result[depth - 1][((depth + 2) * result_index[depth - 1]) + i] = pointStack1[i];
                            }
                            ++result_index[depth - 1];
                            mMutex.unlock();
                            ++cycle_num1;
                            break;
                        }
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
                mMutex.lock();
                for (int i = 0; i < depth + 1; ++i) {
                    result[depth - 2][((depth + 1) * result_index[depth - 2]) + i] = pointStack2[i];
                }
                ++result_index[depth - 2];
                mMutex.unlock();
                ++cycle_num2;
            } else if (!visited2[adjacent] && adjacent > start) {
                if (depth < 5) {
                    findAllSimpleCycles2(start, adjacent, depth + 1);
                } else if (depth == 5) {
                    for (auto &v : reverse_graph[start]) {
                        if (v == adjacent) {
                            pointStack2[depth + 1] = adjacent;
                            mMutex.lock();
                            for (int i = 0; i < depth + 2; ++i) {
                                result[depth - 1][((depth + 2) * result_index[depth - 1]) + i] = pointStack2[i];
                            }
                            ++result_index[depth - 1];
                            mMutex.unlock();
                            ++cycle_num2;
                            break;
                        }
                    }
                }
            }
        }
        visited2[current] = false;
    }

    void output(string &path) {
        FILE *file = fopen(path.c_str(), "w");
        fprintf(file, "%d\n", cycle_num1 + cycle_num2);
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
    bool *visited1, *visited2;
    int pointStack1[7], pointStack2[7];
    vector<int> result[5];
    int result_index[5]{};
    int cycle_num1, cycle_num2;
    mutex mMutex;

    vector<int> vertex_set;
    set<int> *graph, *reverse_graph;
    int total_vertex;
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
    solution.findCycles();
    solution.output(oPath);

    return 0;
}
