import unittest

from game import Game
from pieces import BLACK, RED, Bing, Che, Jiang, Ma, Pao, Shi, Xiang


class RulesTests(unittest.TestCase):
    def setUp(self):
        self.game = Game()

    def empty_board(self):
        return [[None for _ in range(9)] for _ in range(10)]

    def test_initial_position_and_turn(self):
        game = self.game
        self.assertEqual(sum(p is not None for row in game.board for p in row), 32)
        self.assertFalse(game.in_check(RED))
        self.assertFalse(game.in_check(BLACK))
        self.assertEqual(len(list(game.legal_moves(RED))), 44)
        self.assertFalse(game.move((3, 0), (4, 0)))
        self.assertTrue(game.move((6, 0), (5, 0)))
        self.assertEqual(game.turn, BLACK)

    def test_rook_and_cannon_screens(self):
        board = self.empty_board()
        rook = Che("俥", RED)
        cannon = Pao("炮", RED)
        board[5][0] = Bing("兵", RED)
        board[3][0] = Bing("卒", BLACK)
        self.assertFalse(rook.valid_move((7, 0), (3, 0), board))
        self.assertTrue(cannon.valid_move((7, 0), (3, 0), board))
        self.assertFalse(cannon.valid_move((7, 0), (4, 0), board))
        board[5][0] = None
        self.assertFalse(cannon.valid_move((7, 0), (3, 0), board))
        self.assertTrue(cannon.valid_move((7, 0), (4, 0), board))

    def test_horse_and_elephant_blockers(self):
        board = self.empty_board()
        horse = Ma("傌", RED)
        self.assertTrue(horse.valid_move((9, 1), (7, 0), board))
        board[8][1] = Bing("兵", RED)
        self.assertFalse(horse.valid_move((9, 1), (7, 0), board))
        elephant = Xiang("相", RED)
        self.assertTrue(elephant.valid_move((9, 2), (7, 4), board))
        board[8][3] = Bing("兵", RED)
        self.assertFalse(elephant.valid_move((9, 2), (7, 4), board))
        self.assertFalse(elephant.valid_move((5, 2), (3, 4), board))

    def test_palace_and_soldiers(self):
        board = self.empty_board()
        self.assertTrue(Shi("仕", RED).valid_move((9, 3), (8, 4), board))
        self.assertFalse(Shi("仕", RED).valid_move((7, 3), (6, 4), board))
        self.assertFalse(Jiang("帥", RED).valid_move((7, 4), (6, 4), board))
        for color, start, crossed, forward in ((RED, 6, 4, -1), (BLACK, 3, 5, 1)):
            soldier = Bing("兵", color)
            self.assertFalse(soldier.valid_move((start, 4), (start, 5), board))
            self.assertTrue(soldier.valid_move((crossed, 4), (crossed, 5), board))
            self.assertTrue(soldier.valid_move((start, 4), (start + forward, 4), board))
            self.assertFalse(soldier.valid_move((crossed, 4), (crossed - forward, 4), board))

    def test_facing_kings_and_simulation_restore(self):
        game = self.game
        game.board = self.empty_board()
        game.board[0][4] = Jiang("將", BLACK)
        game.board[9][4] = Jiang("帥", RED)
        game.board[5][4] = Che("俥", RED)
        before = [row[:] for row in game.board]
        self.assertFalse(game.legal((5, 4), (5, 3)))
        self.assertEqual(game.board, before)
        self.assertTrue(game.legal((5, 4), (4, 4)))
        self.assertFalse(game.legal((-1, 0), (0, 0)))

    def test_cannot_expose_king(self):
        game = self.game
        game.board = self.empty_board()
        game.board[0][3] = Jiang("將", BLACK)
        game.board[9][4] = Jiang("帥", RED)
        game.board[5][4] = Che("俥", RED)
        game.board[1][4] = Che("車", BLACK)
        self.assertFalse(game.legal((5, 4), (5, 5)))

    def test_stalemate_is_loss(self):
        game = self.game
        game.board = self.empty_board()
        game.board[0][4] = Jiang("將", BLACK)
        game.board[9][3] = Jiang("帥", RED)
        game.board[2][3] = Che("俥", RED)
        game.board[2][5] = Che("俥", RED)
        game.board[2][0] = Che("俥", RED)
        self.assertTrue(game.move((2, 0), (1, 0)))
        self.assertFalse(game.in_check(BLACK))
        self.assertEqual(game.winner, RED)
        self.assertEqual(game.reason, "無棋可走！")

    def test_ai_prefers_capture(self):
        game = self.game
        game.board = self.empty_board()
        game.board[0][3] = Jiang("將", BLACK)
        game.board[9][4] = Jiang("帥", RED)
        game.board[3][0] = Che("車", BLACK)
        game.board[3][2] = Bing("兵", RED)
        game.turn = BLACK
        self.assertTrue(game.ai_move())
        self.assertIsInstance(game.board[3][2], Che)
        self.assertEqual(game.turn, RED)


if __name__ == "__main__":
    unittest.main()
