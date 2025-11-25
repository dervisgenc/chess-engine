#include "types.h"
#include <string>
#include <iostream>


class Board
{
public:
    Board(/* args */);
    ~Board();

    void parse_fen(const std::string&);
    std::string serialize_fen();
    void visualize_board();

    
    private:
    
    uint64_t        m_bitboards[PIECE_NUMBERS] = {0};     //4 bitboards are wasted but simplifies indexing
    Color           m_side_to_move = WHITE;
    int             m_total_moves = 0;
    uint8_t         m_fifty_moves_rule_counter = 0;
    Castling_Rights m_castling_rights = NO_CASTLING;
    Square          m_en_passant_square = SQ_NONE;
    


};

