#include "board.h"
#include "attacks.h"
int main()
{

    init_attacks();
    print_bitboard(knight_attacks[SQ_H6]);
    print_bitboard(king_attacks[SQ_H3]);

    return 0;
}