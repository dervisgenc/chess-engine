#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "attacks.h"
#include "board.h"

uint64_t rook_attacks(Square sq, uint64_t occupancy);
uint64_t bishop_attacks(Square sq, uint64_t occupancy);
uint64_t queen_attacks(Square sq, uint64_t occupancy);

void generate_pseudo_knight_moves(const Board &board, Color side, MoveList list);
void generate_pseudo_king_moves(const Board &board, Color side, MoveList list);
void generate_bishop_bishop_moves(const Board &board, Color side, MoveList &list);
void generate_pseudo_queen_moves(const Board &board, Color side, MoveList &list);

#endif