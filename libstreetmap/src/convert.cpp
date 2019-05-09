/**/
#ifndef CONVERT
#define CONVERT

#include <string>

std::string convert_to_low_case(std::string input);
//this function change up-case to low-case(if there is any)
//return a new string
std::string convert_to_low_case(std::string input){
    std::string output;
    for(unsigned index = 0;index < input.length();index++){
        if(input[index]>='A' && input[index] <='Z')
            output.push_back(input[index]+('a'-'A'));
        else 
            output.push_back(input[index]);

    }
    return output;
}

#endif