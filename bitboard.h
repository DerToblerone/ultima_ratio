#ifndef ATTACKS
#define ATTACKS

#ifdef __INTELLISENSE__
    #pragma diag_suppress 28
#endif

#include <array>
#include <iostream>

#include "types.h"
#include "utility.h"

/*
This file contains the logic and tables to generate attack BBs for
a piece on Square x 

for Knights and Kings it is simple, their possible attacks are stored
in the arrays knight_attack and king_attack respectively. So king_attack[x]
contains a BB with all squares attacked by a king on x

for sliding pieces, the function get_rook_attack_BB and get_bishop_attack_BB
must be called like so:

get_rook_attack_BB(Square, Blockers)

for pawns, all moves are made at once via bit shifts

*/


// direction arrays for move genereation
#define __B_DIR { 0x11, -0x11, -0x10 + 0x01, -0x01 + 0x10}
#define __R_DIR { 0x01, -0x01, -0x10, 0x10}
#define __N_DIR { 0x12, 0x21, -0x10 + 0x02, 0x10 - 0x02, -0x12, -0x21, -0x01 + 0x20, 0x01 -0x20}
#define __K_DIR { 0x01, 0x10, -0x10, -0x01, 0x11, -0x11, -0x01 + 0x10, 0x01 -0x10}


// Values for magic bitboard move generation. Note that the boards in the rook_mask and bishop_mask arrays do not
// contain the squares on the edge of the board, since they are not important for generating the movement masks

static constexpr std::array<uint8_t, 64> rook_shift=
    {
        52, 53, 53, 53, 53, 53, 53, 52,
        53, 54, 54, 54, 54, 54, 54, 53,
        53, 54, 54, 54, 54, 54, 54, 53,
        53, 54, 54, 54, 54, 54, 54, 53,
        53, 54, 54, 54, 54, 54, 54, 53,
        53, 54, 54, 54, 54, 54, 54, 53,
        53, 54, 54, 54, 54, 54, 54, 53,
        53, 54, 54, 53, 53, 53, 53, 53
    };

static constexpr std::array<uint64_t, 64> rook_factor=
    {
         0x0080001020400080ULL,  0x0040001000200040ULL,  0x0080081000200080ULL,  0x0080040800100080ULL,
         0x0080020400080080ULL,  0x0080010200040080ULL,  0x0080008001000200ULL,  0x0080002040800100ULL,
         0x0000800020400080ULL,  0x0000400020005000ULL,  0x0000801000200080ULL,  0x0000800800100080ULL,
         0x0000800400080080ULL,  0x0000800200040080ULL,  0x0000800100020080ULL,  0x0000800040800100ULL,
         0x0000208000400080ULL,  0x0000404000201000ULL,  0x0000808010002000ULL,  0x0000808008001000ULL,
         0x0000808004000800ULL,  0x0000808002000400ULL,  0x0000010100020004ULL,  0x0000020000408104ULL,
         0x0000208080004000ULL,  0x0000200040005000ULL,  0x0000100080200080ULL,  0x0000080080100080ULL,
         0x0000040080080080ULL,  0x0000020080040080ULL,  0x0000010080800200ULL,  0x0000800080004100ULL,
         0x0000204000800080ULL,  0x0000200040401000ULL,  0x0000100080802000ULL,  0x0000080080801000ULL,
         0x0000040080800800ULL,  0x0000020080800400ULL,  0x0000020001010004ULL,  0x0000800040800100ULL,
         0x0000204000808000ULL,  0x0000200040008080ULL,  0x0000100020008080ULL,  0x0000080010008080ULL,
         0x0000040008008080ULL,  0x0000020004008080ULL,  0x0000010002008080ULL,  0x0000004081020004ULL,
         0x0000204000800080ULL,  0x0000200040008080ULL,  0x0000100020008080ULL,  0x0000080010008080ULL,
         0x0000040008008080ULL,  0x0000020004008080ULL,  0x0000800100020080ULL,  0x0000800041000080ULL,
         0x00FFFCDDFCED714AULL,  0x007FFCDDFCED714AULL,  0x003FFFCDFFD88096ULL,  0x0000040810002101ULL,
         0x0001000204080011ULL,  0x0001000204000801ULL,  0x0001000082000401ULL,  0x0001FFFAABFAD1A2ULL
    };

static constexpr std::array<Bitboard, 64> rook_mask=
    {	
         0x000101010101017EULL,  0x000202020202027CULL,  0x000404040404047AULL,  0x0008080808080876ULL,
         0x001010101010106EULL,  0x002020202020205EULL,  0x004040404040403EULL,  0x008080808080807EULL,
         0x0001010101017E00ULL,  0x0002020202027C00ULL,  0x0004040404047A00ULL,  0x0008080808087600ULL,
         0x0010101010106E00ULL,  0x0020202020205E00ULL,  0x0040404040403E00ULL,  0x0080808080807E00ULL,
         0x00010101017E0100ULL,  0x00020202027C0200ULL,  0x00040404047A0400ULL,  0x0008080808760800ULL,
         0x00101010106E1000ULL,  0x00202020205E2000ULL,  0x00404040403E4000ULL,  0x00808080807E8000ULL,
         0x000101017E010100ULL,  0x000202027C020200ULL,  0x000404047A040400ULL,  0x0008080876080800ULL,
         0x001010106E101000ULL,  0x002020205E202000ULL,  0x004040403E404000ULL,  0x008080807E808000ULL,
         0x0001017E01010100ULL,  0x0002027C02020200ULL,  0x0004047A04040400ULL,  0x0008087608080800ULL,
         0x0010106E10101000ULL,  0x0020205E20202000ULL,  0x0040403E40404000ULL,  0x0080807E80808000ULL,
         0x00017E0101010100ULL,  0x00027C0202020200ULL,  0x00047A0404040400ULL,  0x0008760808080800ULL,
         0x00106E1010101000ULL,  0x00205E2020202000ULL,  0x00403E4040404000ULL,  0x00807E8080808000ULL,
         0x007E010101010100ULL,  0x007C020202020200ULL,  0x007A040404040400ULL,  0x0076080808080800ULL,
         0x006E101010101000ULL,  0x005E202020202000ULL,  0x003E404040404000ULL,  0x007E808080808000ULL,
         0x7E01010101010100ULL,  0x7C02020202020200ULL,  0x7A04040404040400ULL,  0x7608080808080800ULL,
         0x6E10101010101000ULL,  0x5E20202020202000ULL,  0x3E40404040404000ULL,  0x7E80808080808000ULL
    };

static constexpr std::array<Bitboard, 64> rook_rays = 
    {
        0x1010101010101feULL,   0x2020202020202fdULL,   0x4040404040404fbULL,   0x8080808080808f7ULL,
        0x10101010101010efULL,  0x20202020202020dfULL,  0x40404040404040bfULL,  0x808080808080807fULL,
        0x10101010101fe01ULL,   0x20202020202fd02ULL,   0x40404040404fb04ULL,   0x80808080808f708ULL,
        0x101010101010ef10ULL,  0x202020202020df20ULL,  0x404040404040bf40ULL,  0x8080808080807f80ULL,
        0x101010101fe0101ULL,   0x202020202fd0202ULL,   0x404040404fb0404ULL,   0x808080808f70808ULL,
        0x1010101010ef1010ULL,  0x2020202020df2020ULL,  0x4040404040bf4040ULL,  0x80808080807f8080ULL,
        0x1010101fe010101ULL,   0x2020202fd020202ULL,   0x4040404fb040404ULL,   0x8080808f7080808ULL,
        0x10101010ef101010ULL,  0x20202020df202020ULL,  0x40404040bf404040ULL,  0x808080807f808080ULL,
        0x10101fe01010101ULL,   0x20202fd02020202ULL,   0x40404fb04040404ULL,   0x80808f708080808ULL,
        0x101010ef10101010ULL,  0x202020df20202020ULL,  0x404040bf40404040ULL,  0x8080807f80808080ULL,
        0x101fe0101010101ULL,   0x202fd0202020202ULL,   0x404fb0404040404ULL,   0x808f70808080808ULL,
        0x1010ef1010101010ULL,  0x2020df2020202020ULL,  0x4040bf4040404040ULL,  0x80807f8080808080ULL,
        0x1fe010101010101ULL,   0x2fd020202020202ULL,   0x4fb040404040404ULL,   0x8f7080808080808ULL,
        0x10ef101010101010ULL,  0x20df202020202020ULL,  0x40bf404040404040ULL,  0x807f808080808080ULL,
        0xfe01010101010101ULL,  0xfd02020202020202ULL,  0xfb04040404040404ULL,  0xf708080808080808ULL,
        0xef10101010101010ULL,  0xdf20202020202020ULL,  0xbf40404040404040ULL,  0x7f80808080808080ULL
    };

static constexpr std::array<uint8_t, 64> bishop_shift=
    {
        58, 59, 59, 59, 59, 59, 59, 58,
        59, 59, 59, 59, 59, 59, 59, 59,
        59, 59, 57, 57, 57, 57, 59, 59,
        59, 59, 57, 55, 55, 57, 59, 59,
        59, 59, 57, 55, 55, 57, 59, 59,
        59, 59, 57, 57, 57, 57, 59, 59,
        59, 59, 59, 59, 59, 59, 59, 59,
        58, 59, 59, 59, 59, 59, 59, 58
    };

static constexpr std::array<uint64_t, 64> bishop_factor=
    {
         0x0002020202020200ULL,  0x0002020202020000ULL,  0x0004010202000000ULL,  0x0004040080000000ULL,
         0x0001104000000000ULL,  0x0000821040000000ULL,  0x0000410410400000ULL,  0x0000104104104000ULL,
         0x0000040404040400ULL,  0x0000020202020200ULL,  0x0000040102020000ULL,  0x0000040400800000ULL,
         0x0000011040000000ULL,  0x0000008210400000ULL,  0x0000004104104000ULL,  0x0000002082082000ULL,
         0x0004000808080800ULL,  0x0002000404040400ULL,  0x0001000202020200ULL,  0x0000800802004000ULL,
         0x0000800400A00000ULL,  0x0000200100884000ULL,  0x0000400082082000ULL,  0x0000200041041000ULL,
         0x0002080010101000ULL,  0x0001040008080800ULL,  0x0000208004010400ULL,  0x0000404004010200ULL,
         0x0000840000802000ULL,  0x0000404002011000ULL,  0x0000808001041000ULL,  0x0000404000820800ULL,
         0x0001041000202000ULL,  0x0000820800101000ULL,  0x0000104400080800ULL,  0x0000020080080080ULL,
         0x0000404040040100ULL,  0x0000808100020100ULL,  0x0001010100020800ULL,  0x0000808080010400ULL,
         0x0000820820004000ULL,  0x0000410410002000ULL,  0x0000082088001000ULL,  0x0000002011000800ULL,
         0x0000080100400400ULL,  0x0001010101000200ULL,  0x0002020202000400ULL,  0x0001010101000200ULL,
         0x0000410410400000ULL,  0x0000208208200000ULL,  0x0000002084100000ULL,  0x0000000020880000ULL,
         0x0000001002020000ULL,  0x0000040408020000ULL,  0x0004040404040000ULL,  0x0002020202020000ULL,
         0x0000104104104000ULL,  0x0000002082082000ULL,  0x0000000020841000ULL,  0x0000000000208800ULL,
         0x0000000010020200ULL,  0x0000000404080200ULL,  0x0000040404040400ULL,  0x0002020202020200ULL
    };

static constexpr std::array<Bitboard, 64> bishop_mask=
    {
         0x0040201008040200ULL,  0x0000402010080400ULL,  0x0000004020100A00ULL,  0x0000000040221400ULL,
         0x0000000002442800ULL,  0x0000000204085000ULL,  0x0000020408102000ULL,  0x0002040810204000ULL,
         0x0020100804020000ULL,  0x0040201008040000ULL,  0x00004020100A0000ULL,  0x0000004022140000ULL,
         0x0000000244280000ULL,  0x0000020408500000ULL,  0x0002040810200000ULL,  0x0004081020400000ULL,
         0x0010080402000200ULL,  0x0020100804000400ULL,  0x004020100A000A00ULL,  0x0000402214001400ULL,
         0x0000024428002800ULL,  0x0002040850005000ULL,  0x0004081020002000ULL,  0x0008102040004000ULL,
         0x0008040200020400ULL,  0x0010080400040800ULL,  0x0020100A000A1000ULL,  0x0040221400142200ULL,
         0x0002442800284400ULL,  0x0004085000500800ULL,  0x0008102000201000ULL,  0x0010204000402000ULL,
         0x0004020002040800ULL,  0x0008040004081000ULL,  0x00100A000A102000ULL,  0x0022140014224000ULL,
         0x0044280028440200ULL,  0x0008500050080400ULL,  0x0010200020100800ULL,  0x0020400040201000ULL,
         0x0002000204081000ULL,  0x0004000408102000ULL,  0x000A000A10204000ULL,  0x0014001422400000ULL,
         0x0028002844020000ULL,  0x0050005008040200ULL,  0x0020002010080400ULL,  0x0040004020100800ULL,
         0x0000020408102000ULL,  0x0000040810204000ULL,  0x00000A1020400000ULL,  0x0000142240000000ULL,
         0x0000284402000000ULL,  0x0000500804020000ULL,  0x0000201008040200ULL,  0x0000402010080400ULL,
         0x0002040810204000ULL,  0x0004081020400000ULL,  0x000A102040000000ULL,  0x0014224000000000ULL,
         0x0028440200000000ULL,  0x0050080402000000ULL,  0x0020100804020000ULL,  0x0040201008040200ULL
    };

static constexpr std::array<Bitboard, 64> bishop_rays = 
    {
        0x8040201008040200ULL,  0x80402010080500ULL,    0x804020110a00ULL,      0x8041221400ULL,
        0x182442800ULL,         0x10204885000ULL,       0x102040810a000ULL,     0x102040810204000ULL,
        0x4020100804020002ULL,  0x8040201008050005ULL,  0x804020110a000aULL,    0x804122140014ULL,
        0x18244280028ULL,       0x1020488500050ULL,     0x102040810a000a0ULL,   0x204081020400040ULL,
        0x2010080402000204ULL,  0x4020100805000508ULL,  0x804020110a000a11ULL,  0x80412214001422ULL,
        0x1824428002844ULL,     0x102048850005088ULL,   0x2040810a000a010ULL,   0x408102040004020ULL,
        0x1008040200020408ULL,  0x2010080500050810ULL,  0x4020110a000a1120ULL,  0x8041221400142241ULL,
        0x182442800284482ULL,   0x204885000508804ULL,   0x40810a000a01008ULL,   0x810204000402010ULL,
        0x804020002040810ULL,   0x1008050005081020ULL,  0x20110a000a112040ULL,  0x4122140014224180ULL,
        0x8244280028448201ULL,  0x488500050880402ULL,   0x810a000a0100804ULL,   0x1020400040201008ULL,
        0x402000204081020ULL,   0x805000508102040ULL,   0x110a000a11204080ULL,  0x2214001422418000ULL,
        0x4428002844820100ULL,  0x8850005088040201ULL,  0x10a000a010080402ULL,  0x2040004020100804ULL,
        0x200020408102040ULL,   0x500050810204080ULL,   0xa000a1120408000ULL,   0x1400142241800000ULL,
        0x2800284482010000ULL,  0x5000508804020100ULL,  0xa000a01008040201ULL,  0x4000402010080402ULL,
        0x2040810204080ULL,     0x5081020408000ULL,     0xa112040800000ULL,     0x14224180000000ULL,
        0x28448201000000ULL,    0x50880402010000ULL,    0xa0100804020100ULL,    0x40201008040201ULL,
    };



// MOVE PRE-GEN:

// Initialization at compile time
// Offsets are used to index the attack tables

// calc rook offsets, offset[i] is start of the table for square i
// offset [i + 1] is the end
static constexpr auto rook_table_offset{[]() constexpr{
    std::array<unsigned int, 65> result{};
    result[0] = 0;
    for (int i = 0; i < 64; i++){
        result[i + 1] = result[i] + (1 << (64 - rook_shift[i]));
    }
    return result;
}()};

// calc bishop offsets
static constexpr auto bishop_table_offset{[]() constexpr{
    std::array<unsigned int, 65> result{};
    result[0] = 0;
    for (int i = 0; i < 64; i++){
        result[i + 1] = result[i] + (1 << (64 - bishop_shift[i]));
    }
    return result;
}()};

// The actual bitboards are generated via 0x88, so some help here:

constexpr Bitboard x88_to_bitboard(short x88_square){
    // 0x88 board square representation to Bitboard
    return (1ULL << (x88_square%8 + ((x88_square&0xF0)>>1)));
}

constexpr Square x88_to_square(short x88_square){
    // 0x88 board square representation to Square
    return (x88_square&0xF) +((x88_square&0xF0)>>1); 
}

// This function is used to generate all possible blocker combinations
constexpr short get_permutations(Bitboard possible_squares){ 
    short num_squares = 0;
    Bitboard bitmask = possible_squares;// store the attack_mask of all possible squares
    while(bitmask){
        bitmask = bitmask^(bitmask & (-bitmask));// erase the lowest bit set and increment counter
        num_squares++;
    }
    return 1 <<  num_squares;// there are 2**num_Squares possible blocking combinations
}

// converts a number in to a possible blocking configuration
// by using a pre mask with all possible blocking bits set
constexpr Bitboard num_to_mask(short permutation_number, Bitboard premask){
    Bitboard   bitmask = premask,
                result = 0;
    int id = 0;
    while(bitmask){
        if(permutation_number & (1 << id)){
            result |= bitmask & (-bitmask);
        }
        bitmask = bitmask^(bitmask & (-bitmask));
        id++;
    }
    return result;
}

// very slow way to generate sliding attacks
constexpr Bitboard gen_rook_attacks(Square sq, Bitboard occup_mask){
    Bitboard result = 0;
    short x88_index = (sq%8) +((sq/8) << 4); //translate to 0x88
    short new_square = 0;

    for(auto offset : __R_DIR){
        for(int i = 1; i < 8; i++){
            new_square = x88_index + offset*i;
            if(!(new_square&0x88)){
                result |= x88_to_bitboard(new_square);
            }
            else{
                break;
            }
            if(occup_mask&(1ULL << x88_to_square(new_square))){
                break;
            }
        }
    }
    return result;
}

constexpr Bitboard gen_bishop_attacks(Square sq, Bitboard occup_mask){
    Bitboard result = 0;
    short x88_index = (sq%8) +((sq/8) << 4); //translate to 0x88
    short new_square = 0;

    for(auto offset : __B_DIR){
        for(int i = 1; i < 8; i++){
            new_square = x88_index + offset*i;
            if(!(new_square&0x88)){
                result |= x88_to_bitboard(new_square);
            }
            else{
                break;
            }
            if(occup_mask&(1ULL << x88_to_square(new_square))){
                break;
            }
        }
    }
    return result;
}

// now generate actual Table

static constexpr auto rook_attacks{[]() constexpr{
    constexpr int size = rook_table_offset[64];
    std::array<Bitboard, size> result{};
    
    for (int i = 0; i < size; i++) result[i] = 0;
    
    for (int sq = 0; sq < 64; sq++)
    {
        short   max_val = get_permutations(rook_mask[sq]),
                index = 0;

        Bitboard    mask = 0ULL;
        
        for(int i = 0; i < max_val; i++){//see if it works in this loop
            mask = num_to_mask(i, rook_mask[sq]);

            index = (mask * rook_factor[sq]) >> rook_shift[sq];
            
            result[rook_table_offset[sq] + index] =  gen_rook_attacks(sq, mask);
            
        }
    }
    return result;
}()};

static constexpr auto bishop_attacks{[]() constexpr{
    constexpr int size = bishop_table_offset[64];
    std::array<Bitboard, size> result{};
    
    for (int i = 0; i < size; i++) result[i] = 0;
    
    for (int sq = 0; sq < 64; sq++)
    {
        short   max_val = get_permutations(bishop_mask[sq]),
                index = 0;

        Bitboard    mask = 0ULL;
        
        for(int i = 0; i < max_val; i++){//see if it works in this loop
            mask = num_to_mask(i, bishop_mask[sq]);

            index = (mask * bishop_factor[sq]) >> bishop_shift[sq];
            
            result[bishop_table_offset[sq] + index] =  gen_bishop_attacks(sq, mask);
            
        }
    }
    return result;
}()};

static constexpr auto knight_attacks{[]() constexpr{
    std::array<Bitboard, 64> result{};
    short attacked_sq = 0;
    Bitboard attack = 0ULL;
    for (int sq = 0; sq < 64; sq++)
    {
        attacked_sq = 0;
        attack = 0ULL;

        for(auto offset : __N_DIR){
            attacked_sq = (sq%8) +((sq/8) << 4) + offset; //translate to 0x88 and add direction
            if(!(attacked_sq&0x88)){ // if it is on the board, add to attack BB
                attack |= x88_to_bitboard(attacked_sq);
            }
        }
        result[sq] = attack;
    }
    return result;
}()};

static constexpr auto king_attacks{[]() constexpr{
    std::array<Bitboard, 64> result{};
    short attacked_sq = 0;
    Bitboard attack = 0ULL;
    for (int sq = 0; sq < 64; sq++)
    {
        attacked_sq = 0;
        attack = 0ULL;

        for(auto offset : __K_DIR){
            attacked_sq = (sq%8) +((sq/8) << 4) + offset; //translate to 0x88 and add direction
            if(!(attacked_sq&0x88)){ // if it is on the board, add to attack BB
                attack |= x88_to_bitboard(attacked_sq);
            }
        }
        result[sq] = attack;
    }
    return result;
}()};

// PRE-GEN END









// Getter functions for rook and bishop attacks.

inline Bitboard get_rook_attack_BB(Square sq, const Bitboard& blockers){
    //this looks up the correct attack board using the magic bitboard technique
    //at this stage, no distinctions between ones own and enemy pieces are made
    return rook_attacks[rook_table_offset[sq] +
        (((blockers&rook_mask[sq])*rook_factor[sq]) >> rook_shift[sq])];
}

inline Bitboard get_bishop_attack_BB(Square sq, const Bitboard& blockers){
    //this looks up the correct attack board using the magic bitboard technique
    //at this stage, no distinctions between ones own and enemy pieces are made
    return bishop_attacks[bishop_table_offset[sq] +
        (((blockers&bishop_mask[sq])*bishop_factor[sq]) >> bishop_shift[sq])];
}

// Pawn moves via shifts

// White
// for a double pawn push, just apply push north twice

inline Bitboard pawn_push_north(const Bitboard& pawns){
    return pawns << 8;
}

inline Bitboard pawn_north_west(const Bitboard& pawns){
    // pawn attack in north west direction
    return (pawns&(~__A_FILE)) << 7;
}  

inline Bitboard pawn_north_east(const Bitboard& pawns){
    // pawn attack in north east direction
    return (pawns&(~__H_FILE)) << 9;
}  

// Black

inline Bitboard pawn_push_south(const Bitboard& pawns){
    return pawns >> 8;
}

inline Bitboard pawn_south_west(const Bitboard& pawns){
    // pawn attack in south west direction
    return (pawns&(~__A_FILE)) >> 9;
}  

inline Bitboard pawn_south_east(const Bitboard& pawns){
    // pawn attack in south east direction
    return (pawns&(~__H_FILE)) >> 7;
}



#endif //ATTACKS