#ifndef MOVEGEN_H
#define MOVEGEN_H

#include "attacks.h"

uint64_t rook_attacks(Square sq, uint64_t occupancy);
uint64_t bishop_attacks(Square sq, uint64_t occupancy);
uint64_t queen_attacks(Square sq, uint64_t occupancy);

#endif