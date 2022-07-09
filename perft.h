#ifndef PERFT_H
#define PERFT_H
#include <iostream>
#include <chrono>

#include "types.h"
#include "display.h"
#include "movegen.h"

#include "position.h"
#include "make_unmake.cpp"

unsigned long long do_perft(int depth, Position& pos);
unsigned long long perft(int depth, Position& pos);


int start_perft(){

    Position pos;
    std::string fen_str;

    std::cout << "Position FEN (0 for initial position): " << std::endl;
    std::getline(std::cin, fen_str);
    if(fen_str.length() >  10){
        read_from_fen(fen_str, pos);
        }
    else{
        read_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 ", pos);
    }
        
    print_position(pos);
    
    int n;
    std::cout << "Perft depth: " << std::endl;
    std::cin >> n;
    
    print_position(pos);

    auto start = std::chrono::high_resolution_clock::now();
    unsigned long long total = do_perft(n, pos);
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


unsigned long long do_perft(int depth, Position& pos){

    MoveList move_list;
    
    Bitboard checkers = get_checkers(pos.to_move, pos);

    
    /*
    generate_captures(pos.to_move, pos, &move_list);
    generate_quiet(pos.to_move, pos, &move_list);
    */
    generate_all(pos.to_move, pos, &move_list);

    // generate_captures_legal(pos.to_move, pos, &move_list);
    // generate_quiet(pos.to_move, pos, &move_list);

    if(move_list.size == 0){return 1;}

    unsigned long long count,
                       total = 0;
    
    for(Move move : move_list.move_stack){
        if(move != 0){
            count = 0;
            
            UndoObject undo = make_move(pos, move);
            
            count = perft(depth - 1, pos);

            unmake_move(pos, undo);

            if(count) std::cout << square_names[from_square(move)] 
                                << square_names[to_square(move)] 
                                << ": " 
                                << count 
                                << std::endl;

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

    generate_captures(pos.to_move, pos, &move_list);
    generate_quiet(pos.to_move, pos, &move_list);
    
    

    
    // generate_captures_legal(pos.to_move, pos, &move_list);
    // generate_quiet(pos.to_move, pos, &move_list);


    unsigned long long count = 0;
    
    for(Move move : move_list.move_stack){
        if(move != 0){
            UndoObject undo = make_move(pos, move);

            count += perft(depth - 1, pos);

            unmake_move(pos, undo);

        }
    } 

    return count;

}

#endif // PERFT_H