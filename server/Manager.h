#include <mysql/mysql.h>
#include <iostream>
#include <string.h>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>


struct ConnectionInfo
{
    /* data */
    const char* host;
    const char* user;
    const char* password;
    const char* database;
    const char* unix_socket;
    long client_flag;
    int port;

    ConnectionInfo():
    host("127.0.0.1"),port(3306),unix_socket(nullptr),client_flag(0){};
};

class Manager
{
    public:
        
        void vec2string(std::vector<float> &feature,std::string &f_data,int size);
        
    

};
