#ifndef BOARD_H
#define BOARD_H
#include "types.h"
#include <string>
#include <iostream>

class Board
{
public:
    Board(/* args */);
    ~Board();

    void parse_fen(const std::string &);
    std::string serialize_fen();
    void visualize_board();
    uint64_t occupancy() const;
    uint64_t occupancy(Color) const;

    uint64_t bitboard(Piece p) const { return m_bitboards[p]; }
    void set_bitboard(Piece p, uint64_t bb) { m_bitboards[p] = bb; }

    Color side_to_move() const { return m_side_to_move; }
    void set_side_to_move(Color c) { m_side_to_move = c; }

    Square en_passant_square() const { return m_en_passant_square; }
    void set_en_passant_square(Square sq) { m_en_passant_square = sq; }

    Castling_Rights castling_rights() const { return m_castling_rights; }
    void set_castling_rights(Castling_Rights cr) { m_castling_rights = cr; }

private:
    uint64_t m_bitboards[PIECE_NUMBERS] = {0}; // 4 bitboards are wasted but simplifies indexing
    Color m_side_to_move = WHITE;
    int m_total_moves = 0;
    uint8_t m_fifty_moves_rule_counter = 0;
    Castling_Rights m_castling_rights = NO_CASTLING;
    Square m_en_passant_square = SQ_NONE;
};
#endif