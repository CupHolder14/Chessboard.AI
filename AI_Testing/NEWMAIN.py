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
    FLAGS and LISTS
    '''
    legal_moves = game.legal_move_generation() #List of all possible legal moves
    get_new_legal_moves = False #when a successful move is made, use this flag to get new legal moves.
    running = False #When false, stop the game
    selected_tile = () #Most recent tile selected will be placed here and used throughout the code.
    tile_sequence = [] #Stores a sequence of selected_tiles
    #WhitePlayer = True #Set to true if a human is playing white (Not Used, set white to default)
    BlackPlayer = False #Set to true if a human is playing black (can be overwritten in LCD monitor menu)
    GreenMoves = []
    legal_moves_refmt = []

    n = bool(0) #Boolean for drawing the board
    '''
    Read from serial looking for setup events (Difficulty, vsHuman, STARTGAME)
    If event = difficulty, or vsHuman: change those variables in the game code
    If event = STARTGAME: Move to game loop where we will search for move events
    When the game is done (event = Quit): Set running = false, and go back to main while loop until next game starts
    '''
    while True:
        UpdateUI()

        SC.ReadSerial()
        #print("Events Queue:" + str(ChessEvents.events))
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
            Ingame Events
            '''
        while running: #GAME STARTED
            UpdateUI()
            PlayerTurn = game.Player1Move or (not game.Player1Move and BlackPlayer) #Determines that is a human's turn
            GameOver = False #Flag used to end the game

            '''
            READ AT EVERY LOOP FOR EVENTS
            '''
            if PlayerTurn and not GameOver:
                SC.ReadSerial() #Read for events
                #print("Events Queue:" + str(ChessEvents.events))
                event, value = ChessEvents.get(ChessEvents.events, ChessEvents.values)

                if event == "TimeOut": #CHANGE GameOver to TimeOut or something
                    running = False
                    #Send the not player's turn to the Arduino to designate the winner
                    SC.WriteSerial("Winner",str(not game.Player1Move)) #Sends Majed Winner Variable

                elif event == "Quit":  #If we're quitting, then just stop it entirely
                    print(value)
                    running = False
                #InitialTile:(2,2)
                elif event == "InitialTile" and ChessEvents.LastEvent != "InitialTile": #If a tile is picked up that is the same as "MouseButtonDown in PyGame"
                    if PlayerTurn and not GameOver and (len(tile_sequence) == 0): 
                        selected_tile = value
                        print("Selected Tile:" + str(selected_tile))
                        tile_sequence.append(selected_tile)

                        '''GREEN MOVE SCANNER'''
                        for i in legal_moves:
                            if i.getChessNotation() not in legal_moves_refmt:
                                legal_moves_refmt.append(i.getChessNotation())
                            #print(i.getChessNotation())
                        for j in legal_moves_refmt:
                            if j[0] == selected_tile:
                                GreenMoves.append(j[1])
                        SC.WriteSerial("LegalMoves:",str(SendMove)) #Sends Helen a Green LED Opcode at those positions
                        GreenMoves = []

                    else:
                        PrintError("InitialTile Error") #If there is any issue, just print an error for diagnosis

                elif event == "NextTile":
                    if PlayerTurn and not GameOver and (len(tile_sequence) != 0):
                        selected_tile = value
                        if selected_tile == tile_sequence[0]: #We want to store our row and col values, if they are already being stored, then it means the same square was clicked twice.
                            selected_tile = () #If we pick up a piece and put it back down, flush the tuple
                            tile_sequence = [] #Our tile sequence should be reset as well
                        else:
                            tile_sequence.append(selected_tile) #Tile sequence is 2 at this point always
                            print("Tile Sequence:" + str(tile_sequence))
                            move = engine.Move(tile_sequence[0],tile_sequence[1],game.boardstate)
                            
                            #Check if the move sent to tile_sequence was a legal move or not!
                            for i in range(len(legal_moves)):
                                if move == legal_moves[i]:
                                    print("LEGAL MOVE")
                                    game.make_move(legal_moves[i]) #If it's a legal move, make it!
                                    #print(selected_tile)
                                    if game.in_check(): 
                                        print('Check!')   
                                        SC.WriteSerial("Check:","True") #Sends Majed Check Flag
                                    selected_tile = () #reset selected_tile if it was a legal move
                                    tile_sequence = [] #reset tile_sequence if it was a legal move
                                    get_new_legal_moves = True #set our flag to true so we can recalculate new legal moves for that player
                            if not get_new_legal_moves: #If a legal move isn't passed, then the flag is never activated
                                print("ILLEGAL MOVE")
                                tile_sequence = [tile_sequence[0]] #if a valid move was not made, our program will ignore the second click and retain the first click. (purely for improving UI experience)
                                SC.WriteSerial("IllegalMove",str(selected_tile)) #Sends Helen a red LED Opcode
                                #Ignore the next "InitialTile" event?
                    else:
                        PrintError("Select an Initial Tile First!")

                #elif event == "ResetGame":  NOT USED ANYMORE

                '''
                AI.py Integrates Here
                '''
            elif not PlayerTurn and not GameOver:
                AIMove = AI.FindBestMoveMinMax(game, legal_moves)
                if AIMove is None:
                    AIMove = AI.findRandomMove(legal_moves)
                game.make_move(AIMove)

                SendMove = AIMove.getChessNotation() 
                SC.WriteSerial("AIMove",str(SendMove)) #Sends Helen a red LED Opcode

                if game.in_check(): 
                    print('Check!')
                    SC.WriteSerial("Check:","True") #Sends Majed Check Flag
                get_new_legal_moves = True


                '''
                Legal Move Generation
                '''
            #If at any point a legal move is made, we must regenerate a new set of legal moves to be played.
            if get_new_legal_moves: 
                legal_moves = game.legal_move_generation()
                get_new_legal_moves = False #set back to false and await another legal move
            
            ChessEvents.LastEvent = event  #Store the LastEvent variable here.


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

