#include "attacks.h"
#include <iostream>
// Define the look-up tables
uint64_t knight_attacks[64];
uint64_t king_attacks[64];
uint64_t ray_attacks[8][64];

uint64_t generate_knight_moves(Square square)
{

    // Create a bitboard where only the square with the knight is one
    uint64_t knight_board = 1ULL << square;

    uint64_t attacks = 0;

    // No need to check rank because if a overflow happens board will be 0, files should be checked
    // 2 up 1 right
    attacks |= (knight_board << 17) & Mask::not_a_file;
    // 2 up 1 left
    attacks |= (knight_board << 15) & Mask::not_h_file;
    // 1 up 2 right
    attacks |= (knight_board << 10) & Mask::not_ab_file;
    // 1 up 2 left
    attacks |= (knight_board << 6) & Mask::not_gh_file;
    // 1 down 2 right
    attacks |= (knight_board >> 6) & Mask::not_ab_file;
    // 1 down 2 left
    attacks |= (knight_board >> 10) & Mask::not_gh_file;
    // 2 down 1 right
    attacks |= (knight_board >> 15) & Mask::not_a_file;
    // 2 down 1 lefts
    attacks |= (knight_board >> 17) & Mask::not_h_file;

    return attacks;
}

uint64_t generate_king_moves(Square square)
{
    // Create a bitboard where only the square with the king is one
    uint64_t king_board = 1ULL << square;

    uint64_t attacks = 0;

    attacks |= Direction::north(king_board);
    attacks |= Direction::south(king_board);
    attacks |= Direction::east(king_board);
    attacks |= Direction::west(king_board);
    attacks |= Direction::south_east(king_board);
    attacks |= Direction::south_west(king_board);
    attacks |= Direction::north_east(king_board);
    attacks |= Direction::north_west(king_board);

    return attacks;
}

void print_bitboard(uint64_t bitboard)
{
    std::cout << "" << std::endl;
    for (int rank = 7; rank >= 0; rank--)
    {
        std::cout << "    " << rank + 1 << "  ";
        for (size_t file = 0; file < 8; file++)
        {
            int square = rank * 8 + file;
            if (bitboard & (1ULL << square))
                std::cout << " 1";
            else
                std::cout << " .";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl
              << "        a b c d e f g h" << std::endl;
}

uint64_t compute_ray(Square square, uint64_t (*func)(uint64_t))
{
    uint64_t ray = 0ULL;
    // Convert sqaure to a bitboard
    uint64_t b = 1ULL << square;

    while ((b = func(b)))
        ray |= b;

    return ray;
}

void init_ray_attacks()
{
    for (Square sq = SQ_A1; sq < SQUARE_NUMBERS; sq++)
    {
        ray_attacks[RAY_NORTH][sq] = compute_ray(sq, Direction::north);
        ray_attacks[RAY_SOUTH][sq] = compute_ray(sq, Direction::south);
        ray_attacks[RAY_EAST][sq] = compute_ray(sq, Direction::east);
        ray_attacks[RAY_WEST][sq] = compute_ray(sq, Direction::west);
        ray_attacks[RAY_NORTH_EAST][sq] = compute_ray(sq, Direction::north_east);
        ray_attacks[RAY_NORTH_WEST][sq] = compute_ray(sq, Direction::north_west);
        ray_attacks[RAY_SOUTH_EAST][sq] = compute_ray(sq, Direction::south_east);
        ray_attacks[RAY_SOUTH_WEST][sq] = compute_ray(sq, Direction::south_west);
    }
}

void init_attacks()
{
    for (Square sq = SQ_A1; sq < SQUARE_NUMBERS; sq++)
    {
        king_attacks[sq] = generate_king_moves(sq);
        knight_attacks[sq] = generate_knight_moves(sq);
    }
    init_ray_attacks();
}