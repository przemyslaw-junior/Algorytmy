#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>

template <typename T>
struct Point {
    T x, y;

    Point() : x(0), y(0) {}

    Point(T xCoord, T yCoord) : x(xCoord), y(yCoord) {}
};

template <typename T>
struct Edge {
    int source, destination;
    T weight;

    Edge() : source(0), destination(0), weight(0) {}

    Edge(int s, int d, T w) : source(s), destination(d), weight(w) {}
};

template <typename T>
class Graph {
public:
    int numVertices;
    Point<T>* nodes;
    Edge<T>* edges;

    Graph(int vertices) : numVertices(vertices) {
        nodes = new Point<T>[vertices];
        edges = new Edge<T>[vertices * vertices];  // Zak³adam, ¿e liczba krawêdzi to max mo¿liwa liczba dla pe³nego grafu

        if (!nodes || !edges) {
            std::cerr << "Error: alokacja pamieci nie powiodla sie.\n";
            exit(1);
        }
    }

    ~Graph() {
        delete[] nodes;
        delete[] edges;
    }

    void addNode(T x, T y) {
        nodes[numNodes++] = Point<T>(x, y);
    }

    void addEdge(int source, int destination, T weight) {
        edges[numEdges++] = Edge<T>(source, destination, weight);
    }

    int getNumEdges() const {
        return numEdges;
    }

private:
    int numNodes = 0;
    int numEdges = 0;
};

class UnionFind {
private:
    int* parent;
    int* rank;

public:
    UnionFind(int n) {
        parent = new int[n];
        rank = new int[n];

        for (int i = 0; i < n; ++i) {
            parent[i] = i;
            rank[i] = 0;
        }
    }

    ~UnionFind() {
        delete[] parent;
        delete[] rank;
    }

    void unionSets(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);

        parent[rootX] = rootY;
    }

    void unionSetsByRank(int x, int y) {
        int rootX = find(x);
        int rootY = find(y);

        if (rootX != rootY) {
            if (rank[rootX] > rank[rootY]) {
                parent[rootY] = rootX;
            }
            else if (rank[rootX] < rank[rootY]) {
                parent[rootX] = rootY;
            }
            else {
                parent[rootY] = rootX;
                rank[rootX]++;
            }
        }
    }

    /*int find(int x) {
        if (x != parent[x]) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }*/
    int find(int x) {
        while (x != parent[x]) {
            parent[x] = parent[parent[x]];  // Path compression
            x = parent[x];
        }
        return x;
    }
};

template <typename T>
void bubbleSortEdges(Edge<T>* edges, int numEdges) {
    for (int i = 0; i < numEdges - 1; ++i) {
        for (int j = 0; j < numEdges - i - 1; ++j) {
            if (edges[j].weight > edges[j + 1].weight) {
                std::swap(edges[j], edges[j + 1]);
            }
        }
    }
}

template <typename T>
Edge<T>* kruskal(Graph<T>& graph, UnionFind& unionFind, int& findOperations) {
    bubbleSortEdges(graph.edges, graph.getNumEdges());

    Edge<T>* result = new Edge<T>[graph.numVertices - 1];
    int resultIndex = 0;

    for (int i = 0; i < graph.getNumEdges(); ++i) {
        int sourceRoot = unionFind.find(graph.edges[i].source);
        int destinationRoot = unionFind.find(graph.edges[i].destination);

        findOperations += 2;

        if (sourceRoot != destinationRoot) {
            result[resultIndex++] = graph.edges[i];
            unionFind.unionSetsByRank(sourceRoot, destinationRoot);
        }

        if (resultIndex == graph.numVertices - 1) {
            break;  // Znaleziono wystarczaj¹c¹ liczbê krawêdzi dla minimalnego drzewa rozpinaj¹cego
        }
    }

    return result;
}

template <typename T>
Graph<T> readGraphFromFile(const std::string& filename) {
    std::ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        std::cerr << "Error: Nie mo¿na otworzyæ pliku.\n";
        exit(1);
    }

    int numVertices, numEdges;
    inputFile >> numVertices >> numEdges;

    Graph<T> graph(numVertices);

    for (int i = 0; i < numVertices; ++i) {
        T x, y;
        inputFile >> x >> y;
        graph.addNode(x, y);
    }

    for (int i = 0; i < numEdges; ++i) {
        int source, destination;
        T weight;
        inputFile >> source >> destination >> weight;
        graph.addEdge(source, destination, weight);
    }

    inputFile.close();

    return graph;
}

int main() {
   
    for (int i = 1; i <= 3; ++i) {
        std::string filename = "C:\\Users\\LENOVO\\Documents\\ZUT\\SEMESTR III\\Algorytmy_2\\alg_Kruskal_Union_Find\\alg_Kruskal_Union_Find\\g" + std::to_string(i) + ".txt";

        Graph<double> graph = readGraphFromFile<double>(filename);
        UnionFind unionFind(graph.numVertices);

        int findOperations = 0;  // Licznik operacji find

        auto startKruskal = std::chrono::high_resolution_clock::now();
        Edge<double>* result = kruskal(graph, unionFind, findOperations);
        auto endKruskal = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> kruskalDuration = endKruskal - startKruskal;

        std::cout << "Eksperyment dla grafu z pliku g" << i << ":" << std::endl;
        std::cout << "Liczba krawedzi w MST: " << graph.numVertices - 1 << std::endl;
        double totalWeight = 0.0;
        for (int i = 0; i < graph.numVertices - 1; ++i) {
            totalWeight += result[i].weight;
        }
        std::cout << "Calkowita waga MST: " << totalWeight << std::endl;
        std::cout << "Czas dla algorytmu Kruskal's: " << kruskalDuration.count() << " seconds" << std::endl;
        std::cout << "Liczba operacji wyszukiwania: " << findOperations << "\n" << std::endl;

        delete[] result;
    }

    return 0;
}

//C:\Users\LENOVO\Documents\ZUT\SEMESTR III\Algorytmy_2\alg_Kruskal_Union_Find\alg_Kruskal_Union_Find