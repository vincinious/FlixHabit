#include "User.h"
#include "Graph.h"
#include "MinHeap.h"

#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <limits>

using namespace std;



// Function to calculate similarity score between users
double calculateSimilarity(const User& user1, const User& user2) {
    double score = 0.0;

    // Age similarity (closer in age = higher score)
    score += 100.0 / (abs(user1.age - user2.age) + 1);

    // Genre match
    if (user1.genre == user2.genre) {
        score += 50.0;
    }

    // Country match
    if (user1.country == user2.country) {
        score += 30.0;
    }

    // Subscription match
    if (user1.subscription == user2.subscription) {
        score += 20.0;
    }

    // Watch time similarity
    score += 100.0 / (abs(user1.watchTime - user2.watchTime) + 1);

    return score;
}

// Function to read users from CSV file
vector<User> readUsersFromCSV(const string& filename) {
    vector<User> users;
    ifstream file(filename);

    if (!file.is_open()) {
        cout << "Error opening file: " << filename << endl;
        return users;
    }

    string line;
    // Skip header line
    getline(file, line);

    while (getline(file, line)) {
        User user;
        stringstream ss(line);
        string token;

        getline(ss, token, ',');
        user.userID = stoi(token);

        getline(ss, user.name, ',');

        getline(ss, token, ',');
        user.age = stoi(token);

        getline(ss, user.country, ',');
        getline(ss, user.subscription, ',');

        getline(ss, token, ',');
        user.watchTime = stod(token);

        getline(ss, user.genre, ',');
        getline(ss, user.lastLogin, ',');

        users.push_back(user);
    }

    file.close();
    return users;
}

// Function to find most common genre for a specific age group
string findMostCommonGenreForAgeGroup(const vector<User>& users, int minAge, int maxAge) {
    map<string, int> genreCounts;

    for (const auto& user : users) {
        if (user.age >= minAge && user.age <= maxAge) {
            genreCounts[user.genre]++;
        }
    }

    string mostCommonGenre = "";
    int maxCount = 0;

    for (const auto& pair : genreCounts) {
        if (pair.second > maxCount) {
            maxCount = pair.second;
            mostCommonGenre = pair.first;
        }
    }

    return mostCommonGenre;
}

// Function to find average watch time by country
map<string, double> findAverageWatchTimeByCountry(const vector<User>& users) {
    map<string, double> totalWatchTime;
    map<string, int> countryCounts;

    for (const auto& user : users) {
        totalWatchTime[user.country] += user.watchTime;
        countryCounts[user.country]++;
    }

    map<string, double> avgWatchTime;
    for (const auto& pair : totalWatchTime) {
        avgWatchTime[pair.first] = pair.second / countryCounts[pair.first];
    }

    return avgWatchTime;
}

// Function to build graph of user relationships based on genre preferences
Graph buildUserGenreGraph(const vector<User>& users) {
    Graph graph;

    // Create vertices for each genre
    map<string, bool> genres;
    for (const auto& user : users) {
        genres[user.genre] = true;
    }

    for (const auto& genre : genres) {
        graph.addVertex(genre.first);
    }

    // Create edges between genres based on users who watch multiple genres
    for (size_t i = 0; i < users.size(); i++) {
        for (size_t j = i + 1; j < users.size(); j++) {
            if (users[i].genre != users[j].genre) {
                /* Link the genres together when different users favor the same genre */
                graph.addEdge(users[i].genre, users[j].genre);
            }
        }
    }

    return graph;
}

// Function to find most similar users using MinHeap
vector<UserSimilarity> findMostSimilarUsers(const vector<User>& users, unsigned int k) {
    MinHeap<UserSimilarity> heap;

    for (size_t i = 0; i < users.size(); i++) {
        for (size_t j = i + 1; j < users.size(); j++) {
            UserSimilarity similarity;
            similarity.user1ID = users[i].userID;
            similarity.user2ID = users[j].userID;
            similarity.similarity = calculateSimilarity(users[i], users[j]);

            heap.insert(similarity);

            // Remove all 'least similar' pairs until there are k pairs left
            if (heap.getSize() > k) {
                heap.removeMin();
            }
        }
    }

    vector<UserSimilarity> result;
    while (true) {
        try {
            result.push_back(heap.getMin());
            heap.removeMin();
        }
        catch (const runtime_error&) {
            break;
        }
    }

    reverse(result.begin(), result.end());
    return result;
}

// Find users by subscription type
vector<User> findUsersBySubscription(const vector<User>& users, const string& subscriptionType) {
    vector<User> result;

    for (const auto& user : users) {
        if (user.subscription == subscriptionType) {
            result.push_back(user);
        }
    }

    return result;
}

// Find most active users (highest watch time)
vector<User> findMostActiveUsers(const vector<User>& users, int k) {
    vector<User> sortedUsers = users;

    sort(sortedUsers.begin(), sortedUsers.end(), [](const User& a, const User& b) {
        return a.watchTime > b.watchTime;
        });

    vector<User> result;
    for (int i = 0; i < k && i < static_cast<int>(sortedUsers.size()); i++) {
        result.push_back(sortedUsers[i]);
    }

    return result;
}

// Generate sample data for testing
vector<User> generateSampleData() {
    vector<User> users;

    // Sample user data
    string genres[] = { "Comedy", "Drama", "Action", "Horror", "Romance", "Documentary" };
    string countries[] = { "USA", "Canada", "UK", "France", "Germany", "Japan" };
    string subscriptions[] = { "Basic", "Standard", "Premium" };

    for (int i = 1; i <= 20; i++) {
        User user;
        user.userID = i;
        user.name = "User" + to_string(i);
        user.age = 18 + (i % 40); // Ages from 18 to 57
        user.country = countries[i % 6];
        user.subscription = subscriptions[i % 3];
        user.watchTime = 10.0 + (i * 2.5); // Different watch times
        user.genre = genres[i % 6];
        user.lastLogin = "2023-04-" + (i < 10 ? "0" + to_string(i) : to_string(i));

        users.push_back(user);
    }

    return users;
}

// Display Menu
void displayMenu() {
    cout << "\n========== Netflix User Data Relationship Analyzer ==========\n";
    cout << "1. Load user data from CSV file\n";
    cout << "2. Generate sample data\n";
    cout << "3. Find most common genre for a specific age group\n";
    cout << "4. Find average watch time by country\n";
    cout << "5. Build and display user-genre relationship graph\n";
    cout << "6. Find most similar users\n";
    cout << "7. Find users by subscription type\n";
    cout << "8. Find most active users\n";
    cout << "9. Display all loaded users\n";
    cout << "0. Exit\n";
    cout << "=============================================================\n";
    cout << "Enter your choice: ";
}

// Main function - entry point for the application
int main() {
    vector<User> users;
    int choice;
    string filename;

    cout << "Welcome to FlixHabit: the Netflix User Data Relationship Analyzer!\n";

    do {
        displayMenu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

        switch (choice) {
        case 1: {
            cout << "Enter CSV filename: ";
            getline(cin, filename);

            /* Direct main to look in relative directory folder 'data' */
            const string dataWD = "../data/";

            /* User only has to enter the filename */
            string fullPath = dataWD + filename;

            users = readUsersFromCSV(fullPath);
            cout << "Loaded " << users.size() << " users from " << fullPath << endl;
            break;
        }
        case 2: {
            users = generateSampleData();
            cout << "Generated sample data with " << users.size() << " users." << endl;
            break;
        }
        case 3: {
            if (users.empty()) {
                cout << "No user data loaded. Please load data first." << endl;
                break;
            }

            int minAge, maxAge;
            cout << "Enter minimum age: ";
            cin >> minAge;
            cout << "Enter maximum age: ";
            cin >> maxAge;
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

            string genre = findMostCommonGenreForAgeGroup(users, minAge, maxAge);
            if (genre.empty()) {
                cout << "No users found in the specified age range." << endl;
            }
            else {
                cout << "Most common genre for age group " << minAge << "-" << maxAge << ": " << genre << endl;
            }
            break;
        }
        case 4: {
            if (users.empty()) {
                cout << "No user data loaded. Please load data first." << endl;
                break;
            }

            map<string, double> avgWatchTime = findAverageWatchTimeByCountry(users);
            cout << "Average watch time by country:\n";
            for (const auto& pair : avgWatchTime) {
                cout << pair.first << ": " << pair.second << " hours\n";
            }
            break;
        }
        case 5: {
            if (users.empty()) {
                cout << "No user data loaded. Please load data first." << endl;
                break;
            }

            Graph userGenreGraph = buildUserGenreGraph(users);
            cout << "User-Genre Relationship Graph:\n";
            userGenreGraph.printGraph();
            break;
        }
        case 6: {
            if (users.empty()) {
                cout << "No user data loaded. Please load data first." << endl;
                break;
            }

            int k;
            cout << "Enter the number of similar user pairs to find: ";
            cin >> k;
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

            vector<UserSimilarity> similarUsers = findMostSimilarUsers(users, k);
            cout << "Most similar users:\n";
            for (const auto& pair : similarUsers) {
                cout << "User " << pair.user1ID << " and User " << pair.user2ID
                    << " (Similarity score: " << pair.similarity << ")\n";
            }
            break;
        }
        case 7: {
            if (users.empty()) {
                cout << "No user data loaded. Please load data first." << endl;
                break;
            }

            string subType;
            cout << "Enter subscription type (Basic, Standard, Premium): ";
            getline(cin, subType);

            vector<User> filteredUsers = findUsersBySubscription(users, subType);
            if (filteredUsers.empty()) {
                cout << "No users found with " << subType << " subscription." << endl;
            }
            else {
                cout << "Users with " << subType << " subscription:\n";
                for (const auto& user : filteredUsers) {
                    cout << "User ID: " << user.userID << ", Name: " << user.name
                        << ", Country: " << user.country << ", Genre: " << user.genre << endl;
                }
            }
            break;
        }
        case 8: {
            if (users.empty()) {
                cout << "No user data loaded. Please load data first." << endl;
                break;
            }

            int k;
            cout << "Enter the number of most active users to find: ";
            cin >> k;
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

            vector<User> activeUsers = findMostActiveUsers(users, k);
            cout << "Most active users:\n";
            for (const auto& user : activeUsers) {
                cout << "User ID: " << user.userID << ", Name: " << user.name
                    << ", Watch Time: " << user.watchTime << " hours, Genre: " << user.genre << endl;
            }
            break;
        }
        case 9: {
            if (users.empty()) {
                cout << "No user data loaded. Please load data first." << endl;
                break;
            }

            cout << "All loaded users:\n";
            cout << "ID\tName\tAge\tCountry\tSubscription\tWatch Time\tGenre\tLast Login\n";
            cout << "-------------------------------------------------------------------------------------\n";
            for (const auto& user : users) {
                cout << user.userID << "\t" << user.name << "\t" << user.age << "\t"
                    << user.country << "\t" << user.subscription << "\t\t"
                    << user.watchTime << "\t\t" << user.genre << "\t" << user.lastLogin << endl;
            }
            break;
        }
        case 0:
            cout << "Exiting program. Goodbye!\n";
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    } while (choice != 0);

    return 0;
}