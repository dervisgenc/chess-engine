#include "board.h"
#include "attacks.h"
#include "movegen.h"
int main()
{

    init_attacks();
    uint64_t attack = queen_attacks(SQ_C6, 0ULL);
    print_bitboard(attack);

    return 0;
}