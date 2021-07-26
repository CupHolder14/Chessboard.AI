from pygame.constants import WINDOWSHOWN
import SerialCommunication as SC
import ChessEvents
import engine
import AI

import pygame
pygame.init()

game = engine.Game()

'''
Board Size UI Variables
'''
board_len = 8 #8x8 chess board 
w = h = 512 #UI pixel dimensions. 512 is divisible by 8 (8x8 board).
tile_size = int(w/board_len) #Pixel size of each tile for shaping purposes

'''
Load Piece Assets Into UI
'''
piece_names = ["bR", "bN", "bB", "bQ", "bK","bP","wR", "wN", "wB", "wQ", "wK","wP"]
piece_images = {} #Initialize dictionary that will hold the png files
for i in piece_names: 
    piece_images[i] = pygame.image.load("png_pieces/" + i + ".png") #Load all the piece images with the corresponding piece_names into your dictionary
    piece_images[i] = pygame.transform.scale(piece_images[i],(tile_size,tile_size)) #Scale the pieces down to the size of each square

def main():    
    '''
    Lists
    '''
    legal_moves = game.legal_move_generation() #List of all possible legal moves
    selected_tile = () #Most recent tile selected will be placed here and used throughout the code.
    tile_sequence = [] #Stores a sequence of selected_tiles
    GreenMoves = []
    legal_moves_refmt = []
    
    '''
    Flags
    '''
    get_new_legal_moves = False #when a successful move is made, use this flag to get new legal moves.
    running = False #When false, stop the game
    BlackPlayer = False #Set to true if a human is playing black (can be overwritten in LCD monitor menu)
    n = bool(0) #Boolean for drawing the board

    '''
    Startup
    '''
    while True:
        UpdateUI()

        SC.ReadSerial()
        event, value = ChessEvents.get(ChessEvents.events, ChessEvents.values)

        '''
        Setup Events
        '''
        if event == "StartGame":
            running = True
            print("Game Is Now Running")
        elif event == "Difficulty": #Depth of the AI
            AI.DEPTH = value
        elif event == "VsHuman": #PLaying against an AI or not
            BlackPlayer = value
        else:
            PrintError("No Setup Opcode Found")



        '''
        Game Begins
        '''
        while running: #GAME STARTED
            UpdateUI()
            PlayerTurn = game.Player1Move or (not game.Player1Move and BlackPlayer) #Determines that is a human's turn
            GameOver = False #Flag used to end the game

            '''
            Human Turn
            '''
            if PlayerTurn and not GameOver:
                SC.ReadSerial() #Read for events
                event, value = ChessEvents.get(ChessEvents.events, ChessEvents.values)



                '''
                AI Turn
                '''
            elif not PlayerTurn and not GameOver:

                '''
                Generates a Move
                '''
                AIMove = AI.FindBestMoveMinMax(game, legal_moves)
                if AIMove is None:
                    AIMove = AI.findRandomMove(legal_moves)
                SendAIMove = AIMove.getChessNotation() 
                #SC.WriteSerial("AIMove",str(SendAIMove)) #Sends Helen a red LED Opcode
                
                while AITurn:
                    SC.ReadSerial() #Read for events 
                    event, value = ChessEvents.get(ChessEvents.events, ChessEvents.values)
                    
                    if event == "InitialTile":
                        if not GameOver:
                            selected_tile = value
                            if selected_tile == SendAIMove[0]:
                                pass #successful process
                            else:
                                pass
                                #FREEZE IT and make them put it back down







                
                '''
                Legal Move Generation
                '''
            #If at any point a legal move is made, we must regenerate a new set of legal moves to be played.
            if get_new_legal_moves: 
                legal_moves = game.legal_move_generation()
                get_new_legal_moves = False #set back to false and await another legal move
            
            ChessEvents.LastEvent = event  #Store the LastEvent variable here.











'''
Helper Functions
'''

def PutBackDown(tile):
    print("Incorrect Piece, Put it back in it's original spot and try again") #Send some sort of indicator to board
    MoveQueue = []
    MoveQueue.append(tile)
    frozen = True
    while frozen: #function: PutBackDown
        if (len(MoveQueue) == 0):
            frozen = False
        SC.ReadSerial()
        event, value = ChessEvents.get(ChessEvents.events, ChessEvents.values)

        if event == "InitialTile":
            MoveQueue.append(value) #If they pick up another piece while holding one, add that coordinate to moves they have to undo
        if event == "NextTile":
            if value in MoveQueue:
                frozen = False      #Break out of loop
            if value != tile:
                print("wrong spot, move to the correct")


def PrintError(string):
    print("Error: " + string)

def UpdateUI():
    '''
    Drawing the Board
    '''
    window = pygame.display.set_mode((w,h))  
    #Draw the tiles and pieces manually with alternating background colours
    colors = [pygame.Color("lightslategray"),pygame.Color("Gainsboro")] #Select two Pygame default colours and create a list
    n = bool(0) #Create a boolean variable that can be flip-flopped to switch between colors in this list.
    for r in range(board_len): #For each row
        n = not(n) #Alternate color
        for c in range(board_len): #For each column
            n = not(n) #Alternate color
            pygame.draw.rect(window, "black", pygame.Rect(c*tile_size, r*tile_size, tile_size, tile_size))  #Creates a black border between each piece the size of a tile
            pygame.draw.rect(window, colors[n], pygame.Rect(c*tile_size+1, r*tile_size+1, tile_size, tile_size)) #Fills in each tile with the corresponding even or odd color tile
            piece = game.boardstate[r][c] #Grab the name of the piece on each tile from our board state. ex. "bQ", "wK"
            if piece != "..": #If it isn't an empty space
                window.blit(piece_images[piece],pygame.Rect(c*tile_size, r*tile_size, tile_size, tile_size)) #This will place the piece_images asset on the correct tile

    pygame.display.update() #This will update the UI after each event is made.

'''Standard Python convention for dealing with multiple scripts in one project. Main should be identified.'''
if __name__ == "__main__":
    main()


