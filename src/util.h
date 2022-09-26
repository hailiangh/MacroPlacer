#ifndef UTIL_H_
#define UTIL_H_

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <random>
#include <set>
#include <fstream>
#include <string>
#include <algorithm>

/// Ref: RippleFPGA
inline bool read_line_as_tokens(std::istream &is, std::vector<std::string> &tokens) {
    tokens.clear();
    std::string line;
    getline(is, line);
    while (is && tokens.empty()) {
        std::string token = "";
        for (unsigned i=0; i<line.size(); ++i) {
            char currChar = line[i];
            if (isspace(currChar)) {
                if (!token.empty()) {
                    // Add the current token to the list of tokens
                    tokens.push_back(token);
                    token.clear();
                }
            }else{
                // Add the char to the current token
                token.push_back(currChar);
            }
        }
        if(!token.empty()){
            tokens.push_back(token);
        }
        if(tokens.empty()){
            // Previous line read was empty. Read the next one.
            getline(is, line);    
        }
    }
    return !tokens.empty();
}








#endif