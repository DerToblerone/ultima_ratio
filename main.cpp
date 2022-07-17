#include <iostream>
#include <string>

#include "perft.h"
#include "play.h"
#include "test.h"

int main(){
    
    std::string command;
    std::cout << "Type command (play, test, perft, divide): " << std::endl;
    std::getline(std::cin, command);
    
    if(command == "perft") start_perft();
    if(command == "divide") start_divide();
    else if (command == "play") start_game();
    else if (command == "test") start_test();

    return 0; 
}
