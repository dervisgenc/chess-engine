#include "board.h"
#include "attacks.h"
#include "movegen.h"
#include "format"
int main()
{
    std::string fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
    Board test_board;
    test_board.parse_fen(fen);
    test_board.visualize_board();
    init_attacks();
    MoveList list;
    generate_pseudo_moves(test_board, list);
    std::cout << list.count << std::endl;
    size_t i = 0;
    for (; i < list.count; i++)
    {
        char from_file = 'a' + (list.moves[i].from % 8);
        char from_rank = '1' + (list.moves[i].from / 8);
        char to_file = 'a' + (list.moves[i].to % 8);
        char to_rank = '1' + (list.moves[i].to / 8);
        std::cout << std::format("From {}{} to {}{}", from_file, from_rank, to_file, to_rank) << std::endl;
    }
    std::cout << i << std::endl;

    return 0;
}