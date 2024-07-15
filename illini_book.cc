#include "illini_book.hpp"
#include "utilities.hpp"

#include <fstream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <utility>
#include <vector>

// Your code here!

IlliniBook::IlliniBook(const std::string& people_fpath, const std::string& relations_fpath) {
    std::ifstream people_file(people_fpath);
    std::ifstream relations_file(relations_fpath);
    std::string line;

    // Load people
    while (std::getline(people_file, line)) {
        if (!line.empty()) {
            people_.push_back(std::stoi(line));
        }
    }

    // Load relationships
    while (std::getline(relations_file, line)) {
        if (!line.empty()) {
            std::vector<std::string> parts = utilities::Split(line, ',');
            if (parts.size() == 3) {
                int uin_a = std::stoi(parts[0]);
                int uin_b = std::stoi(parts[1]);
                std::string relationship = parts[2];

                // Store the relationships bi-directionally assuming relationships are symmetric
                relations_[uin_a].push_back({uin_b, relationship});
                relations_[uin_b].push_back({uin_a, relationship});
            }
        }
    }

    people_file.close();
    relations_file.close();
}

IlliniBook::~IlliniBook() = default;

bool IlliniBook::AreRelated(int uin_1, int uin_2) const {
    if (uin_1 == uin_2) return true; // Immediate return if both UINs are the same

    // Check if both UINs exist in the graph
    auto it1 = relations_.find(uin_1);
    auto it2 = relations_.find(uin_2);
    if (it1 == relations_.end() || it2 == relations_.end()) return false; // If either UIN does not exist

    std::queue<int> q;
    std::set<int> visited;

    // Start BFS from uin_1
    q.push(uin_1);
    visited.insert(uin_1);

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        // Iterate through all neighbors of the current node
        for (const auto& pair : relations_.at(current)) {
            int neighbor = pair.first;
            if (neighbor == uin_2) return true; // uin_2 found, they are related

            // Only add unvisited neighbors to the queue
            if (visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                q.push(neighbor);
            }
        }
    }

    return false; // No path found between uin_1 and uin_2
}

bool IlliniBook::AreRelated(int uin_1, int uin_2, const std::string &relationship) const{
    if (uin_1 == uin_2) return true; // Immediate return if both UINs are the same

    // Check if both UINs exist in the graph
    auto it1 = relations_.find(uin_1);
    auto it2 = relations_.find(uin_2);
    if (it1 == relations_.end() || it2 == relations_.end()) return false; // If either UIN does not exist

    std::queue<int> q;
    std::set<int> visited;

    // Start BFS from uin_1
    q.push(uin_1);
    visited.insert(uin_1);

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        // Iterate through all neighbors of the current node
        for (const auto& pair : relations_.at(current)) {
            int neighbor = pair.first;
            std::string rel = pair.second;
            if (rel == relationship) { // Check the relationship
                if (neighbor == uin_2) return true; // uin_2 found with the right relationship

                // Only add unvisited neighbors to the queue
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    q.push(neighbor);
                }
            }
        }
    }
    return false; // No path found with the specified relationship
}

int IlliniBook::GetRelated(int uin_1, int uin_2) const{
    if (uin_1 == uin_2) return 0; // Shortest path to itself is always 0

    // Check if both UINs exist in the graph
    auto it1 = relations_.find(uin_1);
    auto it2 = relations_.find(uin_2);
    if (it1 == relations_.end() || it2 == relations_.end()) return -1; // If either UIN does not exist

    std::queue<std::pair<int, int>> q; // queue of pairs (node, distance)
    std::set<int> visited;

    q.push({uin_1, 0});
    visited.insert(uin_1);

    while (!q.empty()) {
        auto [current, dist] = q.front();
        q.pop();

        // Explore all neighbors
        for (const auto& pair : relations_.at(current)) {
            int neighbor = pair.first;
            if (neighbor == uin_2) return dist + 1;

            if (visited.insert(neighbor).second) { // Check if inserted (newly visited)
                q.push({neighbor, dist + 1});
            }
        }
    }
    return -1; // If no path is found
}

int IlliniBook::GetRelated(int uin_1, int uin_2, const std::string &relationship) const {
    if (uin_1 == uin_2) return 0;  // Immediate return for the same node

    // Check if both UINs exist in the graph and there is a possibility of connection
    if (relations_.find(uin_1) == relations_.end() || relations_.find(uin_2) == relations_.end()) {
        return -1;
    }

    std::queue<std::pair<int, int>> q;
    std::set<int> visited;
    q.push({uin_1, 0});
    visited.insert(uin_1);

    while (!q.empty()) {
        auto [current, dist] = q.front();
        q.pop();

        // Explore all neighbors with the specific relationship
        for (const auto& pair : relations_.at(current)) {
            int neighbor = pair.first;
            std::string rel = pair.second;
            if (rel == relationship) {
                if (neighbor == uin_2) return dist + 1;
                if (visited.insert(neighbor).second) {
                    q.push({neighbor, dist + 1});
                }
            }
        }
    }
    return -1;  // No path found that matches the specific relationship
}

std::vector<int> IlliniBook::GetSteps(int uin, int n) const{
    std::vector<int> result;
    if (relations_.find(uin) == relations_.end() || n < 0) return result;  // Return empty if start node not found or n is negative

    std::queue<std::pair<int, int>> q;  // Pair of (node, current depth)
    std::set<int> visited;
    q.push({uin, 0});
    visited.insert(uin);

    while (!q.empty()) {
        auto [current, depth] = q.front();
        q.pop();

        // Process nodes at the exact depth
        if (depth == n) {
            result.push_back(current);
            continue;  // Do not go deeper than n levels
        }

        // Add neighbors to the queue
        for (const auto& relation : relations_.at(current)) {
            int neighbor = relation.first;
            if (visited.insert(neighbor).second) {
                q.push({neighbor, depth + 1});
            }
        }
    }

    // Ensure nodes are only added if at the exact depth of n
    if (n == 0) return {uin};  // Special case where n is 0
    return result;
}

size_t IlliniBook::CountGroups() const {
    std::set<int> visited;  // Tracks visited nodes
    size_t number_of_groups = 0;
    std::set<int> all_nodes;
    for (const auto& rel : relations_) {
        all_nodes.insert(rel.first);  // include source
        for (const auto& edge : rel.second) {
            all_nodes.insert(edge.first);  // include destination
        }
    }
    for (int node : people_) {
        all_nodes.insert(node);
    }
    for (int node : all_nodes) {
        if (visited.find(node) == visited.end()) {
            number_of_groups++;  // Found a new group
            std::queue<int> queue;
            queue.push(node);
            visited.insert(node);

            while (!queue.empty()) {
                int current = queue.front();
                queue.pop();

                // Check and traverse all connected nodes
                if (relations_.find(current) != relations_.end()) {
                    for (const auto& relation : relations_.at(current)) {
                        int neighbor = relation.first;
                        if (visited.insert(neighbor).second) {
                            queue.push(neighbor);
                        }
                    }
                }
            }
        }
    }
    return number_of_groups;
}


size_t IlliniBook::CountGroups(const std::string &relationship) const{
    std::set<int> visited;  // Tracks visited nodes to prevent double-counting
    size_t number_of_groups = 0;

    // Prepare a map to efficiently check connections by the specified relationship
    std::map<int, std::vector<int>> connections_by_relationship;
    for (const auto& pair : relations_) {
        for (const auto& rel : pair.second) {
            if (rel.second == relationship) {
                connections_by_relationship[pair.first].push_back(rel.first);
                // Ensure bidirectional connectivity since relationships are symmetric
                connections_by_relationship[rel.first].push_back(pair.first);
            }
        }
    }

    // Iterate over each person; consider all people to catch isolated nodes
    for (int uin : people_) {
        if (visited.find(uin) == visited.end()) { // If this UIN has not been visited
            number_of_groups++; // A new group is found
            std::queue<int> queue;
            queue.push(uin);
            visited.insert(uin);

            while (!queue.empty()) {
                int current = queue.front();
                queue.pop();

                // Process all valid connections for the current node within the specified relationship
                for (int neighbor : connections_by_relationship[current]) {
                    if (visited.find(neighbor) == visited.end()) {
                        visited.insert(neighbor);
                        queue.push(neighbor);
                    }
                }
            }
        }
    }

    return number_of_groups;
}

size_t IlliniBook::CountGroups(const std::vector<std::string> &relationships) const{
    std::set<int> visited_nodes;
    size_t group_count = 0;

    // Iterate over each unique identifier in the system
    for (int unique_id : people_) {  // Assuming 'people_' holds all the UINs
        if (visited_nodes.find(unique_id) == visited_nodes.end()) {
            group_count++;  // New group found
            std::queue<int> processing_queue;
            visited_nodes.insert(unique_id);
            processing_queue.push(unique_id);

            while (!processing_queue.empty()) {
                int current_node = processing_queue.front();
                processing_queue.pop();

                // Check all connections for the current node
                if (relations_.find(current_node) != relations_.end()) {
                    for (const auto& relation : relations_.at(current_node)) {
                        int connected_node = relation.first;
                        std::string relationship_type = relation.second;

                        // Ensure the relationship is one of the specified ones and the node hasn't been visited
                        if (visited_nodes.find(connected_node) == visited_nodes.end() &&
                            std::find(relationships.begin(), relationships.end(), relationship_type) != relationships.end()) {
                            processing_queue.push(connected_node);
                            visited_nodes.insert(connected_node);
                        }
                    }
                }
            }
        }
    }
    return group_count;
}
