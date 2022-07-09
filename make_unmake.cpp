#ifndef MAKE_UNMAKE
#define MAKE_UNMAKE

#include <array>

#include "types.h"
#include "position.h"
#include "bitboard.h"

//Helper for castling rights

static constexpr std::array<uint8_t, 64> cstl_array = {
    0b1101, 0b1111, 0b1111, 0b1111, 0b1100, 0b1111, 0b1111, 0b1110,
    0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111,
    0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111,
    0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111,
    0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111,
    0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111,
    0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111, 0b1111,
    0b0111, 0b1111, 0b1111, 0b1111, 0b0011, 0b1111, 0b1111, 0b1011
};


UndoObject make_move(Position& pos, Move move){
    Square from = move&0b111111;
    Square to = (move >> 6)&0b111111;

    // Target and moved pieces are saved here
    Piece target = pos.board[to];
    Piece moved = pos.board[from];

    UndoObject undo = UndoObject(moved, target, move, pos.en_passant, pos.castling_rights, pos.position_key);

    // Change the mailbox first, since this might need overwritng in case of promotions
    pos.board[to] = moved;
    pos.board[from] = no_piece;

    // If there is an en_passant square, it will be removed from the key regardless what move is played
    if(pos.en_passant) pos.position_key ^= rnd_value_array[enp_rnd_id + pos.en_passant];

    // Now handle special cases
    switch (move&0xF000)
    {
    case 0:
        pos.en_passant = 0;
        break;
    case 0x8000:
        // CASTLING
        if(from < 8){ //white
            if(to > from){ //kingside
                pos.board[H1] = no_piece;
                pos.board[F1] = w_rook;
                pos.piece_bitboards[w_rook] ^= (1ULL << H1) | (1ULL << F1);
                pos.piece_bitboards[no_piece] ^= (1ULL << H1) | (1ULL << F1);
                pos.piece_bitboards[w_piece] ^= (1ULL << H1) | (1ULL << F1);
                // Update the position key
                pos.position_key ^= rnd_value_array[64*w_rook + H1] 
                                    ^ rnd_value_array[64*w_rook + F1];
            }
            else{
                pos.board[A1] = no_piece;
                pos.board[D1] = w_rook;
                pos.piece_bitboards[w_rook] ^= (1ULL << A1) | (1ULL << D1);
                pos.piece_bitboards[no_piece] ^= (1ULL << A1) | (1ULL << D1);
                pos.piece_bitboards[w_piece] ^= (1ULL << A1) | (1ULL << D1);
                // Update the position key
                pos.position_key ^= rnd_value_array[64*w_rook + A1] 
                                    ^ rnd_value_array[64*w_rook + D1];
            }
            pos.castling_rights &= 0b1100;
        }
        else{ //black
            if(to > from){ //kingside
                pos.board[H8] = no_piece;
                pos.board[F8] = b_rook;
                pos.piece_bitboards[b_rook] ^= (1ULL << H8) | (1ULL << F8);
                pos.piece_bitboards[no_piece] ^= (1ULL << H8) | (1ULL << F8);
                pos.piece_bitboards[b_piece] ^= (1ULL << H8) | (1ULL << F8);
                // Update the position key
                pos.position_key ^= rnd_value_array[64*b_rook + H8] 
                                    ^ rnd_value_array[64*b_rook + F8];
            }
            else{
                pos.board[A8] = no_piece;
                pos.board[D8] = b_rook;
                pos.piece_bitboards[b_rook] ^= (1ULL << A8) | (1ULL << D8);
                pos.piece_bitboards[no_piece] ^= (1ULL << A8) | (1ULL << D8);
                pos.piece_bitboards[b_piece] ^= (1ULL << A8) | (1ULL << D8);
                // Update the position key
                pos.position_key ^= rnd_value_array[64*b_rook + A8] 
                                    ^ rnd_value_array[64*b_rook + D8];
            }
            pos.castling_rights &= 0b0011;
        }
        pos.en_passant = 0;
        break;
    case 0x1000:
        // SET EN PASSANT SQUARE, THEN CONTINUE AS NORMAL
        if(pos.to_move){
            pos.en_passant = (to + 8);
        }
        else{
            pos.en_passant = (to - 8);
        }
        // Update position key if there is an en passant square
        pos.position_key ^= rnd_value_array[enp_rnd_id + pos.en_passant];
        break;
    case 0x2000:
        // EN PASSANT CAPTURE
        if(pos.to_move){
            pos.piece_bitboards[w_pawn] ^= 1ULL << (to + 8);
            pos.piece_bitboards[w_piece] ^= 1ULL << (to + 8);
            pos.piece_bitboards[no_piece] ^= 1ULL << (to + 8);
            pos.board[to + 8] = no_piece;
            // Update the positon key
            pos.position_key ^= rnd_value_array[64*w_pawn + to + 8];
        }
        else{
            pos.piece_bitboards[b_pawn] ^= 1ULL << (to - 8);
            pos.piece_bitboards[b_piece] ^= 1ULL << (to - 8);
            pos.piece_bitboards[no_piece] ^= 1ULL << (to - 8);
            pos.board[to - 8] = no_piece;
            // Update the position key
            pos.position_key ^= rnd_value_array[64*b_pawn + to + 8];
        }
        pos.en_passant = 0;
        break;
    case 0x3000: 
        // PROMOTE TO KNIGHT
        pos.board[to] = static_cast<Piece>(pos.board[to] + 1); // Pawn to knight
        pos.piece_bitboards[moved + 1] ^= (1ULL << to);
        pos.piece_bitboards[moved] ^= (1ULL << to);
        pos.en_passant = 0;
        // Update the positon key
        pos.position_key ^= rnd_value_array[64*(moved + 1) + to]
                            ^ rnd_value_array[64*moved + to];
        break;
    case 0x4000:
        // PROMOTE TO BISHOP
        pos.board[to] = static_cast<Piece>(pos.board[to] + 2); // Pawn to bishop
        pos.piece_bitboards[moved + 2] ^= (1ULL << to);
        pos.piece_bitboards[moved] ^= (1ULL << to);
        pos.en_passant = 0;
        // Update the positon key
        pos.position_key ^= rnd_value_array[64*(moved + 2) + to]
                            ^ rnd_value_array[64*moved + to];
        break;
    case 0x5000:
        // PROMOTE TO ROOK
        pos.board[to] = static_cast<Piece>(pos.board[to] + 3); // Pawn to rook
        pos.piece_bitboards[moved + 3] ^= (1ULL << to);
        pos.piece_bitboards[moved] ^= (1ULL << to);
        pos.en_passant = 0;
        // Update the positon key
        pos.position_key ^= rnd_value_array[64*(moved + 3) + to]
                            ^ rnd_value_array[64*moved + to];
        break;
    case 0x6000:
        // PROMOTE TO QUEEN
        pos.board[to] = static_cast<Piece>(pos.board[to] + 4); // Pawn to queen
        pos.piece_bitboards[moved + 4] ^= (1ULL << to);
        pos.piece_bitboards[moved] ^= (1ULL << to);
        pos.en_passant = 0;
        // Update the positon key
        pos.position_key ^= rnd_value_array[64*(moved + 4) + to]
                            ^ rnd_value_array[64*moved + to];
        break;
    default:
        pos.en_passant = 0;
        break;
    }

    // Then change the bitboards
    pos.piece_bitboards[moved] ^= (1ULL << from) | (1ULL << to);
    pos.piece_bitboards[target] ^= (1ULL << to);
    pos.piece_bitboards[no_piece] ^= (1ULL << from);

    // Occupancy bitboards need updating
    pos.piece_bitboards[w_piece + (pos.to_move != white)] ^= (1ULL << from) | (1ULL << to);

    // If there was a piece captured, change the opposing colors occpuancy
    if(target){
        pos.piece_bitboards[w_piece + (pos.to_move == white)] ^= (1ULL << to);
    }

    // Update Castling rights
    pos.castling_rights &= cstl_array[from]&cstl_array[to];

    // Change side to move
    pos.to_move = black - pos.to_move;

    // Last update the key
    pos.position_key ^= rnd_value_array[to_move_rnd_id] 
                        ^ rnd_value_array[64*moved + from]
                        ^ rnd_value_array[64*moved + to]
                        ^ rnd_value_array[64*target + to];

    if(undo.castling_rights != pos.castling_rights){
        // Remove castling rights from hash if they are different
        uint8_t cstl_difference = undo.castling_rights^pos.castling_rights;
        while(cstl_difference){
            pos.position_key ^= rnd_value_array[cstl_K_rnd_id + get_lsb(cstl_difference) - 1];
            cstl_difference &= cstl_difference - 1;
        }
    }

    // Return Undo instructions
    return undo;
}

void unmake_move(Position& pos, const UndoObject& undo){
    Square from = undo.move&0b111111;
    Square to = (undo.move >> 6)&0b111111;

    // Change side to move back
    pos.to_move = black - pos.to_move;

    // load en passant state and castling rights
    pos.en_passant = undo.en_passant_sq;
    pos.castling_rights = undo.castling_rights;

    // Recover key
    pos.position_key = undo.position_key;

    // special undo operations for special moves
    switch (undo.move&0xF000)
    {
    case 0:
        break;
    case 0x8000:
        // CASTLING
        if(from < 8){ //white
            if(to > from){ //kingside
                //basically an additional rook move has to be made, but it is always the same one
                pos.board[F1] = no_piece;
                pos.board[H1] = w_rook;
                pos.piece_bitboards[w_rook] ^= (1ULL << H1) | (1ULL << F1);
                pos.piece_bitboards[no_piece] ^= (1ULL << H1) | (1ULL << F1);
                pos.piece_bitboards[w_piece] ^= (1ULL << H1) | (1ULL << F1);
            }
            else{
                pos.board[D1] = no_piece;
                pos.board[A1] = w_rook;
                pos.piece_bitboards[w_rook] ^= (1ULL << A1) | (1ULL << D1);
                pos.piece_bitboards[no_piece] ^= (1ULL << A1) | (1ULL << D1);
                pos.piece_bitboards[w_piece] ^= (1ULL << A1) | (1ULL << D1);
            }
        }
        else{ //black
            if(to > from){ //kingside
                pos.board[F8] = no_piece;
                pos.board[H8] = b_rook;
                pos.piece_bitboards[b_rook] ^= (1ULL << H8) | (1ULL << F8);
                pos.piece_bitboards[no_piece] ^= (1ULL << H8) | (1ULL << F8);
                pos.piece_bitboards[b_piece] ^= (1ULL << H8) | (1ULL << F8);
            }
            else{
                pos.board[D8] = no_piece;
                pos.board[A8] = b_rook;
                pos.piece_bitboards[b_rook] ^= (1ULL << A8) | (1ULL << D8);
                pos.piece_bitboards[no_piece] ^= (1ULL << A8) | (1ULL << D8);
                pos.piece_bitboards[b_piece] ^= (1ULL << A8) | (1ULL << D8);
            }
        }
        break;
    case 0x2000:
        // EN PASSANT CAPTURE
        if(pos.to_move){
            pos.piece_bitboards[w_pawn] ^= 1ULL << (to + 8);
            pos.piece_bitboards[w_piece] ^= 1ULL << (to + 8);
            pos.piece_bitboards[no_piece] ^= 1ULL << (to + 8);
            pos.board[to + 8] = w_pawn;
        }
        else{
            pos.piece_bitboards[b_pawn] ^= 1ULL << (to - 8);
            pos.piece_bitboards[b_piece] ^= 1ULL << (to - 8);
            pos.piece_bitboards[no_piece] ^= 1ULL << (to - 8);
            pos.board[to - 8] = b_pawn;
        }
        break;
    case 0x3000:
        // PROMOTE TO KNIGHT
        pos.piece_bitboards[undo.moved_piece + 1] ^= (1ULL << to); //remove the knight
        pos.piece_bitboards[undo.moved_piece] ^= (1ULL << to); //put a pawn there instead
        break;
    case 0x4000:
        // PROMOTE TO BISHOP
        pos.piece_bitboards[undo.moved_piece + 2] ^= (1ULL << to); 
        pos.piece_bitboards[undo.moved_piece] ^= (1ULL << to);
        break;
    case 0x5000:
        // PROMOTE TO ROOK
        pos.piece_bitboards[undo.moved_piece + 3] ^= (1ULL << to); 
        pos.piece_bitboards[undo.moved_piece] ^= (1ULL << to);
        break;
    case 0x6000:
        // PROMOTE TO QUEEN
        pos.piece_bitboards[undo.moved_piece + 4] ^= (1ULL << to); 
        pos.piece_bitboards[undo.moved_piece] ^= (1ULL << to);
        break;
    default:
        break;
    }

    // Undo changes to the mailbox
    pos.board[from] = undo.moved_piece;
    pos.board[to] = undo.target_piece;

    // Undo changes to the bitboards
    pos.piece_bitboards[undo.moved_piece] ^= (1ULL << from) | (1ULL << to);
    pos.piece_bitboards[undo.target_piece] ^= (1ULL << to);
    pos.piece_bitboards[no_piece] ^= (1ULL << from);

    // After changing back the side to move, undo the occupancy changes    
    pos.piece_bitboards[w_piece + (pos.to_move != white)] ^= (1ULL << from) | (1ULL << to);

    // If there was a piece captured, change the opposing colors occpuancy back as well
    if(undo.target_piece){
        pos.piece_bitboards[w_piece + (pos.to_move == white)] ^= (1ULL << to);
    }

}



#endif // MAKE_UNMAKE