#ifndef SEARCH_H
#define SEARCH_H
#include <iostream>
#include <chrono>
#include <string>
// uncomment to disable assert()
#define NDEBUG
#include <cassert>

#include "types.h"
#include "display.h"
#include "movegen.h"
#include "utility.h"

#include "position.h"
#include "make_unmake.cpp"
#include "movepicker.h"
#include "position_tables.h"
#include "table.h"


int search(int alpha, int beta, int depth);
void prepare_tables(const Position& position);

constexpr uint8_t max_pv_len  = 32;

// Triangular array to store the PV
// the final PV ends up at pv[max_pv_len*depth] and is as long as depth
std::array<Move, max_pv_len*max_pv_len> principal_variation = {0};

Position pos;

unsigned long long node_count;


void display_search_result(int depth, int score, unsigned long long total_nodes, int time){
    // Print depth
    std::cout << "D" << depth << ": ";

    // Display score as well as PV
    if(abs(score) >= checkmate_score){
        short mate_in = 0;

        if(score > 0) 
            mate_in = (1 + depth - (abs(score) - checkmate_score))/2;
        else
            mate_in = (-(1 + depth) + (abs(score) - checkmate_score))/2;

        std::cout << "#" << mate_in << "  ";
    }
    else if(principal_variation[max_pv_len*depth] == 0){
        std::cout << "Stalemate" << "  ";
    }
    else std::cout << (1.0f*score)/100 << "  ";

    // Show Nodes and time on same line then on next line PV
    std::cout << "Nodes: " << total_nodes << " Time: " << time/1000.0f << std::endl << "PV: ";

    for(int i=0; i<depth; i++){
        Move move = principal_variation[max_pv_len*depth + i];
        
        std::cout           << square_names[from_square(move)] 
                            << square_names[to_square(move)]  
                            << "  ";
    }
    std::cout << std::endl;
}


Move search_position(Position& root_position, int min_depth){
    int score = 0;
    Move best_move = 0;
    unsigned long total_nodes = 0;
    int depth = 1;

    bool done = false;

    pos = root_position;
    prepare_tables(pos);

    clear_table();

    for (int i = 0; i < pos.total_move_count; i++){
        store_entry(pos.position_history[i].position_key,
                    0,
                    0,
                    const_entry,
                    47);
    }

    std::cout   << "Table size: " << tbl_size*sizeof(TableEntry)/(1024.0f*1024.0f) << " Mb, " 
                << tbl_size << " entries." << std::endl;


    principal_variation = {0};

    auto full_start = std::chrono::high_resolution_clock::now();
    while(!done){
        // Reset node count to count only for current iteration
        node_count = 0;

    
        auto start = std::chrono::high_resolution_clock::now();
        // Search for best move
        //score = search(-infinity_score, infinity_score, depth);

        MovePicker move_picker(pos);

        TableEntry table_entry = probe_table(pos.position_key);
        if(table_entry.info != 0){
            if(pos.is_pseudolegal(table_entry.move)) {
                move_picker.add_move(table_entry.move);
            }
        }

        Move move = move_picker.pick_next_move();
        best_move = 0;


        short moves_played = 0;


        int best_score = -infinity_score;

        score = 0;

        while(move){;

            make_move(pos, move);

            score = -search(-infinity_score, -best_score, depth - 1);
            //score += incremental_eval(pos, move);

            if(score == -illegal_position){
                    // If the move was illegal, just undo and search the next one
                    unmake_move(pos);
                    move = move_picker.pick_next_move();
                    continue;
                }

            unmake_move(pos);

            moves_played++;


            // If a score surpasses alpha, a new best move is found.
            if(score > best_score){

                best_score = score;

                principal_variation[max_pv_len*depth] = best_move = move;
                
                
                std::copy_n(  &principal_variation[(max_pv_len*(depth - 1))],
                            depth-1,
                            &principal_variation[(max_pv_len*depth) + 1]);
            

            }

            move = move_picker.pick_next_move();

        }
        if(moves_played == 0){
            if(get_checkers(pos.to_move, pos))  best_score = -(checkmate_score + depth);
            else                                best_score = stalemate_score;
            done = true;
        }

        store_entry(pos.position_key, best_move, best_score, exact_score, depth);




        auto stop = std::chrono::high_resolution_clock::now();

        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

        

        // Update values:
        total_nodes += node_count;

        //best_move = principal_variation[max_pv_len*depth];

        if(moves_played == 1) done = true; // FIS SCORE (USE LAST SCORE)

        // If a checkmate is found for the side to move, stop search and play
        if(score >= checkmate_score) done = true;

        //OUTPUT:

        if(pos.to_move == black) score = -score;

        // When done, print the score and principal variation
        display_search_result(depth, score, node_count, duration.count());
        
        duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - full_start);
        if((depth >= min_depth) && ((duration.count()/1000.0f) > 1.2f)) done = true;
        
        depth++;

    }
    auto full_stop = std::chrono::high_resolution_clock::now();

    auto full_duration = std::chrono::duration_cast<std::chrono::milliseconds>(full_stop - full_start);

    int filled_entries = 0;
    for(auto entry : transp_table){
        if(entry.info) filled_entries++;
    }
    // Print final node count and total time of all iterations combined
    std::cout << "Total time: " << full_duration.count()/1000.0f << 
                " Total nodes: " << total_nodes << 
                " Table fill status: " << filled_entries*100.0f/tbl_size << "% " <<
                std::endl;

    
    
    return best_move;
    
}

// Quiet Position evaluation
inline int eval(){

    int score = 0;
    for(int sq = 0; sq < 64; sq++){
        if(pos.board[sq]&black) score -= piece_square_tbl[64*pos.board[sq] + sq] << 2;
        else score += piece_square_tbl[64*pos.board[sq] + sq] << 2;
    }

    if(pos.to_move) return -score;
    else return score;
    
}

//Quiescience Search
int qs_search(int alpha, int beta){

    if(get_checkers(black ^ pos.to_move, pos)){
        // This position is illegal
        return illegal_position;
    }

    int stand_pat = eval();

    if(stand_pat >= beta) return beta;
    if(alpha < stand_pat) alpha = stand_pat;

    MovePicker move_picker(pos);
    move_picker.set_qs();
    Move move = move_picker.pick_next_move();

    int score;

    while(move){
        // Do not look at pawn captures.
        // The moves are ordered via MVV/LVA, thus just return if
        // the captured piece is a pawn
        if(pos.board[to_square(move)] == w_pawn) return alpha;
        if(pos.board[to_square(move)] == b_pawn) return alpha;

        // Actual QS
        make_move(pos, move);

        score = -qs_search(-beta, -alpha);
        //score += incremental_eval(pos, move);

        if(score == -illegal_position){
                // If the move was illegal, just undo and search the next one
                unmake_move(pos);
                move = move_picker.pick_next_move();
                continue;
            }

        unmake_move(pos);

        move = move_picker.pick_next_move();

        
        if(score > alpha){
            if(score >= beta){
                return beta;
            }
            alpha = score;
        }
    }

    return alpha;
}








// Main Search
int search(int alpha, int beta, int depth){
    
    
    if (depth == 0){
        
        return qs_search(alpha,beta);
    } 
    
    if(get_checkers(black ^ pos.to_move, pos)){
        // This position is illegal
        return illegal_position;
    }

    
    // if(pos.is_repetition()) return 0;// Score position as draw if it repeats
    if(pos.halfmove_clock >= 50) return 0; 

    node_count++;

    MovePicker move_picker(pos);

    TableEntry table_entry = probe_table(pos.position_key);
    if(table_entry.info != 0){
        if(pos.is_pseudolegal(table_entry.move)) {
            move_picker.add_move(table_entry.move);
        }
        if((table_entry.info&entry_dep_mask) >= depth){
            //score = table_entry.score;

            switch (table_entry.info&entry_flag_mask)
            {
            case lower_bound:
                if(table_entry.score >= beta) return beta;
                break;
            
            case upper_bound:
                if(table_entry.score <= alpha) return alpha;
                break;

            case exact_score:
                if(table_entry.score >= beta) return beta;
                else if(table_entry.score <= alpha) return alpha;
                else return table_entry.score;
                break;
            case const_entry:
                return table_entry.score;
            default:
                break;
            }
        }
    }

    Move move = move_picker.pick_next_move();
    Move best_move = 0;


    short moves_played = 0;

    EntryFlags flag = upper_bound;

    int score;

    while(move){;

        make_move(pos, move);

        assert(pos.piece_bitboards[w_king] != 0ULL);
        assert(pos.piece_bitboards[b_king] != 0ULL);

        score = -search(-beta, -alpha, depth - 1);
        //score += incremental_eval(pos, move);

        if(score == -illegal_position){
                // If the move was illegal, just undo and search the next one
                unmake_move(pos);
                move = move_picker.pick_next_move();
                continue;
            }

        unmake_move(pos);

        moves_played++;


        // If a score surpasses alpha, a new best move is found.
        if(score > alpha){

            // If beta is exceeded as well, perform beta cutoff
            if(score >= beta){

                store_entry(pos.position_key, move, score, lower_bound, depth);

                return beta;
            }

            alpha = score;

            // Alpha was raised, potential PV-Node
            flag = exact_score;

            principal_variation[max_pv_len*depth] = best_move = move;
            
            
            std::copy_n(  &principal_variation[(max_pv_len*(depth - 1))],
                        depth-1,
                        &principal_variation[(max_pv_len*depth) + 1]);
        

        }

        move = move_picker.pick_next_move();

    }
    if(moves_played == 0){
        if(get_checkers(pos.to_move, pos)) return -(checkmate_score + depth);
        return stalemate_score;
    }
    // TODO: ONLY POSSIBLE MOVE FLAG, then play move instantly

    store_entry(pos.position_key, best_move, alpha, flag, depth);
    return alpha;
}

void prepare_tables(const Position& position){
    int total_material = 0;
    for(int sq = 0; sq < 64; sq++){
        total_material += material_value[position.board[sq]];
    }

    // 78 = opening material
    // 30 = endgame material
    float scaler = (78 - total_material)/78.0f;
    std::cout << "Game Phase: " << scaler << std::endl;

    for(int i = 0; i < pos_table_size; i++){
        piece_square_tbl[i] = static_cast<uint8_t>(
                            piece_square_tbl_endgame[i]*scaler
                            + piece_square_tbl_opening[i]*(1-scaler));
    }
}




#endif //SEARCH_H