
/* Completed by Vinicius Intravartola */
#pragma once
#include "User.h"
#include <string>
#include <list>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <filesystem>





using namespace std;

/* Genre Graph - option 5: */
class Graph 
{
   private:

        /* adjacency list to map a vertex(string) to a list of vertices(neighbors). */
        unordered_map <string, vector <string> > adjList;

    public:

        Graph();
        ~Graph();

        /* Add a vertex to the graph if it does not already exist */
        void addVertex(const string& vertex);

        /* Add an edge, undirected (directs to both edges) or directed */
        void addEdge(const string& from, const string& to, bool undirected = true);

        void printGraph() const;

        unordered_map<string, vector<string>> getAdjList() const {
            return adjList;
        }

};

class ActivityGraph 
{
    private:
        /* For each user‐index u, adj[user] is List of (neighborIndex, weight) */
        vector<vector<pair<int, double>>> adj;

    public:
        /* Construct the Graph with n user nodes*/
        ActivityGraph(int n);

        /* Connect u<->v with edge‐weight w */ 
        void addEdge(int u, int v, double w);

        /* Return the k neighbors with smallest w */
        vector<int> topKClosest(int src, int k) const;
};

ActivityGraph buildActivityGraph(const vector<User>& users, int highestWatch);
