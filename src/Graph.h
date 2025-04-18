
/* Completed by Vinicius Intravartola */
#pragma once
#include <string>
#include <list>
#include <unordered_map>
#include <iostream>
#include <vector>

using namespace std;

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



};

