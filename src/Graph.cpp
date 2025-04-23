/* Completed by Vinicius Intravartola (WIP) */
#include "Graph.h"
#include <filesystem>


using namespace std;

/* Graph implementation: Shortest-paths to find most similar users and their interests */

// define constructor
Graph::Graph() { }

// define destructor (must match the header)
Graph::~Graph() { }


/* Adds a vertex to the graph if it does not already exist */
void Graph::addVertex(const string& vertex) 
{
    /* Create an empty list for this vertex */
    if (adjList.find(vertex) == adjList.end())  { adjList[vertex] = {}; }
}

/* Adds an edge(a connection between two identifiers of a user in the data).If undirected is true, adds an edge directed in the opposite direction too */
void Graph::addEdge(const string& from, const string& to, bool undirected)
{
    /* Add 'from' and 'to' vertices to the list*/
    addVertex(from); 
    addVertex(to); 

    adjList[from].push_back(to);

    if (undirected == true)    { adjList[to].push_back(from); }
}

/* Prints the contents of the adjacency list */
void Graph::printGraph() const
{
    cout << "Graph Adjacency List:" << endl;

    for (auto const& pair : adjList)
    {
        cout << pair.first << " -> ";
        auto const &neighbors = pair.second;

        for (unsigned int i = 0; i < neighbors.size(); ++i) 
        {
            cout << neighbors[i];

            if (i + 1 < neighbors.size())   { cout << ", "; }
        }

        cout << "\n";
    }


}


// ActivityGraph implementation
// ───────────────────────────────────────────────────────────

ActivityGraph::ActivityGraph(int n)
    : adj(n)
{}

void ActivityGraph::addEdge(int u, int v, double w) 
{
    if (u < 0 || u >= (int)adj.size() ||
        v < 0 || v >= (int)adj.size()) {
        return;
    }
    adj[u].push_back({ v, w });
    adj[v].push_back({ u, w });
}

vector<int> ActivityGraph::topKClosest(int src, int k) const 
{
    // copy so we can sort without mutating the original
    vector<pair<int, double>> list = adj[src];

    sort(
        list.begin(), list.end(),
        [](const pair<int, double>& a, const pair<int, double>& b) {
            return a.second < b.second;
        }
    );

    vector<int> result;
    for (int i = 0; i < k && i < (int)list.size(); ++i) {
        result.push_back(list[i].first);
    }
    return result;
}

ActivityGraph buildActivityGraph(const std::vector<User>& users, int highestWatch)
{
    int n = (int)users.size();
    ActivityGraph ag(n);

    for (int i = 0; i < n; ++i) 
    {
        if (i == highestWatch) { continue; }
        double w = fabs(users[i].watchTime - users[highestWatch].watchTime);

        ag.addEdge(highestWatch, i, w);

    }

    return ag;
}
