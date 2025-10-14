#include<iostream>
#include<vector>
#include<cctype>
using namespace std;

enum state {
    START,
    KEYWORD,
    IDENTIFIER,
    NUMBER,
    OPERATOR,
    STRING
};

class lex {
public:
    vector<string> tokenize(string &cmd) {
        vector<string> tokens_vector;
        if (cmd.empty()) return tokens_vector;  // Handle empty input
        
        state current = START;
        string token = "";
        
        for(char c : cmd) {
            switch(current) {
                case START:
                    if(isalpha(c)) {
                        current = KEYWORD; 
                        token += c;
                    }
                    else if(isdigit(c)) {
                        current = NUMBER; 
                        token += c;
                    }
                    else if(c == '\"' || c == '\'') {
                        current = STRING; 
                        token += c;
                    }
                    else if(isspace(c)) {
                        continue;
                    }
                    else {
                        current = OPERATOR; 
                        token += c;
                    }
                    break;
                    
                case KEYWORD:
                case IDENTIFIER:
                    if(isalnum(c) || c == '_') {
                        token += c;
                    }
                    else {
                        if (!token.empty()) {
                            tokens_vector.push_back(token);
                        }
                        token = "";
                        if(isspace(c)) {
                            current = START;
                        }
                        else {
                            token += c;
                            if(c == '\"' || c == '\'') {
                                current = STRING;
                            }
                            else {
                                current = OPERATOR;
                            }
                        }
                    }
                    break;
                    
                case NUMBER:
                    if(isdigit(c)) {
                        token += c;
                    }
                    else {
                        if (!token.empty()) {
                            tokens_vector.push_back(token);
                        }
                        token = "";
                        if(isspace(c)) {
                            current = START;
                        }
                        else {
                            token += c;
                            if(isalpha(c)) {
                                current = IDENTIFIER;
                            }
                            else if(c == '\"' || c == '\'') {
                                current = STRING;
                            }
                            else {
                                current = OPERATOR;
                            }
                        }
                    }
                    break;
                    
                case STRING:
                    token += c;
                    if(c == '\"' || c == '\'') {
                        if (!token.empty()) {
                            tokens_vector.push_back(token);
                        }
                        token = "";
                        current = START;
                    }
                    break;
                    
                case OPERATOR:
                    if(token == ">" && c == '=') {
                        token += c;
                        if (!token.empty()) {
                            tokens_vector.push_back(token);
                        }
                        token = "";
                        current = START;
                    }
                    else {
                        if (!token.empty()) {
                            tokens_vector.push_back(token);
                        }
                        token = "";
                        if(isspace(c)) {
                            current = START;
                        }
                        else {
                            token += c;
                            if(isalpha(c)) {
                                current = KEYWORD;
                            }
                            else if(isdigit(c)) {
                                current = NUMBER;
                            }
                            else if(c == '\"' || c == '\'') {
                                current = STRING;
                            }
                        }
                    }
                    break;
            }
        }
        
        // Don't forget to add the last token if there is one
        if(!token.empty()) {
            tokens_vector.push_back(token);
        }
        
        return tokens_vector;
    }
};