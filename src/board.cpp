//board.cpp
#include "board.h"
#include <sstream>
#include <iostream>
#include <bitset>

Board::Board(){

}

Board::~Board(){

}


void Board::parse_fen(const std::string& fen){

    std::stringstream ss(fen);

    std::string board;
    std::string side;
    std::string castling;
    std::string ep;
    std::string half_move;
    std::string full_move;

    ss >> board;
    ss >> side;
    ss >> castling;
    ss >> ep;
    ss >> half_move;
    ss >> full_move;

    int rank = 7;
    int file = 0;
    int square_index = 0;
    Piece piece;
    
    for (size_t i = 0; i < board.length(); i++)
    {
        char cursor = board[i];
        
        if (cursor == '/')
        {
            rank--;
            file = 0;
            
        }
        else{
            square_index = (rank * 8) + file;

            switch (cursor)
            {
            case 'r':
                piece = BLACK_ROOK;
                break;
            case 'n':
                piece = BLACK_KNIGHT;
                break;
            case 'b':
                piece = BLACK_BISHOP;
                break;
            case 'q':
                piece = BLACK_QUEEN;
                break;
            case 'k':
                piece = BLACK_KING;
                break;
            case 'p':
                piece = BLACK_PAWN;
                break;
            case 'R':
                piece = WHITE_ROOK;
                break;
            case 'N':
                piece = WHITE_KNIGHT;
                break;
            case 'B':
                piece = WHITE_BISHOP;
                break;
            case 'Q':
                piece = WHITE_QUEEN;
                break;
            case 'K':
                piece = WHITE_KING;
                break;
            case 'P':
                piece = WHITE_PAWN;
                break;
            default:
                piece = NO_PIECE;
                int num = cursor - '0'; //No need to use stoi bc it can be 8 max
                file += num - 1;
                break;
            
            }
            
            this->m_bitboards[piece] = m_bitboards[piece] | (1ULL << square_index); //Find the piece location and add to the board by OR

            file++;
        }
        
    }


    if (side == "w")
    {
        this->m_side_to_move = WHITE;
    }
    else {
        this->m_side_to_move = BLACK;
    }

    for (size_t i = 0; i < castling.length(); i++)
    {
        switch (castling[i])
        {
        case 'Q':
            this->m_castling_rights = static_cast<Castling_Rights>(this->m_castling_rights | WHITE_CASTLING_000);
            break;
        case 'K':
            this->m_castling_rights = static_cast<Castling_Rights>(this->m_castling_rights | WHITE_CASTLING_00);
            break;
        case 'q':
            this->m_castling_rights = static_cast<Castling_Rights>(this->m_castling_rights | BLACK_CASTLING_000);

            break;
        case 'k':
            this->m_castling_rights = static_cast<Castling_Rights>(this->m_castling_rights | BLACK_CASTLING_00);

            break;
        
        default:
            this->m_castling_rights = NO_CASTLING;
            break;
        }
    }
    
    if (ep != "-") {
        int file = ep[0] - 'a';
        int rank = ep[1] - '1';
        this->m_en_passant_square = static_cast<Square>(rank * 8 + file);
    } else {
        this->m_en_passant_square = SQ_NONE;
    }

    this->m_fifty_moves_rule_counter = std::stoi(half_move);
    this->m_total_moves = std::stoi(full_move);

}

std::string Board::serialize_fen(){
    std::string fen = "";
    int counter = 0;
    bool is_prev_empty = false;
    int empty_counter = 0;
    int file = 0;
    int i;

    for (int rank = 7; rank >= 0; rank--)
    {
        for (int file = 0; file < 8; file++)
        {
            i = rank * 8 + file;
            if (this->m_bitboards[BLACK_PAWN] & (1ULL << i))
            {
                if(empty_counter != 0){
                    fen += std::to_string(empty_counter);
                }
                empty_counter = 0;
                fen  += "p";
            }else if(this->m_bitboards[BLACK_ROOK] & (1ULL << i)){
                if(empty_counter != 0){
                    fen += std::to_string(empty_counter);
                }
                empty_counter = 0;
                fen += "r";
            }else if(this->m_bitboards[BLACK_BISHOP] & (1ULL << i)){
                if(empty_counter != 0){
                    fen += std::to_string(empty_counter);
                }
                empty_counter = 0;
                fen += "b";
            }else if(this->m_bitboards[BLACK_KNIGHT] & (1ULL << i)){
                if(empty_counter != 0){
                    fen += std::to_string(empty_counter);
                }
                empty_counter = 0;
                fen += "n";
            }else if(this->m_bitboards[BLACK_QUEEN] & (1ULL << i)){
                if(empty_counter != 0){
                    fen += std::to_string(empty_counter);
                }
                empty_counter = 0;
                fen += "q";
            }else if(this->m_bitboards[BLACK_KING] & (1ULL << i)){
                if(empty_counter != 0){
                    fen += std::to_string(empty_counter);
                }
                empty_counter = 0;
                fen += "k";
            }else if(this->m_bitboards[WHITE_PAWN] & (1ULL << i)){
                if(empty_counter != 0){
                    fen += std::to_string(empty_counter);
                }
                empty_counter = 0;
                fen += "P";
            }else if(this->m_bitboards[WHITE_ROOK] & (1ULL << i)){
                if(empty_counter != 0){
                    fen += std::to_string(empty_counter);
                }
                empty_counter = 0;
                fen += "R";
            }else if(this->m_bitboards[WHITE_KNIGHT] & (1ULL << i)){
                if(empty_counter != 0){
                    fen += std::to_string(empty_counter);
                }
                empty_counter = 0;
                fen += "N";
            }else if(this->m_bitboards[WHITE_BISHOP] & (1ULL << i)){
                if(empty_counter != 0){
                    fen += std::to_string(empty_counter);
                }
                empty_counter = 0;
                fen += "B";
            }else if(this->m_bitboards[WHITE_QUEEN] & (1ULL << i)){
                if(empty_counter != 0){
                    fen += std::to_string(empty_counter);
                }
                empty_counter = 0;
                fen += "Q";
            }else if(this->m_bitboards[WHITE_KING] & (1ULL << i)){
                if(empty_counter != 0){
                    fen += std::to_string(empty_counter);
                }
                empty_counter = 0;
                fen += "K";
            }else{
                empty_counter++;
            }

        }
        if (empty_counter != 0) {
            fen += std::to_string(empty_counter);
            empty_counter = 0;
        }
        if (rank > 0) {
            fen += "/";
        }
        
    }
    
    fen += " ";

    if(this->m_side_to_move == WHITE){
        fen += "w";
    }else {
        fen += "b";
    }
    
    fen += " ";

    if (this->m_castling_rights == ANY_CASTLING)
    {
        fen += "KQkq";
    }else if(this->m_castling_rights == WHITE_CASTLING){
        fen += "KQ";
    }else if(this->m_castling_rights == WHITE_CASTLING_00){
        fen += "K";
    }else if(this->m_castling_rights == WHITE_CASTLING_000){
        fen += "Q";
    }else if (this->m_castling_rights == BLACK_CASTLING){
        fen += "kq";
    }else if (this->m_castling_rights == BLACK_CASTLING_000){
        fen += "q";
    }else if (this->m_castling_rights == BLACK_CASTLING_00){
        fen += "k";
    }else{
        fen += "-";
    }
    
    fen += " ";

    if (this->m_en_passant_square != SQ_NONE)
    {
        int rank = this->m_en_passant_square / 8;
        int file = this->m_en_passant_square % 8;

        fen +=  (char)('a' + file);
        fen +=  (char)('1' + rank);
    }
    else{
        fen += "-";
    }
    
    fen += " " + std::to_string(this->m_fifty_moves_rule_counter);
    fen += " " + std::to_string(this->m_total_moves);

    return fen;
}
