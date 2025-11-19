//board.cpp
#include "board.h"
#include <sstream>
#include <iostream>

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
                file += num;
                break;
            
            }
            
            this->m_bitboards[piece] = m_bitboards[piece] | (1ULL << square_index); //Find the piece location and add to the board by OR

        }
        file++;
        
    }


    if (side.compare("w"))
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
            this->m_castling_rights = static_cast<Castling_Rights>(this->m_castling_rights | BLACK_CASTLING_OO0);

            break;
        case 'k':
            this->m_castling_rights = static_cast<Castling_Rights>(this->m_castling_rights | BLACK_CASTLING_OO);

            break;
        
        default:
            this->m_castling_rights = NO_CASTLING;
            break;
        }
    }
    
    
    this->m_fifty_moves_rule_counter = std::stoi(half_move);
    this->m_total_moves = std::stoi(full_move);

    std::cout << std::bitset<64>(this->m_bitboards[WHITE_PAWN]) << std::endl;
}