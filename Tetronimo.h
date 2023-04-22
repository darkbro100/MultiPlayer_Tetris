//
// Created by pauli on 4/21/2023.
//

#ifndef MPTETRIS_TETRONIMO_H
#define MPTETRIS_TETRONIMO_H


namespace Tetris {

    class Tetromino {
    public:
        /**
         * The size of an array representing a tetromino
        */
        const static int TETROMINO_SIZE = 4;

        /**
         * Total number of tetrominos
         */
        const static int TOTAL_TETROMINOS = 7;

        constexpr static unsigned int I_SHAPE[TETROMINO_SIZE * TETROMINO_SIZE] = {
                1, 0, 0, 0,
                1, 0, 0, 0,
                1, 0, 0, 0,
                1, 0, 0, 0
        };

        constexpr static unsigned int J_SHAPE[TETROMINO_SIZE * TETROMINO_SIZE] = {
                1, 1, 1, 0,
                0, 0, 1, 0,
                0, 0, 0, 0,
                0, 0, 0, 0
        };

        constexpr static unsigned int L_SHAPE[TETROMINO_SIZE * TETROMINO_SIZE] = {
                0, 1, 1, 1,
                0, 1, 0, 0,
                0, 0, 0, 0,
                0, 0, 0, 0
        };

        constexpr static unsigned int O_SHAPE[TETROMINO_SIZE * TETROMINO_SIZE] = {
                0, 0, 0, 0,
                0, 1, 1, 0,
                0, 1, 1, 0,
                0, 0, 0, 0
        };

        constexpr static unsigned int S_SHAPE[TETROMINO_SIZE * TETROMINO_SIZE] = {
                0, 0, 0, 0,
                0, 1, 1, 0,
                1, 1, 0, 0,
                0, 0, 0, 0
        };

        constexpr static unsigned int T_SHAPE[TETROMINO_SIZE * TETROMINO_SIZE] = {
                0, 0, 0, 0,
                0, 1, 1, 1,
                0, 0, 1, 0,
                0, 0, 0, 0
        };

        constexpr static unsigned int Z_SHAPE[TETROMINO_SIZE * TETROMINO_SIZE] = {
                0, 0, 0, 0,
                1, 1, 0, 0,
                0, 1, 1, 0,
                0, 0, 0, 0
        };

        // All the shapes
        static const unsigned int * SHAPES[TOTAL_TETROMINOS];

        /**
         * Gets the shape of a tetromino
         * @param id Id of the shape
         * @return The pointer to the array of the shape
         */
        const static unsigned int * getShape(int id);

        /**
         * Rotates a shape by 90 degrees clockwise
         * @param x X position of the shape
         * @param y Y position of the shape
         * @param rotation Rotation value (0 = 90 degrees, 1 = 180 degrees, 2 = 270 degrees, 3 = 360 degrees)
         *
         * @return The rotated index for the shape
         */
        static int rotate(int x, int y, int rotation);

        /**
         * Determines if a piece, given a rotation, can fit in the field
         * @param shapeId
         * @param rotation
         * @param field
         * @return
         */
        static bool canFit(int shapeId, int x, int y, int rotation, const unsigned int * field);
    };


}


#endif //MPTETRIS_TETRONIMO_H
