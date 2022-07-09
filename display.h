#ifndef DISPLAY_H
#define DISPLAY_H

#include <iostream>
#include <array>
#include <string>
#include <bitset>

#include "position.h"
#include "types.h"


void print_bb(Bitboard bb){
    Bitboard mask;
    std::cout << "  |Bitboard=========|" << std::endl;

    for(int j = 7; j >= 0; j--){
        std::cout << j+1 << " | ";
        for(int i = 0; i < 8; i++){
            mask = 1;
            mask = mask << (i + (j*8));
            if(bb&mask){std::cout << "+";}
            else {std::cout << ".";}
            std::cout << " ";
             
        }
        
        std::cout << "|" << std::endl;
    }
    std::cout << "  |=================|" << std::endl;
    std::cout << "    A B C D E F G H  " << std::endl;
}

void print_position(const Position& pos){
    Bitboard mask;
    std::cout << "  |Version2=========|" << std::endl;

    for(int j = 7; j >= 0; j--){
        std::cout << j+1 << " | ";
        for(int i = 0; i < 8; i++){
            mask = 1;
            mask = mask << (i + (j*8));
            if(pos.piece_bitboards[b_pawn]&mask)        {std::cout << "♙ ";}
            else if( pos.piece_bitboards[b_knight]&mask){std::cout << "♘ ";}
            else if( pos.piece_bitboards[b_king]&mask)  {std::cout << "♔ ";}
            else if( pos.piece_bitboards[b_bishop]&mask){std::cout << "♗ ";}
            else if( pos.piece_bitboards[b_rook]&mask)  {std::cout << "♖ ";}
            else if( pos.piece_bitboards[b_queen]&mask) {std::cout << "♕ ";}
            else if( pos.piece_bitboards[w_pawn]&mask)  {std::cout << "♟ ";}
            else if( pos.piece_bitboards[w_knight]&mask){std::cout << "♞ ";}
            else if( pos.piece_bitboards[w_king]&mask)  {std::cout << "♚ ";}
            else if( pos.piece_bitboards[w_bishop]&mask){std::cout << "♝ ";}
            else if( pos.piece_bitboards[w_rook]&mask)  {std::cout << "♜ ";}
            else if( pos.piece_bitboards[w_queen]&mask) {std::cout << "♛ ";}
            else if(((i%2) + (j%2))%2){std::cout << "██";}
            else {std::cout << "  ";}
    
        }

        
        std::cout << "|";
        if(j == 6) std::cout << "  Castling Rights: " << std::bitset<8>(pos.castling_rights);
        if(j == 5) std::cout << "  Key: " << std::hex << pos.position_key << std::dec;
        if(j == 4) std::cout <<  (pos.to_move ? "  Black" : "  White") << " to move"  << std::dec;
        if((j == 3) && pos.en_passant) std::cout << "  En passant square: " << square_names[pos.en_passant];


        std::cout << std::endl;
    }
    std::cout << "  |=================|" << std::endl;
    std::cout << "    A B C D E F G H  " << std::endl;
}


#endif //DISPLAY_H