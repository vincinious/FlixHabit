

#include <string>

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

