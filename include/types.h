#ifndef TYPES_H
#include <cstdint>

enum Color : std::uint8_t{
    WHITE = 0b0,
    BLACK = 0b1,
    COLOR_NUMBERS = 2  
};

enum Castling_Rights : std::uint8_t {
    NO_CASTLING = 0b0,                
    WHITE_CASTLING_00 = 0b0001,
    WHITE_CASTLING_000 = 0b0010,
    BLACK_CASTLING_OO = 0b0100,
    BLACK_CASTLING_OO0 = 0b1000,

    WHITE_CASTLING = WHITE_CASTLING_00 | WHITE_CASTLING_000,
    BLACK_CASTLING = BLACK_CASTLING_OO | BLACK_CASTLING_OO0,
    KING_SIDE = WHITE_CASTLING_00 | BLACK_CASTLING_OO,
    QUEEN_SIDE = WHITE_CASTLING_000 | BLACK_CASTLING_OO0,
    ANY_CASTLING = WHITE_CASTLING | BLACK_CASTLING,

    CASTLING_RIGHT_NUMBERS = 16
};

enum PieceType : std::uint8_t {
    NO_PIECE_TYPE,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING,

    ALL_PIECES = 0,
    PIECE_TYE_NUMBERS = 8 
};

enum Piece : std::uint8_t {
    NO_PIECE,
    WHITE_PAWN = PAWN,          //0b 0000 0001 1
    WHITE_KNIGHT,               // 2
    WHITE_BISHOP,               // 3
    WHITE_ROOK,                 // 4
    WHITE_QUEEN,                // 5
    WHITE_KING ,                // 6
    BLACK_PAWN = PAWN + 8,      //0b 0001 0001 9
    BLACK_KNIGHT,               // 10
    BLACK_BISHOP,               // 11
    BLACK_ROOK,                 // 12
    BLACK_QUEEN,                // 13
    BLACK_KING,                 // 14

    PIECE_NUMBERS = 16,

};

enum Square : std::uint8_t {
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    SQ_NONE,

    SQ_ZERO = 0,
    SQUARE_NUMBERS = 64
};

enum File : std::uint8_t {
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H,
    FILE_NUMBERS
};

enum Rank : std::uint8_t {
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
    RANK_NUMBERS
};


#endif
