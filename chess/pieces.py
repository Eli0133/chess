"""象棋棋子與走法；座標採 (列, 欄)，黑方位於上方。"""

from dataclasses import dataclass

RED = "red"
BLACK = "black"
ROWS, COLS = 10, 9


@dataclass(frozen=True)
class Piece:
    name: str
    color: str

    def valid_move(self, src, dst, board):
        r, c = src
        y, x = dst
        if not (0 <= y < ROWS and 0 <= x < COLS) or src == dst:
            return False
        target = board[y][x]
        return target is None or target.color != self.color


def between(src, dst, board):
    r, c = src
    y, x = dst
    if r == y:
        return sum(board[r][i] is not None for i in range(min(c, x) + 1, max(c, x)))
    return sum(board[i][c] is not None for i in range(min(r, y) + 1, max(r, y)))


class Che(Piece):
    def valid_move(self, src, dst, board):
        return (super().valid_move(src, dst, board)
                and (src[0] == dst[0] or src[1] == dst[1])
                and between(src, dst, board) == 0)


class Ma(Piece):
    def valid_move(self, src, dst, board):
        if not super().valid_move(src, dst, board):
            return False
        r, c = src
        dr, dc = dst[0] - r, dst[1] - c
        if abs(dr) == 2 and abs(dc) == 1:
            return board[r + dr // 2][c] is None
        if abs(dr) == 1 and abs(dc) == 2:
            return board[r][c + dc // 2] is None
        return False


class Pao(Piece):
    def valid_move(self, src, dst, board):
        if not super().valid_move(src, dst, board):
            return False
        if src[0] != dst[0] and src[1] != dst[1]:
            return False
        return between(src, dst, board) == (1 if board[dst[0]][dst[1]] else 0)


class Xiang(Piece):
    def valid_move(self, src, dst, board):
        if not super().valid_move(src, dst, board):
            return False
        r, c = src
        y, x = dst
        return (abs(y - r) == abs(x - c) == 2
                and board[(r + y) // 2][(c + x) // 2] is None
                and (y <= 4 if self.color == BLACK else y >= 5))


def in_palace(dst, color):
    r, c = dst
    return 3 <= c <= 5 and (0 <= r <= 2 if color == BLACK else 7 <= r <= 9)


class Shi(Piece):
    def valid_move(self, src, dst, board):
        return (super().valid_move(src, dst, board)
                and abs(dst[0] - src[0]) == abs(dst[1] - src[1]) == 1
                and in_palace(dst, self.color))


class Jiang(Piece):
    def valid_move(self, src, dst, board):
        return (super().valid_move(src, dst, board)
                and abs(dst[0] - src[0]) + abs(dst[1] - src[1]) == 1
                and in_palace(dst, self.color))


class Bing(Piece):
    def valid_move(self, src, dst, board):
        if not super().valid_move(src, dst, board):
            return False
        dr, dc = dst[0] - src[0], dst[1] - src[1]
        forward = 1 if self.color == BLACK else -1
        crossed = src[0] >= 5 if self.color == BLACK else src[0] <= 4
        return (dr == forward and dc == 0) or (crossed and dr == 0 and abs(dc) == 1)
