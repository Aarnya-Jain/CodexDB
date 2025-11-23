#include <unordered_set>
#include <unordered_map>
#include <cctype>
#include "./databases/database.h"
#include "./mapper/fetch.h"

using namespace std;

enum QueryType {
    SELECT,
    INSERT,
    DELETE,
    UPDATE,
    UNKNOWN,
    CREATE,
    DROP,
    OPEN,
    ADD
};

/// THE SORT ONE IS OF THE LEAST PRIORITY
/// SHOW , CLEAR , EXIT ----> ARE THE GENERAL COMMANDS

struct SelectQuery {
    string all_operator;
    unordered_set<string> columns;
    string table;
    string where_column;
    string where_operator;
    string where_value;
    string sort_by;
    string sort_type;
};

struct DropQuery{
    string table;
    string database;
};

struct DeleteQuery{
    string table;
    string col;
    string where_col;
    string where_value;
    string where_operator;
};

struct InsertQuery {
    string table;
    vector<string> values;
};

struct AddQuery{
    string table;
    vector<string> values;
};

struct CreateQuery{
    string table;
    string database;
    vector<string> values;
};

struct UpdateQuery {
    string table;
    string set_column;
    string set_value;
    string where_column;
    string where_operator;
    string where_value;
};

struct OpenQuery {
    string database_name;
};

struct ASTNode {
    QueryType type;
    SelectQuery selectQuery;
    InsertQuery insertQuery;
    DeleteQuery deleteQuery;
    UpdateQuery updateQuery;
    CreateQuery createQuery;
    OpenQuery openQuery;
    DropQuery dropQuery;
    AddQuery addQuery;
};


class DatabaseManager {
    private:
        static DatabaseManager* instance;
        string current_database;

        DatabaseManager() {}

    public:
        static DatabaseManager* getInstance() {
            if (!instance) {
                instance = new DatabaseManager();
            }
            return instance;
        }

        void setCurrentDatabase(const string& dbName) {
            current_database = dbName;
        }

        string getCurrentDatabase() const {
            return current_database;
        }
    };

    DatabaseManager* DatabaseManager::instance = nullptr;

    inline string getCurrentDatabase() {

        string path = "./data/" + DatabaseManager::getInstance()->getCurrentDatabase();
        if (filesystem::exists(path)){
            return DatabaseManager::getInstance()->getCurrentDatabase();
        }

        return "";
    }


class Parser {
    vector<string> tokens;
    int current = 0;

public:
    Parser(const vector<string>& toks) : tokens(toks) {}

    string peek() {
        return current < tokens.size() ? tokens[current] : "";
    }

    string advance() {
        return current < tokens.size() ? tokens[current++] : "";
    }

    bool match(const string& expected) {
        string s = peek();

        s = toupper(s);

        if (s == expected) {
            advance();
            return true;
        }
        return false;
    }

    ASTNode parse() {
        if (match("SELECT")) return parseSelect();
        if (match("INSERT")) return parseInsert();
        if (match("DELETE")) return parseDelete();
        if (match("UPDATE")) return parseUpdate();
        if (match("CREATE")) return parseCreate();
        if (match("OPEN")) return parseOpen();
        if (match("DROP")) return parseDrop();
        if (match("ADD")) return parseAdd();
        return { UNKNOWN };
    }

private:
    ASTNode parseSelect() {
        SelectQuery query;

        if(peek().empty()){
            return{UNKNOWN};
        }

        if (peek() == "*") {
            query.all_operator = advance();
            query.columns = {};

            if (!match("FROM")) {
                cout << "Syntax Error: Expected 'FROM' after '*'" << endl;
                return { UNKNOWN };
            }

            query.table = advance();
        }
        else {

            while (!match("FROM")) {
                string col = advance();
                if (col != ",") query.columns.insert(col);
            }

            query.table = advance();
        }

        if (match("WHERE")) {
            query.where_column = advance();
            query.where_operator = advance();
            query.where_value = advance();
        }


        if (match("SORT")) {
            if (!match("BY")) {
                cout << "Syntax Error: Expected 'BY' after 'SORT'" << endl;
            } else {
                query.sort_by = advance();
                query.sort_type = advance();
            }
        }


        if (match(";")) {
            // ok
        }

        if (!peek().empty()) {
            cout << "Syntax Error: Unexpected token '" << peek() << "' after the statement" << endl;
            return { UNKNOWN };
        }

        return { SELECT, query };
    }

    ASTNode parseInsert() {
        InsertQuery query;

        if (!match("INTO")) {
            cout << "Expected 'INTO' after 'INSERT'" << endl;
            return { UNKNOWN };
        }

        if (peek().empty()) {
            cout << "Expected table name after 'INTO'" << endl;
            return { UNKNOWN };
        }

        query.table = advance();

        if (!match("VALUES")) {
            cout << "Expected 'VALUES' after table name" << endl;
            return { UNKNOWN };
        }

        if(!match("(")){
            cout << "Expected '(' after table name" << endl;
            return { UNKNOWN };
        }

        vector<string> row;
        int i=0;
        // int n=tokens.size();
        // if(tokens[n-1]==";"){
        //     if(tokens[n-2]!=")"){
        //         cout << "Expected ')' after table name" << endl;
        //         return { UNKNOWN };
        //     }
        // }

        while(i<tokens.size() && tokens[i]!="(")i++;

        i++;

        while(i<tokens.size() ){

            if(tokens[i]==")"){
                break;
            }

            if(tokens[i]==",")i++;
            else{
            row.push_back(tokens[i]);
            i++;
            }
        }
        if(i==tokens.size()){
            cout << "Expected ')' after table name" << endl;
            return { UNKNOWN };
        }

        // if (match(";")) {
        //     // ok
        // }
        // if (!peek().empty()) {
        //     cout << "Syntax Error: Unexpected token '" << peek() << "' after the statement" << endl;
        //     return { UNKNOWN };
        // }

        query.values=row;

        return { INSERT, {}, query };
    }


    ASTNode parseDelete() {
        DeleteQuery query;

        if(match("ROWS")){
            if (!match("FROM")) {
                cout << "Syntax Error: Expected 'FROM' "<< endl;
                return { UNKNOWN };
            }

            query.table = advance();

            if(match("WHERE")){
                query.where_col = advance();
                query.where_operator = advance();
                query.where_value = advance();
            }
        }

        if (match("COLUMN")){
            query.col = advance();

            if (!match("FROM")) {
                cout << "Syntax Error: Expected 'FROM' "<< endl;
                return { UNKNOWN };
            }

            query.table = advance();
        }

        if (match(";")) {
            // ok
        }
        if (!peek().empty()) {
            cout << "Syntax Error: Unexpected token '" << peek() << "' after the statement" << endl;
            return { UNKNOWN };
        }

        return {DELETE,{},{},query};

    }


    // come here
    ASTNode parseUpdate() {
        UpdateQuery query;
        query.table = advance();
        if(!match("SET")){
            cout << "Expected 'SET' ..." <<endl;
            return{UNKNOWN};
        }
        else{
            match("SET");
        }
        query.set_column = advance();
        if(!match("=")){
            cout << "Expected '=' ..." <<endl;
            return{UNKNOWN};
        }
        else{
            match("=");
        }
        query.set_value = advance();

        if (match("WHERE")) {
            query.where_column = advance();
            query.where_operator = advance();
            query.where_value = advance();
        }
        if (match(";")) {
            // ok
        }
        if (!peek().empty()) {
            cout << "Syntax Error: Unexpected token '" << peek() << "' after the statement" << endl;
            return { UNKNOWN };
        }
        return { UPDATE, {}, {}, {}, query };
    }

    ASTNode parseCreate() {
        CreateQuery query;
        if(match("TABLE")){
            query.table = advance();

            if(!match("(")){
                cout << "Expected '(' after table name but found '" << peek() << "'" << endl;
                return{UNKNOWN};
            }

            vector<string> row;
            int i=0;
            while(i<tokens.size() && tokens[i]!="(")i++;
            i++;
            while(i<tokens.size() ){
                if(tokens[i]==")"){
                    current = i;
                    advance();
                    break;
                }
                if(tokens[i]==",")i++;
                else{
                row.push_back(tokens[i]);
                i++;
                }
            }

            query.values = row;

        }
        else if(match("DATABASE")){
            query.database = advance();

        }


        if (match(";")) {
            // ok
        }
        if (!peek().empty()) {
            cout << "Syntax Error: Unexpected token after the statement" << endl;
            return { UNKNOWN };
        }

        return {CREATE, {}, {}, {}, {}, query};

    }

    ASTNode parseOpen(){
        OpenQuery query;
        query.database_name = advance();
        if (match(";")) {
            // ok
        }
        if (!peek().empty()) {
            cout << "Syntax Error: Unexpected token '" << peek() << "' after the statement" << endl;
            return { UNKNOWN };
        }
        return {OPEN,{},{},{},{},{},query};
    }

    ASTNode parseDrop(){
        DropQuery query;
        if(match("TABLE")){
            query.table = advance();
        }

        if(match("DATABASE")){
            query.database = advance();
        }

        if (match(";")) {
            // ok
        }
        if (!peek().empty()) {
            cout << "Syntax Error: Unexpected token '" << peek() << "' after the statement" << endl;
            return { UNKNOWN };
        }
        return {DROP,{},{},{},{},{},{},query};

    }

    ASTNode parseAdd(){
        AddQuery query;

        if(!match("COLUMN")){
            cout << "Expected 'COLUMN' ..." <<endl;
            return{UNKNOWN};
        }
        else{
            match("COLUMN");
        }

        if(!match("(")){
            cout << "Expected '(' ..." <<endl;
            return{UNKNOWN};
        }
        else{
            match("(");
        }

        vector<string> row;
        int i=0;
        while(i<tokens.size() && tokens[i]!="(")i++;
        i++;
        while(i<tokens.size() ){
            if(tokens[i]==")"){
                current = i;
                advance();
                break;
            }
            if(tokens[i]==",")i++;
            else{
            row.push_back(tokens[i]);
            i++;
            }
        }

        query.values = row;

        if(!match("INTO")){
            cout << "Expected 'INTO' ..." <<endl;
            return{UNKNOWN};
        }
        else{
            match("INTO");
        }

        if(peek().empty()){
            cout << "Expected table name after 'INTO' ..." <<endl;
            return{UNKNOWN};
        }

        query.table = advance();

        if (match(";")) {
            // ok
        }
        if (!peek().empty()) {
            cout << "Syntax Error: Unexpected token '" << peek() << "' after the statement" << endl;
            return { UNKNOWN };
        }

        return{ADD,{},{},{},{},{},{},{},query};

    }

};


class ExecutionEngine {
private:
    DatabaseManager* dbManager;
public:
    ExecutionEngine() {
        dbManager = DatabaseManager::getInstance();
    }

    void execute(const ASTNode& ast) {
        switch (ast.type) {
            case SELECT:  runSelect(ast.selectQuery); break;
            case INSERT:  runInsert(ast.insertQuery); break;
            case DELETE:  runDelete(ast.deleteQuery); break;
            case UPDATE:  runUpdate(ast.updateQuery); break;
            case CREATE:  runCreate(ast.createQuery); break;
            case OPEN:    runOpen(ast.openQuery); break;
            case DROP:    runDrop(ast.dropQuery); break;
            case ADD:     runAdd(ast.addQuery); break;
            default: cout << "Unknown query type\n";
        }
    }

private:
    void runSelect(const SelectQuery& q);
    void runInsert(const InsertQuery& q);
    void runDelete(const DeleteQuery& q);
    void runUpdate(const UpdateQuery& q);
    void runCreate(const CreateQuery& q);
    void runOpen(const OpenQuery& q);
    void runDrop(const DropQuery& q);
    void runAdd(const AddQuery& q);
};

void ExecutionEngine::runOpen(const OpenQuery& q){
    dbManager->setCurrentDatabase(toupper(q.database_name));
    string path = "./data/" + dbManager->getCurrentDatabase();
    if (filesystem::exists(path)){
        cout << "Using database: " << dbManager->getCurrentDatabase() << endl;
    }
    else{
        cout << "Database not found" << endl;
        dbManager->setCurrentDatabase("");
    }
}

void ExecutionEngine::runDrop(const DropQuery& q){
    tables t;
    databases d;
    if(!q.database.empty()){
        d.delete_database(q.database);
        auto tree = fetch_structure();
        remove_database(tree, q.database);
        save_structure(tree);
    }

    if(!q.table.empty()){
        if(dbManager->getCurrentDatabase() == ""){
            cout << "Please Open a Database to drop a Table ..." << endl;
        }

        else{
            t.delete_table(dbManager->getCurrentDatabase(),q.table);
            auto tree = fetch_structure();
            remove_table(tree, dbManager->getCurrentDatabase(), q.table);
            save_structure(tree);

        }

    }

    else if(q.database.empty() && q.table.empty()){
        cout << "Specify what to delete ... " << endl;
    }
}

void ExecutionEngine::runInsert(const InsertQuery& q) {
    tables t;
    if (dbManager->getCurrentDatabase().empty()) {
        cout << "Please open a database to insert into a table ..." << endl;
        return;
    }

    vector<vector<string>> data = t.read(dbManager->getCurrentDatabase(),q.table);

    if (data.empty()) {
        cout << "Table is empty or does not exist ..." << endl;
        return;
    }

    t.addrow(q.values,data,dbManager->getCurrentDatabase(),q.table);

}

void ExecutionEngine::runSelect(const SelectQuery& q) {
    tables t;

    // OPEN
    if (dbManager->getCurrentDatabase().empty()) {
        cout << "Please open a database to select a table ..." << endl;
        return;
    }

    // CHECK TABLE EXIST
    vector<vector<string>> data = t.read(dbManager->getCurrentDatabase(), q.table);
    if (data.empty()) {
        cout << "Table is empty or does not exist ..." << endl;
        return;
    }

    // CHECK WHERE
    if (!q.where_column.empty() && !q.where_operator.empty() && !q.where_value.empty()) {
        data = t.where(data, q.where_column, q.where_operator, q.where_value);
        if (data.size() <= 1) {
            // nothing to show
            cout << "No rows match the WHERE condition ..." << endl;
            return;
        }
    }

    // CHECK SORT BY
    if (!q.sort_by.empty() && !q.sort_type.empty()) {

        if (q.sort_type == "ASC"||q.sort_type == "asc") {
            data = t.sort_asc(data, q.sort_by);
        } else if (q.sort_type == "DESC"||q.sort_type == "desc") {
            data = t.sort_desc(data, q.sort_by);
        } else {
            cout << "Invalid sort type. Use ASC or DESC." << endl;
            return;
        }
    }

    // CHECK *
    if (q.all_operator == "*") {
        displaytable(data);
    } else if (!q.columns.empty()) {
        data = t.select_by_col(data, q.columns);
        displaytable(data);
    } else {
        cout << "Invalid Query. No columns specified and no '*' operator." << endl;
    }
}


void ExecutionEngine::runDelete(const DeleteQuery& q) {
    tables t;

    if (dbManager->getCurrentDatabase().empty()) {
        cout << "Please open a database to alter a table ..." << endl;
        return;
    }

    vector<vector<string>> data = t.read(dbManager->getCurrentDatabase(), q.table);

    if (data.empty()) {
        cout << "Table is empty or does not exist ..." << endl;
        return;
    }

    if(!q.col.empty()){
        t.deletecol(data,q.col,dbManager->getCurrentDatabase(),q.table);

    }

    if(!q.where_col.empty() && !q.where_operator.empty() && !q.where_value.empty()){
        t.deleterow(data,q.where_col,q.where_value,q.where_operator,dbManager->getCurrentDatabase(), q.table);

    }

}

void ExecutionEngine::runCreate(const CreateQuery& q) {
    if (q.table.empty()) {
        if (q.database.empty()) {
            cout << "Invalid query ." << endl;
            return;
        }

        string path = "./data/" + toupper(q.database);

        if (filesystem::exists(path)) {
            cout << "Database '" << toupper(q.database) << "' already exists." << endl;
        } else {
            databases(q.database);

            if (filesystem::exists(path)) {
                auto tree = fetch_structure();
                add_database(tree, q.database);
                save_structure(tree);
            }
            else {
                cout << "Database not created OR Unknown query ..." << endl;
            }
        }
    }

    else if(q.database.empty()){
        if(dbManager->getCurrentDatabase() == ""){
            cout << "Please Open a Database to create a Table ..." << endl;
        }

        else {
            tables t;
            vector<vector<string>> data = t.read(dbManager->getCurrentDatabase(), q.table);
            t.create_table(dbManager->getCurrentDatabase(),q.table);
            t.addrow(q.values,data,dbManager->getCurrentDatabase(),q.table);

            auto tree = fetch_structure();
            add_table(tree, dbManager->getCurrentDatabase(), q.table);
            save_structure(tree);

            cout << q.table << " created successfully in database " << dbManager->getCurrentDatabase() << " ..." << endl;
        }
    }
}

void ExecutionEngine::runUpdate(const UpdateQuery& q) {
    tables t;

    if (dbManager->getCurrentDatabase().empty()) {
        cout << "Please open a database to update a table ..." << endl;
        return;
    }


    vector<vector<string>> data = t.read(dbManager->getCurrentDatabase(), q.table);
    if (data.empty()) {
        cout << "Table is empty or does not exist ..." << endl;
        return;
    }


    if (!q.where_column.empty() && !q.where_operator.empty() && !q.where_value.empty()) {
        vector<vector<string>> filteredData = t.where(data, q.where_column, q.where_operator, q.where_value);

        if (filteredData.size() <= 1) {
            cout << "No rows match the WHERE condition ..." << endl;
            return;
        }

        t.update_table(dbManager->getCurrentDatabase(), q.table, q.set_column, q.set_value, q.where_column, q.where_value, q.where_operator);

    }

    else {
        t.update_table(dbManager->getCurrentDatabase(), q.table, q.set_column, q.set_value, "", "", "");
    }

}

void ExecutionEngine::runAdd(const AddQuery& q){
    tables t;
    if (dbManager->getCurrentDatabase().empty()) {
        cout << "Please open a database to insert into a table ..." << endl;
        return;
    }

    vector<vector<string>> data = t.read(dbManager->getCurrentDatabase(),q.table);

    if (data.empty()) {
        cout << "Table is empty or does not exist ..." << endl;
        return;
    }

    t.addcol(data,q.values,dbManager->getCurrentDatabase(),q.table);

}

