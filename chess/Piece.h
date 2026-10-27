#ifndef PIECE_H
#define PIECE_H

#include <SFML/Graphics.hpp>
#include <algorithm>

const int ROWS = 10;
const int COLS = 9;

class Piece {
public:
    sf::String name;
    sf::Color color;
    bool isEmpty;

    Piece() : name(L""), color(sf::Color::Transparent), isEmpty(true) {}
    Piece(sf::String n, sf::Color c) : name(n), color(c), isEmpty(false) {}
    virtual ~Piece() {}

    virtual bool isValidMove(int srcR, int srcC, int destR, int destC, Piece* board[ROWS][COLS]) {
        if (srcR == destR && srcC == destC) return false;
        if (!board[destR][destC]->isEmpty && board[destR][destC]->color == this->color) {
            return false;
        }
        return true;
    }
};

class EmptyPiece : public Piece {
public:
    EmptyPiece() : Piece() {}
    bool isValidMove(int srcR, int srcC, int destR, int destC, Piece* board[ROWS][COLS]) override {
        return false;
    }
};

class Che : public Piece {
public:
    Che(sf::String n, sf::Color c) : Piece(n, c) {}

    bool isValidMove(int srcR, int srcC, int destR, int destC, Piece* board[ROWS][COLS]) override {
        if (!Piece::isValidMove(srcR, srcC, destR, destC, board)) return false;
        if (srcR != destR && srcC != destC) return false;

        if (srcR == destR) {
            int minC = std::min(srcC, destC);
            int maxC = std::max(srcC, destC);
            for (int c = minC + 1; c < maxC; ++c) {
                if (!board[srcR][c]->isEmpty) return false;
            }
        }
        else {
            int minR = std::min(srcR, destR);
            int maxR = std::max(srcR, destR);
            for (int r = minR + 1; r < maxR; ++r) {
                if (!board[r][srcC]->isEmpty) return false;
            }
        }
        return true;
    }
};

class Ma : public Piece {
public:
    Ma(sf::String n, sf::Color c) : Piece(n, c) {}

    bool isValidMove(int srcR, int srcC, int destR, int destC, Piece* board[ROWS][COLS]) override {
        if (!Piece::isValidMove(srcR, srcC, destR, destC, board)) return false;

        int dr = destR - srcR;
        int dc = destC - srcC;

        if (std::abs(dr) == 2 && std::abs(dc) == 1) {
            if (!board[srcR + dr / 2][srcC]->isEmpty) return false; // 拐馬腳判定
            return true;
        }
        if (std::abs(dr) == 1 && std::abs(dc) == 2) {
            if (!board[srcR][srcC + dc / 2]->isEmpty) return false; // 拐馬腳判定
            return true;
        }
        return false;
    }
};

class Pao : public Piece {
public:
    Pao(sf::String n, sf::Color c) : Piece(n, c) {}

    bool isValidMove(int srcR, int srcC, int destR, int destC, Piece* board[ROWS][COLS]) override {
        if (!Piece::isValidMove(srcR, srcC, destR, destC, board)) return false;
        if (srcR != destR && srcC != destC) return false;

        int count = 0;
        if (srcR == destR) {
            int minC = std::min(srcC, destC);
            int maxC = std::max(srcC, destC);
            for (int c = minC + 1; c < maxC; ++c) {
                if (!board[srcR][c]->isEmpty) count++;
            }
        }
        else {
            int minR = std::min(srcR, destR);
            int maxR = std::max(srcR, destR);
            for (int r = minR + 1; r < maxR; ++r) {
                if (!board[r][srcC]->isEmpty) count++;
            }
        }

        if (board[destR][destC]->isEmpty) return count == 0;
        return count == 1; // 吃子需具備炮架
    }
};

class Xiang : public Piece {
public:
    Xiang(sf::String n, sf::Color c) : Piece(n, c) {}

    bool isValidMove(int srcR, int srcC, int destR, int destC, Piece* board[ROWS][COLS]) override {
        if (!Piece::isValidMove(srcR, srcC, destR, destC, board)) return false;

        int dr = destR - srcR;
        int dc = destC - srcC;

        if (std::abs(dr) != 2 || std::abs(dc) != 2) return false;
        if (!board[srcR + dr / 2][srcC + dc / 2]->isEmpty) return false; // 塞象眼判定

        if (this->color == sf::Color::Black && destR > 4) return false; // 限制於己方陣地
        if (this->color == sf::Color::Red && destR < 5) return false;

        return true;
    }
};

class Shi : public Piece {
public:
    Shi(sf::String n, sf::Color c) : Piece(n, c) {}

    bool isValidMove(int srcR, int srcC, int destR, int destC, Piece* board[ROWS][COLS]) override {
        if (!Piece::isValidMove(srcR, srcC, destR, destC, board)) return false;

        int dr = destR - srcR;
        int dc = destC - srcC;

        if (std::abs(dr) != 1 || std::abs(dc) != 1) return false;

        if (destC < 3 || destC > 5) return false; // 限制於九宮格內
        if (this->color == sf::Color::Black && destR > 2) return false;
        if (this->color == sf::Color::Red && destR < 7) return false;

        return true;
    }
};

class Jiang : public Piece {
public:
    Jiang(sf::String n, sf::Color c) : Piece(n, c) {}

    bool isValidMove(int srcR, int srcC, int destR, int destC, Piece* board[ROWS][COLS]) override {
        if (!Piece::isValidMove(srcR, srcC, destR, destC, board)) return false;

        int dr = std::abs(destR - srcR);
        int dc = std::abs(destC - srcC);

        if (dr + dc != 1) return false;

        if (destC < 3 || destC > 5) return false; // 限制於九宮格內
        if (this->color == sf::Color::Black && destR > 2) return false;
        if (this->color == sf::Color::Red && destR < 7) return false;

        return true;
    }
};

class Bing : public Piece {
public:
    Bing(sf::String n, sf::Color c) : Piece(n, c) {}

    bool isValidMove(int srcR, int srcC, int destR, int destC, Piece* board[ROWS][COLS]) override {
        if (!Piece::isValidMove(srcR, srcC, destR, destC, board)) return false;

        int dr = destR - srcR;
        int dc = std::abs(destC - srcC);

        if (this->color == sf::Color::Black) {
            if (dr < 0) return false;
            if (srcR <= 4) {
                if (dr != 1 || dc != 0) return false;
            }
            else {
                if (dr + dc != 1) return false;
            }
        }
        else {
            if (dr > 0) return false;
            if (srcR >= 5) {
                if (dr != -1 || dc != 0) return false;
            }
            else {
                if (std::abs(dr) + dc != 1) return false;
            }
        }
        return true;
    }
};

#endif