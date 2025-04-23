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
#include <iomanip>   
#include <nlohmann/json.hpp>
#include <filesystem>
#include <set>
#include <chrono>

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

// Optimized function to build graph of user relationships based on genre preferences
Graph buildUserGenreGraph(const vector<User>& users) {
    Graph graph;

    // Create vertices for each unique genre
    map<string, bool> genres;
    for (const auto& user : users) {
        genres[user.genre] = true;
    }

    for (const auto& genre : genres) {
        graph.addVertex(genre.first);
    }

    // Track which genre pairs we've already connected to avoid duplicates
    map<pair<string, string>, bool> connectedPairs;

    // Create meaningful connections between genres
    // We'll connect genres that have users with significant similarities
    for (size_t i = 0; i < users.size() && i < 100; i++) {  // Limit to first 100 users for performance
        for (size_t j = i + 1; j < users.size() && j < 100; j++) {  // Limit to first 100 users
            // Only connect different genres
            if (users[i].genre != users[j].genre) {
                // Sort genre names to create consistent key
                string genre1 = users[i].genre;
                string genre2 = users[j].genre;
                if (genre1 > genre2) {
                    swap(genre1, genre2);
                }

                // Check if we've already connected these genres
                pair<string, string> genrePair = {genre1, genre2};
                if (!connectedPairs[genrePair]) {
                    // Calculate similarity between users
                    double similarity = calculateSimilarity(users[i], users[j]);

                    // Only connect if there's meaningful similarity (threshold of 70)
                    if (similarity > 70.0) {
                        graph.addEdge(genre1, genre2);
                        connectedPairs[genrePair] = true;
                    }
                }
            }
        }
    }

    return graph;
}

using json = nlohmann::json;
// Convert graph into a json object
json toJson(const Graph& graph) {
    json j;
    j["nodes"] = json::array();
    j["edges"] = json::array();

    for (auto const& kv : graph.getAdjList()) {
        j["nodes"].push_back({
            {"id",    kv.first},
            {"label", kv.first}
        });
    }

    set<pair<string,string>> seen;
    for (auto const& kv : graph.getAdjList()) {
        for (auto const& nbr : kv.second) {
            auto p = minmax(kv.first, nbr);
            if (seen.insert(p).second) {
                j["edges"].push_back({
                    {"from", kv.first},
                    {"to",   nbr}
                });
            }
        }
    }

    return j;
}

void exportGraphToJson(const Graph& graph, const string& filepath) {
    json j = toJson(graph);
    ofstream ofs(filepath);
    ofs << j.dump(2) << endl;
}


// Optimized function to find most similar users
vector<UserSimilarity> findMostSimilarUsers(const vector<User>& users, unsigned int k) {
    // Limit the number of user comparisons for better performance
    const size_t MAX_USERS = 100;  // Limit to first 100 users

    // Create a vector for the top k similar pairs
    vector<UserSimilarity> topSimilarities;
    double lowestSimilarityInTop = -1.0;  // Keep track of the lowest similarity in our top k

    // Calculate similarity only for a reasonable number of users
    for (size_t i = 0; i < users.size() && i < MAX_USERS; i++) {
        for (size_t j = i + 1; j < users.size() && j < MAX_USERS; j++) {
            double similarityScore = calculateSimilarity(users[i], users[j]);

            // Only process this pair if it might be in the top k
            if (topSimilarities.size() < k || similarityScore > lowestSimilarityInTop) {
                UserSimilarity similarity;
                similarity.user1ID = users[i].userID;
                similarity.user2ID = users[j].userID;
                similarity.similarity = similarityScore;

                // Add to our collection
                topSimilarities.push_back(similarity);

                // If we exceed k elements, remove the lowest and update lowestSimilarityInTop
                if (topSimilarities.size() > k) {
                    // Find the index of the minimum similarity
                    int minIndex = 0;
                    for (size_t idx = 1; idx < topSimilarities.size(); idx++) {
                        if (topSimilarities[idx].similarity < topSimilarities[minIndex].similarity) {
                            minIndex = idx;
                        }
                    }

                    // Remove the lowest similarity from our collection
                    lowestSimilarityInTop = topSimilarities[minIndex].similarity;
                    topSimilarities.erase(topSimilarities.begin() + minIndex);

                    // Update the lowest similarity in our collection
                    lowestSimilarityInTop = numeric_limits<double>::max();
                    for (const auto& sim : topSimilarities) {
                        if (sim.similarity < lowestSimilarityInTop) {
                            lowestSimilarityInTop = sim.similarity;
                        }
                    }
                }
            }
        }
    }

    // Sort the final results in descending order of similarity
    sort(topSimilarities.begin(), topSimilarities.end(),
         [](const UserSimilarity& a, const UserSimilarity& b) {
             return a.similarity > b.similarity;
         });

    return topSimilarities;
}

void writeSimilaritiesToJSON(const vector<UserSimilarity>& sims,
                             const filesystem::path& filePath)
{
    namespace fs = filesystem;
    using nlohmann::json;

    fs::create_directories(filePath.parent_path());

    json j = json::array();
    for (const auto& s : sims) {
        j.push_back({
            {"user1ID",    s.user1ID},
            {"user2ID",    s.user2ID},
            {"similarity", s.similarity}
        });
    }

    ofstream out(filePath);
    if (!out) {
        cerr << "Cannot write to " << fs::absolute(filePath) << '\n';
        return;
    }
    out << j.dump(2);
    cout << "Wrote " << sims.size() << " pairs → "
              << fs::absolute(filePath) << '\n';
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

nlohmann::json usersToJson(const vector<User>& users)
{
    using nlohmann::json;

    json arr = json::array();
    for (const auto& u : users)
        arr.push_back({
            {"userID",    u.userID},
            {"name",      u.name},
            {"age",       u.age},
            {"country",   u.country},
            {"subscription", u.subscription},
            {"watchTime", u.watchTime},
            {"genre",     u.genre},
            {"lastLogin", u.lastLogin}
        });

    return arr;        // e.g. [ {…}, {…}, … ]
}

bool writeJsonToFile(const nlohmann::json& j,
                     const filesystem::path& filePath,
                     int indent = 2)      
{
    namespace fs = filesystem;
    fs::create_directories(filePath.parent_path());   

    ofstream out(filePath);
    if (!out) {
        cerr << "Cannot open " << fs::absolute(filePath) << '\n';
        return false;
    }
    out << j.dump(indent);
    return true;
}


// Find most active users (Min Fixed Size Heap)
vector<User> findMostActiveUsers(const vector<User>& users, int k) 
{
    FixedMinHeap<UserWatch> heap(k);

    for (auto& u : users)
    { 
        UserWatch uw{ u.watchTime, u };
        heap.insert(uw);
    }

    vector<User> result;
    vector<UserWatch> buf;
    while (heap.empty() == false) 
    {
        buf.push_back(heap.getMin());
        heap.removeMin();

    }

    reverse(buf.begin(), buf.end());

    for (auto& uw : buf)
       { result.push_back(uw.user); }

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

            int minAge = 15, maxAge = 80;
            nlohmann::json buckets = nlohmann::json::array();

            // cout << "Enter minimum age: ";
            // cin >> minAge;
            // cout << "Enter maximum age: ";
            // cin >> maxAge;
            // cin.ignore(numeric_limits<streamsize>::max(), '\n'); // Clear input buffer

            for (int lo = minAge; lo < maxAge; lo += 5) {
                int hi = lo + 5;            // 15‑20, 20‑25

                string genre = findMostCommonGenreForAgeGroup(users, lo, hi);

                if (genre.empty()) {
                    cout << "  " << lo << "-" << hi << ": (no users)\n";
                    continue;
                }

                cout << "  " << lo << "-" << hi << ": " << genre << '\n';

                buckets.push_back({
                    {"ageRange", to_string(lo) + "-" + to_string(hi)},
                    {"genre",    genre}
                });
            }

            // Write the whole array once
            {
                filesystem::create_directories("../frontend/public/data");
                ofstream out("../frontend/flixhabit-frontend/public/data/genreForAgeGroup.json");
                out << buckets.dump(2);
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

            nlohmann::json j;
            for (const auto& [country, hrs] : avgWatchTime)
                j[country] = hrs;

            // ④ write it once
            {
                namespace fs = filesystem;
                fs::create_directories("../frontend/public/data");
                ofstream out("../frontend/flixhabit-frontend/public/data/avgWatchTimeByCountry.json");
                out << j.dump(2);
            }

            break;
        }
        case 5: {
            if (users.empty()) {
                cout << "No user data loaded. Please load data first." << endl;
                break;
            }

            Graph userGenreGraph = buildUserGenreGraph(users);
            // Export the user-genre graph to JSON for frontend visualization
            exportGraphToJson(userGenreGraph,"../frontend/flixhabit-frontend/public/data/genre_graph.json");
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
            writeSimilaritiesToJSON(similarUsers, "../frontend/flixhabit-frontend/public/data/similar_users.json");

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
            nlohmann::json j = usersToJson(filteredUsers);

            writeJsonToFile(j,
                "../frontend/flixhabit-frontend/public/data/" + subType + "_users.json");

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

            cout << "Process with:\n";
            cout << "1. Fixed-Size Min-Heap\n";  
            cout << "2. ActivityGraph\n";
            cout << "Enter choice: ";

            int structureChoice;
            cin >> structureChoice;
            cin.ignore(numeric_limits<streamsize>::max(), '\n');

            vector<User> activeUsers; 

            if (structureChoice == 1)
            {
                /* Measure the elapsed time of the dataset using FIXED SIZE HEAP */
                /* Source: https://cplusplus.com/reference/chrono/high_resolution_clock/ */
                auto heapStart = chrono::high_resolution_clock::now();
                vector<User> heapResult = findMostActiveUsers(users, k);
                activeUsers = heapResult;
                auto heapFinish = chrono::high_resolution_clock::now();
                auto heapUS = chrono::duration_cast<chrono::microseconds>(heapFinish - heapStart).count();

                cout << "Processed user database in " << heapUS << " μs using a Min Heap." << endl;

                for (auto& u : activeUsers)
                   { cout << "User " << u.userID << " - " << u.watchTime << " h\n"; }
            }
            else if (structureChoice == 2)
            {
                /* Find the index of the user with the highest watch time */
                int highestWatch = 0;
                for (int i = 1; i < (int)users.size(); ++i)
                {
                    if (users[i].watchTime > users[highestWatch].watchTime) { highestWatch = i; }
                }

                /* Measure the elapsed time of the dataset using GRAPH */
                /* Source: https://cplusplus.com/reference/chrono/high_resolution_clock/ */
                auto graphStart = chrono::high_resolution_clock::now();

                /* Build the graph */
                ActivityGraph ag = buildActivityGraph(users, highestWatch);

                auto graphResult = ag.topKClosest(highestWatch, k);
                auto graphFinish = chrono::high_resolution_clock::now();
                auto graphUS = chrono::duration_cast<chrono::microseconds>(graphFinish - graphStart).count();

                // 3) Map the indices back to User objects
                activeUsers.clear();
                activeUsers.reserve(graphResult.size());

                for (int index : graphResult)
                   { activeUsers.push_back(users[index]); }

                cout << "Processed user database in " << graphUS << " μs using a Graph." << endl;

                cout << "Most active users:\n";
                for (const auto& user : activeUsers) 
                {
                    cout << "User ID: " << user.userID 
                        << ", Name: " << user.name 
                        << ", Watch Time: " << user.watchTime 
                        << " hours, Genre: " << user.genre << endl;
                }

            }

            nlohmann::json j = usersToJson(activeUsers);

            writeJsonToFile(j,
                "../frontend/flixhabit-frontend/public/data/topActive_users.json");

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
