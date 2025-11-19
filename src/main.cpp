#include "board.cpp"

int main(){

    Board my_board;

    my_board.parse_fen("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
    std::string new_fen = my_board.serialize_fen();

    std::cout<< new_fen << std::endl;
    return 0;
}