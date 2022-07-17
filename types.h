#ifndef TYPES_H
#define TYPES_H


#include <stdint.h>
#include <array>
#include <map>

// Macros, Type definitions and enums

// MACROS:

// Maximum number of moves in a MoveList
#define max_moves 256

//Helper macros for castling, format: qkQK
#define cstl_q 0b1000
#define cstl_k 0b100
#define cstl_Q 0b10
#define cstl_K 0b1

#define cstl_w 0b11
#define cstl_b 0b1100

// These squares must be free in order for castling to be possible
#define cstl_squares_q 0x0E00000000000000ULL
#define cstl_squares_k 0x6000000000000000ULL
#define cstl_squares_Q 0xEULL
#define cstl_squares_K 0x60ULL

// Squares that the king has to traverse while castling
#define cstl_traverse_q 0x1C00000000000000ULL
#define cstl_traverse_k 0x7000000000000000ULL
#define cstl_traverse_Q 0x1CULL
#define cstl_traverse_K 0x70ULL

// Actual moves for castling
#define cstl_move_q 0x8EBC
#define cstl_move_k 0x8FBC
#define cstl_move_Q 0x8084
#define cstl_move_K 0x8184




// Types and enums
typedef uint64_t Bitboard;
typedef uint16_t Move;
typedef uint16_t Score;
typedef uint8_t Square;
typedef uint8_t PieceColor;
typedef uint8_t Piece; // new

constexpr uint16_t is_special_pawn_move = 0x7000;

enum PieceType
{
    empty = 0,
    pawn = 1,
    knight = 2,
    bishop = 3,
    rook = 4,
    queen = 5,
    king = 6
};

enum Colors
{
    black = 8, //1000
    white = 0,
};

// This is also the mapping of the Bitboards
// PieceBitboards&type_mask = PieceType
// Piece
enum PieceBitboards
{
    no_piece = 0,
    w_pawn = 1,
    w_knight = 2,
    w_bishop = 3,
    w_rook = 4,
    w_queen = 5,
    w_king = 6,
    w_piece = 7,
    
    dummy = 8,
    b_pawn = 9,
    b_knight = 10,
    b_bishop = 11,
    b_rook = 12,
    b_queen = 13,
    b_king = 14,
    b_piece = 15
};

constexpr char num_types = 16;

constexpr char color_mask = 0b1000; // 8
constexpr char type_mask = 0b111; // 7

// Square constants and strings

enum SquareMacros{
    A1 = 0,
    B1 = 1,
    C1 = 2,
    D1 = 3,
    E1 = 4,
    F1 = 5,
    G1 = 6,
    H1 = 7,
    A2 = 8,
    B2 = 9,
    C2 = 10,
    D2 = 11,
    E2 = 12,
    F2 = 13,
    G2 = 14,
    H2 = 15,
    A3 = 16,
    B3 = 17,
    C3 = 18,
    D3 = 19,
    E3 = 20,
    F3 = 21,
    G3 = 22,
    H3 = 23,
    A4 = 24,
    B4 = 25,
    C4 = 26,
    D4 = 27,
    E4 = 28,
    F4 = 29,
    G4 = 30,
    H4 = 31,
    A5 = 32,
    B5 = 33,
    C5 = 34,
    D5 = 35,
    E5 = 36,
    F5 = 37,
    G5 = 38,
    H5 = 39,
    A6 = 40,
    B6 = 41,
    C6 = 42,
    D6 = 43,
    E6 = 44,
    F6 = 45,
    G6 = 46,
    H6 = 47,
    A7 = 48,
    B7 = 49,
    C7 = 50,
    D7 = 51,
    E7 = 52,
    F7 = 53,
    G7 = 54,
    H7 = 55,
    A8 = 56,
    B8 = 57,
    C8 = 58,
    D8 = 59,
    E8 = 60,
    F8 = 61,
    G8 = 62,
    H8 = 63
};

const char* square_names[64] ={
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1",
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
};

std::map<char, int> char_value_dict {   {'A', 0}, {'B', 1}, {'C', 2},  {'D', 3},  {'E', 4},  {'F', 5}, {'G', 6},  {'H', 7}, 
                                        {'a', 0}, {'b', 1}, {'c', 2},  {'d', 3},  {'e', 4},  {'f', 5}, {'g', 6},  {'h', 7},
                                        {'1', 0}, {'2', 8}, {'3', 16}, {'4', 24}, {'5', 32}, {'6', 40}, {'7', 48}, {'8', 56}, };

// Organizing moves

struct MoveList
{

    std::array<Move, max_moves> move_stack;
    std::array<Move, max_moves> score_stack;
    int size;

    MoveList() : size(0), move_stack({0}), score_stack({0}) {}
};

struct UndoObject
{

    Piece moved_piece;
    Piece target_piece;

    Move move;

    Square en_passant_sq;

    uint8_t castling_rights;

    uint64_t position_key;

    UndoObject(Piece moved, Piece target, Move m, Square enp_sq, uint8_t cstl, uint64_t pos_key) : 
                moved_piece(moved), 
                target_piece(target), 
                move(m), 
                en_passant_sq(enp_sq), 
                castling_rights(cstl),
                position_key(pos_key) {}
    
    UndoObject() {};
};

// SEARCH INFO AND TT
enum SpecialScores{
    illegal_position =  0xFFFFFFF,
    infinity_score =    0xFFFFFF,
    checkmate_score =   0xFFFFF,
    stalemate_score =   0,
    mate_dep_margin = 200
};


constexpr uint8_t entry_flag_mask = 0b11000000;
constexpr uint8_t entry_dep_mask  =   0b111111;
/*
enum DataOffsets{
    score_data_offset = 32,
    move_data_offset = 16,
    info_data_offset = 0
};*/

enum EntryFlags{
    const_entry = 0xC0,

    upper_bound = 0x80,
    exact_score = 0x40,
    lower_bound = 0x00,
};

struct TableEntry{
    uint64_t validation_key;

    //uint64_t data;
    Move move;
    int32_t score;
    uint8_t info;
    // Info is structured as such
    // Top 2 bits are flags
    // Bottom 6 bits are depth (up to 63)

    TableEntry() : validation_key(0), /*data(0) {}*/ move(0), score(0), info(0) {}
    TableEntry(uint64_t v_key, Move m, int32_t sc, uint8_t nfo) : 
        validation_key(v_key) , move(m), score(sc), info(nfo) {}
        //data = (move << move_data_offset) | (score<< score_data_offset) | (info << info_data_offset);
        //}
};

#endif // TYPES_H