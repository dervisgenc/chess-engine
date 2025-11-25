#include "board.h"

int main(){

    Board my_board;
    std::string fen = "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2";

    my_board.parse_fen(fen);
    std::string new_fen = my_board.serialize_fen();

    my_board.visualize_board();
    return 0;
}