"""執行方式：python main.py。所有素材皆相對於本檔載入。"""

import os
from pathlib import Path

os.environ.setdefault("PYGAME_HIDE_SUPPORT_PROMPT", "1")
import pygame

from game import Game
from pieces import RED, BLACK, ROWS, COLS

ASSETS = Path(__file__).resolve().parent
WIDTH, HEIGHT = 1920, 1080
START_X, START_Y, CELL = 600, 120, 90
COLORS = {RED: (195, 28, 30), BLACK: (30, 25, 20)}
PIECE_SOUNDS = dict(zip("將士仕象相車俥馬傌砲炮卒帥兵",
                       ("jiang", "shi", "shi", "xiang", "xiang", "che", "che",
                        "ma", "ma", "bao", "pao", "zu", "shuai", "bing")))


class ChessApp:
    def __init__(self):
        pygame.init()
        self.window = pygame.display.set_mode((1280, 720), pygame.RESIZABLE)
        pygame.display.set_caption("中國象棋 · Python")
        self.canvas = pygame.Surface((WIDTH, HEIGHT))
        self.clock = pygame.time.Clock()
        self.fonts = {}
        self.sounds = {}
        if pygame.mixer.get_init():
            for name in set(PIECE_SOUNDS.values()) | {
                "opening", "AIbattle", "twoPlayer", "move", "invalidMove", "retry", "checkMate", "win"
            }:
                try:
                    self.sounds[name] = pygame.mixer.Sound(str(ASSETS / f"{name}.mp3"))
                except (pygame.error, OSError):
                    pass
            if "opening" in self.sounds:
                self.sounds["opening"].set_volume(0.3)
        try:
            self.background = pygame.transform.smoothscale(
                pygame.image.load(str(ASSETS / "bg.jpg")).convert(), (WIDTH, HEIGHT))
        except (pygame.error, OSError):
            self.background = None
        self.game = Game()
        self.state = "title"
        self.single = False
        self.selected = None
        self.previews = []
        self.error_until = 0
        self.ai_at = 0
        self.running = True
        self.viewport = pygame.Rect(0, 0, 1280, 720)
        self.play("opening")

    def play(self, name):
        if name in self.sounds:
            self.sounds[name].play()

    def text(self, value, size, center, color=(255, 255, 255)):
        if size not in self.fonts:
            path = ASSETS / "Iansui-Regular.ttf"
            self.fonts[size] = pygame.font.Font(str(path) if path.exists() else None, size)
        surface = self.fonts[size].render(value, True, color)
        self.canvas.blit(surface, surface.get_rect(center=center))

    def button(self, rect, label, color):
        pygame.draw.rect(self.canvas, color, rect, border_radius=10)
        pygame.draw.rect(self.canvas, "white", rect, width=3, border_radius=10)
        self.text(label, 40, rect.center)

    @staticmethod
    def point(row, col):
        return START_X + col * CELL, START_Y + row * CELL

    def board_lines(self):
        color = (75, 48, 28)
        for row in range(ROWS):
            pygame.draw.line(self.canvas, color, self.point(row, 0), self.point(row, 8), 2)
        for col in range(COLS):
            pygame.draw.line(self.canvas, color, self.point(0, col), self.point(4, col), 2)
            pygame.draw.line(self.canvas, color, self.point(5, col), self.point(9, col), 2)
        for col in (0, 8):
            pygame.draw.line(self.canvas, color, self.point(4, col), self.point(5, col), 2)
        for row in (0, 7):
            pygame.draw.line(self.canvas, color, self.point(row, 3), self.point(row + 2, 5), 2)
            pygame.draw.line(self.canvas, color, self.point(row, 5), self.point(row + 2, 3), 2)

    def result_sound(self):
        self.play("win" if self.game.winner else
                  "checkMate" if self.game.in_check(self.game.turn) else "move")

    def click(self, pos):
        if self.state == "title":
            self.state = "mode"
            return
        if self.state == "mode":
            for single, rect in ((True, pygame.Rect(460, 700, 450, 100)),
                                 (False, pygame.Rect(1010, 700, 450, 100))):
                if rect.collidepoint(pos):
                    self.single = single
                    self.state = "playing"
                    self.play("AIbattle" if single else "twoPlayer")
            return
        if pygame.Rect(1450, 850, 320, 100).collidepoint(pos):
            self.running = False
            return
        if pygame.Rect(1450, 700, 320, 100).collidepoint(pos):
            self.game.reset()
            self.selected = None
            self.previews = []
            self.error_until = 0
            self.state = "mode"
            self.play("retry")
            return
        if self.game.winner or (self.single and self.game.turn == BLACK):
            return
        col = int((pos[0] - START_X + CELL / 2) // CELL)
        row = int((pos[1] - START_Y + CELL / 2) // CELL)
        if not (0 <= row < ROWS and 0 <= col < COLS):
            return
        piece = self.game.board[row][col]
        if piece and piece.color == self.game.turn:
            self.selected = (row, col)
            self.previews = [(r, c) for r in range(ROWS) for c in range(COLS)
                             if self.game.legal(self.selected, (r, c))]
            self.play(PIECE_SOUNDS[piece.name])
        elif self.selected:
            if self.game.move(self.selected, (row, col)):
                self.result_sound()
                self.ai_at = pygame.time.get_ticks() + 500
            else:
                self.error_until = pygame.time.get_ticks() + 1500
                self.play("invalidMove")
            self.selected = None
            self.previews = []

    def draw(self):
        self.canvas.fill((218, 180, 110))
        if self.background:
            self.canvas.blit(self.background, (0, 0))
        self.board_lines()
        now = pygame.time.get_ticks()
        if self.state in ("title", "mode"):
            overlay = pygame.Surface((WIDTH, HEIGHT), pygame.SRCALPHA)
            overlay.fill((0, 0, 0, 180))
            self.canvas.blit(overlay, (0, 0))
            self.text("中 國 象 棋", 190, (960, 350), (255, 220, 65))
            if self.state == "title":
                if (now // 600) % 2 == 0:
                    self.text("- 點擊畫面任意處開始 -", 60, (960, 750))
            else:
                self.button(pygame.Rect(460, 700, 450, 100), "單人模式 (VS電腦)", (50, 130, 50))
                self.button(pygame.Rect(1010, 700, 450, 100), "雙人模式 (玩家對戰)", (150, 50, 50))
            return
        for row, pieces in enumerate(self.game.board):
            for col, piece in enumerate(pieces):
                x, y = self.point(row, col)
                if (row, col) == self.selected:
                    pygame.draw.circle(self.canvas, (0, 200, 255), (x, y), 47, 5)
                if piece:
                    pygame.draw.circle(self.canvas, (90, 65, 40), (x + 5, y + 5), 42)
                    pygame.draw.circle(self.canvas, (235, 195, 135), (x, y), 42)
                    pygame.draw.circle(self.canvas, (130, 80, 40), (x, y), 42, 3)
                    pygame.draw.circle(self.canvas, (180, 130, 80), (x, y), 34, 1)
                    self.text(piece.name, 55, (x, y), COLORS[piece.color])
        for row, col in self.previews:
            x, y = self.point(row, col)
            if self.game.board[row][col]:
                pygame.draw.circle(self.canvas, (40, 185, 70), (x, y), 43, 5)
            else:
                pygame.draw.circle(self.canvas, (40, 185, 70), (x, y), 13)
        turn = self.game.turn
        self.text("目前回合", 55, (1610, 180), COLORS[turn])
        self.text("紅 方" if turn == RED else "黑 方", 65, (1610, 290), COLORS[turn])
        player = "玩家" if turn == RED else "電腦" if self.single else "玩家2"
        self.text(f"({player})", 45, (1610, 380), COLORS[turn])
        self.button(pygame.Rect(1450, 700, 320, 100), "重新開始", (50, 100, 150))
        self.button(pygame.Rect(1450, 850, 320, 100), "退出遊戲", (100, 100, 100))
        if self.game.winner:
            name = "紅方" if self.game.winner == RED else "黑方"
            self.banner(self.game.reason + name + "獲勝！", 540, (255, 220, 65))
        elif self.game.in_check(turn):
            self.banner(("紅方" if turn == RED else "黑方") + "被將軍！", 520, (220, 40, 40))
        if now < self.error_until:
            self.banner("無效的移動！", 660, (220, 40, 40))

    def banner(self, label, y, color):
        pygame.draw.rect(self.canvas, (245, 230, 195), (570, y - 55, 780, 110), border_radius=12)
        self.text(label, 70, (960, y), color)

    def run(self, max_frames=None):
        frames = 0
        try:
            while self.running:
                # 維持 16:9，點擊座標與畫面共用同一個縮放區域。
                width, height = self.window.get_size()
                scale = min(width / WIDTH, height / HEIGHT)
                size = (max(1, round(WIDTH * scale)), max(1, round(HEIGHT * scale)))
                self.viewport = pygame.Rect((width - size[0]) // 2, (height - size[1]) // 2, *size)
                for event in pygame.event.get():
                    if event.type == pygame.QUIT:
                        self.running = False
                    elif event.type == pygame.MOUSEBUTTONDOWN and event.button == 1:
                        if self.viewport.collidepoint(event.pos):
                            self.click(((event.pos[0] - self.viewport.x) * WIDTH / self.viewport.width,
                                        (event.pos[1] - self.viewport.y) * HEIGHT / self.viewport.height))
                if (self.state == "playing" and self.single and self.game.turn == BLACK
                        and not self.game.winner and pygame.time.get_ticks() >= self.ai_at):
                    self.game.ai_move()
                    self.result_sound()
                self.draw()
                self.window.fill((20, 18, 15))
                self.window.blit(pygame.transform.smoothscale(self.canvas, size), self.viewport)
                pygame.display.flip()
                self.clock.tick(60)
                frames += 1
                if max_frames is not None and frames >= max_frames:
                    break
        finally:
            pygame.quit()


if __name__ == "__main__":
    ChessApp().run()
