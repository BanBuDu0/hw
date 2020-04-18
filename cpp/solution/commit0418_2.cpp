//
// Created by syj on 2020/4/18.
// 修改了图的存储结构
// 在commit0418_1的基础上修改了vertex_set为int，没有报错，说明vertex都是int
// 7.7371
//
#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <set>
#include <deque>
#include <unordered_map>

#pragma comment(linker, "/STACK:10000000")

using namespace std;

class FindCycleSolution {
public:
    FindCycleSolution() {
        cycle_num = 0, total_vertex = 0;
        result[0].resize(3 * 500000);
        result[1].resize(4 * 500000);
        result[2].resize(5 * 1000000);
        result[3].resize(6 * 2000000);
        result[4].resize(7 * 3000000);
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
        set<unsigned int> st(vertex_set.begin(), vertex_set.end());
        vertex_set.assign(make_move_iterator(st.begin()), make_move_iterator(st.end()));

        for (auto &vertex : vertex_set) {
            vertex_hash[vertex] = total_vertex++;
        }

        graph.resize(total_vertex);
        reverse_graph.resize(total_vertex);
        visited = new bool[total_vertex]{false};

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
                    result[depth - 2][((depth + 1) * result_index[depth - 2]) + i] = vertex_set[pointStack[i]];
                }
                ++result_index[depth - 2];
                ++cycle_num;
            } else if (!visited[adjacent] && adjacent > start) {
                if (depth < 5) {
                    findAllSimpleCycles(start, adjacent, depth + 1);
                } else if (depth == 5) {
                    if (reverse_graph[start][adjacent]) {
                        pointStack[depth + 1] = adjacent;
                        for (int i = 0; i < depth + 2; ++i) {
                            result[depth - 1][((depth + 2) * result_index[depth - 1]) + i] = vertex_set[pointStack[i]];
                        }
                        ++result_index[depth - 1];
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
    vector<unsigned int> result[5];
    int result_index[5]{};

    vector<int> vertex_set;
    vector<set<int>> graph;
    vector<vector<bool>> reverse_graph;
    int total_vertex;
    int cycle_num;
};

int main() {
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
