#ifndef MOVEPICKER_H
#define MOVEPICKER_H

#include <iostream>
#include <array>
#include <algorithm>

#include "types.h"
#include "position.h"
#include "movegen.h"

enum GenerationState{
    capture_state = 2,
    quiet_state = 1,
    done_state = 0
};

std::array<uint8_t, 16> victim_values ={
        // empty just as buffer
        0,

        // w_pawn
        // w_knight
        // etc.
        10,
        20,
        30,
        40,
        50,
        // king captures can not occur
        0,
        0, // w piece
        0, // dummy
        // Black
        10,
        20,
        30,
        40,
        50,
        0,
        0
};

std::array<uint8_t, 16> attacker_values ={
        // empty just as buffer
        0,

        // w_pawn
        // w_knight
        // etc.
        5,
        4,
        3,
        2,
        1,
        // king captures can not occur
        0,

        0,
        0,
        // Black
        5,
        4,
        3,
        2,
        1,
        0,
        0
};



class MovePicker{
    public:
        MovePicker(const Position& chess_position) : 
                    pos(chess_position),  
                    generation_state(capture_state),
                    quiescience(false),
                    index(0) {}

        // Returns next move to be searched
        Move pick_next_move();

        // Add moves, for PV and TT move 
        void add_move(Move m);

        void set_qs();

    private:
        // Store moves in the move list
        MoveList move_list;
        // Use index to keep track what moves were looked at so far
        int index;

        // Keeps track of what moves have already been generated
        int generation_state;

        // If in quiescience search, do not generate quiet moves
        bool quiescience;

        //generates the next move stage
        void generate_next();

        const Position& pos;

        // Sorting routines
        bool mvv_lva(Move x, Move y);


};


Move MovePicker::pick_next_move(){
    Move next_move = this->move_list.move_stack[index++];
    // If next_move is a move, return it
    if(next_move) return next_move;
    else{ // If it is empty, generate more moves
        this->generate_next();

        // If there were no no moves generated, this returns 0
        // Since the index was iterated, look at index - 1
        return this->move_list.move_stack[index - 1];
    }
}

void MovePicker::generate_next(){
    uint8_t starting_size = move_list.size;

    switch (generation_state)
    {
    case capture_state:
        generate_captures(pos, &move_list);
        generation_state = quiet_state;

        // Sort the captures via MVV/LVA
        std::sort(  &move_list.move_stack[starting_size], 
                    &move_list.move_stack[move_list.size],
                    [this](int x, int y){
                    return this->mvv_lva(x, y);}
                );

        if(quiescience){ 
            generation_state = done_state; 
            break;}

        // If no moves were generated and not in QS, generate quiets
        if(move_list.size > starting_size){ 
            break;
            }

    case quiet_state:
        generate_quiet(pos, &move_list);
        generation_state = done_state;
        break;
    
    default:
        break;
    }
}

void MovePicker::add_move(Move m){
    this->move_list.move_stack[this->move_list.size++] = m;
}

void MovePicker::set_qs(){quiescience = true;}

inline bool MovePicker::mvv_lva(Move x, Move y) 
{   
    uint8_t val_x = victim_values[pos.board[to_square(x)]] + 
                    attacker_values[pos.board[from_square(x)]];
    uint8_t val_y = victim_values[pos.board[to_square(y)]] + 
                    attacker_values[pos.board[from_square(y)]];
    if (val_x > val_y) 
        return true; 
    else
        return false; 
} 

#endif // MOVEPICKER_H
