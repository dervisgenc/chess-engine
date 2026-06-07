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
            if (!(board.occupancy() & ((1ULL << SQ_F1) | (1Ull << SQ_G1))))
            {
                list.add(Move{SQ_E1, SQ_G1, KING_CASTLE});
            }
        }
        if (board.castling_rights() & WHITE_CASTLING_000)
        {
            if (!(board.occupancy() & ((1ULL << SQ_B1) | (1Ull << SQ_C1) | (1ULL << SQ_D1))))
            {
                list.add(Move{SQ_E1, SQ_C1, QUEEN_CASTLE});
            }
        }
    }
    else
    {
        if (board.castling_rights() & BLACK_CASTLING_00)
        {
            if (!(board.occupancy() & ((1ULL << SQ_F8) | (1Ull << SQ_G8))))
            {
                list.add(Move{SQ_E8, SQ_G8, KING_CASTLE});
            }
        }
        if (board.castling_rights() & BLACK_CASTLING_000)
        {
            if (!(board.occupancy() & ((1ULL << SQ_B8) | (1Ull << SQ_C8) | (1ULL < SQ_D8))))
            {
                list.add(Move{SQ_E8, SQ_C8, QUEEN_CASTLE});
            }
        }
    }
}

void generate_pseudo_white_pawn_moves(const Board &board, MoveList &list)
{
    uint64_t white_pawns = board.bitboard(WHITE_PAWN);

    Square ep_square = board.en_passant_square();
    uint64_t ep_target = ep_square == SQ_NONE ? 0ULL : (1ULL << ep_square);

    while (white_pawns)
    {
        int square = bitscan_forward(white_pawns);
        white_pawns &= white_pawns - 1;

        uint64_t captures = pawn_attacks[WHITE][square] & board.occupancy(BLACK);
        uint64_t quiets = Direction::north(1ULL << square) & ~board.occupancy();
        uint64_t double_push = 0ULL;
        uint64_t en_passant_capture = pawn_attacks[WHITE][square] & ep_target;

        if (en_passant_capture)
        {
            int move_to = bitscan_forward(en_passant_capture);
            list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), EN_PASSANT});
        }

        if (square >= SQ_A2 && square <= SQ_H2)
            double_push = Direction::north(quiets) & ~board.occupancy();

        if (double_push)
        {
            int move_to = bitscan_forward(double_push);
            double_push &= double_push - 1;

            list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), DOUBLE_PAWN_PUSH});
        }

        while (captures)
        {
            int move_to = bitscan_forward(captures);
            captures &= captures - 1;

            if (move_to >= SQ_A8 && move_to <= SQ_H8)
            {
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), QUEEN_PROMOTION_CAPTURE});
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), ROOK_PROMOTION_CAPTURE});
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), BISHOP_PROMOTION_CAPTURE});
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), KNIGHT_PROMOTION_CAPTURE});
            }
            else
            {
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), CAPTURE});
            }
        }

        while (quiets)
        {
            int move_to = bitscan_forward(quiets);
            quiets &= quiets - 1;
            if (move_to >= SQ_A8 && move_to <= SQ_H8)
            {
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), QUEEN_PROMOTION});
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), ROOK_PROMOTION});
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), BISHOP_PROMOTION});
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), KNIGHT_PROMOTION});
            }
            else
            {
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), QUIET});
            }
        }
    }
}

void generate_pseudo_black_pawn_moves(const Board &board, MoveList &list)
{
    uint64_t black_pawns = board.bitboard(BLACK_PAWN);

    Square ep_square = board.en_passant_square();
    uint64_t ep_target = ep_square == SQ_NONE ? 0ULL : (1ULL << ep_square);

    while (black_pawns)
    {
        int square = bitscan_forward(black_pawns);
        black_pawns &= black_pawns - 1;

        uint64_t captures = pawn_attacks[BLACK][square] & board.occupancy(WHITE);
        uint64_t quiets = Direction::south(1ULL << square) & ~board.occupancy();
        uint64_t double_push = 0ULL;
        uint64_t en_passant_capture = pawn_attacks[BLACK][square] & ep_target;

        if (en_passant_capture)
        {
            int move_to = bitscan_forward(en_passant_capture);
            list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), EN_PASSANT});
        }

        if (square >= SQ_A2 && square <= SQ_H2)
            double_push = Direction::south(Direction::south(1ULL << square)) & ~board.occupancy();

        while (double_push)
        {
            int move_to = bitscan_forward(double_push);
            double_push &= double_push - 1;

            list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), DOUBLE_PAWN_PUSH});
        }

        while (captures)
        {
            int move_to = bitscan_forward(captures);
            captures &= captures - 1;

            if (move_to >= SQ_A1 && move_to <= SQ_H1)
            {
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), QUEEN_PROMOTION_CAPTURE});
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), ROOK_PROMOTION_CAPTURE});
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), BISHOP_PROMOTION_CAPTURE});
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), KNIGHT_PROMOTION_CAPTURE});
            }
            else
            {
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), CAPTURE});
            }
        }

        while (quiets)
        {
            int move_to = bitscan_forward(quiets);
            quiets &= quiets - 1;
            if (move_to >= SQ_A1 && move_to <= SQ_H1)
            {
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), QUEEN_PROMOTION});
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), ROOK_PROMOTION});
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), BISHOP_PROMOTION});
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), KNIGHT_PROMOTION});
            }
            else
            {
                list.add(Move{static_cast<Square>(square), static_cast<Square>(move_to), QUIET});
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
    if (side == WHITE)
    {
        generate_pseudo_white_pawn_moves(board, list);
    }
    else
    {
        generate_pseudo_black_pawn_moves(board, list);
    }

    generate_pseudo_castling_moves(board, side, list);
}
