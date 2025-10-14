#include <iostream>
#include <fstream>
#include <string.h>
#include <filesystem>
#include <vector>
#include <stdlib.h>
#include "parsing.h"
#include "lexer.h"

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

bool check_general(string &cmd){
    if(!(strcasecmp(cmd.c_str(),"CLEAR")) || !(strcasecmp(cmd.c_str(),"CLS"))){
        system("clear");
        return true;
    }

    if(!(strcasecmp(cmd.c_str(),"SHOW DATABASES"))){

        // THESE ONES TAKEN FROM THE fetch.h HEADER FILE LOADED IN parsing.h
        show_tree(fetch_structure());
        return true;
    }

    return false;
}

int main(){
    // cout << " .d8888b.                888                        8888888b.  888888b.   " << endl;
    // cout << "d88P  Y88b               888                        888   Y88b 888  \"88b  " << endl;
    // cout << "888    888               888                        888    888 888  .88P " << endl;
    // cout << "888         .d88b.   .d88888  .d88b.  888  888      888    888 8888888K. " << endl;
    // cout << "888        d88\"\"88b d88\" 888 d8P  Y8b 'Y8bd8P'      888    888 888  \"Y88b " << endl;
    // cout << "888    888 888  888 888  888 88888888   X88K        888    888 888    888  " << endl;
    // cout << "Y88b  d88P Y88..88P Y88b 888 Y8b.     .d8\"\"8b.      888  .d88P 888   d88P  " << endl;
    // cout << " \"Y8888P\"   \"Y88P\"   \"Y88888  \"Y8888  888  888      8888888P\"  8888888P\"  " << endl;
    // cout << endl;
    // cout << endl;

    printlogo("./public/logo.txt");

    while(1)
    {

        string currentDB = getCurrentDatabase();
        cout<<endl;
        cout << BOLD << RED << currentDB << " " << BOLD << GREEN << "> " << RESET;

        string cmd;
        getline(cin, cmd);
        cmd = normalizeSpaces(cmd);

        if (cmd.empty()) {
            continue;
        }

        if(!strcasecmp(cmd.c_str(),"EXIT")){
            cout << "Exiting the running instance ... \n\n";
            break;
        }

        if(check_general(cmd)) continue;

        lex l;
        vector<string> tokens = l.tokenize(cmd);

        Parser p(tokens);
        ASTNode ast = p.parse();

        ExecutionEngine engine;
        engine.execute(ast);
}

}