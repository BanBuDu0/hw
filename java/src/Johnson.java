import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.util.*;

public class Johnson {
    static class Edge {
        private Vertex vertex1;
        private Vertex vertex2;
        private int weight;

        Edge(Vertex vertex1, Vertex vertex2, int weight) {
            this.vertex1 = vertex1;
            this.vertex2 = vertex2;
            this.weight = weight;
        }

        Vertex getVertex1() {
            return vertex1;
        }

        Vertex getVertex2() {
            return vertex2;
        }

        int getWeight() {
            return weight;
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + ((vertex1 == null) ? 0 : vertex1.hashCode());
            result = prime * result + ((vertex2 == null) ? 0 : vertex2.hashCode());
            return result;
        }

        @Override
        public String toString() {
            return "vertex1=" + vertex1
                    + ", vertex2=" + vertex2 + ", weight=" + weight + "]";
        }
    }

    static class Vertex {
        long id;
        private List<Edge> edges = new ArrayList<>();
        private List<Vertex> adjacentVertex = new ArrayList<>();

        Vertex(long id) {
            this.id = id;
        }

        public long getId() {
            return id;
        }

        public void addAdjacentVertex(Edge e, Vertex v) {
            edges.add(e);
            adjacentVertex.add(v);
        }

        public String toString() {
            return String.valueOf(id);
        }

        public List<Vertex> getAdjacentVertexes() {
            return adjacentVertex;
        }

        public List<Edge> getEdges() {
            return edges;
        }

        @Override
        public int hashCode() {
            final int prime = 31;
            int result = 1;
            result = prime * result + (int) (id ^ (id >>> 32));
            return result;
        }

        @Override
        public boolean equals(Object obj) {
            if (this == obj)
                return true;
            if (obj == null)
                return false;
            if (getClass() != obj.getClass())
                return false;
            Vertex other = (Vertex) obj;
            return id == other.id;
        }
    }

    static class Graph {
        private List<Edge> allEdges;
        private Map<Long, Vertex> allVertex;

        public Graph() {
            allEdges = new ArrayList<>();
            allVertex = new HashMap<>();
        }

        public void addEdge(long id1, long id2) {
            addEdge(id1, id2, 0);
        }

        public void addEdge(long id1, long id2, int weight) {
            Vertex vertex1;
            if (allVertex.containsKey(id1)) {
                vertex1 = allVertex.get(id1);
            } else {
                vertex1 = new Vertex(id1);
                allVertex.put(id1, vertex1);
            }
            Vertex vertex2;
            if (allVertex.containsKey(id2)) {
                vertex2 = allVertex.get(id2);
            } else {
                vertex2 = new Vertex(id2);
                allVertex.put(id2, vertex2);
            }

            Edge edge = new Edge(vertex1, vertex2, weight);
            allEdges.add(edge);
            vertex1.addAdjacentVertex(edge, vertex2);

        }

        public Vertex getVertex(long id) {
            return allVertex.get(id);
        }

        public List<Edge> getAllEdges() {
            return allEdges;
        }

        public List<Vertex> getAllVertex() {
            return new ArrayList<>(allVertex.values());
        }


        @Override
        public String toString() {
            StringBuilder buffer = new StringBuilder();
            for (Edge edge : getAllEdges()) {
                buffer.append(edge.getVertex1()).append(" ").append(edge.getVertex2()).append(" ").append(edge.getWeight());
                buffer.append("\n");
            }
            return buffer.toString();
        }
    }

    static class Scc {
        private Map<Vertex, Integer> visitedTime;
        private Map<Vertex, Integer> lowTime;
        private Set<Vertex> onStack;
        private Deque<Vertex> stack;
        private Set<Vertex> visited;
        private List<Set<Vertex>> result;
        private int time;

        public List<Set<Vertex>> scc(Graph graph) {

            //keeps the time when every vertex is visited
            time = 0;
            //keeps map of vertex to time it was visited
            visitedTime = new HashMap<>();

            //keeps map of vertex and time of first vertex visited in current DFS
            lowTime = new HashMap<>();

            //tells if a vertex is in stack or not
            onStack = new HashSet<>();

            //stack of visited vertices
            stack = new LinkedList<>();

            //tells if vertex has ever been visited or not. This is for DFS purpose.
            visited = new HashSet<>();

            //stores the strongly connected components result;
            result = new ArrayList<>();

            //start from any vertex in the graph.
            for (Vertex vertex : graph.getAllVertex()) {
                if (visited.contains(vertex)) {
                    continue;
                }
                sccUtil(vertex);
            }
            return result;
        }

        private void sccUtil(Vertex vertex) {
            visited.add(vertex);
            visitedTime.put(vertex, time);
            lowTime.put(vertex, time);
            time++;
            stack.addFirst(vertex);
            onStack.add(vertex);

            for (Vertex child : vertex.getAdjacentVertexes()) {
                //if child is not visited then visit it and see if it has link back to vertex's ancestor. In that case update
                //low time to ancestor's visit time
                if (!visited.contains(child)) {
                    sccUtil(child);
                    //sets lowTime[vertex] = min(lowTime[vertex], lowTime[child]);
                    lowTime.compute(vertex, (v, low) ->
                            Math.min(low, lowTime.get(child))
                    );
                } //if child is on stack then see if it was visited before vertex's low time. If yes then update vertex's low time to that.
                else if (onStack.contains(child)) {
                    //sets lowTime[vertex] = min(lowTime[vertex], visitedTime[child]);
                    lowTime.compute(vertex, (v, low) -> Math.min(low, visitedTime.get(child))
                    );
                }
            }

            //if vertex low time is same as visited time then this is start vertex for strongly connected component.
            //keep popping vertices out of stack still you find current vertex. They are all part of one strongly
            //connected component.
            if (visitedTime.get(vertex) == lowTime.get(vertex)) {
                Set<Vertex> stronglyConnectedComponenet = new HashSet<>();
                Vertex v;
                do {
                    v = stack.pollFirst();
                    onStack.remove(v);
                    stronglyConnectedComponenet.add(v);
                } while (!vertex.equals(v));
                result.add(stronglyConnectedComponenet);
            }
        }
    }


    Set<Vertex> blockedSet;
    Map<Vertex, Set<Vertex>> blockedMap;
    Deque<Vertex> stack;
    List<List<Vertex>> allCycles;

    public List<List<Vertex>> simpleCyles(Graph graph) {
        blockedSet = new HashSet<>();
        blockedMap = new HashMap<>();
        stack = new LinkedList<>();
        allCycles = new ArrayList<>();
        long startIndex = 1;
        Scc tarjan = new Scc();
        while (startIndex <= graph.getAllVertex().size()) {
            Graph subGraph = createSubGraph(startIndex, graph);
            List<Set<Vertex>> sccs = tarjan.scc(subGraph);
            //this creates graph consisting of strongly connected components only and then returns the
            //least indexed vertex among all the strongly connected component graph.
            //it also ignore one vertex graph since it wont have any cycle.
            Optional<Vertex> maybeLeastVertex = leastIndexSCC(sccs, subGraph);
            if (maybeLeastVertex.isPresent()) {
                Vertex leastVertex = maybeLeastVertex.get();
                blockedSet.clear();
                blockedMap.clear();
                findCyclesInSCG(leastVertex, leastVertex);
                startIndex = leastVertex.getId() + 1;
            } else {
                break;
            }
        }
        return allCycles;
    }

    private Graph createSubGraph(long startVertex, Graph graph) {
        Graph subGraph = new Graph();
        for (Edge edge : graph.getAllEdges()) {
            if (edge.getVertex1().getId() >= startVertex && edge.getVertex2().getId() >= startVertex) {
                subGraph.addEdge(edge.getVertex1().getId(), edge.getVertex2().getId());
            }
        }
        return subGraph;
    }


    private Optional<Vertex> leastIndexSCC(List<Set<Vertex>> sccs, Graph subGraph) {
        long min = Integer.MAX_VALUE;
        Vertex minVertex = null;
        Set<Vertex> minScc = null;
        for (Set<Vertex> scc : sccs) {
            if (scc.size() == 1) {
                continue;
            }
            for (Vertex vertex : scc) {
                if (vertex.getId() < min) {
                    min = vertex.getId();
                    minVertex = vertex;
                    minScc = scc;
                }
            }
        }

        if (minVertex == null) {
            return Optional.empty();
        }
        Graph graphScc = new Graph();
        for (Edge edge : subGraph.getAllEdges()) {
            if (minScc.contains(edge.getVertex1()) && minScc.contains(edge.getVertex2())) {
                graphScc.addEdge(edge.getVertex1().getId(), edge.getVertex2().getId());
            }
        }
        return Optional.of(graphScc.getVertex(minVertex.getId()));
    }

    private boolean findCyclesInSCG(
            Vertex startVertex,
            Vertex currentVertex) {
        boolean foundCycle = false;
        stack.push(currentVertex);
        blockedSet.add(currentVertex);

        for (Edge e : currentVertex.getEdges()) {
            Vertex neighbor = e.getVertex2();
            //if neighbor is same as start vertex means cycle is found.
            //Store contents of stack in final result.
            if (neighbor == startVertex) {
                List<Vertex> cycle = new ArrayList<>();
                stack.push(startVertex);
                cycle.addAll(stack);
                Collections.reverse(cycle);
                stack.pop();
                allCycles.add(cycle);
                foundCycle = true;
            } //explore this neighbor only if it is not in blockedSet.
            else if (!blockedSet.contains(neighbor)) {
                boolean gotCycle =
                        findCyclesInSCG(startVertex, neighbor);
                foundCycle = foundCycle || gotCycle;
            }
        }
        //if cycle is found with current vertex then recursively unblock vertex and all vertices which are dependent on this vertex.
        if (foundCycle) {
            //remove from blockedSet  and then remove all the other vertices dependent on this vertex from blockedSet
            unblock(currentVertex);
        } else {
            //if no cycle is found with current vertex then don't unblock it. But find all its neighbors and add this
            //vertex to their blockedMap. If any of those neighbors ever get unblocked then unblock current vertex as well.
            for (Edge e : currentVertex.getEdges()) {
                Vertex w = e.getVertex2();
                Set<Vertex> bSet = getBSet(w);
                bSet.add(currentVertex);
            }
        }
        //remove vertex from the stack.
        stack.pop();
        return foundCycle;
    }

    private void unblock(Vertex u) {
        blockedSet.remove(u);
        if (blockedMap.get(u) != null) {
            blockedMap.get(u).forEach(v -> {
                if (blockedSet.contains(v)) {
                    unblock(v);
                }
            });
            blockedMap.remove(u);
        }
    }

    private Set<Vertex> getBSet(Vertex v) {
        return blockedMap.computeIfAbsent(v, (key) ->
                new HashSet<>());
    }

    private Graph generate_data(String path) {
        Graph graph = new Graph();
        File file = new File(path);
        try {
            BufferedReader br = new BufferedReader(new FileReader(file));
            String temp;
            while ((temp = br.readLine()) != null) {
                String[] t = temp.split(",");
                graph.addEdge(Long.parseLong(t[0]), Long.parseLong(t[1]));
            }
            br.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
        return graph;
    }

    public static void main(String[] args) {
        String data_path = "D:\\hw\\data\\test_data.txt";
        Johnson johnson = new Johnson();
        Graph graph = johnson.generate_data(data_path);

        List<List<Vertex>> allCycles = johnson.simpleCyles(graph);
        allCycles.forEach(cycle -> {
            StringJoiner joiner = new StringJoiner("->");
            cycle.forEach(vertex -> joiner.add(String.valueOf(vertex.getId())));
            System.out.println(joiner);
        });
    }


}
