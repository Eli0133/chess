"""不依賴繪圖套件的遊戲狀態、合法性與簡易電腦對手。"""

import random

from pieces import BLACK, RED, ROWS, COLS, Bing, Che, Jiang, Ma, Pao, Shi, Xiang, between


def opponent(color):
    return BLACK if color == RED else RED


class Game:
    def __init__(self):
        self.reset()

    def reset(self):
        self.board = [[None for _ in range(COLS)] for _ in range(ROWS)]
        self.turn = RED
        self.winner = None
        self.reason = ""
        classes = (Che, Ma, Xiang, Shi, Jiang, Shi, Xiang, Ma, Che)
        for color, row, names, cannon, soldier in (
            (BLACK, 0, "車馬象士將士象馬車", "砲", "卒"),
            (RED, 9, "俥傌相仕帥仕相傌俥", "炮", "兵"),
        ):
            for col, (kind, name) in enumerate(zip(classes, names)):
                self.board[row][col] = kind(name, color)
            for col in (1, 7):
                self.board[2 if color == BLACK else 7][col] = Pao(cannon, color)
            for col in range(0, COLS, 2):
                self.board[3 if color == BLACK else 6][col] = Bing(soldier, color)

    def king(self, color):
        return next(((r, c) for r in range(ROWS) for c in range(COLS)
                     if isinstance(self.board[r][c], Jiang)
                     and self.board[r][c].color == color), None)

    def in_check(self, color):
        king = self.king(color)
        if king is None:
            return True
        other = self.king(opponent(color))
        if other and king[1] == other[1] and between(king, other, self.board) == 0:
            return True
        return any(piece and piece.color != color
                   and piece.valid_move((r, c), king, self.board)
                   for r, row in enumerate(self.board) for c, piece in enumerate(row))

    def legal(self, src, dst):
        if not all(0 <= r < ROWS and 0 <= c < COLS for r, c in (src, dst)):
            return False
        r, c = src
        y, x = dst
        piece = self.board[r][c]
        if piece is None or not piece.valid_move(src, dst, self.board):
            return False
        target = self.board[y][x]
        self.board[y][x], self.board[r][c] = piece, None
        try:
            return not self.in_check(piece.color)
        finally:
            self.board[r][c], self.board[y][x] = piece, target

    def legal_moves(self, color):
        for r, row in enumerate(self.board):
            for c, piece in enumerate(row):
                if piece and piece.color == color:
                    for y in range(ROWS):
                        for x in range(COLS):
                            if self.legal((r, c), (y, x)):
                                yield (r, c), (y, x)

    def move(self, src, dst):
        if self.winner or not self.legal(src, dst):
            return False
        r, c = src
        y, x = dst
        piece = self.board[r][c]
        if piece.color != self.turn:
            return False
        captured = self.board[y][x]
        self.board[y][x], self.board[r][c] = piece, None
        self.turn = opponent(self.turn)
        if isinstance(captured, Jiang):
            self.winner, self.reason = piece.color, ""
        elif next(self.legal_moves(self.turn), None) is None:
            self.winner = piece.color
            self.reason = "將死！" if self.in_check(self.turn) else "無棋可走！"
        return True

    def ai_move(self):
        moves = list(self.legal_moves(self.turn))
        if not moves:
            self.winner = opponent(self.turn)
            self.reason = "無棋可走！"
            return False
        captures = [move for move in moves if self.board[move[1][0]][move[1][1]]]
        return self.move(*random.choice(captures or moves))
