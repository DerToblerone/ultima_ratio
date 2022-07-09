#ifndef TEST_H
#define TEST_H

#include <chrono>
#include <string>
#include <fstream>
#include <iostream>
#include <regex>

#include "search.h"
#include "position.h"
#include "bitboard.h"
#include "utility.h"

std::string resolve_ambigous(Square from, Square to, PieceType type, const Position& pos);
std::string convert_to_algebraic(Move move, Position& pos, bool check_sign);

void start_test(){
    Position pos;
    std::string fen_str;
    std::string filepath = "test_data/wac.epd";

    std::ifstream in_stream;
    
    std::cout << "Path to test file: " << std::endl;
    std::getline(std::cin, filepath);

    in_stream.open(filepath);

    int n = 8;

    std::string solved_list = "";
    

    std::string test_position;

    std::string additional_info;

    std::string engine_move = "";

    bool check_sign = true;

    if(filepath == "test_data/eret.epd") check_sign = false;

    int solved = 0;
    int total = 0;
    unsigned long long total_duration = 0;

    while(std::getline(in_stream, test_position)){
        if(test_position.length() < 1) break;
        total += 1;

        additional_info = read_from_fen(test_position, pos);
        
        std::string command = "";
        bool command_parsed = false;
        
        std::string move = "";
        bool move_parsed = false;
        short move_count = 0;

        std::string label = "";

        for(auto c : additional_info){
            if(!command_parsed){
                switch (c)
                {
                case ' ':
                    command_parsed = true;
                    break;
                
                default:
                    command += c;
                    break;
                }
            }
            else if(!move_parsed){
                switch (c)
                {
                case ' ':
                    move_count++;
                    move += c;
                    break;
                case ';':
                    move_parsed = true;
                    break;
                default:
                    move += c;
                    break;
                }
                
            }
            else{
                label += c;
            }
        }
        std::cout << label << std::endl;
        print_position(pos);
        if(command=="bm") {
            auto start = std::chrono::high_resolution_clock::now();
            Move m = search_position(pos, n);
            auto stop = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
            std::cout << "Best move(s): " << move << std::endl;
            engine_move = convert_to_algebraic(m, pos, check_sign);
            std::cout << "Engine move: " << engine_move << " Time: " << duration.count() << std::endl;
            
            if(move_count){
                // If there are several best moves, check for any of them if it was the engine move
                std::string temp = "";
                for(auto c : move){
                    switch (c)
                    {
                    case ' ':
                        if(engine_move == temp){
                            solved_list += additional_info + "\n";
                            solved++;
                        }
                        temp = "";
                        break;
                    
                    default:
                        temp += c;
                        break;
                    }
                }
                if(engine_move == temp){
                    solved_list += additional_info + "\n"; 
                    solved++;
                }
            }
            else if(engine_move == move){
                solved_list += additional_info + "\n";
                solved++;
            } 
            total_duration += duration.count();
        }

    }
    std::cout << filepath << std::endl;
    std::cout << "Searched until ply " << n << std::endl;
    std::cout << "Solved " << solved << "/" << total << std::endl;
    std::cout << "Total time: " << total_duration/1000.0f << std::endl;

    // Save a list of completed positions
    std::ofstream out_stream;

    out_stream.open("output.txt");
    out_stream << solved_list;

    in_stream.close();
    out_stream.close();
        
}

std::string convert_to_algebraic(Move move, Position& pos, bool check_sign){
    // Takes a Move and outputs the move in algebraic notation
    std::string result = "";
    //
    std::string files = "abcdefgh";
    std::string ranks = "12345678";

    bool capture = false;
    Square from = from_square(move);
    Square to = to_square(move);


    if(pos.board[to] != 0) capture = true;


    if(move == cstl_move_K) return "O-O";
    else if(move == cstl_move_Q) return "O-O-O";
    else if(move == cstl_move_k) return "O-O";
    else if(move == cstl_move_q) return "O-O-O";

    switch (pos.board[from] - pos.to_move)
    {
    case pawn: // Pawn move
        if(capture) result += files[from%8];
        break;
    case knight: // Knight move
        result += 'N';
        result += resolve_ambigous(from, to, knight,  pos);
        break;
    
    case bishop: 
        result += 'B';
        result += resolve_ambigous(from, to, bishop,  pos);
        break;

    case rook: 
        result += 'R';
        result += resolve_ambigous(from, to, rook,  pos);
        break;

    case queen: 
        result += 'Q';
        result += resolve_ambigous(from, to, queen,  pos);
        break;

    case king: 
        result += 'K';
        break;
        
    
    default:
        break;
    }

    if(capture){
        result += 'x';
        result += square_names[to];
    }
    else{
        result += square_names[to];
    }

    switch (move&0xF000)
    {
    case 0x6000:
        result += "=Q";
        break;
    case 0x5000:
        result += "=R";
        break;
    case 0x4000:
        result += "=B";
        break;
    case 0x3000:
        result += "=N";
        break;
    
    default:
        break;
    }

    if(check_sign){
        UndoObject undo = make_move(pos, move);
        if(get_checkers(pos.to_move, pos)) result += "+";
        unmake_move(pos, undo);
    }
    


    return result;
}

std::string resolve_ambigous(Square from, Square to, PieceType type, const Position& pos){
    std::string result;

    std::string file_labels = "abcdefgh";
    std::string rank_labels = "12345678";

    Bitboard target = 1ULL << to;
    Bitboard possible_attackers = pos.piece_bitboards[type + pos.to_move];
    Bitboard ambig_pieces = 0ULL;

    while(possible_attackers){
        Square sq = get_lsb(possible_attackers) - 1;
        possible_attackers &= possible_attackers - 1;

        if(sq == from) continue;

        if((type == knight) && (knight_attacks[sq]&target)){
            ambig_pieces |= 1ULL << sq;
        }
        else if((type == bishop) && (get_bishop_attack_BB(sq, ~pos.piece_bitboards[no_piece])&target)){
            ambig_pieces |= 1ULL << sq; // Bishop ambiguity can only happen after promotion 
        }
        else if((type == rook) && (get_rook_attack_BB(sq, ~pos.piece_bitboards[no_piece])&target)){
            ambig_pieces |= 1ULL << sq;
        }
        else if((type == queen) && ((get_rook_attack_BB(sq, ~pos.piece_bitboards[no_piece])&target) ||
                                    (get_bishop_attack_BB(sq, ~pos.piece_bitboards[no_piece])&target)) ){
            ambig_pieces |= 1ULL << sq;
        }

    }

    if(ambig_pieces){
        char file = from%8;
        char rank = from/8;
        if(ambig_pieces&(~file_array[file])){
            result += file_labels[file];
            if(ambig_pieces&file_array[file]){
                result += rank_labels[rank];
                return result;
            }
            return result;
        }
        if(ambig_pieces&file_array[file]){
                result += rank_labels[rank];
                return result;
            }
        
    }
    
    return result;
}

#endif // TEST_H