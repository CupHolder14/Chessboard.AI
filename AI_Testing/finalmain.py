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
    tile_sequence = [] #Stores a sequence of selected_tiles

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
            # a = ((0,0))
            # for i in range (0,8): 
            #     for j in range(0,8):
            #        a = a + ((i,j),)
            # SC.WriteSerial("LegalMoves:",str([a[:18]])) #Sends a Green LED Opcode at those positions
            # SC.WriteSerial("IllegalMove:",str([a[18:34]])) #Sends a Green LED Opcode at those positions
            # SC.WriteSerial("AIMove:",str([a[34:50]])) #Sends a Green LED Opcode at those positions
            # SC.WriteSerial("LegalMoves:",str([a[50:66]])) #Sends a Green LED Opcode at those positions
            # SC.WriteSerial("TurnOff:","True")
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
                if ChessEvents.events:
                    event, value = ChessEvents.get(ChessEvents.events, ChessEvents.values)
                else:
                    SC.ReadSerial() #Read for events
                    event, value = ChessEvents.get(ChessEvents.events, ChessEvents.values)
                print('event='+ str(event))
                print('value=' + str(value))
                if event == "TimeOut":
                    running = False
                    game.checkmate = True #End the game
                    GameOver = True
                elif event == "Quit":  #If we're quitting, then just stop it entirely
                    print(value)
                    running = False
                    break

                #Process a Move
                elif event == "InitialTile": #If a tile is picked up that is the same as "MouseButtonDown in PyGame"
                    if ChessEvents.LastEvent == "InitialTile": #If we pick up two pieces in a row, only allow a take
                        if value in [LegalMovesRefmt[i][1] for i in range(len(LegalMovesRefmt))]: #If the second piece that is picked up is a take, then allow it
                            print("Piece Taken Successfully")
                        else:
                            print(value)
                            PutBackDown(value) #If two pieces are picked up in a row, it's not allowed, put the 2nd one back
                            SC.WriteSerial("LegalMoves:",str(GreenMoves)) #Sends a Green LED Opcode at those positions
                    else:
                        GreenMoves, LegalMovesRefmt = GreenMoveScanner(value, legal_moves)
                        if value in [LegalMovesRefmt[i][0] for i in range(len(LegalMovesRefmt))]: #If it was a legal move:
                            tile_sequence.append(value)
                            SC.WriteSerial("LegalMoves:",str(GreenMoves)) #Sends a Green LED Opcode at those positions
                            print("Legal Piece " + str(tile_sequence))
                        else:
                            PutBackDown(value) #If you pick up a wrong piece, you will be forced to put it back down before continuing
                        
                elif event == "NextTile":
                    if value == tile_sequence[0]: #The piece was placed in the same position
                        print("Piece successfully re-placed, select another tile")
                        tile_sequence = [] #Our tile sequence should be reset as well
                        SC.WriteSerial("TurnOff:","True")
                    else: 
                        GreenMoves, LegalMovesRefmt = GreenMoveScanner(tile_sequence[0], legal_moves)
                        if value in [GreenMoves[i] for i in range(len(GreenMoves))]: #if the 2nd tile is a legal move, allow it to be processed
                            tile_sequence.append(value)
                            print("Tile Sequence:" + str(tile_sequence))
                            move = engine.Move(tile_sequence[0],tile_sequence[1],game.boardstate)
                            game.make_move(move) #If it's a legal move, make it!
                            if game.in_check(): 
                                print('Check!')   
                                SC.WriteSerial("Check:","True") #Sends Majed Check Flag        
                            tile_sequence = []
                            get_new_legal_moves = True #set our flag to true so we can recalculate new legal moves for that player
                            AITurn = True      
                            SC.WriteSerial("TurnOff:","True")          
                        else: #If they put it in an illegal spot, force them to put it back in the original spot
                            IllegalMove = []
                            IllegalMove.extend((tile_sequence[0],value))
                            SC.WriteSerial("IllegalMove:",str(IllegalMove)) #Sends Illegal Moves to light up red
                            PutBackDown(tile_sequence[0], value)
                            tile_sequence = []
                else:
                    PrintError("Opcode Error")
                ChessEvents.LastEvent = event  #Store the LastEvent variable here. 
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
                SC.WriteSerial("AIMove:",str(SendAIMove)) #Sends Helen a yellow LED Opcode
                print(SendAIMove)

                '''
                Verify the move is made
                '''
                while AITurn:
                    if ChessEvents.events:
                        event, value = ChessEvents.get(ChessEvents.events, ChessEvents.values)   
                    else:                    
                        SC.ReadSerial() #Read for events 
                        event, value = ChessEvents.get(ChessEvents.events, ChessEvents.values)    
                    print('event ='+ str(event))
                    print('value=' + str(value))
                    if event == "Quit":  #If we're quitting, then just stop it entirely
                        print(value)
                        AITurn = False
                        break
                    if event == "InitialTile":
                        if not GameOver:
                            if value == SendAIMove[0]:
                                print("Now Place In Correct Spot")
                            elif value == SendAIMove[1]: #Take
                                print("Processing Take")
                            else:
                                PutBackDown(value) #Freeze game and make them put it back down
                                print("Board State Fixed, AIMove:" + str(SendAIMove))
                                SC.WriteSerial("AIMove:",str(SendAIMove)) #Sends Helen a yellow LED Opcode

                    elif event == "NextTile":
                        if not GameOver:
                            if value == SendAIMove[1]:   #Move made in line with AI's decision
                                game.make_move(AIMove)
                                AITurn = False
                                SC.WriteSerial("TurnOff:","True")
                            else:
                                PutBackDown(SendAIMove[1],value) 
                                print("Well Done")
                                game.make_move(AIMove)
                                AITurn = False
                            UpdateUI() #Optional?
                if game.in_check(): 
                    print('Check!')
                    SC.WriteSerial("Check:","True") #Sends Majed Check Flag
                get_new_legal_moves = True         

                '''
                Legal Move Generation
                '''
            #If at any point a legal move is made, we must regenerate a new set of legal moves to be played.
            if get_new_legal_moves: 
                print('turn finished, generating new legal moves')
                legal_moves = game.legal_move_generation()
                get_new_legal_moves = False #set back to false and await another legal move
                ChessEvents.events = []
                ChessEvents.values = []
                ChessEvents.LastEvent = []
                event = None
                value = None
            '''
            Game Over Check
            '''
        if game.checkmate or game.stalemate:  #Flag for if the game is over
            if game.stalemate: #Stalemate
                Winner = 2
                SC.WriteSerial("Winner:","["+str(Winner)+"]")
            elif game.Player1Move: #Black wins!
                Winner = 1
                SC.WriteSerial("Winner:","["+str(Winner)+"]")
            else:
                Winner = 0 #White wins!
                SC.WriteSerial("Winner:","["+str(Winner)+"]") 
            print(Winner)

'''
Helper Functions
'''

def GreenMoveScanner(InitialTile, LegalMoves):
    '''
    Given an initial tile and a list of all legal moves, parse a list of all of the legal moves
    '''
    LegalMovesRefmt = []
    GreenMoves = []
    for i in LegalMoves:
        if i.getChessNotation() not in LegalMovesRefmt:
            LegalMovesRefmt.append(i.getChessNotation()) #All legal moves with respect to the initial tile
    for j in LegalMovesRefmt:
        if j[0] == InitialTile:
            GreenMoves.append(j[1])                      #All legal moves' second position
    return GreenMoves, LegalMovesRefmt

def PutBackDown(OriginalTile, WrongTile = None):
    '''
    Force a Player to put a piece back down in it's original spot
    '''
    MoveQueue = []
    MoveQueue.append(OriginalTile)
    WrongNextTiles = [] 
    if WrongTile:
        WrongNextTiles.append(WrongTile)
    while True:
        if (len(MoveQueue) == 0) and (len(WrongNextTiles) == 0): 
            #Only error is if someone makes the move while holding another piece, they will have to pick up the move piece and put it back down
            print("exiting put back down")
            SC.WriteSerial("TurnOff:","True")
            event = None
            value = None
            break
        else:
            SC.WriteSerial("IllegalMove:",str(MoveQueue+WrongNextTiles))
            print("Re-place pieces:" + str(MoveQueue+WrongNextTiles))
        SC.ReadSerial(False)
        event, value = ChessEvents.get(ChessEvents.events, ChessEvents.values)
        ChessEvents.LastEvent = event  #Store the LastEvent variable here.
        print(ChessEvents.LastEvent)
        if event == "InitialTile" and value not in list(WrongNextTiles):
            MoveQueue.append(value) #If they pick up another piece while holding one, add that coordinate to moves they have to undo
        elif event == "InitialTile" and value in list(WrongNextTiles):
            WrongNextTiles.remove(value) #Ignore the pickup and remove it from the list of WrongNextTiles
        elif event == "NextTile":
            if value in MoveQueue:
                MoveQueue.remove(value) #All is good, piece back in it's original spot
                print("Piece: " + str(value) + " returned to it's original spot")
            else:
                WrongNextTiles.append(value)
                print("wrong spot, move to the correct spot")


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