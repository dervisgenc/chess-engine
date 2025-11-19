#include "board.cpp"

int main(){

    Board my_board;

    my_board.parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 0");

    return 0;
}