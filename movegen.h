#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <iostream>
#include <stdint.h>

#include "types.h"
#include "utility.h"
#include "position.h"
#include "bitboard.h"
#include "display.h"


// These functions generate moves and add them to the Move
inline void pawn_quiet_white(Bitboard pieces, const Bitboard& targets, MoveList* move_list){
    // First look at all pawns that are not on the 7th rank
    Bitboard moves = pawn_push_north(pieces&non_promoting_w) & targets;
    Bitboard double_push = pawn_push_north(moves & __3_RANK) & targets;
    Square sq;

    while(double_push){
        sq = get_lsb(double_push) - 1;
        move_list->move_stack[move_list->size++] = (sq - 16) | (sq << 6) | 0x1000; // Add set en passant flag
        double_push &= double_push - 1;
    }

    while(moves){
        sq = get_lsb(moves) - 1;
        move_list->move_stack[move_list->size++] = (sq - 8) | (sq << 6);
        moves &= moves - 1;
    }

    // Take special care for the pawns that are promoting 
    moves = pawn_push_north(pieces&__7_RANK) & targets;

    while(moves){
        sq = get_lsb(moves) - 1;
        move_list->move_stack[move_list->size++] = (sq - 8) | (sq << 6) | 0x6000;
        move_list->move_stack[move_list->size++] = (sq - 8) | (sq << 6) | 0x5000;
        move_list->move_stack[move_list->size++] = (sq - 8) | (sq << 6) | 0x4000;
        move_list->move_stack[move_list->size++] = (sq - 8) | (sq << 6) | 0x3000;
        moves &= moves - 1;
    }
    
}

inline void pawn_quiet_black(Bitboard pieces, const Bitboard& targets, MoveList* move_list){
    Bitboard moves = pawn_push_south(pieces&non_promoting_b) & targets;
    Bitboard double_push = pawn_push_south(moves & __6_RANK) & targets;
    Square sq;

    while(double_push){
        sq = get_lsb(double_push) - 1;
        move_list->move_stack[move_list->size++] = (sq + 16) | (sq << 6) | 0x1000; // Add set en passant flag
        double_push &= double_push - 1;
    }

    while(moves){
        sq = get_lsb(moves) - 1;
        move_list->move_stack[move_list->size++] = (sq + 8) | (sq << 6);
        moves &= moves - 1;
    }

    moves = pawn_push_south(pieces&__2_RANK) & targets;

    while(moves){
        sq = get_lsb(moves) - 1;
        move_list->move_stack[move_list->size++] = (sq + 8) | (sq << 6) | 0x6000;
        move_list->move_stack[move_list->size++] = (sq + 8) | (sq << 6) | 0x5000;
        move_list->move_stack[move_list->size++] = (sq + 8) | (sq << 6) | 0x4000;
        move_list->move_stack[move_list->size++] = (sq + 8) | (sq << 6) | 0x3000;
        moves &= moves - 1;
    }

}

inline void pawn_captures_white(Bitboard pieces, const Bitboard& targets, MoveList* move_list){
    // Shift pawns and then extract moves
    Bitboard moves = pawn_north_west(pieces&non_promoting_w) & targets;
    Square sq;

    while(moves){
        sq = get_lsb(moves) - 1;
        // Only one pawn can be the attacker of this square from this direction
        move_list->move_stack[move_list->size++] = (sq - 7) | (sq << 6);
        moves &= moves - 1;
    }
    // Pieces on the 7th rank are promoting, so 4 moves are generated
    moves = pawn_north_west(pieces&__7_RANK) & targets;
    while(moves){
        sq = get_lsb(moves) - 1;
        // Only one pawn can be the attacker of this square from this direction
        move_list->move_stack[move_list->size++] = (sq - 7) | (sq << 6) | 0x6000;
        move_list->move_stack[move_list->size++] = (sq - 7) | (sq << 6) | 0x5000;
        move_list->move_stack[move_list->size++] = (sq - 7) | (sq << 6) | 0x4000;
        move_list->move_stack[move_list->size++] = (sq - 7) | (sq << 6) | 0x3000;
        moves &= moves - 1;
    }

    moves = pawn_north_east(pieces&non_promoting_w)& targets;
    while(moves){
        sq = get_lsb(moves) - 1;
        move_list->move_stack[move_list->size++] = (sq - 9) | (sq << 6);
        moves &= moves - 1;
    }

    moves = pawn_north_east(pieces&__7_RANK)& targets;
    while(moves){
        sq = get_lsb(moves) - 1;
        move_list->move_stack[move_list->size++] = (sq - 9) | (sq << 6) | 0x3000;
        move_list->move_stack[move_list->size++] = (sq - 9) | (sq << 6) | 0x4000;
        move_list->move_stack[move_list->size++] = (sq - 9) | (sq << 6) | 0x5000;
        move_list->move_stack[move_list->size++] = (sq - 9) | (sq << 6) | 0x6000;
        moves &= moves - 1;
    }

}

inline void pawn_captures_black(Bitboard pieces, const Bitboard& targets, MoveList* move_list){
    Bitboard moves = pawn_south_west(pieces&non_promoting_b) & targets;
    Square sq;

    while(moves){
        sq = get_lsb(moves) - 1;
        move_list->move_stack[move_list->size++] = (sq + 9) | (sq << 6);
        moves &= moves - 1;
    }

    moves = pawn_south_west(pieces&__2_RANK) & targets;
    while(moves){
        sq = get_lsb(moves) - 1;
        move_list->move_stack[move_list->size++] = (sq + 9) | (sq << 6) | 0x6000;
        move_list->move_stack[move_list->size++] = (sq + 9) | (sq << 6) | 0x5000;
        move_list->move_stack[move_list->size++] = (sq + 9) | (sq << 6) | 0x4000;
        move_list->move_stack[move_list->size++] = (sq + 9) | (sq << 6) | 0x3000;
        moves &= moves - 1;
    }


    moves = pawn_south_east(pieces&non_promoting_b)& targets;
    while(moves){
        sq = get_lsb(moves) - 1;
        move_list->move_stack[move_list->size++] = (sq + 7) | (sq << 6);
        moves &= moves - 1;
    }

    moves = pawn_south_east(pieces&__2_RANK)& targets;
    while(moves){
        sq = get_lsb(moves) - 1;
        move_list->move_stack[move_list->size++] = (sq + 7) | (sq << 6) | 0x3000;
        move_list->move_stack[move_list->size++] = (sq + 7) | (sq << 6) | 0x4000;
        move_list->move_stack[move_list->size++] = (sq + 7) | (sq << 6) | 0x5000;
        move_list->move_stack[move_list->size++] = (sq + 7) | (sq << 6) | 0x6000;
        moves &= moves - 1;
    }

}

inline void knight_moves(Bitboard pieces, const Bitboard& targets, MoveList* move_list){
    Bitboard moves;
    Square sq;

    while(pieces){
        sq = get_lsb(pieces) - 1;
        pieces &= pieces - 1;

        // bitwise & with opposite color bitboard
        moves = knight_attacks[sq]&targets;

        while(moves){
            move_list->move_stack[move_list->size++] = sq | ((get_lsb(moves) -1) << 6);
            moves &= moves - 1;
        }
    }
}

inline void king_moves(Bitboard pieces, const Bitboard & targets, MoveList* move_list){
    
    Square sq = get_lsb(pieces) - 1;

    // bitwise & with opposite color bitboard
    Bitboard moves = king_attacks[sq]&targets;

    while(moves){
        move_list->move_stack[move_list->size++] = sq | ((get_lsb(moves)- 1) << 6);
        moves &= moves - 1;
        }
    
}

inline void bishop_moves(Bitboard pieces, const Bitboard& targets, const Bitboard& blockers, MoveList* move_list ){
    Bitboard moves;
    Square sq;

    while(pieces){
        sq = get_lsb(pieces) - 1;
        pieces &= pieces - 1;

        // bitwise & with opposite color bitboard
        moves = get_bishop_attack_BB(sq, blockers)&targets;

        while(moves){
            move_list->move_stack[move_list->size++] = sq | ((get_lsb(moves) -1) << 6);
            moves &= moves - 1;
        }
    }
}

inline void rook_moves(Bitboard pieces, const Bitboard& targets, const Bitboard& blockers, MoveList* move_list ){
    Bitboard moves;
    Square sq;

    while(pieces){
        sq = get_lsb(pieces) - 1;
        pieces &= pieces - 1;

        // bitwise & with opposite color bitboard
        moves = get_rook_attack_BB(sq, blockers)&targets;

        while(moves){
            move_list->move_stack[move_list->size++] = sq | ((get_lsb(moves) -1) << 6);
            moves &= moves - 1;
        }
    }
}


// Psuedolegal moves

void generate_quiet(const Position& pos, MoveList* move_list){
    uint8_t color = pos.to_move;

    // Take all free squares
    Bitboard free_squares = pos.piece_bitboards[no_piece];

    knight_moves(pos.piece_bitboards[knight | color], free_squares, move_list);

    king_moves(pos.piece_bitboards[king | color], free_squares, move_list);
    
    // Queen Moves get generated in at the same time.
    bishop_moves(pos.piece_bitboards[bishop | color]| pos.piece_bitboards[queen | color], 
                free_squares, ~free_squares, move_list);
    
    rook_moves(pos.piece_bitboards[rook | color] | pos.piece_bitboards[queen | color], 
                free_squares, ~free_squares, move_list);
    
    if(color){ // Black
        pawn_quiet_black(pos.piece_bitboards[b_pawn], free_squares, move_list);

        if(pos.castling_rights&(cstl_b)){ 
            //If castling is still possible, generate attacked squares
            Bitboard attacked_sq = attacked_squares(white, pos);

            if(pos.castling_rights&cstl_q){ 
                // Now first check if there is a possibility for queenside castle
                if(!((~pos.piece_bitboards[no_piece])&cstl_squares_q)){ // In between squares must be free
                    if(!(cstl_traverse_q&attacked_sq)){ // The squares that the king needs to pass must not be attacked
                        move_list->move_stack[move_list->size++] = cstl_move_q;
                    }
                    
                }
            }
            if(pos.castling_rights&cstl_k){ 
                if(!((~pos.piece_bitboards[no_piece])&cstl_squares_k)){
                    if(!(cstl_traverse_k&attacked_sq)){
                        move_list->move_stack[move_list->size++] = cstl_move_k;
                    }
                }
            }
        }
        
    }
    else{ // White
        pawn_quiet_white(pos.piece_bitboards[w_pawn], free_squares, move_list);
        
        if(pos.castling_rights&(cstl_w)){ 
            //If castling is still possible, generate attacked squares
            Bitboard attacked_sq = attacked_squares(black, pos);

            if(pos.castling_rights&cstl_Q){ 
                // Now first check if there is a possibility for queenside castle
                if(!((~pos.piece_bitboards[no_piece])&cstl_squares_Q)){ // In between squares must be free
                    if(!(cstl_traverse_Q&attacked_sq)){ // The squares that the king needs to pass must not be attacked
                        move_list->move_stack[move_list->size++] = cstl_move_Q;
                    }
                    
                }
            }
            if(pos.castling_rights&cstl_K){ 
                if(!((~pos.piece_bitboards[no_piece])&cstl_squares_K)){
                    if(!(cstl_traverse_K&attacked_sq)){
                        move_list->move_stack[move_list->size++] = cstl_move_K;
                    }
                }
            }
        }
    }
}


void generate_captures(const Position& pos, MoveList* move_list){
    uint8_t color = pos.to_move;
    // Targets are enemy pieces: if color is white, then the targets are black pieces
    Bitboard targets = pos.piece_bitboards[b_piece ^ color];
    
    knight_moves(pos.piece_bitboards[knight | color], targets, move_list);

    king_moves(pos.piece_bitboards[king | color], targets, move_list);
    
    bishop_moves(pos.piece_bitboards[bishop | color]| pos.piece_bitboards[queen | color], 
                targets, ~pos.piece_bitboards[no_piece], move_list);
    
    rook_moves(pos.piece_bitboards[rook | color] | pos.piece_bitboards[queen | color], 
                targets, ~pos.piece_bitboards[no_piece], move_list);

    if(color){ // Black
        pawn_captures_black(pos.piece_bitboards[b_pawn], targets, move_list);
        // Handle en passant
        if(pos.en_passant){
            if((pos.en_passant != 16) && (pos.board[pos.en_passant + 7] == b_pawn)){
                move_list->move_stack[move_list->size++] 
                = (pos.en_passant + 7) | (pos.en_passant << 6) | 0x2000;
            }
            if((pos.en_passant != 23) && (pos.board[pos.en_passant + 9] == b_pawn)){
                move_list->move_stack[move_list->size++] 
                = (pos.en_passant + 9) | (pos.en_passant << 6) | 0x2000;
            }
        }
    }
    else{ // White
        pawn_captures_white(pos.piece_bitboards[w_pawn], targets, move_list);
        // Handle en passant
        if(pos.en_passant){
            if((pos.en_passant != 47) && (pos.board[pos.en_passant - 7] == w_pawn)){
                move_list->move_stack[move_list->size++] 
                = (pos.en_passant - 7) | (pos.en_passant << 6) | 0x2000;
            }
            if((pos.en_passant != 40) && (pos.board[pos.en_passant - 9] == w_pawn)){
                move_list->move_stack[move_list->size++] 
                = (pos.en_passant - 9) | (pos.en_passant << 6) | 0x2000;
            }
        }
    }

}

void generate_all(const Position& pos, MoveList* move_list){
    uint8_t color = pos.to_move;

    Bitboard targets = pos.piece_bitboards[b_piece ^ color];
    Bitboard free_squares = pos.piece_bitboards[no_piece];
    Bitboard possible_square = targets | free_squares;

    knight_moves(pos.piece_bitboards[knight | color], possible_square, move_list);

    king_moves(pos.piece_bitboards[king | color], possible_square, move_list);
    
    bishop_moves(pos.piece_bitboards[bishop | color]| pos.piece_bitboards[queen | color], 
                possible_square, ~pos.piece_bitboards[no_piece], move_list);
    
    rook_moves(pos.piece_bitboards[rook | color] | pos.piece_bitboards[queen | color], 
                possible_square, ~pos.piece_bitboards[no_piece], move_list);

    if(color){ // Black
        pawn_captures_black(pos.piece_bitboards[b_pawn], targets, move_list);
        // Handle en passant
        if(pos.en_passant){
            if((pos.en_passant != 16) && (pos.board[pos.en_passant + 7] == b_pawn)){
                move_list->move_stack[move_list->size++] 
                = (pos.en_passant + 7) | (pos.en_passant << 6) | 0x2000;
            }
            if((pos.en_passant != 23) && (pos.board[pos.en_passant + 9] == b_pawn)){
                move_list->move_stack[move_list->size++] 
                = (pos.en_passant + 9) | (pos.en_passant << 6) | 0x2000;
            }
        }
        pawn_quiet_black(pos.piece_bitboards[b_pawn], free_squares, move_list);

        if(pos.castling_rights&(cstl_b)){ 
            //If castling is still possible, generate attacked squares
            Bitboard attacked_sq = attacked_squares(white, pos);

            if(pos.castling_rights&cstl_q){ 
                // Now first check if there is a possibility for queenside castle
                if(!((~pos.piece_bitboards[no_piece])&cstl_squares_q)){ // In between squares must be free
                    if(!(cstl_traverse_q&attacked_sq)){ // The squares that the king needs to pass must not be attacked
                        move_list->move_stack[move_list->size++] = cstl_move_q;
                    }
                    
                }
            }
            if(pos.castling_rights&cstl_k){ 
                if(!((~pos.piece_bitboards[no_piece])&cstl_squares_k)){
                    if(!(cstl_traverse_k&attacked_sq)){
                        move_list->move_stack[move_list->size++] = cstl_move_k;
                    }
                }
            }
        }
    }
    else{ // White
        pawn_captures_white(pos.piece_bitboards[w_pawn], targets, move_list);
        // Handle en passant
        if(pos.en_passant){
            if((pos.en_passant != 47) && (pos.board[pos.en_passant - 7] == w_pawn)){
                move_list->move_stack[move_list->size++] 
                = (pos.en_passant - 7) | (pos.en_passant << 6) | 0x2000;
            }
            if((pos.en_passant != 40) && (pos.board[pos.en_passant - 9] == w_pawn)){
                move_list->move_stack[move_list->size++] 
                = (pos.en_passant - 9) | (pos.en_passant << 6) | 0x2000;
            }
        }

        pawn_quiet_white(pos.piece_bitboards[w_pawn], free_squares, move_list);
        
        if(pos.castling_rights&(cstl_w)){ 
            //If castling is still possible, generate attacked squares
            Bitboard attacked_sq = attacked_squares(black, pos);

            if(pos.castling_rights&cstl_Q){ 
                // Now first check if there is a possibility for queenside castle
                if(!((~pos.piece_bitboards[no_piece])&cstl_squares_Q)){ // In between squares must be free
                    if(!(cstl_traverse_Q&attacked_sq)){ // The squares that the king needs to pass must not be attacked
                        move_list->move_stack[move_list->size++] = cstl_move_Q;
                    }
                    
                }
            }
            if(pos.castling_rights&cstl_K){ 
                if(!((~pos.piece_bitboards[no_piece])&cstl_squares_K)){
                    if(!(cstl_traverse_K&attacked_sq)){
                        move_list->move_stack[move_list->size++] = cstl_move_K;
                    }
                }
            }
        }

    }
}

#endif //MOVEGEN