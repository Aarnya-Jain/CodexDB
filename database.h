#include <stdio.h>
#include <filesystem>
#include <string.h>
#include "tables.h"

using namespace std;

class databases{
public:

    databases(string name=""){
        string path = "./data/"+name; 
        filesystem::create_directories(path);
    }

    void delete_database(string name){
        string path = "./data/"+name;
        if(!(filesystem::exists(path))){
            cout << "Database \"" << name << "\" not found ... " << endl;
            return;
        }

        else{
            cout << "Confirm the deletion of database \"" << name << "\"... (Y/n) : " ;
            
            while(1){
                char input ;
                cin >> input;
                if(input == 'y' || input == 'Y'){
                    cout << "\n\n";
                    error_code ec;
                    filesystem::remove_all(path,ec);

                    if(!(filesystem::exists(path))){
                    cout << name << " removed succesfully ... " << endl;
                    break;}

                    else{
                        cout << "Error occured ... " << endl;
                        return;
                    }
                }

                else if(input == 'n' || input == 'N'){
                    cout << "\nDeletion cancelled.\n";
                    return;
                }

                else{
                    cout << endl;
                    cout << "enter (Y or n) ... : " ;
                }
            }
            
        }

        return;
    }

};

