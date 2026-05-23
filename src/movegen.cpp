#include "movegen.h"

uint64_t rook_attacks(Square sq, uint64_t occupancy)
{
    uint64_t attacks = 0;
    uint64_t ray = ray_attacks[RAY_NORTH][sq];
    uint64_t blockers = ray & occupancy;
    int blocker_sq = SQ_NONE;
    if (blockers)
    {
        // Gives the index of first blocker
        blocker_sq = bitscan_forward(blockers);
        ray ^= ray_attacks[RAY_NORTH][blocker_sq];
    }
    attacks |= ray;

    ray = ray_attacks[RAY_EAST][sq];
    blockers = ray & occupancy;
    if (blockers)
    {
        blocker_sq = bitscan_forward(blockers);
        ray ^= ray_attacks[RAY_EAST][blocker_sq];
    }
    attacks |= ray;

    ray = ray_attacks[RAY_WEST][sq];
    blockers = ray & occupancy;
    if (blockers)
    {
        blocker_sq = bitscan_reverse(blockers);
        ray ^= ray_attacks[RAY_WEST][blocker_sq];
    }
    attacks |= ray;

    ray = ray_attacks[RAY_SOUTH][sq];
    blockers = ray & occupancy;
    if (blockers)
    {
        blocker_sq = bitscan_reverse(blockers);
        ray ^= ray_attacks[RAY_SOUTH][blocker_sq];
    }
    attacks |= ray;

    return attacks;
}

uint64_t bishop_attacks(Square sq, uint64_t occupancy)
{
    {
        uint64_t attacks = 0;
        uint64_t ray = ray_attacks[RAY_NORTH_EAST][sq];
        uint64_t blockers = ray & occupancy;
        int blocker_sq = SQ_NONE;
        if (blockers)
        {
            // Gives the index of first blocker
            blocker_sq = bitscan_forward(blockers);
            ray ^= ray_attacks[RAY_NORTH_EAST][blocker_sq];
        }
        attacks |= ray;

        ray = ray_attacks[RAY_NORTH_WEST][sq];
        blockers = ray & occupancy;
        if (blockers)
        {
            // Gives the index of first blocker
            blocker_sq = bitscan_forward(blockers);
            ray ^= ray_attacks[RAY_NORTH_WEST][blocker_sq];
        }
        attacks |= ray;

        ray = ray_attacks[RAY_SOUTH_EAST][sq];
        blockers = ray & occupancy;
        if (blockers)
        {
            // Gives the index of first blocker
            blocker_sq = bitscan_reverse(blockers);
            ray ^= ray_attacks[RAY_SOUTH_EAST][blocker_sq];
        }
        attacks |= ray;

        ray = ray_attacks[RAY_SOUTH_WEST][sq];
        blockers = ray & occupancy;
        if (blockers)
        {
            // Gives the index of first blocker
            blocker_sq = bitscan_reverse(blockers);
            ray ^= ray_attacks[RAY_SOUTH_WEST][blocker_sq];
        }
        attacks |= ray;

        return attacks;
    }
}

uint64_t queen_attacks(Square sq, uint64_t occupancy)
{
    uint64_t attacks = 0;
    attacks = rook_attacks(sq, occupancy) | bishop_attacks(sq, occupancy);

    return attacks;
}

void generate_pseudo_knight_moves(const Board &board, Color side, MoveList &list)
{
    uint64_t knights = board.bitboard(side == WHITE ? WHITE_KNIGHT : BLACK_KNIGHT);
    while (knights)
    {
        int sq = bitscan_forward(knights);
        knights &= knights - 1; // Pop LSB

        uint64_t targets = knight_attacks[sq];
        targets &= ~board.occupancy(side);

        uint64_t captures = targets & board.occupancy(side == WHITE ? BLACK : WHITE);
        uint64_t quites = targets & ~board.occupancy();

        while (captures)
        {
            int move_to = bitscan_forward(captures);
            captures &= captures - 1;

            list.add(Move{static_cast<Square>(sq), static_cast<Square>(move_to), CAPTURE});
        }
        while (quites)
        {
            int move_to = bitscan_forward(quites);
            quites &= quites - 1;

            list.add(Move{static_cast<Square>(sq), static_cast<Square>(move_to), QUIET});
        }
    }
}

void generate_pseudo_king_moves(const Board &board, Color side, MoveList &list)
{
    uint64_t king = board.bitboard(side == WHITE ? WHITE_KING : BLACK_KING);

    int sq = bitscan_forward(king);

    uint64_t targets = king_attacks[sq];
    targets &= ~board.occupancy(side);

    uint64_t captures = targets & board.occupancy(side == WHITE ? BLACK : WHITE);
    uint64_t quites = targets & ~board.occupancy();

    while (captures)
    {
        int move_to = bitscan_forward(captures);
        captures &= captures - 1;

        list.add(Move{static_cast<Square>(sq), static_cast<Square>(move_to), CAPTURE});
    }
    while (quites)
    {
        int move_to = bitscan_forward(quites);
        quites &= quites - 1;

        list.add(Move{static_cast<Square>(sq), static_cast<Square>(move_to), QUIET});
    }
}

void generate_pseudo_rook_moves(const Board &board, Color side, MoveList &list)
{
    uint64_t rooks = board.bitboard(side == WHITE ? WHITE_ROOK : BLACK_ROOK);
    while (rooks)
    {
        int sq = bitscan_forward(rooks);
        rooks &= rooks - 1;

        uint64_t targets = rook_attacks(static_cast<Square>(sq), board.occupancy());
        targets &= ~board.occupancy(side);

        uint64_t captures = targets & board.occupancy(side == WHITE ? BLACK : WHITE);
        uint64_t quiets = targets & ~board.occupancy();
        while (captures)
        {
            int move_to = bitscan_forward(captures);
            captures &= captures - 1;

            list.add(Move{static_cast<Square>(sq), static_cast<Square>(move_to), CAPTURE});
        }
        while (quiets)
        {
            int move_to = bitscan_forward(quiets);
            quiets &= quiets - 1;

            list.add(Move{static_cast<Square>(sq), static_cast<Square>(move_to), QUIET});
        }
    }
}

void generate_pseudo_bishop_moves(const Board &board, Color side, MoveList &list)
{
    uint64_t bishops = board.bitboard(side == WHITE ? WHITE_BISHOP : BLACK_BISHOP);
    while (bishops)
    {
        int sq = bitscan_forward(bishops);
        bishops &= bishops - 1;

        uint64_t targets = bishop_attacks(static_cast<Square>(sq), board.occupancy());
        targets &= ~board.occupancy(side);

        uint64_t captures = targets & board.occupancy(side == WHITE ? BLACK : WHITE);
        uint64_t quiets = targets & ~board.occupancy();
        while (captures)
        {
            int move_to = bitscan_forward(captures);
            captures &= captures - 1;

            list.add(Move{static_cast<Square>(sq), static_cast<Square>(move_to), CAPTURE});
        }
        while (quiets)
        {
            int move_to = bitscan_forward(quiets);
            quiets &= quiets - 1;

            list.add(Move{static_cast<Square>(sq), static_cast<Square>(move_to), QUIET});
        }
    }
}

void generate_pseudo_queen_moves(const Board &board, Color side, MoveList &list)
{
    uint64_t queens = board.bitboard(side == WHITE ? WHITE_QUEEN : BLACK_QUEEN);
    while (queens)
    {
        int sq = bitscan_forward(queens);
        queens &= queens - 1;

        uint64_t targets = queen_attacks(static_cast<Square>(sq), board.occupancy());
        targets &= ~board.occupancy(side);

        uint64_t captures = targets & board.occupancy(side == WHITE ? BLACK : WHITE);
        uint64_t quiets = targets & ~board.occupancy();

        while (captures)
        {
            int move_to = bitscan_forward(captures);
            captures &= captures - 1;

            list.add(Move{static_cast<Square>(sq), static_cast<Square>(move_to), CAPTURE});
        }
        while (quiets)
        {
            int move_to = bitscan_forward(quiets);
            quiets &= quiets - 1;

            list.add(Move{static_cast<Square>(sq), static_cast<Square>(move_to), QUIET});
        }
    }
}

void generate_pseudo_castling_moves(const Board &board, Color side, MoveList &list)
{
    if (side == WHITE)
    {
        if (board.castling_rights() & WHITE_CASTLING_00)
        {
            if (board.occupancy() & ((1ULL << SQ_F1) | (1Ull << SQ_G1)))
            {
                list.add(Move{SQ_E1, SQ_G1, KING_CASTLE});
            }
        }
        if (board.castling_rights() & WHITE_CASTLING_000)
        {
            if (board.occupancy() & ((1ULL << SQ_B1) | (1Ull << SQ_C1) | (1ULL << SQ_D1)))
            {
                list.add(Move{SQ_E1, SQ_C1, QUEEN_CASTLE});
            }
        }
    }
    else
    {
        if (board.castling_rights() & BLACK_CASTLING_00)
        {
            if (board.occupancy() & ((1ULL << SQ_F8) | (1Ull << SQ_G8)))
            {
                list.add(Move{SQ_E8, SQ_G8, KING_CASTLE});
            }
        }
        if (board.castling_rights() & BLACK_CASTLING_000)
        {
            if (board.occupancy() & ((1ULL << SQ_B8) | (1Ull << SQ_C8) | (1ULL < SQ_D8)))
            {
                list.add(Move{SQ_E8, SQ_C8, QUEEN_CASTLE});
            }
        }
    }
}
void generate_pseudo_moves(const Board &board, MoveList &list)
{
    Color side = board.side_to_move();

    generate_pseudo_bishop_moves(board, side, list);
    generate_pseudo_knight_moves(board, side, list);
    generate_pseudo_king_moves(board, side, list);
    generate_pseudo_queen_moves(board, side, list);
    generate_pseudo_rook_moves(board, side, list);

    generate_pseudo_castling_moves(board, side, list);
}
