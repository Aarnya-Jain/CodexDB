#ifndef TABLES_H
#define TABLES_H

#include <filesystem>
#include <fstream>
#include <sstream>
#include <string.h>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <unordered_set>
#include <deque>
#include "../mapper/fetch.h"
#include "../structures/rbmap.h"

#define GREEN   "\033[32m"
#define RESET   "\033[0m"

using namespace std;

// func to check if string is a number
bool isNumber(const string &s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!isdigit((unsigned char)c)) return false;
    }
    return true;
}

struct Row {
    RBMap<string, string> data;
};

class Table {
public:
    RBMap<string, bool> columns;
    deque<Row> rows;
    // Indexing: Column Name -> Value -> List of Rows
    RBMap<string, RBMap<int, vector<Row*>>> indexNum;
    RBMap<string, RBMap<string, vector<Row*>>> indexStr;

    void addColumn(const string &col) {
        if (!columns.count(col)) {
            columns[col] = true;
            for (auto &r : rows) r.data[col] = "";
        }
    }

    void insertRow(const RBMap<string,string> &input) {
        for (auto it = input.begin(); it != input.end(); ++it) {
            string colInInput = (*it).first;
            if (!columns.count(colInInput)) addColumn(colInInput);
        }

        rows.push_back(Row());
        Row* rPtr = &rows.back();

        for (auto it = columns.begin(); it != columns.end(); ++it) {
            string col = (*it).first;
            auto found = input.find(col);
            if (found != input.end()) {
                rPtr->data[col] = (*found).second;
            } else {
                rPtr->data[col] = "";
            }
        }

        // Update Indexes
        for (auto it = rPtr->data.begin(); it != rPtr->data.end(); ++it) {
            string col = (*it).first;
            string val = (*it).second;

            if (isNumber(val)) {
                int num = stoi(val);
                indexNum[col][num].push_back(rPtr);
            } else {
                indexStr[col][val].push_back(rPtr);
            }
        }
    }

    vector<Row*> selectEqual(const string &col, const string &val) {
        if (!columns.count(col)) return {};
        if (isNumber(val)) {
            int x = stoi(val);
            auto it = indexNum[col].find(x);
            if (it != indexNum[col].end()) return (*it).second;
            return {};
        } else {
            auto it = indexStr[col].find(val);
            if (it != indexStr[col].end()) return (*it).second;
            return {};
        }
    }

    vector<Row*> selectGreater(const string &col, const string &val) {
        vector<Row*> out;
        if (!columns.count(col)) return out;

        if (isNumber(val)) {
            int x = stoi(val);
            for (auto it = indexNum[col].upper_bound(x); it != indexNum[col].end(); ++it)
                out.insert(out.end(), (*it).second.begin(), (*it).second.end());
        } else {
            for (auto it = indexStr[col].upper_bound(val); it != indexStr[col].end(); ++it)
                out.insert(out.end(), (*it).second.begin(), (*it).second.end());
        }
        return out;
    }

    vector<Row*> selectLess(const string &col, const string &val) {
        vector<Row*> out;
        if (!columns.count(col)) return out;

        if (isNumber(val)) {
            int x = stoi(val);
            for (auto it = indexNum[col].begin(); it != indexNum[col].lower_bound(x); ++it)
                out.insert(out.end(), (*it).second.begin(), (*it).second.end());
        } else {
            for (auto it = indexStr[col].begin(); it != indexStr[col].lower_bound(val); ++it)
                out.insert(out.end(), (*it).second.begin(), (*it).second.end());
        }
        return out;
    }
};


void write_table(vector<vector<string>> data, string owner_database, string table_name) {
    owner_database = toupper(owner_database);
    table_name = toupper(table_name);

    string path = "./data/"+owner_database+"/"+table_name+".csv";
    ofstream file(path);

    if (!file.is_open()) {
        cerr << "Error: Could not open file " << path << " for writing." << endl;
        return;
    }

    for (const auto& row : data) {
        for (size_t i = 0; i < row.size(); ++i) {
            file << row[i];
            if (i != row.size() - 1)
                file << ",";
        }
        file << "\n";
    }

    file.close();
}

void displaytable(vector<vector<string>> data) {
    if (data.empty() || data[0].empty()) return;

    int rows = data.size();
    int cols = data[0].size();
    int cellWidth = 0;

    for (int j = 0; j < cols; j++) {
        int len = data[0][j].length();
        if (len > cellWidth) cellWidth = len;
    }
    cellWidth += 6;

    for (int j = 0; j < cellWidth * cols; j++) cout << GREEN << "=" << RESET;
    cout << endl;

    for (int j = 0; j < cols; j++) {
        cout << setw(cellWidth - 2) << data[0][j] << GREEN << " |" << RESET;
    }
    cout << endl;

    for (int j = 0; j < cellWidth * cols; j++) cout << GREEN << "=" << RESET;
    cout << endl;

    for (int i = 1; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << setw(cellWidth - 2) << data[i][j] << GREEN << " |" << RESET;
        }
        cout << endl;
        for (int j = 0; j < cellWidth * cols; j++) cout << GREEN << "-" << RESET;
        cout << endl;
    }
}

// Convert vector<vector<string>> to Table
void vectorToTable(vector<vector<string>> &data, Table &t) {
    t.rows.clear();
    t.columns.clear();
    t.indexNum.clear();
    t.indexStr.clear();

    if (data.empty()) return;

    // Add columns from header
    for (const string &col : data[0]) {
        t.addColumn(col);
    }

    // Add rows
    for (size_t i = 1; i < data.size(); i++) {
        RBMap<string, string> rowData;
        for (size_t j = 0; j < data[i].size() && j < data[0].size(); j++) {
            rowData[data[0][j]] = data[i][j];
        }
        t.insertRow(rowData);
    }
}

class tables {
public:
    vector<vector<string>> data;
    string name;
    Table internalTable;

    tables() {}

    void create_table(string owner_database, string table_name) {
        owner_database = toupper(owner_database);
        table_name = toupper(table_name);
        string path = "./data/"+owner_database+"/"+table_name+".csv";
        ofstream table(path);
    }

    void delete_table(string owner_database, string table_name) {
        owner_database = toupper(owner_database);
        table_name = toupper(table_name);
        string path = "./data/" + owner_database + "/" + table_name + ".csv";
        if (filesystem::exists(path)) {
            filesystem::remove(path);
            cout << "Table '" << table_name << "' deleted successfully.\n";
        } else {
            cout << "Table '" << table_name << "' does not exist.\n";
        }
    }

    vector<vector<string>> read(string owner_database, string table_name) {
        data.clear();
        owner_database = toupper(owner_database);
        table_name = toupper(table_name);
        string path = "./data/"+owner_database+"/"+table_name+".csv";
        ifstream file(path);
        string line;

        while(getline(file,line)){
            stringstream ss(line);
            string cell;
            vector<string> row;

            while (getline(ss, cell, ',')) {
                row.push_back(cell);
            }

            data.push_back(row);
        }

        // Build Index
        vectorToTable(data, internalTable);

        return data;
    }

    vector<vector<string>> refine(vector<vector<string>> data) {
        if (data.empty()) return data;
        int maxi = data[0].size();
        int mini = data[data.size()-1].size();
        for(int i=0;i<maxi-mini;i++){
            data[data.size()-1].push_back("-");
        }
        return data;
    }

    void addcol(vector<vector<string>> data, vector<string> newcol, string owner_database, string table_name) {
        if(data.size()<newcol.size()){
            cout << "Error : Too many arguements for column values ..."<<endl;
            return;
        } else if(newcol.size()<data.size()){
            int size = data.size() - newcol.size();
            for(int i=0;i<size;i++){
                newcol.push_back("-");
            }
        }

        for(size_t i=0;i<data.size();i++){
            data[i].push_back(newcol[i]);
        }

        write_table(data,owner_database,table_name);
        cout << "Column inserted successfully ..." << endl;
    }

    void addrow(vector<string> rows, vector<vector<string>> data, string owner_database, string table_name) {
        if(data.empty()){
            data.push_back(rows);
            write_table(data,owner_database,table_name);
            return;
        }

        size_t maxi = data[0].size();
        for(size_t i=0;i<data.size();i++){
            if(data[i].size()>maxi){
                maxi = data[i].size();
            }
        }

        if(rows.size()>maxi){
            cout << "Values exceed the no. of columns ..." <<endl;
            return;
        }

        data.push_back(rows);
        data = refine(data);
        write_table(data,owner_database,table_name);
        cout << "Row inserted successfully ..." << endl;
    }

    void deletecol(vector<vector<string>> data, string colname, string owner_database, string table_name) {
        int idx=-1;
        for(size_t i=0;i<data[0].size();i++){
            if(data[0][i]==colname){
                idx=i;break;
            }
        }

        if(idx==-1){
            cout << "Column not found ... "<<endl;
            return;
        }

        for(size_t i=idx;i<data[0].size()-1;i++){
            for(size_t j=0;j<data.size();j++){
                data[j][idx]=data[j][idx+1];
            }
        }
        for(size_t i=0;i<data.size();i++){
            data[i].pop_back();
        }

        write_table(data,owner_database,table_name);
        cout << "Column \""<<colname<<"\" deleted successfully ..." << endl;
    }

    void deleterow(vector<vector<string>> data, string colname, string colval, string op, string owner_database, string table_name) {
        int idx=-1;
        for(size_t i=0;i<data[0].size();i++){
            if(data[0][i]==colname){
                idx=i;break;
            }
        }
        if(idx==-1)return;

        // Check if we are comparing numbers
        bool isNumComparison = isNumber(colval);

        if(op=="="){
            for(size_t i=1;i<data.size();i++){
                bool match = false;
                if (isNumComparison && isNumber(data[i][idx])) {
                    if (stoll(data[i][idx]) == stoll(colval)) match = true;
                } else {
                    if (data[i][idx] == colval) match = true;
                }

                if(match){
                    for(size_t j=i;j<data.size()-1;j++){
                        data[j]=data[j+1];
                    }
                    data.pop_back();i--;
                }
            }
        }
        else if(op==">"){
             for(size_t i=1;i<data.size();i++){
                bool match = false;
                if (isNumComparison && isNumber(data[i][idx])) {
                    if (stoll(data[i][idx]) > stoll(colval)) match = true;
                } else {
                    if (data[i][idx] > colval) match = true;
                }

                if(match){
                    for(size_t j=i;j<data.size()-1;j++){
                        data[j]=data[j+1];
                    }
                    data.pop_back();i--;
                }
            }
        }
        else if(op=="<"){
             for(size_t i=1;i<data.size();i++){
                bool match = false;
                if (isNumComparison && isNumber(data[i][idx])) {
                    if (stoll(data[i][idx]) < stoll(colval)) match = true;
                } else {
                    if (data[i][idx] < colval) match = true;
                }

                if(match){
                    for(size_t j=i;j<data.size()-1;j++){
                        data[j]=data[j+1];
                    }
                    data.pop_back();i--;
                }
            }
        }
        else if(op==">="){
             for(size_t i=1;i<data.size();i++){
                bool match = false;
                if (isNumComparison && isNumber(data[i][idx])) {
                    if (stoll(data[i][idx]) >= stoll(colval)) match = true;
                } else {
                    if (data[i][idx] >= colval) match = true;
                }

                if(match){
                    for(size_t j=i;j<data.size()-1;j++){
                        data[j]=data[j+1];
                    }
                    data.pop_back();i--;
                }
            }
        }
        else if(op=="<="){
             for(size_t i=1;i<data.size();i++){
                bool match = false;
                if (isNumComparison && isNumber(data[i][idx])) {
                    if (stoll(data[i][idx]) <= stoll(colval)) match = true;
                } else {
                    if (data[i][idx] <= colval) match = true;
                }

                if(match){
                    for(size_t j=i;j<data.size()-1;j++){
                        data[j]=data[j+1];
                    }
                    data.pop_back();i--;
                }
            }
        }
        else if(op=="!="){
             for(size_t i=1;i<data.size();i++){
                bool match = false;
                if (isNumComparison && isNumber(data[i][idx])) {
                    if (stoll(data[i][idx]) != stoll(colval)) match = true;
                } else {
                    if (data[i][idx] != colval) match = true;
                }

                if(match){
                    for(size_t j=i;j<data.size()-1;j++){
                        data[j]=data[j+1];
                    }
                    data.pop_back();i--;
                }
            }
        }

        write_table(data,owner_database,table_name);
        cout << "Row deleted successfully ..." << endl;
    }

    vector<vector<string>> sort_asc(vector<vector<string>> vec, string column) {
        int idx=-1;
        for(size_t i=0;i<vec[0].size();i++){
            if(vec[0][i]==column){
                idx=i;
                break;
            }
        }

        for(size_t i=1;i<vec.size();i++){
            for(size_t j=1;j<vec.size()-i;j++){
                if(vec[j][idx]>vec[j+1][idx]){
                    swap(vec[j],vec[j+1]);
                }
            }
        }
        return vec;
    }

    vector<vector<string>> sort_desc(vector<vector<string>> vec, string column) {
        int idx=-1;
        for(size_t i=0;i<vec[0].size();i++){
            if(vec[0][i]==column){
                idx=i;
                break;
            }
        }

        for(size_t i=1;i<vec.size();i++){
            for(size_t j=1;j<vec.size()-i;j++){
                if(vec[j][idx]<vec[j+1][idx]){
                    swap(vec[j],vec[j+1]);
                }
            }
        }
        return vec;
    }

    vector<vector<string>> where(vector<vector<string>> &vec, string column, string op, string val) {

        Table tempTable;
        vectorToTable(vec, tempTable);

        vector<Row*> result;

        if (op == "=") {
            result = tempTable.selectEqual(column, val);
        }
        else if (op == ">") {
            result = tempTable.selectGreater(column, val);
        }
        else if (op == "<") {
            result = tempTable.selectLess(column, val);
        }
        else if (op == ">=") {
            vector<Row*> equal = tempTable.selectEqual(column, val);
            vector<Row*> greater = tempTable.selectGreater(column, val);
            result.insert(result.end(), equal.begin(), equal.end());
            result.insert(result.end(), greater.begin(), greater.end());
        }
        else if (op == "<=") {
            vector<Row*> equal = tempTable.selectEqual(column, val);
            vector<Row*> less = tempTable.selectLess(column, val);
            result.insert(result.end(), equal.begin(), equal.end());
            result.insert(result.end(), less.begin(), less.end());
        }
        else if (op == "!=") {
             for (auto &rowRef : tempTable.rows) {
                auto found = rowRef.data.find(column);
                if (found != rowRef.data.end() && (*found).second != val) {
                    result.push_back(&rowRef);
                }
            }
        }

        // Convert back to vector<vector<string>>
        vector<vector<string>> ans;
        if(!vec.empty()) ans.push_back(vec[0]);

        for (Row* rPtr : result) {
            vector<string> row;
            for (const string& colName : vec[0]) {
                auto found = rPtr->data.find(colName);
                if (found != rPtr->data.end()) {
                    row.push_back((*found).second);
                } else {
                    row.push_back("");
                }
            }
            ans.push_back(row);
        }

        return ans;
    }

    vector<vector<string>> select_by_col(vector<vector<string>> vec, unordered_set<string> col) {
        vector<vector<string>>ans;
        for(size_t i=0;i<vec.size();i++){
            vector<string>temp;
            for(size_t j=0;j<vec[i].size();j++){
                if(col.find(vec[0][j])!=col.end()){
                    temp.push_back(vec[i][j]);
                }
            }
            ans.push_back(temp);
        }
        return ans;
    }

    void update_table(string owner_database, string table_name, string c1name, string c1value, string c2name, string c2value, string op) {
        tables t;
        vector<vector<string>> vec = t.read(owner_database,table_name);
        int c1idx=-1;
        int c2idx=-1;
        for(size_t i=0;i<vec[0].size();i++){
            if(vec[0][i]==c1name){
                c1idx=i;
            }
            if(vec[0][i]==c2name){
                c2idx=i;
            }
        }

        if (c1idx == -1 || (op != "" && c2idx == -1)) {
            cout << "Not a valid column name.\n";
            return;
        }

        for(size_t i=1;i<vec.size();i++){
            if(op=="="){
                if(vec[i][c2idx] ==c2value){
                    vec[i][c1idx]=c1value;
                }
            }
             if(op==">"){
                if(vec[i][c2idx] >c2value){
                    vec[i][c1idx]=c1value;
                }
            }
            if(op==">="){
                if(vec[i][c2idx] >=c2value){
                    vec[i][c1idx]=c1value;
                }
            }
            if(op=="<"){
                if(vec[i][c2idx] <c2value){
                    vec[i][c1idx]=c1value;
                }
            }
            if(op=="<="){
                if(vec[i][c2idx] <=c2value){
                    vec[i][c1idx]=c1value;
                }
            }
            if(op=="!="){
                if(vec[i][c2idx] !=c2value){
                    vec[i][c1idx]=c1value;
                }
            }
            if (op == ""){
                vec[i][c1idx] = c1value;
            }
        }

        write_table(vec,owner_database,table_name);
        cout << "Update successful on matching rows." << endl;
    }
};

#endif
