#include <iostream>
#include <fstream>
#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <set>
#include <stack>

using namespace std;

class Vertex {
public:
    Vertex() = default;

    explicit Vertex(long i) {
        id = i;
    }

    ~Vertex() = default;

    void addAdjacentVertex(const Vertex &v) {
        adjacentVertex.push_back(v);
    }

    vector<Vertex> getAdjacentVertexes() {
        return adjacentVertex;
    }

    long getId() {
        return id;
    }

private:
    long id{};
    vector<Vertex> adjacentVertex;

};

class Edge {
public:
    Edge(const Vertex &v1, const Vertex &v2, int w) {
        vertex1 = v1;
        vertex2 = v2;
        weight = w;
    }

    ~Edge() = default;

    Vertex getVertex1() {
        return vertex1;
    }

    Vertex getVertex2() {
        return vertex2;
    }

    int getWeight() {
        return weight;
    }

private:
    Vertex vertex1;
    Vertex vertex2;
    int weight;
};

class Graph {
public:
    Graph() = default;

    ~Graph() = default;

    void addEdge(long id1, long id2, int weight) {
        Vertex vertex1;
        if (allVertex.count(id1) > 0) {
            vertex1 = allVertex[id1];
        } else {
            vertex1 = Vertex(id1);
            allVertex.insert(make_pair(id1, vertex1));
        }
        Vertex vertex2;
        if (allVertex.count(id2) > 0) {
            vertex2 = allVertex[id2];
        } else {
            vertex2 = Vertex(id2);
            allVertex.insert(make_pair(id2, vertex2));
        }

        Edge edge(vertex1, vertex2, weight);
        allEdges.push_back(edge);
        vertex1.addAdjacentVertex(vertex2);
    }

    vector<Edge> getAllEdges() {
        return allEdges;
    }

    vector<Vertex> getAllVertex() {
        vector<Vertex> temp;
        for (auto &it : allVertex) {
            long key = it.first;
            temp.push_back(it.second);
        }
        return temp;
    }

    void printAllEdges() {
        for (auto &allEdge : allEdges) {
            cout << allEdge.getVertex1().getId() << " -> " << allEdge.getVertex2().getId() << endl;
        }
    }

private:
    vector<Edge> allEdges;
    map<long, Vertex> allVertex;
};


void split(const string &s, vector<long> &sv, const char flag = ' ') {
    sv.clear();
    istringstream iss(s);
    string temp;

    while (getline(iss, temp, flag)) {
        sv.push_back(stoi(temp));
    }
}


void generate_graph(const string &file, Graph &g) {
    ifstream infile;
    infile.open(file.data());
    assert(infile.is_open());

    string s;
    while (getline(infile, s)) {
        vector<long> temp;
        split(s, temp, ',');
        //cout << "read: " << temp[0] << endl;
        g.addEdge(temp[0], temp[1], temp[2]);

    }
    infile.close();
}


class Tarjan {
public:
    Tarjan() = default;

    ~Tarjan() = default;

    void findCycles(Graph g, vector<vector<Vertex> > &result) {
        for (auto &item : g.getAllVertex()) {
            findAllSimpleCycles(item, item, result);
            visited.insert(item);
            while (!markedStack.empty()) {
                Vertex v = markedStack.top();
                markedStack.pop();
                markedSet.erase(v);
            }
        }
    }

    bool findAllSimpleCycles(Vertex start, Vertex current, vector<vector<Vertex> > &result) {
        bool hasCycle = false;
        pointStack.push(current);
        markedSet.insert(current);
        markedStack.push(current);

        for (Vertex w : current.getAdjacentVertexes()) {
            if (visited.count(w) == 0) {
                if (w.getId() == start.getId()) {
                    hasCycle = true;
                    if (pointStack.size() > 2 && pointStack.size() < 8) {
                        vector<Vertex> cycle;
                        stack<Vertex> temp;
                        while (!pointStack.empty()) {
                            temp.push(pointStack.top());
                            pointStack.pop();
                        }
                        while (!temp.empty()) {
                            cycle.push_back(temp.top());
                            temp.pop();
                        }
                        result.push_back(cycle);
                    }
                } else if (markedSet.count(w) == 0) {
                    hasCycle = findAllSimpleCycles(start, w, result) || hasCycle;
                }
            }
        }

        if (hasCycle) {
            while (markedStack.top().getId() != current.getId()) {
                Vertex v = markedStack.top();
                markedStack.pop();
                markedSet.erase(v);
            }
            Vertex v = markedStack.top();
            markedStack.pop();
            markedSet.erase(v);
        }

        pointStack.pop();
        return hasCycle;
    }

private:
    set<Vertex> visited, markedSet;
    stack<Vertex> pointStack, markedStack;
};


int main() {
    Graph g;
    string data_path = R"(D:\code\leecode\src\com\huawei\data\test_data.txt)";
    generate_graph(data_path, g);
    g.printAllEdges();
    vector<vector<Vertex> > result;
    Tarjan t;
    t.findCycles(g, result);
    for (auto &i : result) {
        for (auto &j : i) {
            cout << j.getId();
            cout << " ";
        }
        cout << endl;
    }
    return 0;
}
