#include <iostream>
#include <vector>
#include <random>
#include <queue>
#include <limits>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <fstream>

// Structure to represent a weighted edge
struct Edge {
    int dest;
    int weight;
};

// Function to generate a connected weighted graph
std::vector<std::vector<Edge>> generateConnectedWeightedGraph(int vertices, int minConnections) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> weightDist(1, 20);
    std::uniform_int_distribution<> connectionDist(minConnections, vertices-1);
    
    std::vector<std::vector<Edge>> graph(vertices);
    
    // First ensure minimum connectivity by connecting each vertex to minConnections others
    for (int i = 0; i < vertices; i++) {
        std::vector<int> possibleConnections;
        for (int j = 0; j < vertices; j++) {
            if (i != j) possibleConnections.push_back(j);
        }
        std::shuffle(possibleConnections.begin(), possibleConnections.end(), gen);
        
        int connections = connectionDist(gen);
        for (int j = 0; j < std::min(connections, (int)possibleConnections.size()); j++) {
            int dest = possibleConnections[j];
            int weight = weightDist(gen);
            graph[i].push_back({dest, weight});
            graph[dest].push_back({i, weight}); // Undirected graph
        }
    }
    
    return graph;
}

// Print adjacency matrix
void printAdjacencyMatrix(std::ofstream& outFile, const std::vector<std::vector<Edge>>& graph) {
    outFile << "Adjacency Matrix:\n";
    for (int i = 0; i < graph.size(); i++) {
        for (int j = 0; j < graph.size(); j++) {
            bool found = false;
            for (const Edge& edge : graph[i]) {
                if (edge.dest == j) {
                    outFile << std::setw(4) << edge.weight;
                    found = true;
                    break;
                }
            }
            if (!found) outFile << std::setw(4) << "0";
        }
        outFile << "\n";
    }
    outFile << "\n";
}

// Dijkstra's algorithm implementation
std::vector<int> dijkstra(const std::vector<std::vector<Edge>>& graph, int start) {
    int V = graph.size();
    std::vector<int> dist(V, std::numeric_limits<int>::max());
    dist[start] = 0;
    
    std::priority_queue<std::pair<int,int>, std::vector<std::pair<int,int>>, std::greater<>> pq;
    pq.push({0, start});
    
    while (!pq.empty()) {
        int u = pq.top().second;
        int d = pq.top().first;
        pq.pop();
        
        if (d > dist[u]) continue;
        
        for (const Edge& edge : graph[u]) {
            int v = edge.dest;
            int weight = edge.weight;
            
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                pq.push({dist[v], v});
            }
        }
    }
    
    return dist;
}

int main() {
    std::ofstream outFile("output.txt");
    if (!outFile) {
        std::cerr << "Failed to open output.txt\n";
        return 1;
    }

    std::vector<int> verticesCounts = {10, 20, 50, 100};
    std::vector<int> minConnectionsCounts = {3, 4, 10, 20};
    const int NUM_TESTS = 5;
    
    // Store timing results
    std::vector<std::pair<int, double>> timingResults;
    
    for (size_t i = 0; i < verticesCounts.size(); i++) {
        int vertices = verticesCounts[i];
        int minConnections = minConnectionsCounts[i];
        
        outFile << "\nTesting graph with " << vertices << " vertices and minimum " 
                << minConnections << " connections per vertex\n\n";
        
        double avgTime = 0;
        
        for (int test = 0; test < NUM_TESTS; test++) {
            outFile << "Test " << (test + 1) << ":\n";
            
            // Generate graph
            auto graph = generateConnectedWeightedGraph(vertices, minConnections);
            
            // Print adjacency matrix
            printAdjacencyMatrix(outFile, graph);
            
            // Measure time for Dijkstra's algorithm from all vertices
            auto start = std::chrono::high_resolution_clock::now();
            
            for (int source = 0; source < vertices; source++) {
                dijkstra(graph, source);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
            avgTime += duration;
            
            outFile << "Time taken: " << duration << " ms\n\n";
        }
        
        avgTime /= NUM_TESTS;
        timingResults.push_back({vertices, avgTime});
        
        outFile << "Average time for " << vertices << " vertices: " << avgTime << " ms\n";
    }
    
    // Print final timing results for plotting
    outFile << "\nFinal timing results (for plotting):\n";
    outFile << "Vertices\tTime (ms)\n";
    for (const auto& result : timingResults) {
        outFile << result.first << "\t\t" << result.second << "\n";
    }
    
    outFile.close();
    return 0;
}
