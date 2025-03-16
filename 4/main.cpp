#include <vector>
#include <random>
#include <queue>
#include <stack>
#include <limits>
#include <chrono>
#include <iostream>
#include <algorithm>

// Class representing a graph
class Graph {
private:
    std::vector<std::vector<int>> adjacencyMatrix;
    std::vector<std::vector<int>> incidenceMatrix;
    std::vector<std::vector<int>> adjacencyList;
    std::vector<std::pair<int,int>> edgeList;
    int vertices;
    bool directed;

public:
    Graph(int v, bool isDirected) : vertices(v), directed(isDirected) {
        adjacencyMatrix = std::vector<std::vector<int>>(v, std::vector<int>(v, 0));
    }

    void addEdge(int from, int to) {
        adjacencyMatrix[from][to] = 1;
        if (!directed) {
            adjacencyMatrix[to][from] = 1;
        }
        edgeList.push_back({from, to});
        
        // Update adjacency list
        if (adjacencyList.empty()) {
            adjacencyList.resize(vertices);
        }
        adjacencyList[from].push_back(to);
        if (!directed) {
            adjacencyList[to].push_back(from);
        }
    }

    std::vector<std::vector<int>> getAdjacencyMatrix() const {
        return adjacencyMatrix;
    }

    std::vector<std::vector<int>> getIncidenceMatrix() {
        if (incidenceMatrix.empty()) {
            buildIncidenceMatrix();
        }
        return incidenceMatrix;
    }

    std::vector<std::vector<int>> getAdjacencyList() const {
        return adjacencyList;
    }

    std::vector<std::pair<int,int>> getEdgeList() const {
        return edgeList;
    }

private:
    void buildIncidenceMatrix() {
        incidenceMatrix = std::vector<std::vector<int>>(vertices, std::vector<int>(edgeList.size(), 0));
        for (size_t e = 0; e < edgeList.size(); e++) {
            if (directed) {
                incidenceMatrix[edgeList[e].first][e] = 1;
                incidenceMatrix[edgeList[e].second][e] = -1;
            } else {
                incidenceMatrix[edgeList[e].first][e] = 1;
                incidenceMatrix[edgeList[e].second][e] = 1;
            }
        }
    }
};

// Graph generator class
class GraphGenerator {
private:
    std::mt19937 gen;
    
public:
    GraphGenerator() : gen(std::random_device{}()) {}

    Graph generateGraph(int minVertices, int maxVertices,
                       int minEdges, int maxEdges,
                       int maxDegree, bool directed,
                       int maxInDegree = -1, int maxOutDegree = -1) {
        
        std::uniform_int_distribution<> vertDist(minVertices, maxVertices);
        int vertices = vertDist(gen);
        
        Graph graph(vertices, directed);
        
        std::vector<int> degrees(vertices, 0);
        std::vector<int> inDegrees(vertices, 0);
        std::vector<int> outDegrees(vertices, 0);
        
        std::uniform_int_distribution<> edgeDist(minEdges, std::min(maxEdges, vertices * (vertices-1) / (directed ? 1 : 2)));
        int targetEdges = edgeDist(gen);
        
        std::uniform_int_distribution<> vertexDist(0, vertices-1);
        
        int edges = 0;
        while (edges < targetEdges) {
            int from = vertexDist(gen);
            int to = vertexDist(gen);
            
            if (from != to) {
                bool canAdd = true;
                
                if (!directed) {
                    if (degrees[from] >= maxDegree || degrees[to] >= maxDegree) {
                        canAdd = false;
                    }
                } else {
                    if (maxInDegree > 0 && inDegrees[to] >= maxInDegree) canAdd = false;
                    if (maxOutDegree > 0 && outDegrees[from] >= maxOutDegree) canAdd = false;
                }
                
                if (canAdd) {
                    graph.addEdge(from, to);
                    edges++;
                    
                    if (!directed) {
                        degrees[from]++;
                        degrees[to]++;
                    } else {
                        inDegrees[to]++;
                        outDegrees[from]++;
                    }
                }
            }
        }
        
        return graph;
    }
};

// Path finding algorithms
std::vector<int> bfs(const Graph& graph, int start, int end) {
    std::vector<std::vector<int>> adj = graph.getAdjacencyList();
    std::vector<bool> visited(adj.size(), false);
    std::vector<int> parent(adj.size(), -1);
    std::queue<int> q;
    
    q.push(start);
    visited[start] = true;
    
    while (!q.empty()) {
        int current = q.front();
        q.pop();
        
        if (current == end) break;
        
        for (int next : adj[current]) {
            if (!visited[next]) {
                visited[next] = true;
                parent[next] = current;
                q.push(next);
            }
        }
    }
    
    if (!visited[end]) return {};
    
    std::vector<int> path;
    for (int v = end; v != -1; v = parent[v]) {
        path.push_back(v);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<int> dfs(const Graph& graph, int start, int end) {
    std::vector<std::vector<int>> adj = graph.getAdjacencyList();
    std::vector<bool> visited(adj.size(), false);
    std::vector<int> parent(adj.size(), -1);
    std::stack<int> s;
    
    s.push(start);
    
    while (!s.empty()) {
        int current = s.top();
        s.pop();
        
        if (!visited[current]) {
            visited[current] = true;
            
            if (current == end) break;
            
            for (int next : adj[current]) {
                if (!visited[next]) {
                    parent[next] = current;
                    s.push(next);
                }
            }
        }
    }
    
    if (!visited[end]) return {};
    
    std::vector<int> path;
    for (int v = end; v != -1; v = parent[v]) {
        path.push_back(v);
    }
    std::reverse(path.begin(), path.end());
    return path;
}

int main() {
    GraphGenerator generator;
    std::random_device rd;
    std::mt19937 gen(rd());
    
    // Test parameters for 10 increasingly complex graphs
    std::vector<std::pair<int,int>> testCases = {
        {10, 15}, {20, 35}, {40, 80}, {80, 180}, {160, 400},
        {320, 900}, {640, 2000}, {1280, 4500}, {2560, 10000}, {5120, 22000}
    };
    
    std::cout << "Vertices\tEdges\tBFS Time\tDFS Time\tPath Found\n";
    
    for (const auto& test : testCases) {
        int vertices = test.first;
        int edges = test.second;
        
        Graph g = generator.generateGraph(
            vertices, vertices,  // min/max vertices
            edges, edges,       // min/max edges
            vertices/2,         // max degree
            true,              // directed
            vertices/4,        // max in-degree
            vertices/4         // max out-degree
        );
        
        std::uniform_int_distribution<> vertexDist(0, vertices-1);
        int start = vertexDist(gen);
        int end = vertexDist(gen);
        
        // Test BFS
        auto bfsStart = std::chrono::high_resolution_clock::now();
        auto bfsPath = bfs(g, start, end);
        auto bfsEnd = std::chrono::high_resolution_clock::now();
        auto bfsDuration = std::chrono::duration_cast<std::chrono::microseconds>(bfsEnd - bfsStart);
        
        // Test DFS
        auto dfsStart = std::chrono::high_resolution_clock::now();
        auto dfsPath = dfs(g, start, end);
        auto dfsEnd = std::chrono::high_resolution_clock::now();
        auto dfsDuration = std::chrono::duration_cast<std::chrono::microseconds>(dfsEnd - dfsStart);
        
        std::cout << vertices << "\t\t" << edges << "\t"
                  << bfsDuration.count() << "\t\t"
                  << dfsDuration.count() << "\t\t"
                  << (!bfsPath.empty() ? "Yes" : "No") << "\n";
    }
    
    return 0;
}
