#include <stdlib.h>
#include "./src/parsing.h"
#include "./src/lexer.h"

using namespace std;
namespace fs = std::filesystem;

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define BLUE    "\033[34m"
#define BOLD    "\033[1m"

void printlogo(const std::string& filePath) {

    if (!fs::exists(filePath)) {
        std::cerr << "Error: File does not exist -> " << filePath << "\n";
        return;
    }

    if (!fs::is_regular_file(filePath)) {
        std::cerr << "Error: Path is not a regular file -> " << filePath << "\n";
        return;
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file -> " << filePath << "\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::cout << BLUE << line << '\n';
    }

    file.close();
    cout << "\n";
}

void TrimTheColon(string &cmd){
    // removing the semicolon from the string if present
        while(!cmd.empty() && (isspace(cmd.back()) || cmd.back() == ';')) {
            if(cmd.back() == ';') {
                cmd.pop_back();
                break; // break loop once ; is found and removed
            }
            cmd.pop_back(); // remove spaces before semicolon if any
        }

        // remove trailing spaces after semicolon is handled
        while (!cmd.empty() && isspace(cmd.back())) {
            cmd.pop_back();
        }

}

bool check_general(vector<string> &tokens){

    if(!(strcasecmp(tokens[0].c_str(),"CLEAR")) || !(strcasecmp(tokens[0].c_str(),"CLS"))){
        system("clear");
        return true;
    }

    if(!(strcasecmp(tokens[0].c_str(),"SHOW")) && !(strcasecmp(tokens[1].c_str(),"DATABASES")) && (tokens.size() == 2)){

        // THESE ONES TAKEN FROM THE fetch.h HEADER FILE LOADED IN parsing.h
        show_tree(fetch_structure());
        return true;
    }

    return false;
}

int main(){

    system("clear");
    printlogo("./public/logo.txt");

    while(1)
    {

        string currentDB = getCurrentDatabase();
        cout<<endl;
        cout << BOLD << RED << currentDB << " " << BOLD << GREEN << "> " << RESET;

        string cmd;
        getline(cin, cmd);

        if (cmd.empty()) {
            continue;
        }

        TrimTheColon(cmd); // trim called to handle ';' and other trailing spaces at the end

        if(!strcasecmp(cmd.c_str(),"EXIT")){
            cout << "Exiting the running instance ... \n\n";
            break;
        } // handling the exit loop

        // calling the lexer , parser and the execution engine
        lex l;
        vector<string> tokens = l.tokenize(cmd);

            // for (auto &t : tokens) cout << "[" << t << "] ";
            // cout << endl;

        // checking for general commands
        if(check_general(tokens)) continue;

        Parser p(tokens);
        ASTNode ast = p.parse();

        ExecutionEngine engine;
        engine.execute(ast);
}

}