//
// Created by syj on 2020/4/8.
// 优化了，搜索时如果顶点不在子图就不搜索该点
// scc的时候不生成子图，直接在子顶点上做
// 暂时去掉了边上的weight
//
#include <iostream>
#include <list>
#include <vector>
#include <string>
#include <fstream>
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

void ccc() {
    cout << "a";
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
            graph[from].push_back(to);
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
        scc_visited.insert(vertex);
        visitedTime[vertex] = time;
        lowTime[vertex] = time;
        time++;
        stack.push_front(vertex);
        onStack.insert(vertex);

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
                scc_vertexes.push_back(stronglyConnectedComponent);
            }
        }
    }

    void print_scc() {
        for (auto &s:scc_vertexes) {
            if (s.size() > 2) {
                for (auto &v:s) {
                    printf("%u ", v);
                }
                printf("\n");
            }
        }
    }

    void test_graph() {
        for (auto &g : scc_vertexes) {
            cout << "graph-----------------------------------------" << endl;
            for (auto &it : g) {
                cout << it << " ";
            }
            cout << endl;
        }
    }

    void findCyclesInScc() {
        int i = 0;
        for (auto &scc_vertex: scc_vertexes) {
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
        pointStack.push_front(current);
        visited.insert(current);

        for (auto &adjacent : graph[current]) {
            //when depth=6, there 7 points in the pointStack
            //so just take care about the adjacent is start or not
            if (adjacent == start) {
                if (pointStack.size() > 2 && pointStack.size() < 8) {
                    string temp;
                    vector<unsigned int> cycle;
                    for (auto it = pointStack.rbegin(); it != pointStack.rend(); ++it) {
                        cycle.push_back(*it);
                    }
                    result.push_back(cycle);
                }
            } else if (visited.count(adjacent) == 0 && depth < 6 && adjacent > start &&
                       scc_vertexes[subGraph_index].count(adjacent) != 0) {
                findAllSimpleCycles(start, adjacent, depth + 1, subGraph_index);
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
    vector<set<unsigned int>> scc_vertexes;
    int time;

    set<unsigned int> visited;
    deque<unsigned int> pointStack;
    unordered_map<unsigned int, list < unsigned int>> graph;
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

