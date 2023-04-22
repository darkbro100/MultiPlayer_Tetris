//
// Created by pauli on 4/21/2023.
//

#include "Tetronimo.h"

namespace Tetris {

    const unsigned int * Tetromino::SHAPES[Tetromino::TOTAL_TETROMINOS] = { Tetromino::I_SHAPE, Tetromino::J_SHAPE, Tetromino::L_SHAPE, Tetromino::O_SHAPE, Tetromino::S_SHAPE, Tetromino::T_SHAPE, Tetromino::Z_SHAPE };

    int Tetromino::rotate(int x, int y, int rotation) {
        int index = y * TETROMINO_SIZE + x;

        switch (rotation) {
            case 0:
                return index;
            case 1:
                return 12 + y - (x * TETROMINO_SIZE);
            case 2:
                return 15 - (y * TETROMINO_SIZE) - x;
            case 3:
                return 3 - y + (x * TETROMINO_SIZE);
            default:
                return index;
        }
    }

    const unsigned int *Tetromino::getShape(int id) {
        return SHAPES[id];
    }
}
