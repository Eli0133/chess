#include "Piece.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>

// Layout and UI Constants
const float START_X = 600.0f;
const float START_Y = 120.0f;
const float CELL_SIZE = 90.0f;

const float QUIT_BTN_X = 1450.0f;
const float QUIT_BTN_Y = 850.0f;
const float RESTART_BTN_X = 1450.0f;
const float RESTART_BTN_Y = 700.0f;
const float RIGHT_BTN_WIDTH = 320.0f;
const float RIGHT_BTN_HEIGHT = 100.0f;

const float MODE_BTN_WIDTH = 450.0f;
const float MODE_BTN_HEIGHT = 100.0f;
const float BTN_1P_X = 460.0f;
const float BTN_1P_Y = 700.0f;
const float BTN_2P_X = 1010.0f;
const float BTN_2P_Y = 700.0f;

// Scoped enum to resolve Warning C26812
enum class GameState { TITLE, MODE_SELECT, PLAYING };

struct ChessMove {
    int srcR, srcC, destR, destC;
};

void drawBoardLines(sf::RenderWindow& window) {
    sf::Color lineColor = sf::Color(60, 40, 20, 180);
    for (int i = 0; i < ROWS; ++i) {
        sf::Vertex line[] = {
            sf::Vertex(sf::Vector2f(START_X, START_Y + i * CELL_SIZE), lineColor),
            sf::Vertex(sf::Vector2f(START_X + (COLS - 1) * CELL_SIZE, START_Y + i * CELL_SIZE), lineColor) };
        window.draw(line, 2, sf::Lines);
    }
    for (int i = 0; i < COLS; ++i) {
        sf::Vertex lineTop[] = {
            sf::Vertex(sf::Vector2f(START_X + i * CELL_SIZE, START_Y), lineColor),
            sf::Vertex(sf::Vector2f(START_X + i * CELL_SIZE, START_Y + 4 * CELL_SIZE), lineColor) };
        window.draw(lineTop, 2, sf::Lines);
        sf::Vertex lineBottom[] = {
            sf::Vertex(sf::Vector2f(START_X + i * CELL_SIZE, START_Y + 5 * CELL_SIZE), lineColor),
            sf::Vertex(sf::Vector2f(START_X + i * CELL_SIZE, START_Y + 9 * CELL_SIZE), lineColor) };
        window.draw(lineBottom, 2, sf::Lines);
    }
    sf::Vertex riverL[] = {
        sf::Vertex(sf::Vector2f(START_X, START_Y + 4 * CELL_SIZE), lineColor), sf::Vertex(sf::Vector2f(START_X, START_Y + 5 * CELL_SIZE), lineColor) };
    sf::Vertex riverR[] = {
        sf::Vertex(sf::Vector2f(START_X + (COLS - 1) * CELL_SIZE, START_Y + 4 * CELL_SIZE), lineColor),
        sf::Vertex(sf::Vector2f(START_X + (COLS - 1) * CELL_SIZE, START_Y + 5 * CELL_SIZE), lineColor) };
    window.draw(riverL, 2, sf::Lines);
    window.draw(riverR, 2, sf::Lines);
    sf::Vertex p1[] = {
        sf::Vertex(sf::Vector2f(START_X + 3 * CELL_SIZE, START_Y), lineColor),
        sf::Vertex(sf::Vector2f(START_X + 5 * CELL_SIZE, START_Y + 2 * CELL_SIZE), lineColor) };
    sf::Vertex p2[] = {
        sf::Vertex(sf::Vector2f(START_X + 5 * CELL_SIZE, START_Y), lineColor),
        sf::Vertex(sf::Vector2f(START_X + 3 * CELL_SIZE, START_Y + 2 * CELL_SIZE), lineColor) };
    sf::Vertex p3[] = {
        sf::Vertex(sf::Vector2f(START_X + 3 * CELL_SIZE, START_Y + 7 * CELL_SIZE), lineColor),
        sf::Vertex(sf::Vector2f(START_X + 5 * CELL_SIZE, START_Y + 9 * CELL_SIZE), lineColor) };
    sf::Vertex p4[] = {
        sf::Vertex(sf::Vector2f(START_X + 5 * CELL_SIZE, START_Y + 7 * CELL_SIZE), lineColor),
        sf::Vertex(sf::Vector2f(START_X + 3 * CELL_SIZE, START_Y + 9 * CELL_SIZE), lineColor) };
    window.draw(p1, 2, sf::Lines);
    window.draw(p2, 2, sf::Lines);
    window.draw(p3, 2, sf::Lines);
    window.draw(p4, 2, sf::Lines);
}

bool isKingsFacing(Piece* board[ROWS][COLS]) {
    int rJR = -1, rJC = -1, bJR = -1, bJC = -1;
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (board[r][c]->name == L"帥") {
                rJR = r;
                rJC = c;
            }
            else if (board[r][c]->name == L"將") {
                bJR = r;
                bJC = c;
            }
        }
    }
    if (rJR == -1 || bJR == -1)
        return false;
    if (rJC == bJC) {
        int piecesBetween = 0;
        int minR = std::min(rJR, bJR), maxR = std::max(rJR, bJR);
        for (int r = minR + 1; r < maxR; ++r) {
            if (!board[r][rJC]->isEmpty)
                piecesBetween++;
        }
        if (piecesBetween == 0)
            return true;
    }
    return false;
}

bool isUnderCheck(Piece* board[ROWS][COLS], sf::Color myColor) {
    int gR = -1, gC = -1;
    sf::String gN = (myColor == sf::Color::Red) ? L"帥" : L"將";
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (!board[r][c]->isEmpty && board[r][c]->name == gN && board[r][c]->color == myColor) {
                gR = r;
                gC = c;
                break;
            }
        }
        if (gR != -1)
            break;
    }
    if (gR == -1)
        return false;
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (!board[r][c]->isEmpty && board[r][c]->color != myColor) {
                if (board[r][c]->isValidMove(r, c, gR, gC, board))
                    return true;
            }
        }
    }
    return false;
}

bool isMoveLegalAndSafe(int sR, int sC, int dR, int dC, Piece* board[ROWS][COLS]) {
    if (!board[sR][sC]->isValidMove(sR, sC, dR, dC, board))
        return false;
    Piece* moving = board[sR][sC];
    Piece* target = board[dR][dC];
    board[dR][dC] = moving;
    board[sR][sC] = new EmptyPiece();
    bool f = isKingsFacing(board);
    bool u = isUnderCheck(board, moving->color);
    delete board[sR][sC];
    board[sR][sC] = moving;
    board[dR][dC] = target;
    return (!f && !u);
}

bool hasLegalMoves(Piece* board[ROWS][COLS], sf::Color myColor) {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (!board[r][c]->isEmpty && board[r][c]->color == myColor) {
                for (int dr = 0; dr < ROWS; ++dr) {
                    for (int dc = 0; dc < COLS; ++dc) {
                        if (isMoveLegalAndSafe(r, c, dr, dc, board))
                            return true;
                    }
                }
            }
        }
    }
    return false;
}

void makeAIMove(Piece* board[ROWS][COLS], bool& isRed, bool& isOver, sf::String& wTxt) {
    std::vector<ChessMove> captures;
    std::vector<ChessMove> normals;
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (!board[r][c]->isEmpty && board[r][c]->color == sf::Color::Black) {
                for (int dr = 0; dr < ROWS; ++dr) {
                    for (int dc = 0; dc < COLS; ++dc) {
                        if (isMoveLegalAndSafe(r, c, dr, dc, board)) {
                            if (!board[dr][dc]->isEmpty)
                                captures.push_back({ r, c, dr, dc });
                            else
                                normals.push_back({ r, c, dr, dc });
                        }
                    }
                }
            }
        }
    }
    if (captures.empty() && normals.empty()) {
        isOver = true;
        wTxt = L"死局！紅方獲勝！";
        return;
    }
    ChessMove move = (!captures.empty()) ? captures[rand() % captures.size()] : normals[rand() % normals.size()];
    if (board[move.destR][move.destC]->name == L"帥") {
        isOver = true;
        wTxt = L"黑方獲勝！";
    }
    delete board[move.destR][move.destC];
    board[move.destR][move.destC] = board[move.srcR][move.srcC];
    board[move.srcR][move.srcC] = new EmptyPiece();
    isRed = true;
    if (!isOver && !hasLegalMoves(board, sf::Color::Red)) {
        isOver = true;
        wTxt = L"死局！黑方獲勝！";
    }
}

void resetBoard(Piece* board[ROWS][COLS]) {
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            if (board[r][c])
                delete board[r][c];
            board[r][c] = new EmptyPiece();
        }
    }
    board[0][0] = new Che(L"車", sf::Color::Black);
    board[0][1] = new Ma(L"馬", sf::Color::Black);
    board[0][2] = new Xiang(L"象", sf::Color::Black);
    board[0][3] = new Shi(L"士", sf::Color::Black);
    board[0][4] = new Jiang(L"將", sf::Color::Black);
    board[0][5] = new Shi(L"士", sf::Color::Black);
    board[0][6] = new Xiang(L"象", sf::Color::Black);
    board[0][7] = new Ma(L"馬", sf::Color::Black);
    board[0][8] = new Che(L"車", sf::Color::Black);
    board[2][1] = new Pao(L"砲", sf::Color::Black);
    board[2][7] = new Pao(L"砲", sf::Color::Black);
    board[3][0] = new Bing(L"卒", sf::Color::Black);
    board[3][2] = new Bing(L"卒", sf::Color::Black);
    board[3][4] = new Bing(L"卒", sf::Color::Black);
    board[3][6] = new Bing(L"卒", sf::Color::Black);
    board[3][8] = new Bing(L"卒", sf::Color::Black);
    board[9][0] = new Che(L"俥", sf::Color::Red);
    board[9][1] = new Ma(L"傌", sf::Color::Red);
    board[9][2] = new Xiang(L"相", sf::Color::Red);
    board[9][3] = new Shi(L"仕", sf::Color::Red);
    board[9][4] = new Jiang(L"帥", sf::Color::Red);
    board[9][5] = new Shi(L"仕", sf::Color::Red);
    board[9][6] = new Xiang(L"相", sf::Color::Red);
    board[9][7] = new Ma(L"傌", sf::Color::Red);
    board[9][8] = new Che(L"俥", sf::Color::Red);
    board[7][1] = new Pao(L"炮", sf::Color::Red);
    board[7][7] = new Pao(L"炮", sf::Color::Red);
    board[6][0] = new Bing(L"兵", sf::Color::Red);
    board[6][2] = new Bing(L"兵", sf::Color::Red);
    board[6][4] = new Bing(L"兵", sf::Color::Red);
    board[6][6] = new Bing(L"兵", sf::Color::Red);
    board[6][8] = new Bing(L"兵", sf::Color::Red);
}

void drawTurnIndicator(sf::RenderWindow& window, sf::Font& font, bool isRed, bool is1P) {
    sf::Text t;
    t.setFont(font);
    t.setCharacterSize(60);
    t.setOutlineColor(sf::Color::White);
    t.setOutlineThickness(4.0f);

    if (isRed) {
        t.setString(L"目前回合:\n\n  紅 方\n (玩家)");
        t.setFillColor(sf::Color::Red);
    }
    else {
        t.setString(is1P ? L"目前回合:\n\n  黑 方\n (電腦)" : L"目前回合:\n\n  黑 方\n (玩家2)");
        t.setFillColor(sf::Color::Black);
    }
    t.setPosition(1450.0f, 150.0f);
    window.draw(t);
}

void drawButton(sf::RenderWindow& window, sf::Font& font, float x, float y, float w, float h, sf::String s, sf::Color c) {
    sf::RectangleShape b(sf::Vector2f(w, h));
    b.setPosition(x, y);
    b.setFillColor(c);
    b.setOutlineThickness(3.0f);
    b.setOutlineColor(sf::Color::White);
    window.draw(b);
    sf::Text t;
    t.setFont(font);
    t.setString(s);
    t.setCharacterSize(40);
    t.setFillColor(sf::Color::White);
    sf::FloatRect bounds = t.getLocalBounds();
    t.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);
    t.setPosition(x + w / 2.0f, y + h / 2.0f);
    window.draw(t);
}

void drawMovePreviews(sf::RenderWindow& window, Piece* board[ROWS][COLS], int sR, int sC) {
    if (sR == -1 || sC == -1)
        return;
    float r = 16.0f;
    sf::CircleShape d(r);
    d.setFillColor(sf::Color(0, 255, 0, 130));
    d.setOrigin(r, r);
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {
            if (isMoveLegalAndSafe(sR, sC, row, col, board)) {
                d.setPosition(START_X + col * CELL_SIZE, START_Y + row * CELL_SIZE);
                window.draw(d);
            }
        }
    }
}

void drawPieces(sf::RenderWindow& window, sf::Font& font, Piece* board[ROWS][COLS], int sR, int sC) {
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(55);
    for (int r = 0; r < ROWS; ++r) {
        for (int c = 0; c < COLS; ++c) {
            float px = START_X + c * CELL_SIZE, py = START_Y + r * CELL_SIZE;

            if (r == sR && c == sC) {
                sf::CircleShape h(46.0f);
                h.setFillColor(sf::Color::Transparent);
                h.setOutlineThickness(5.0f);
                h.setOutlineColor(sf::Color(0, 200, 255, 180));
                h.setOrigin(46.0f, 46.0f);
                h.setPosition(px, py);
                window.draw(h);
            }

            if (!board[r][c]->isEmpty) {
                float radius = 42.0f;

                sf::CircleShape shadow(radius);
                shadow.setOrigin(radius, radius);
                shadow.setFillColor(sf::Color(0, 0, 0, 100));
                shadow.setPosition(px + 6.0f, py + 6.0f);
                window.draw(shadow);

                sf::CircleShape base(radius);
                base.setOrigin(radius, radius);
                base.setFillColor(sf::Color(235, 195, 135));
                base.setOutlineThickness(-3.0f);
                base.setOutlineColor(sf::Color(130, 80, 40));
                base.setPosition(px, py);
                window.draw(base);

                sf::CircleShape ring(radius - 8.0f);
                ring.setOrigin(radius - 8.0f, radius - 8.0f);
                ring.setFillColor(sf::Color::Transparent);
                ring.setOutlineThickness(1.0f);
                ring.setOutlineColor(sf::Color(180, 130, 80, 150));
                ring.setPosition(px, py);
                window.draw(ring);

                text.setString(board[r][c]->name);
                sf::FloatRect textBounds = text.getLocalBounds();
                text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);

                text.setFillColor(sf::Color(60, 40, 20, 120));
                text.setPosition(px + 1.5f, py + 1.5f);
                window.draw(text);

                text.setFillColor(board[r][c]->color);
                text.setPosition(px, py);
                window.draw(text);
            }
        }
    }
}

struct SoundManager {
    sf::SoundBuffer openingB, AIbattleB, twoPlayerB, moveB, invalidMoveB, retryB, checkMateB, winB;
    sf::SoundBuffer jiangB, shiB, xiangB, cheB, maB, baoB, zuB, shuaiB, paoB, bingB;

    sf::Sound opening, AIbattle, twoPlayer, move, invalidMove, retry, checkMate, win;
    sf::Sound jiang, shi, xiang, che, ma, bao, zu, shuai, pao, bing;

    void loadAll() {
        if (openingB.loadFromFile("opening.mp3")) opening.setBuffer(openingB);
        opening.setVolume(200.f);
        if (AIbattleB.loadFromFile("AIbattle.mp3")) AIbattle.setBuffer(AIbattleB);
        if (twoPlayerB.loadFromFile("twoPlayer.mp3")) twoPlayer.setBuffer(twoPlayerB);
        if (moveB.loadFromFile("move.mp3")) move.setBuffer(moveB);
        if (invalidMoveB.loadFromFile("invalidMove.mp3")) invalidMove.setBuffer(invalidMoveB);
        if (retryB.loadFromFile("retry.mp3")) retry.setBuffer(retryB);
        if (checkMateB.loadFromFile("checkMate.mp3")) checkMate.setBuffer(checkMateB);
        if (winB.loadFromFile("win.mp3")) win.setBuffer(winB);

        if (jiangB.loadFromFile("jiang.mp3")) jiang.setBuffer(jiangB);
        if (shiB.loadFromFile("shi.mp3")) shi.setBuffer(shiB);
        if (xiangB.loadFromFile("xiang.mp3")) xiang.setBuffer(xiangB);
        if (cheB.loadFromFile("che.mp3")) che.setBuffer(cheB);
        if (maB.loadFromFile("ma.mp3")) ma.setBuffer(maB);
        if (baoB.loadFromFile("bao.mp3")) bao.setBuffer(baoB);
        if (zuB.loadFromFile("zu.mp3")) zu.setBuffer(zuB);
        if (shuaiB.loadFromFile("shuai.mp3")) shuai.setBuffer(shuaiB);
        if (paoB.loadFromFile("pao.mp3")) pao.setBuffer(paoB);
        if (bingB.loadFromFile("bing.mp3")) bing.setBuffer(bingB);
    }
};

void playPieceSound(const sf::String& name, SoundManager& sound) {
    if (name == L"將") sound.jiang.play();
    else if (name == L"士" || name == L"仕") sound.shi.play();
    else if (name == L"象" || name == L"相") sound.xiang.play();
    else if (name == L"車" || name == L"俥") sound.che.play();
    else if (name == L"馬" || name == L"傌") sound.ma.play();
    else if (name == L"砲") sound.bao.play();
    else if (name == L"炮") sound.pao.play();
    else if (name == L"卒") sound.zu.play();
    else if (name == L"帥") sound.shuai.play();
    else if (name == L"兵") sound.bing.play();
}

int main() {
    srand(static_cast<unsigned>(time(NULL)));
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "My Chess Game");

    SoundManager sound;
    sound.loadAll();
    sound.opening.setVolume(30.f);
    sound.opening.play();

    sf::Font font;
    if (!font.loadFromFile("Iansui-Regular.ttf"))
        std::cout << "Font Error\n";

    sf::Texture bgT;
    bool hasBg = bgT.loadFromFile("bg.jpg");
    sf::Sprite bgS;
    if (hasBg) {
        bgS.setTexture(bgT);
        sf::Vector2u sz = bgT.getSize();
        bgS.setScale(1920.0f / sz.x, 1080.0f / sz.y);
    }

    Piece* board[ROWS][COLS] = { nullptr };
    resetBoard(board);

    GameState state = GameState::TITLE;
    bool is1P = false;
    int sR = -1, sC = -1;
    bool isRed = true, isOver = false;
    int checkState = 0;
    sf::String wTxt = L"";
    bool sErr = false;
    sf::Clock eT, aiT, bT;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                int mX = event.mouseButton.x, mY = event.mouseButton.y;

                if (state == GameState::TITLE) {
                    state = GameState::MODE_SELECT;
                    continue;
                }

                if (state == GameState::MODE_SELECT) {
                    if (mX >= BTN_1P_X && mX <= BTN_1P_X + MODE_BTN_WIDTH && mY >= BTN_1P_Y && mY <= BTN_1P_Y + MODE_BTN_HEIGHT) {
                        is1P = true;
                        state = GameState::PLAYING;
                        sound.AIbattle.play();
                    }
                    if (mX >= BTN_2P_X && mX <= BTN_2P_X + MODE_BTN_WIDTH && mY >= BTN_2P_Y && mY <= BTN_2P_Y + MODE_BTN_HEIGHT) {
                        is1P = false;
                        state = GameState::PLAYING;
                        sound.twoPlayer.play();
                    }
                    continue;
                }

                if (state == GameState::PLAYING) {
                    if (mX >= QUIT_BTN_X && mX <= QUIT_BTN_X + RIGHT_BTN_WIDTH && mY >= QUIT_BTN_Y && mY <= QUIT_BTN_Y + RIGHT_BTN_HEIGHT)
                        window.close();
                    if (mX >= RESTART_BTN_X && mX <= RESTART_BTN_X + RIGHT_BTN_WIDTH && mY >= RESTART_BTN_Y && mY <= RESTART_BTN_Y + RIGHT_BTN_HEIGHT) {
                        resetBoard(board);
                        sR = sC = -1;
                        isRed = true;
                        isOver = false;
                        checkState = 0;
                        state = GameState::MODE_SELECT;
                        sound.retry.play();
                        continue;
                    }

                    if (is1P && !isRed)
                        continue;
                    if (isOver)
                        continue;

                    int cC = static_cast<int>(std::round((mX - START_X) / CELL_SIZE));
                    int cR = static_cast<int>(std::round((mY - START_Y) / CELL_SIZE));

                    if (cR >= 0 && cR < ROWS && cC >= 0 && cC < COLS) {
                        if (sR == -1) {
                            if (!board[cR][cC]->isEmpty && (board[cR][cC]->color == sf::Color::Red) == isRed) {
                                sR = cR;
                                sC = cC;
                                playPieceSound(board[cR][cC]->name, sound);
                            }
                        }
                        else {
                            if (!board[cR][cC]->isEmpty && board[cR][cC]->color == board[sR][sC]->color) {
                                sR = cR;
                                sC = cC;
                                playPieceSound(board[cR][cC]->name, sound);
                            }
                            else {
                                if (isMoveLegalAndSafe(sR, sC, cR, cC, board)) {
                                    bool isWin = false;

                                    if (board[cR][cC]->name == L"將") {
                                        isOver = true;
                                        wTxt = L"紅方獲勝！";
                                        isWin = true;
                                    }
                                    else if (board[cR][cC]->name == L"帥") {
                                        isOver = true;
                                        wTxt = L"黑方獲勝！";
                                        isWin = true;
                                    }

                                    delete board[cR][cC];
                                    board[cR][cC] = board[sR][sC];
                                    board[sR][sC] = new EmptyPiece();
                                    isRed = !isRed;
                                    aiT.restart();

                                    checkState = 0;
                                    if (isUnderCheck(board, sf::Color::Black)) checkState = 1;
                                    else if (isUnderCheck(board, sf::Color::Red)) checkState = 2;

                                    if (!isOver && !hasLegalMoves(board, isRed ? sf::Color::Red : sf::Color::Black)) {
                                        isOver = true;
                                        wTxt = isRed ? L"死局！黑方獲勝！" : L"死局！紅方獲勝！";
                                        isWin = true;
                                    }

                                    if (isWin) {
                                        sound.win.play();
                                    }
                                    else if (checkState != 0) {
                                        sound.checkMate.play();
                                    }
                                    else {
                                        sound.move.play();
                                    }
                                }
                                else {
                                    sErr = true;
                                    eT.restart();
                                    sound.invalidMove.play();
                                }
                                sR = sC = -1;
                            }
                        }
                    }
                }
            }
        }

        if (state == GameState::PLAYING && is1P && !isRed && !isOver && aiT.getElapsedTime().asSeconds() > 0.5f) {
            makeAIMove(board, isRed, isOver, wTxt);
            checkState = 0;
            if (isUnderCheck(board, sf::Color::Black)) checkState = 1;
            else if (isUnderCheck(board, sf::Color::Red)) checkState = 2;

            if (!isOver) {
                if (checkState != 0) {
                    sound.checkMate.play();
                }
                else {
                    sound.move.play();
                }
            }
        }

        window.clear(sf::Color(218, 165, 32));
        if (hasBg) {
            window.draw(bgS);
        }

        if (state == GameState::TITLE || state == GameState::MODE_SELECT) {
            drawBoardLines(window);
            sf::RectangleShape o(sf::Vector2f(1920, 1080));
            o.setFillColor(sf::Color(0, 0, 0, 180));
            window.draw(o);

            sf::Text t;
            t.setFont(font);
            t.setString(L"中 國 象 棋");
            t.setCharacterSize(200);
            t.setFillColor(sf::Color::Yellow);
            t.setOutlineColor(sf::Color::Red);
            t.setOutlineThickness(8);

            sf::FloatRect bounds = t.getLocalBounds();
            t.setOrigin(bounds.left + bounds.width / 2, bounds.top + bounds.height / 2);
            t.setPosition(960, 350);
            window.draw(t);

            if (state == GameState::TITLE) {
                if (static_cast<int>(bT.getElapsedTime().asMilliseconds() / 600) % 2 == 0) {
                    sf::Text s;
                    s.setFont(font);
                    s.setString(L"- 點擊畫面任意處開始 -");
                    s.setCharacterSize(60);
                    s.setFillColor(sf::Color::White);
                    sf::FloatRect sBounds = s.getLocalBounds();
                    s.setOrigin(sBounds.left + sBounds.width / 2, sBounds.top + sBounds.height / 2);
                    s.setPosition(960, 750);
                    window.draw(s);
                }
            }
            else {
                drawButton(window, font, BTN_1P_X, BTN_1P_Y, MODE_BTN_WIDTH, MODE_BTN_HEIGHT, L"單人模式 (VS電腦)", sf::Color(50, 150, 50));
                drawButton(window, font, BTN_2P_X, BTN_2P_Y, MODE_BTN_WIDTH, MODE_BTN_HEIGHT, L"雙人模式 (玩家對戰)", sf::Color(150, 50, 50));
            }
        }
        else {
            drawBoardLines(window);
            drawMovePreviews(window, board, sR, sC);
            drawPieces(window, font, board, sR, sC);
            drawTurnIndicator(window, font, isRed, is1P);
            drawButton(window, font, RESTART_BTN_X, RESTART_BTN_Y, RIGHT_BTN_WIDTH, RIGHT_BTN_HEIGHT, L"重新開始", sf::Color(50, 100, 150));
            drawButton(window, font, QUIT_BTN_X, QUIT_BTN_Y, RIGHT_BTN_WIDTH, RIGHT_BTN_HEIGHT, L"退出遊戲", sf::Color(100, 100, 100));

            if (sErr && eT.getElapsedTime().asSeconds() < 1.5f) {
                sf::Text e;
                e.setFont(font);
                e.setString(L"無效的移動！");
                e.setCharacterSize(80);
                e.setFillColor(sf::Color::Red);
                e.setOutlineColor(sf::Color::White);
                e.setOutlineThickness(4);
                sf::FloatRect eBounds = e.getLocalBounds();
                e.setOrigin(eBounds.left + eBounds.width / 2, eBounds.top + eBounds.height / 2);

                float errY = 540.0f;
                if (checkState != 0) errY = 690.0f;
                e.setPosition(960, errY);

                window.draw(e);
            }

            if (checkState != 0 && !isOver) {
                sf::Color warningColor = (checkState == 1) ? sf::Color::Red : sf::Color::Black;

                sf::Text smallTxt;
                smallTxt.setFont(font);
                smallTxt.setString(checkState == 1 ? L"紅方" : L"黑方");
                smallTxt.setCharacterSize(40);
                smallTxt.setFillColor(warningColor);
                smallTxt.setOutlineColor(sf::Color::White);
                smallTxt.setOutlineThickness(3);
                sf::FloatRect sb = smallTxt.getLocalBounds();
                smallTxt.setOrigin(sb.left + sb.width / 2.0f, sb.top + sb.height / 2.0f);
                smallTxt.setPosition(START_X + 4.0f * CELL_SIZE, START_Y + 4.0f * CELL_SIZE - 30.0f);
                window.draw(smallTxt);

                sf::Text checkTxt;
                checkTxt.setFont(font);
                checkTxt.setString(L"將軍！");
                checkTxt.setCharacterSize(100);
                checkTxt.setFillColor(warningColor);
                checkTxt.setOutlineColor(sf::Color::White);
                checkTxt.setOutlineThickness(6);
                sf::FloatRect checkBounds = checkTxt.getLocalBounds();
                checkTxt.setOrigin(checkBounds.left + checkBounds.width / 2.0f, checkBounds.top + checkBounds.height / 2.0f);
                checkTxt.setPosition(START_X + 4.0f * CELL_SIZE, START_Y + 4.5f * CELL_SIZE + 20.0f);
                window.draw(checkTxt);
            }

            if (isOver) {
                sf::Text w;
                w.setFont(font);
                w.setString(wTxt);
                w.setCharacterSize(120);
                w.setFillColor(sf::Color::Yellow);
                w.setOutlineColor(sf::Color::Red);
                w.setOutlineThickness(6);
                sf::FloatRect wBounds = w.getLocalBounds();
                w.setOrigin(wBounds.left + wBounds.width / 2, wBounds.top + wBounds.height / 2);
                w.setPosition(960, 540);
                window.draw(w);
            }
        }
        window.display();
    }
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++)
            delete board[r][c];
    }
    return 0;
}