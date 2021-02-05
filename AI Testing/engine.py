'''
Stores all the info of the current state. Determines valid moves at the current state.
Keeps a gamelog.
'''

class GameState():
    def __init__(self):
        #Board is 8x8, each element has 2 characters
        #b or w = colour
        #'R', 'N', 'B', 'Q', 'K', 'P' = type of piece
        #'--' = empty space (no piece)
        self.board = [
            ["bR", "bN", "bB", "bQ", "bK", "bB", "bN", "bR"],
            ["bP", "bP", "bP", "bP", "bP", "bP", "bP", "bP"],
            ["--", "--", "--", "--", "--", "--", "--", "--"],
            ["--", "--", "--", "--", "--", "--", "--", "--"],
            ["--", "--", "--", "--", "--", "--", "--", "--"],
            ["--", "--", "--", "--", "--", "--", "--", "--"],
            ["wP", "wP", "wP", "wP", "wP", "wP", "wP", "wP"],
            ["wR", "wN", "wB", "wQ", "wK", "wB", "wN", "wR"]        
        ]
        self.WhiteToMove = True
        self.moveLog = []