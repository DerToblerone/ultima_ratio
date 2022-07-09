#ifndef POSITION_H
#define POSITION_H

#include <array>
#include <stdint.h>
#include <string>

#include "types.h"
#include "utility.h"
#include "bitboard.h"


// Calculate how many random values are needed for the Zobrist hash
// 12 Boards for the pieces
// 2 Boards for empty squares and en passant
// 4 Numbers for castling rights
// 1 Number for side to move

constexpr short random_value_count = 64*14 + 5;

// Give meaningful names to the offsets in the rad_value_array
enum RandTableIndices{
    enp_rnd_id = 64*13,
    to_move_rnd_id = 64*14,
    cstl_K_rnd_id = 64*14 + 1,
    cstl_Q_rnd_id = 64*14 + 2,
    cstl_k_rnd_id = 64*14 + 3,
    cstl_q_rnd_id = 64*14 + 4
};


// When running the program, put random numbers in this array
auto rnd_value_array{[]() {
    std::array<uint64_t, random_value_count> result{};
    
    // Set random seed to get the same hash values every time
    srand(1234567);

    result = {0};
    
    // TODO: MAKE RANDOMNESS BETTER

    // Fill up the array with random values, but we start at 64
    // since the empty squares will not influence the hash, thus
    // their values are all 0 to simplify the code in make/unmake
    for (int i = 64; i < random_value_count; i++)
    {
        result[i] = rand() 
                    ^ ((uint64_t)rand() << 15) 
                    ^ ((uint64_t)rand() << 30) 
                    ^ ((uint64_t)rand() << 45) 
                    ^ ((uint64_t)rand() << 60);
    }
    return result;
}()};

class Position{
    public:
        Position(){
            piece_bitboards = {0ULL};
            piece_bitboards[no_piece]   = 0x0000FFFFFFFF0000ULL;

            piece_bitboards[w_pawn]     = __2_RANK;
            piece_bitboards[w_knight]   = 0x0000000000000042ULL; 
            piece_bitboards[w_king]     = 0x0000000000000010ULL;
            piece_bitboards[w_bishop]   = 0x0000000000000024ULL;
            piece_bitboards[w_rook]     = 0x0000000000000081ULL;
            piece_bitboards[w_queen]    = 0x0000000000000008ULL;

            piece_bitboards[b_pawn]     = __7_RANK;
            piece_bitboards[b_knight]   = 0x4200000000000000ULL;
            piece_bitboards[b_king]     = 0x1000000000000000ULL;
            piece_bitboards[b_bishop]   = 0x2400000000000000ULL;
            piece_bitboards[b_rook]     = 0x8100000000000000ULL;
            piece_bitboards[b_queen]    = 0x0800000000000000ULL;
            
            piece_bitboards[w_piece]    = 0x000000000000FFFFULL;
            piece_bitboards[b_piece]    = 0xFFFF000000000000ULL;

            en_passant = 0;

            board = {   w_rook,     w_knight,   w_bishop,   w_queen,    w_king,     w_bishop,   w_knight,   w_rook,
                        w_pawn,     w_pawn,     w_pawn,     w_pawn,     w_pawn,     w_pawn,     w_pawn,     w_pawn,
                        no_piece,   no_piece,   no_piece,   no_piece,   no_piece,   no_piece,   no_piece,   no_piece,
                        no_piece,   no_piece,   no_piece,   no_piece,   no_piece,   no_piece,   no_piece,   no_piece,
                        no_piece,   no_piece,   no_piece,   no_piece,   no_piece,   no_piece,   no_piece,   no_piece,
                        no_piece,   no_piece,   no_piece,   no_piece,   no_piece,   no_piece,   no_piece,   no_piece,
                        b_pawn,     b_pawn,     b_pawn,     b_pawn,     b_pawn,     b_pawn,     b_pawn,     b_pawn,
                        b_rook,     b_knight,   b_bishop,   b_queen,    b_king,     b_bishop,   b_knight,   b_rook     };

            to_move = white;

            // Bits, from left to right: qkQK
            castling_rights = 0b1111;

            // Initialize the position key
            init_position_key();
            
        }
        /*
        Position bitboards correspond to the enum Piece numbers 
        in types.h, so no_piece = 0 is all empty squares,
        w_pawn = 1 all squares with pawns etc.

        Position board is a mailbox representation, and the same
        enums that index the bitboards represent pieces in the pos_board

        */
        std::array<Bitboard, 15> piece_bitboards;

        std::array<Piece, 64> board;

        Square en_passant;
        
        uint8_t to_move;
        uint8_t castling_rights;

        uint64_t position_key;

        void init_position_key();

        bool is_pseudolegal(Move move);
};

void Position::init_position_key(){
    Bitboard pieces = 0ULL;
    Square sq = 0;

    // Start with a zero key
    this->position_key = 0ULL;

    // First hash in the Piece Bitboards and empty squares
    for(int index; index < 13; index++){
        pieces = this->piece_bitboards[index];
        while(pieces){
            sq = get_lsb(pieces) - 1;
            this->position_key ^= rnd_value_array[64*index + sq];
            pieces &= pieces - 1;
        }
    }

    // Now hash in en passant square if there is one
    if(this->en_passant) this->position_key ^= rnd_value_array[enp_rnd_id + this->en_passant];

    // Consider side to move
    if(this->to_move) this->position_key ^= rnd_value_array[to_move_rnd_id];

    // Consider castling rights
    if(this->castling_rights&cstl_K) this->position_key ^= rnd_value_array[cstl_K_rnd_id];
    if(this->castling_rights&cstl_Q) this->position_key ^= rnd_value_array[cstl_Q_rnd_id];
    if(this->castling_rights&cstl_k) this->position_key ^= rnd_value_array[cstl_k_rnd_id];
    if(this->castling_rights&cstl_q) this->position_key ^= rnd_value_array[cstl_q_rnd_id];

}

bool Position::is_pseudolegal(Move move){
    Square from = from_square(move);
    Square to = to_square(move);
    Piece pce = board[from];
    PieceColor col;

    if(pce==0){
        return false;
    }
    else{
        col = (pce < 6) ? white : black;
    }

    if(col != to_move) return false;
    // else if((board[to] == w_king) || (board[to] == b_king)) return false;
    else{
        switch (pce - col)
        {
            
        case king:
            if  (king_attacks[from]
                &(~piece_bitboards[col ? b_piece : w_piece])
                &(1ULL << to)) return true; 
            break;


        case knight:
            if  (knight_attacks[from]
                &(~piece_bitboards[col ? b_piece : w_piece])
                &(1ULL << to)) return true; 
            break;
        
        case bishop:
            if( get_bishop_attack_BB(from, ~piece_bitboards[no_piece])
                &(~piece_bitboards[col ? b_piece : w_piece])
                &(1ULL << to)) return true;
            break;
        
        case rook:
            if( get_rook_attack_BB(from, ~piece_bitboards[no_piece])
                &(~piece_bitboards[col ? b_piece : w_piece])
                &(1ULL << to)) return true;
            break;

        case queen:
            if( (
                get_bishop_attack_BB(from, ~piece_bitboards[no_piece])
                |get_rook_attack_BB(from, ~piece_bitboards[no_piece])
                )
                &(~piece_bitboards[col ? b_piece : w_piece])
                &(1ULL << to)) return true;
            break;

        
        case pawn:
            if(col){
                
                switch (from - to)
                {
                    case 8:
                        // straight pawn push
                        if(!board[to]) return true;
                        break;

                    case 7:
                        // capture right 
                        // Note that the illegal pawn move a2h1 would not be filtered. But it is impossible for the
                        // move generator to produce this move so it does not need to be checked
                        if((to&0b111) == 7) return false;
                        if(!board[to]) return false;
                        if(board[to] <= 6) return true;
                        else if(en_passant == to) return true;
                        break;

                    case 9:
                        // capture left
                        if((to&0b111) == 0) return false;
                        if(!board[to]) return false;
                        if(board[to] <= 6) return true;
                        else if(en_passant == to) return true;
                        break;

                    case 16:
                        // double push, only possible if both squares are free and the pawn is on rank 2
                        if((!board[to]) && (!board[to + 8]) && (from > 47)) return true;
                        break;
                    
                    default:
                        break;
                }
            }
            else{
                //First calculate the difference between from and two square
                switch (to - from)
                {
                    case 8:
                        // straight pawn push
                        if(!board[to]) return true;
                        break;

                    case 9:
                        // capture right 
                        // Note that the illegal pawn move a2h1 would not be filtered. But it is impossible for the
                        // move generator to produce this move so it does not need to be checked
                        if((to&0b111) == 7) return false;
                        if(board[to] > 6) return true;
                        else if(en_passant == to) return true;
                        break;

                    case 7:
                        // capture left
                        if((to&0b111) == 0) return false;
                        if(board[to] > 6) return true;
                        else if(en_passant == to) return true;
                        break;

                    case 16:
                        // double push, only possible if both squares are free and the pawn is on rank 2
                        if((!board[to]) && (!board[to - 8]) && (from < 16)) return true;
                        break;
                    
                    default:
                        break;
                }
            }
            break;
            
        
        default:
            break;
        }
    }

    return false;
}


std::string read_from_fen(std::string fen, Position& pos){
    int     square_id = 0,
            row_id = 7;

    // Different parsing phases
    bool    board_done = false,
            to_move_done = false,
            castle_done = false,
            all_done = false;

    std::string en_passant_sq = "";
    std::string remaining_string = "";

    // Clear the whole position
    pos.piece_bitboards = {0};
    pos.board = {no_piece};
    pos.en_passant = 0;
    pos.to_move = 0;
    pos.castling_rights = 0;

    // Parse the input string by character
    for(auto c : fen){ 
        if(square_id > 7){
                row_id--;
                square_id = square_id%8;
            }
        if(!board_done){ 
            //first arrange the board
            switch(c){
                case 'P':
                    pos.piece_bitboards[w_pawn] |= 1ULL << (square_id + 8*row_id);
                    pos.piece_bitboards[w_piece] |= 1ULL << (square_id + 8*row_id);
                    pos.board[(square_id + 8*row_id)] = w_pawn;
                    square_id++;
                    break;
                case 'N':
                    pos.piece_bitboards[w_knight] |= 1ULL << (square_id + 8*row_id);
                    pos.piece_bitboards[w_piece] |= 1ULL << (square_id + 8*row_id);
                    pos.board[(square_id + 8*row_id)] = w_knight;
                    square_id++;
                    break;
                case 'K':
                    pos.piece_bitboards[w_king] |= 1ULL << (square_id + 8*row_id);
                    pos.piece_bitboards[w_piece] |= 1ULL << (square_id + 8*row_id);
                    pos.board[(square_id + 8*row_id)] = w_king;
                    square_id++;
                    break;
                case 'B':
                    pos.piece_bitboards[w_bishop] |= 1ULL << (square_id + 8*row_id);
                    pos.piece_bitboards[w_piece] |= 1ULL << (square_id + 8*row_id);
                    pos.board[(square_id + 8*row_id)] = w_bishop;
                    square_id++;
                    break;
                case 'R':
                    pos.piece_bitboards[w_rook] |= 1ULL << (square_id + 8*row_id);
                    pos.piece_bitboards[w_piece] |= 1ULL << (square_id + 8*row_id);
                    pos.board[(square_id + 8*row_id)] = w_rook;
                    square_id++;
                    break;
                case 'Q':
                    pos.piece_bitboards[w_queen] |= 1ULL << (square_id + 8*row_id);
                    pos.piece_bitboards[w_piece] |= 1ULL << (square_id + 8*row_id);
                    pos.board[(square_id + 8*row_id)] = w_queen;
                    square_id++;
                    break;
                case 'p':
                    pos.piece_bitboards[b_pawn] |= 1ULL << (square_id + 8*row_id);
                    pos.piece_bitboards[b_piece] |= 1ULL << (square_id + 8*row_id);
                    pos.board[(square_id + 8*row_id)] = b_pawn;
                    square_id++;
                    break;
                case 'n':
                    pos.piece_bitboards[b_knight] |= 1ULL << (square_id + 8*row_id);
                    pos.piece_bitboards[b_piece] |= 1ULL << (square_id + 8*row_id);
                    pos.board[(square_id + 8*row_id)] = b_knight;
                    square_id++;
                    break;
                case 'k':
                    pos.piece_bitboards[b_king] |= 1ULL << (square_id + 8*row_id);
                    pos.piece_bitboards[b_piece] |= 1ULL << (square_id + 8*row_id);
                    pos.board[(square_id + 8*row_id)] = b_king;
                    square_id++;
                    break;
                case 'b':
                    pos.piece_bitboards[b_bishop] |= 1ULL << (square_id + 8*row_id);
                    pos.piece_bitboards[b_piece] |= 1ULL << (square_id + 8*row_id);
                    pos.board[(square_id + 8*row_id)] = b_bishop;
                    square_id++;
                    break;
                case 'r':
                    pos.piece_bitboards[b_rook] |= 1ULL << (square_id + 8*row_id);
                    pos.piece_bitboards[b_piece] |= 1ULL << (square_id + 8*row_id);
                    pos.board[(square_id + 8*row_id)] = b_rook;
                    square_id++;
                    break;
                case 'q':
                    pos.piece_bitboards[b_queen] |= 1ULL << (square_id + 8*row_id);
                    pos.piece_bitboards[b_piece] |= 1ULL << (square_id + 8*row_id);
                    pos.board[(square_id + 8*row_id)] = b_queen;
                    square_id++;
                    break;
                case '/':
                    break;
                case '1':
                    square_id++;
                    break;
                case '2':
                    square_id+=2;
                    break;
                case '3':
                    square_id+=3;
                    break;
                case '4':
                    square_id+=4;
                    break;
                case '5':
                    square_id+=5;
                    break;
                case '6':
                    square_id+=6;
                    break;
                case '7':
                    square_id+=7;
                    break;
                case '8':
                    square_id+=8;
                    break;
                case ' ':
                    board_done = true;
                    break;

            }
        }
        else if(!to_move_done){
            // Set the side to move
            switch(c){
                case 'w':
                    pos.to_move = white;
                    break;
                case 'b':
                    pos.to_move = black;
                    break;
                    
                default:
                    to_move_done = true;
            }

        }
        else if(!castle_done){
            // Parse castling rights
            switch(c){
                case 'Q':
                     pos.castling_rights |= 0b0010;
                    break;
                case 'K':
                     pos.castling_rights |= 0b0001;
                    break;
                case 'q':
                     pos.castling_rights |= 0b1000;
                    break;
                case 'k':
                     pos.castling_rights |= 0b0100;
                    break;
                case ' ':
                    castle_done = true;
                    break;
                case '-':
                    break;
            }
        }
        else if(!all_done){
            switch(c){
                // Get en passant square
                case '-':
                    break;
                case ' ':
                    all_done = true;
                    break;
                default:
                    en_passant_sq += c;
                    pos.en_passant += char_value_dict[c];
                    if(en_passant_sq.length() > 2){
                        all_done = true;
                    }
                    break;
            }
        }
        else{
            remaining_string += c;
        }
    }

    // Calculate new occuopancy
    pos.piece_bitboards[no_piece] = ~(pos.piece_bitboards[w_piece] | pos.piece_bitboards[b_piece]);
    pos.init_position_key();
    return remaining_string;
}



// INFORMATION BITBOARDS
inline Bitboard attacked_squares(uint_fast8_t color, const Position& pos){
    // Returns squares attacked by this color
    Bitboard attacked_sq = 0ULL;
    Bitboard pieces = pos.piece_bitboards[knight + color];
    Square sq = 0;


    while(pieces){
        sq = get_lsb(pieces) - 1;
        pieces &= pieces - 1;

        attacked_sq |= knight_attacks[sq];
    }

    pieces = pos.piece_bitboards[king + color];
    sq = 0;

    while(pieces){
        sq = get_lsb(pieces) - 1;
        pieces &= pieces - 1;

        attacked_sq |= king_attacks[sq];
    }

    pieces = pos.piece_bitboards[bishop + color]|pos.piece_bitboards[queen + color];
    sq = 0;

    while(pieces){
        sq = get_lsb(pieces) - 1;
        pieces &= pieces - 1;

        attacked_sq |= get_bishop_attack_BB(sq, ~pos.piece_bitboards[no_piece]);
    }

    pieces = pos.piece_bitboards[rook + color]|pos.piece_bitboards[queen + color];
    sq = 0;

    while(pieces){
        sq = get_lsb(pieces) - 1;
        pieces &= pieces - 1;

        attacked_sq |= get_rook_attack_BB(sq, ~pos.piece_bitboards[no_piece]);
    }


    if(color){
        attacked_sq |= pawn_south_east(pos.piece_bitboards[b_pawn]);
        attacked_sq |= pawn_south_west(pos.piece_bitboards[b_pawn]);
    }
    else{
        attacked_sq |= pawn_north_east(pos.piece_bitboards[w_pawn]);
        attacked_sq |= pawn_north_west(pos.piece_bitboards[w_pawn]);
    }



    return attacked_sq;
}

inline Bitboard get_checkers(uint_fast8_t color, const Position& pos){
    // Returns a bitboard of all pieces of the enemy color attacking the king (the checkers)

    // Checks if the king is under attack
    Square king_square = get_lsb(pos.piece_bitboards[king + color]) - 1;


    Bitboard attackers = knight_attacks[king_square]&pos.piece_bitboards[b_knight - color];

    //king 
    attackers |= king_attacks[king_square]&pos.piece_bitboards[b_king - color];

    attackers |= get_bishop_attack_BB(king_square, ~pos.piece_bitboards[no_piece])&
                                    (pos.piece_bitboards[b_bishop - color]|pos.piece_bitboards[b_queen - color]);

    attackers |= get_rook_attack_BB(king_square, ~pos.piece_bitboards[no_piece])&
                                    (pos.piece_bitboards[b_rook - color]|pos.piece_bitboards[b_queen - color]);
    
    // Now check if there are pawn attacks:
    if(color){ 
        attackers |= (pawn_south_east(pos.piece_bitboards[b_king])|
                      pawn_south_west(pos.piece_bitboards[b_king]))&
                      pos.piece_bitboards[w_pawn];
    }
    else{
        attackers |= (pawn_north_east(pos.piece_bitboards[w_king])|
                      pawn_north_west(pos.piece_bitboards[w_king]))&
                      pos.piece_bitboards[b_pawn];
    }

    return attackers;

}

inline Bitboard attacked_by(uint_fast8_t color, Bitboard squares, const Position& pos){
    // Returns a bitboard of all pieces of the enemy color attacking the king (the checkers)
    Bitboard    attackers = 0ULL, 
                squares_to_check = squares;
    Square      sq;
    // Checks if the king is under attack

    if(color){ 
            attackers = (pawn_south_east(pos.piece_bitboards[b_pawn])|
                        pawn_south_west(pos.piece_bitboards[b_pawn]));
        }
    else{
            attackers = (pawn_north_east(pos.piece_bitboards[w_pawn])|
                        pawn_north_west(pos.piece_bitboards[w_pawn]));
        }
    if(attackers&squares) return true;

    while(squares_to_check){
        sq = get_lsb(squares_to_check) - 1;

        attackers |= get_bishop_attack_BB(sq, ~pos.piece_bitboards[no_piece])&
                     (pos.piece_bitboards[w_bishop + color]|pos.piece_bitboards[w_queen + color]);

        if(attackers&squares) return true;

        attackers |= get_rook_attack_BB(sq, ~pos.piece_bitboards[no_piece])&
                     (pos.piece_bitboards[w_rook + color]|pos.piece_bitboards[w_queen + color]);
                     
        if(attackers&squares) return true;

        attackers |= knight_attacks[sq]&pos.piece_bitboards[w_knight + color];

        attackers |= king_attacks[sq]&pos.piece_bitboards[w_king + color];

        if(attackers&squares) return true;

        squares_to_check &= squares_to_check - 1;
    }
    return false;

}


#endif //POSITION_H