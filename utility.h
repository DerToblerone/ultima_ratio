#ifndef UTILITY_H
#define UTILITY_H

#include "types.h"

/*
This file contains simple macros for manipulating the board as 
well as the instructions for performing bitscans and counting the 
bits set



*/

// Convenient Macros
#define from_square(move) ((move) & 0b111111)
#define to_square(move) ((move >> 6) & 0b111111)
#define occupancy(color) (occupancy_index + (color == black))


// Constants, all caps and __ before

#define __A_FILE 0x0101010101010101ULL
#define __B_FILE 0x0202020202020202ULL
#define __C_FILE 0x0404040404040404ULL
#define __D_FILE 0x0808080808080808ULL
#define __E_FILE 0x1010101010101010ULL
#define __F_FILE 0x2020202020202020ULL
#define __G_FILE 0x4040404040404040ULL
#define __H_FILE 0x8080808080808080ULL

#define __1_RANK 0x00000000000000FFULL
#define __2_RANK 0x000000000000FF00ULL
#define __3_RANK 0x0000000000FF0000ULL
#define __4_RANK 0x00000000FF000000ULL
#define __5_RANK 0x000000FF00000000ULL
#define __6_RANK 0x0000FF0000000000ULL
#define __7_RANK 0x00FF000000000000ULL
#define __8_RANK 0xFF00000000000000ULL


//Helper macros for promotion handling
#define non_promoting_w 0xFF00FFFFFFFFFFFFULL
#define non_promoting_b 0xFFFFFFFFFFFF00FFULL


constexpr static Bitboard rank_array[8] = { __1_RANK,
                                            __2_RANK,
                                            __3_RANK,
                                            __4_RANK,
                                            __5_RANK,
                                            __6_RANK,
                                            __7_RANK,
                                            __8_RANK};

constexpr static Bitboard file_array[8] = { __A_FILE,
                                            __B_FILE,
                                            __C_FILE,
                                            __D_FILE,
                                            __E_FILE,
                                            __F_FILE,
                                            __G_FILE,
                                            __H_FILE};


// FAST BIT PARSING
// get lsb and count bits set use gcc builtin functions
inline int get_lsb(Bitboard bb)     {return __builtin_ffsll(bb);}
// NOTE: Board indices = lsb - 1
// this may not be portable, it is a gcc specific compiler instruction
// TO DO: implement de Bruijn sequence ffsll to make code portable

inline int count_bits(Bitboard bb)  {return __builtin_popcountll(bb);}

// This template makes it possible to just write for(auto i : reverse(array))
// to iterate backwards over an array
template<typename It>
class Range
{
    It b, e;
public:
    Range(It b, It e) : b(b), e(e) {}
    It begin() const { return b; }
    It end() const { return e; }
};

template<typename ORange, typename OIt = decltype(std::begin(std::declval<ORange>())), typename It = std::reverse_iterator<OIt>>
Range<It> reverse(ORange && originalRange) {
    return Range<It>(It(std::end(originalRange)), It(std::begin(originalRange)));
}

#endif //UTILITY_H