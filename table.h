#ifndef TABLE_H
#define TABLE_H

#include <array>
#include <iostream>
// uncomment to disable assert()
#define NDEBUG
#include <cassert>

#include "types.h"

// Hash table index length can be specified here
constexpr short tbl_idx_len= 22;
constexpr short tbl_shift = 64 - tbl_idx_len;
constexpr unsigned long tbl_size = 1UL << (tbl_idx_len + 1);

// Bitmask for the validation portion of the key
constexpr uint16_t v_mask = 0xFFFF; 

static std::array<TableEntry, tbl_size> transp_table;

void clear_table(){
    // Put all 0 entries in the table
    std::fill(transp_table.begin(), transp_table.end(),TableEntry());

}

// Stores over the old position if it is not constant type
void store_entry(uint64_t key, Move move, int score, EntryFlags flag, int depth){

    assert((key >> tbl_shift) < tbl_size);
    assert(move != 0);
    assert(depth > 0);

    // Check if entry is marked as constant
    if((transp_table[key >> tbl_shift].info&entry_flag_mask) != const_entry){
        // If not, then overwrite with new entry
        transp_table[key >> tbl_shift] = TableEntry(key&v_mask, move, score, flag|depth);

    }
}

// Returns a zero entry if no hit, otherwise the entry
TableEntry probe_table(uint64_t key){

    assert((key >> tbl_shift) < tbl_size);

    if(transp_table[key >> tbl_shift].validation_key == (key&v_mask)){

        return transp_table[key >> tbl_shift];

    }
    return TableEntry();
}



#endif // TABLE_H