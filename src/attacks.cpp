#include "attacks.h"

//Define the look-up tables
uint64_t knight_attacks[64];
uint64_t king_attacks[64];
uint64_t pawn_attacks[2][64];

// TO DO: Mask according the files and ranks prevent overhang beyond the board
uint64_t generate_knight_moves(int square){
    uint64_t attacks = 0;
    attacks |= 1ULL << (square + 17);
    attacks |= 1ULL << (square + 15);
    attacks |= 1ULL << (square + 10);
    attacks |= 1ULL << (square + 6);
    attacks |= 1ULL >> (square - 17);
    attacks |= 1ULL >> (square - 10);
    attacks |= 1ULL >> (square - 6);
    
}

uint64_t generate_king_moves(int square){

}

uint64_t generate_pawn_moves(int square){

}
