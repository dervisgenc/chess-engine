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

    std::cout<<board<<std::endl;
    std::cout<<side<<std::endl;
    std::cout<<castling<<std::endl;
    std::cout<<full_move<<std::endl;



    
}