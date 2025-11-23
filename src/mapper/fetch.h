#ifndef FETCH_H
#define FETCH_H

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <fstream>
#include <sstream>
#include "../structures/hashmap.h"

using namespace std;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define CYAN    "\033[36m"
#define BOLD    "\033[1m"

string toupper(const string &str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(),[](unsigned char c){ return toupper(c); });
    return result;
}

void show_tree(const HashMap<string, vector<string>>& tree)
{
    cout << endl;
    for (auto p : tree) {
        cout << BOLD << BLUE << "[DB] " << RESET << p.first << endl;

        for (size_t i = 0; i < p.second.size(); i++) {
            string indent = "   ";
            cout << indent << CYAN << "|-- " << RESET << p.second[i] << endl;
        }
        cout << endl;
    }
}


HashMap<string, vector<string>> fetch_structure() {
    HashMap<string, vector<string>> tree;
    string path = "./structure/structure.csv";
    ifstream file(path);

    if (!file.is_open()) {
        cerr << "Failed to open file: " << path << endl;
        return tree;
    }

    string line;
    while (getline(file, line)) {
        // Trim whitespace
        line.erase(0, line.find_first_not_of(" \t\r\n"));
        line.erase(line.find_last_not_of(" \t\r\n") + 1);

        if (line.empty()) continue;  // skip blank lines

        stringstream ss(line);
        string db, table;
        getline(ss, db, ',');
        db = toupper(db);

        if (db.empty()) continue; // skip malformed line

        vector<string> tables;
        while (getline(ss, table, ',')) {
            table.erase(0, table.find_first_not_of(" \t\r\n"));
            table.erase(table.find_last_not_of(" \t\r\n") + 1);
            if (!table.empty()) {
                tables.push_back(toupper(table));
            }
        }

        tree.insert(db, tables);
    }

    file.close();
    return tree;
}

// THE TASKS BELOW WORK TO UPDATE THE structure FOLDER --

void save_structure(HashMap<string, vector<string>>& tree) {
    string path = "./structure/structure.csv";
    ofstream file(path);

    if (!file.is_open()) {
        cerr << "Failed to write to file: " << path << endl;
        return;
    }

    for (auto p : tree) {
        file << p.first;
        for (const string& table : p.second) {
            file << "," << table;
        }
        file << "\n";
    }

    file.close();
}

void add_database(HashMap<string, vector<string>> &tree,string db_name) {
    db_name = toupper(db_name);
    if (tree.contains(db_name)) {
        cout << "Database already exists.\n";
        return;
    }

    tree[db_name] = {};
    save_structure(tree);
    cout << "Created Database '" << db_name << "'..."<<endl;
}

void add_table(HashMap<string, vector<string>> &tree,string db_name,string table_name) {
    db_name = toupper(db_name);
    table_name = toupper(table_name);

    tree[db_name].push_back(table_name);

    save_structure(tree);
    cout << "Created table '" << table_name << "'..." << endl;

}

void remove_database(HashMap<string, vector<string>> &tree, string db_name) {
    db_name = toupper(db_name);

    if (!tree.contains(db_name)) {
        return;
    }

    tree.erase(db_name);
    save_structure(tree);
}

void remove_table(HashMap<string, vector<string>> &tree, string db_name, string table_name) {
    db_name = toupper(db_name);
    table_name = toupper(table_name);

    vector<string> &tables = tree[db_name];
    auto it = find(tables.begin(), tables.end(), table_name);
    if (it != tables.end()) {
        tables.erase(it);
        save_structure(tree);
    }
}

#endif // FETCH_H
