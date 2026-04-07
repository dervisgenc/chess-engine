#include "board.h"
#include "attacks.h"
int main()
{

    init_attacks();
    print_bitboard(knight_attacks[SQ_H8]);
    print_bitboard(king_attacks[SQ_A1]);

    return 0;
}