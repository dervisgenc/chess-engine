#include "board.h"
#include "attacks.h"
int main()
{

    init_attacks();
    print_bitboard(king_attacks[SQ_H3]);
    print_bitboard(ray_attacks[RAY_NORTH][SQ_C3]);
    print_bitboard(ray_attacks[RAY_SOUTH_EAST][SQ_D6]);

    return 0;
}