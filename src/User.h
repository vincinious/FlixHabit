#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <limits>
#include "Graph.h"
#include "MinHeap.h"
#pragma once

using namespace std;

struct User {
    int userID;
    string name;
    int age;
    string country;
    string subscription;
    double watchTime;
    string genre;
    string lastLogin;
};