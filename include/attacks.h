// attacks.h
#ifndef ATTACKS_H
#define ATTACKS_H

#include <cstdint>

// We store knight pawn and king moves bc they slide. Squares they can go doesn't change by position.
// To store knights attacks on each square - It's kind a look up table
extern uint64_t knight_attacks[64];

extern uint64_t king_attacks[64];
// One for black one for white bc pawn moves opposite sides
extern uint64_t pawn_attacks[2][64];

void init_attacks();

#endif