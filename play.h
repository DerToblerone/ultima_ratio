#ifndef PLAY_H
#define PLAY_H

#include <fstream>
#include <iostream>
#include <string>
#include <array>

#include "types.h"
#include "position.h"
#include "display.h"
#include "movegen.h"
#include "make_unmake.cpp"
#include "search.h"

// TODO: Consider promotions

Move parse_input_move(std::string input_move);

void start_game(){
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

    Move player_move;
    Move computer_move;

    // If no move was made, make it impossible to undo
    int move_count = 0;
    
    // Keeps track if the human player has played a move 
    bool engine_move;

    int depth;
    std::cout << "Search depth: " << std::endl;
    std::cin >> depth;
    
    std::string dummy;
    std::getline(std::cin , dummy);



    while(true){

        engine_move = false;
        player_move = 0;
        std::string move_string;
        std::cout << "Move to play (0 to reverse, 1 for computer): " << std::endl;
        std::getline(std::cin , move_string);


        if(move_string.length() > 5) std::cout << "Input too long!" << std::endl;
        else if(move_string == "0"){
            if(pos.total_move_count){
                unmake_move(pos);
                print_position(pos);
            }
            else std::cout << "No move to undo!" << std::endl;
        }
        else if(move_string == "1") engine_move = true;
        else if(move_string == "save"){
            // Save the moves that have been played to text file
            /*
            std::ofstream out_stream;

            out_stream.open("game.txt");
            for(int i = 0; i < move_count; i++){
                if(undo_stack[i].move){
                    out_stream  << square_names[from_square(undo_stack[i].move)] 
                                << square_names[to_square(undo_stack[i].move)]
                                << std::endl;
                }
            }
            out_stream.close();
            */
        }
        else{
            player_move = parse_input_move(move_string);
        }

        if(player_move){
            MoveList all_moves;
            generate_all(pos, &all_moves);
            
            for(auto move: all_moves.move_stack){
                if(     (from_square(move) == from_square(player_move)) 
                    &&  (to_square(move) == to_square(player_move))){
                        // If the move has special flags set, check if it is a 
                        // promotion and see if the flag has been set on player move
                        switch (move&0xF000)
                        {
                        case 0x8000:
                            // For castling do nothing
                            break;

                        case 0x1000:
                            // For set en passant do nothing
                            break;

                        case 0x2000:
                            // For en passant capture do nothing
                            break;
                        
                        default:
                            // If promotion, only play the move if player_move and move have
                            // the same flags set
                            if((move&0xF000) == (player_move&0xF000)) break;
                            else if((player_move&0xF000) == 0){
                                // If the player move has no flags at all
                                // promote to queen
                                if((move&0xF000) == 0x6000) break;
                                else continue;
                            }
                            else continue;
                        }

                        make_move(pos, move);
                        
                        if(get_checkers(black - pos.to_move, pos)){
                            unmake_move(pos);
                            std::cout << "Illegal move!" << std::endl;
                            break;
                        }
                        print_position(pos);
                        // move_made = true;
                }
            }
        }

        // Do computer move
        if(engine_move){
            std::cout << "Computer is thinking..." << std::endl;
            computer_move = search_position(pos, depth);

            make_move(pos, computer_move);
            print_position(pos);

        }
    }
}

Move parse_input_move(std::string input_move){
    short index = 0;
    Square  from = 0,
            to = 0;
    short promotion = 0;

    // Convert string to from and to square
    for(auto c: input_move){
        if(index < 2)   from += char_value_dict[c];
        else if(index == 4){
            switch (c)
            {
                
            case 'q':
                promotion = 0x6000;
                break;

            case 'r':
                promotion = 0x5000;
                break;

            case 'b':
                promotion = 0x4000;
                break;

            case 'n':
                promotion = 0x3000;
                break;
            
            default:
                // If the input is too long and not a promotion input
                // return no move
                return 0;
            }
        }
        else            to   += char_value_dict[c];
        index++;
    }
    // If the move was too short, return no move
    if(index < 4) return 0;
    else return from + (to << 6) | promotion;
}

#endif // PLAY_H