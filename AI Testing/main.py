'''
Driver file. Handle user information and display current gamestate object
'''

import pygame as p
import engine 

WIDTH = HEIGHT = 512 #400 is good too with these images
DIMENSIONS = 8 #8x8
SQ_SIZE = HEIGHT//DIMENSIONS
MAX_FPS = 15 #for animations later on
IMAGES = {}

'''
Initialize a global dictionary of images. This will be called only once in main.
'''

def load_images():
    pieces = ["bR", "bN", "bB", "bQ", "bK","bP","wR", "wN", "wB", "wQ", "wK","wP"]
    for piece in pieces: 
        IMAGES[piece] = p.transform.scale(p.image.load("images/" + piece + ".png"),(SQ_SIZE,SQ_SIZE))
    #You can access images now with IMAGES["wP"] for example

'''
Main driver for the code. Handles user input and updating the graphics
'''

def main():
    p.init()
    screen = p.display.set_mode((WIDTH,HEIGHT))
    clock = p.time.Clock()
    screen.fill(p.Color("white"))
    gs = engine.GameState()
    load_images() #do this once, before the while loop
    running = True
    while running:
        for e in p.event.get():
            if e.type == p.QUIT:
                running = False
            drawGameState(screen,gs)            
            clock.tick(MAX_FPS)
            p.display.flip()

'''
Responsible for all the graphics within a current game state
'''

def drawGameState(screen, gs):
    drawBoard(screen) #draw the squares on the board
    #add in piece highlighting or move suggestions here
    drawPieces(screen,gs.board) #draw the pieces on top of the squares

'''
Draw the squares on the board. The top left square is always light
'''
def drawBoard(screen):
    colors = [p.Color("white"),p.Color("gray")]
    for r in range(DIMENSIONS):
        for c in range(DIMENSIONS):
            color = colors[((r+c) % 2)]
            p.draw.rect(screen, color, p.Rect(c*SQ_SIZE, r*SQ_SIZE, SQ_SIZE, SQ_SIZE))

'''
Draw the pieces on the board using the current 'gs.board' variable
'''
def drawPieces(screen,board):
    for r in range(DIMENSIONS):
        for c in range(DIMENSIONS):
            piece = board[r][c]
            if piece != "--": #not empty square
                screen.blit(IMAGES[piece],p.Rect(c*SQ_SIZE, r*SQ_SIZE, SQ_SIZE, SQ_SIZE))


if __name__ == "__main__":
    main()