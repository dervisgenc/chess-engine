#include "attacks.h"

//Define the look-up tables
uint64_t knight_attacks[64];
uint64_t king_attacks[64];
uint64_t pawn_attacks[2][64];

uint64_t generate_knight_moves(int square){

    // Create a bitboard where only the square with the knight is one 
    uint64_t knight_board = 1ULL << square;

    uint64_t attacks = 0;


    // No need to check rank because if a overflow happens board will be 0, files should be checked
    // 2 up 1 right
    attacks |= (knight_board << 17) && not_a_file;
    // 2 up 1 left
    attacks |= (knight_board << 15) && not_h_file;
    // 1 up 2 right
    attacks |= (knight_board << 10) && not_ab_file;
    // 1 up 2 left 
    attacks |= (knight_board << 6) && not_gh_file;
    // 1 down 2 right
    attacks |= (knight_board >> 6) && not_ab_file;
    // 1 down 2 left
    attacks |= (knight_board >> 10) && not_gh_file;
    // 2 down 1 right
    attacks |= (knight_board >> 15) && not_a_file;
    // 2 down 1 lefts   
    attacks |= (knight_board >> 17) && not_h_file;

    return attacks;
}

uint64_t generate_king_moves(int square){
    // Create a bitboard where only the square with the king is one 
    uint64_t king_board = 1ULL << square;

    uint64_t attacks = 0;


    attacks |= (king_board << 9) && not_a_file;
    attacks |= (king_board << 8) && not_h_file;
    attacks |= (king_board << 7) && not_ab_file;
    attacks |= (king_board << 1) && not_gh_file;
    attacks |= (king_board >> 1) && not_ab_file;
    attacks |= (king_board >> 7) && not_gh_file;
    attacks |= (king_board >> 8) && not_a_file;
    attacks |= (king_board >> 9) && not_h_file;


}

uint64_t generate_pawn_moves(int square){

}
