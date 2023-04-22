//
// Created by pauli on 4/21/2023.
//

#include "Tetronimo.h"
#include "GameMenuStateSP.h"

namespace Tetris {

    const unsigned int *Tetromino::SHAPES[Tetromino::TOTAL_TETROMINOS] = {Tetromino::I_SHAPE, Tetromino::J_SHAPE,
                                                                          Tetromino::L_SHAPE, Tetromino::O_SHAPE,
                                                                          Tetromino::S_SHAPE, Tetromino::T_SHAPE,
                                                                          Tetromino::Z_SHAPE};

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

    bool Tetromino::canFit(int shapeId, int x, int y, int rotation, const unsigned int *field) {
        for (int i = 0; i < TETROMINO_SIZE; i++) {
            for (int j = 0; j < TETROMINO_SIZE; j++) {
                int index = rotate(i, j, rotation);
                int pX = x + i;
                int pY = y + j;

                int fieldIndex = pX + (pY * GameMenuStateSP::FIELD_WIDTH);

                if (SHAPES[shapeId][index] != 0 && field[fieldIndex] != GameMenuStateSP::EMPTY_ID)
                    return false;
            }
        }
        return true;
    }
}
