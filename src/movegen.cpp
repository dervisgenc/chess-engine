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
        // Gives the index of first blocker
        blocker_sq = bitscan_forward(blockers);
        ray ^= ray_attacks[RAY_EAST][blocker_sq];
    }
    attacks |= ray;

    ray = ray_attacks[RAY_WEST][sq];
    blockers = ray & occupancy;
    if (blockers)
    {
        // Gives the index of first blocker
        blocker_sq = bitscan_reverse(blockers);
        ray ^= ray_attacks[RAY_WEST][blocker_sq];
    }
    attacks |= ray;

    ray = ray_attacks[RAY_SOUTH][sq];
    blockers = ray & occupancy;
    if (blockers)
    {
        // Gives the index of first blocker
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
