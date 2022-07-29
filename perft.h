#ifndef PERFT_H
#define PERFT_H
#include <iostream>
#include <chrono>
#include <fstream>

#include "types.h"
#include "display.h"
#include "movegen.h"

#include "position.h"
#include "make_unmake.cpp"

unsigned long long do_perft(int depth, Position& pos, bool divide);
unsigned long long perft(int depth, Position& pos);

// suboptimal speed, just for testing purposes of node count
int start_perft(){

    Position pos;
    std::string filepath;
    std::string options;

    std::cout << "Path to perft file: " << std::endl;
    std::getline(std::cin, filepath);

    std::ifstream in_stream;

    in_stream.open(filepath);

    std::string perft_position;

    std::string additional_info;

    int correct = 0;
    int total_perfts = 0;
    int total_pos = 0;

    while(std::getline(in_stream, perft_position)){
        if(perft_position.length() < 1) break;

        total_pos++;
        std::cout << total_pos << ":  ";
        std::cout << perft_position << std::endl;
        

        additional_info = read_from_fen(perft_position, pos);


        long time = 0;
        unsigned long long total;
        double nodes_per_sec;


        std::string delimiter = "; ";
        size_t index = additional_info.find(delimiter);
        additional_info.erase(0, index + delimiter.length());

        index = 0;
        std::string token;
        delimiter = " ";

        while ((index = additional_info.find(delimiter)) != std::string::npos) {
            token = additional_info.substr(0, index);

            if(token[0] == 'D'){
                token.erase(0,1);
                int dep = stoi(token);
                auto start = std::chrono::high_resolution_clock::now();
                total = do_perft(dep, pos, false);
                auto stop = std::chrono::high_resolution_clock::now();

                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

                time = duration.count();

                total_perfts++;

                
            }
            else{
                std::cout << "  Time: " << time/1000000.0f;
                nodes_per_sec = total*1.0f/time;
                std::cout << "  Mnps: " << nodes_per_sec;

                std::cout << "  #: " << total;
                std::cout << "  Correct #: " << token;
                

                if(total == stoi(token)) correct++;

                std::cout << "  " << correct << "/" << total_perfts << std::endl;
            }

            
            additional_info.erase(0, index + delimiter.length());
        }

        // last output
        std::cout << "  Time: " << time/1000000.0f;
        nodes_per_sec = total*1.0f/time;
        std::cout << "  Mnps: " << nodes_per_sec;

        std::cout << "  #: " << total;
        std::cout << "  Correct #: " << additional_info;
        std::cout << "  " << correct << "/" << total_perfts << std::endl << std::endl;

        if(total == stol(additional_info)) correct++;
    }

    std::cout << "Total results: " << correct << "/" << total_perfts << std::endl;

    return 0;
}

// for analysing a single position, good speed
int start_divide(){
    Position pos;
    std::string fen_str;
    std::string options;

    std::cout << "Position FEN (0 for initial position): " << std::endl;
    std::getline(std::cin, fen_str);
    if(fen_str.length() >  10){
        read_from_fen(fen_str, pos);
        }
    else{
        options = read_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ", pos);
    }
        
    print_position(pos);
    
    int n;
    std::cout << "Perft depth: " << std::endl;
    std::cin >> n;
    

    bool divide = true;

    auto start = std::chrono::high_resolution_clock::now();
    unsigned long long total = do_perft(n, pos, divide);
    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    
    print_position(pos);

    std::cout << "Results: " << std::endl;
    std::cout << "Time spent: " << duration.count() << " microseconds" << std::endl;
    std::cout << "Total moves made: " << total << std::endl;

    double nodes_per_sec = total*1.0f/duration.count();
    std::cout << "Mil. NPS: " << nodes_per_sec << std::endl;


    return 0;
}

unsigned long long do_perft(int depth, Position& pos, bool divide){

    MoveList move_list;
    
    Bitboard checkers = get_checkers(pos.to_move, pos);

    generate_all(pos, &move_list);

    if(move_list.size == 0){return 1;}

    unsigned long long count,
                       total = 0;
    
    for(Move move : move_list.move_stack){
        if(move != 0){
            count = 0;
            
            make_move(pos, move);
            
            count = perft(depth - 1, pos);

            unmake_move(pos);

            if(divide){
                if(count) std::cout << square_names[from_square(move)] 
                                << square_names[to_square(move)] 
                                << ": " 
                                << count 
                                << std::endl;
            }
            

            total += count;
        }
    } 
    return total;

}

unsigned long long perft(int depth, Position& pos){

    if(get_checkers(black - pos.to_move, pos)){
        return 0;
    }

    if(depth == 0){return 1;}
    
    MoveList move_list;
    
    generate_all(pos, &move_list);

    unsigned long long count = 0;
    
    for(Move move : move_list.move_stack){
        if(move != 0){
            make_move(pos, move);

            count += perft(depth - 1, pos);

            unmake_move(pos);

        }
    } 

    return count;

}

#endif // PERFT_H