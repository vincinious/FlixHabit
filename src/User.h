/* Completed by Vinicius Intravartola */

#pragma once

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

using namespace std;

struct User 
{
    int userID;              // numeric ID for user
    string name;             // profile name
    int age;                 // user's age according to account 
    string country;          // user's country
    string subscription;     // subscription plan type
    double watchTime;        // hours watched in the last month
    string genre;            // user's preferred genre
    string lastLogin;        // date of last login (dash-delimited)
};

struct UserSimilarity 
{
    int    user1ID;
    int    user2ID;
    double similarity;
    bool operator<(const UserSimilarity& o) const
    {
        return similarity > o.similarity; 
    }
};