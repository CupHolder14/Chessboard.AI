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



def main():
    '''
    LOAD PIECE ASSETS INTO UI
    '''
    piece_names = ["bR", "bN", "bB", "bQ", "bK","bP","wR", "wN", "wB", "wQ", "wK","wP"]
    piece_images = {} #Initialize dictionary that will hold the png files
    for i in piece_names: 
        piece_images[i] = pygame.image.load("png_pieces/" + i + ".png") #Load all the piece images with the corresponding piece_names into your dictionary
        piece_images[i] = pygame.transform.scale(piece_images[i],(tile_size,tile_size)) #Scale the pieces down to the size of each square

    '''
    PyGame Window
    '''
    window = pygame.display.set_mode((w,h))  
    
    '''
    FLAGS and LISTS
    '''
    legal_moves = game.legal_move_generation() #List of all possible legal moves
    get_new_legal_moves = False #when a successful move is made, use this flag to get new legal moves.
    running = False #When false, stop the game
    selected_tile = () #Most recent tile selected will be placed here and used throughout the code.
    tile_sequence = [] #Stores a sequence of selected_tiles
    WhitePlayer = True #Set to true if a human is playing white
    BlackPlayer = False #Set to true if a human is playing black (can be overwritten in LCD monitor menu)
    '''
    Read from serial looking for setup events (Difficulty, vsHuman, STARTGAME)
    If event = difficulty, or vsHuman: change those variables in the game code
    If event = STARTGAME: Move to game loop where we will search for move events
    When the game is done (event = Quit): Set running = false, and go back to main while loop until next game starts
    '''
    while True:
        SC.ReadSerial()
        event, value = ChessEvents.get(ChessEvents.events, ChessEvents.values)
        
        '''
        Setup Events
        '''
        if (event.type == pygame.QUIT):
            running = False #This will end the loop.
        if event == "StartGame":
            running = True
        elif event == "Difficulty":
            AI.DEPTH = value
        elif event == "VsHuman":
            BlackPlayer = value
        else:
            PrintError("No Inital Identifier Found")

            '''
            Ingame Events
            '''
        while running: #GAME STARTED
            PlayerTurn = game.Player1Move or (not game.Player1Move and BlackPlayer) #Determines that is a human's turn
            GameOver = False #Flag used to end the game


            '''
            THIS IS HOW WE SEND THE WINNER TO MAJED
            '''
            # if game.checkmate or game.stalemate:
            #     GameOver = True
            #     if game.stalemate: #Stalemate
            #         Winner = 2
            #     elif game.Player1Move: #Black wins!
            #         Winner = 1
            #     else:
            #         Winner = 0 #White wins!
            #     print(Winner) #To stop it from printing indefinitely consider using a flag variable external to the loop
            #     PrintWinner = True

            '''
            READ AT EVERY LOOP FOR EVENTS
            '''
            SC.ReadSerial() #Read for events
            event, value = ChessEvents.get(ChessEvents.events, ChessEvents.values)

            if event == "GameOver": #CHANGE GameOver to TimeOut or something
                running = False
                #Send the not player's turn to the Arduino to designate the winner
            elif event == "Quit":  #If we're quitting, then just stop it entirely
                running = False
            elif event == "InitialTile" and ChessEvents.LastEvent != "InitialTile": #If a tile is picked up that is the same as "MouseButtonDown in PyGame"
                if PlayerTurn and not GameOver and (len(tile_sequence) == 0): 
                    row = value[0]
                    col = value[1]
                    selected_tile = (row,col)
                    tile_sequence.append(selected_tile)
                    #Send all legal moves to Helen based on this first move
                else:
                    PrintError("InitialTile Error")
            elif event == "NextTile":
                if PlayerTurn and not GameOver and (len(tile_sequence) != 0):
                    row = value[0]
                    col = value[1]
                    selected_tile = (row,col)
                    if selected_tile == tile_sequence[0]: #We want to store our row and col values, if they are already being stored, then it means the same square was clicked twice.
                        selected_tile = () #If we pick up a piece and put it back down, flush the tuple
                        tile_sequence = [] #Our tile sequence should be reset as well
                    else:
                        tile_sequence.append(selected_tile) #Tile sequence is 2 at this point always
                        move = engine.Move(tile_sequence[0],tile_sequence[1],game.boardstate)
                        #print(move.getChessNotation())
                        
                        #Check if the move sent to tile_sequence was a legal move or not!
                        for i in range(len(legal_moves)):
                            if move == legal_moves[i]:
                                game.make_move(legal_moves[i]) #If it's a legal move, make it!
                                #print(selected_tile)
                                if game.in_check(): 
                                    print('Check!')   
                                    #Send Check flag to Majed            
                                selected_tile = () #reset selected_tile if it was a legal move
                                tile_sequence = [] #reset tile_sequence if it was a legal move
                                get_new_legal_moves = True #set our flag to true so we can recalculate new legal moves for that player
                        if not get_new_legal_moves: #If a legal move isn't passed, then the flag is never activated
                            tile_sequence = [tile_sequence[0]] #if a valid move was not made, our program will ignore the second click and retain the first click. (purely for improving UI experience)
                            #SC.WriteSerial("IllegalMove",str(selected_tile)) #Sends Helen a red LED Opcode
                            #Ignore the next "InitialTile" event?
                else:
                    PrintError("NextTile Error")

            #elif event == "ResetGame":  NOT USED ANYMORE

                '''
                AI.py Integrates Here
                '''
            if not PlayerTurn and not GameOver:
                AIMove = AI.FindBestMoveMinMax(game, legal_moves)
                if AIMove is None:
                    AIMove = AI.findRandomMove(legal_moves)
                game.make_move(AIMove)
                if game.in_check(): 
                    print('Check!')
                    #Send "Check message to Majed"
                get_new_legal_moves = True
                #print(AIMove)


                '''
                Legal Move Generation
                '''
            #If at any point a legal move is made, we must regenerate a new set of legal moves to be played.
            if get_new_legal_moves: 
                legal_moves = game.legal_move_generation()
                get_new_legal_moves = False #set back to false and await another legal move
            

                '''
                Drawing the Board
                '''
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

            ChessEvents.LastEvent = event  #Store the LastEvent variable here.


def PrintError(string):
    print("Error: " + string)

'''Standard Python convention for dealing with multiple scripts in one project. Main should be identified.'''
if __name__ == "__main__":
    main()

