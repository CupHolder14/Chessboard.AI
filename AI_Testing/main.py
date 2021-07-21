'''
Temporary UI simply for purposes. Also handles high level movements from engine.py. 
Note: User Interface open source (Pygame) material included in this section will not be necessary for the final physical chessboard, and are
simply being used for preliminary testing purposes. 
'''

'''
Imports
'''
import SerialCommunication as SC
import ChessEvents
import engine, AI
game = engine.Game() #Name a variable to access the GameState class in our engine function
import pygame #Pygame simplifies the UI creation process for us, as this is just a temporary part of the project.
pygame.init() #initialize pygame

'''
Board Size UI Variables
'''
board_len = 8 #8x8 chess board 
w = h = 512 #UI pixel dimensions. 512 is divisible by 8 (8x8 board).
tile_size = int(w/board_len) #Pixel size of each tile for shaping purposes

'''
Main Function for UI and temporary "mouse click" user inputs. Also handles some moving operations.
'''
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
    legal_moves = game.legal_move_generation()
    get_new_legal_moves = False #when a successful move is made, use this flag to get new legal moves.
    running = True
    PrintBoard = False
    PrintWinner = False
    PrintMoves = False #JUST USE GET_NEW_LEGAL_MOVES
    selected_tile = () #Most recent tile selected will be placed here and used throughout the code.
    tile_sequence = [] #Stores a sequence of selected_tiles
    potential_moves = [] #List of moves sent to Helen when a piece is picked up


    '''
    SET THE GAME MODE (PvAI OR PvP)
    '''
    WhitePlayer = True #Set to true if a human is playing white
    
    BlackPlayer = False #Set to true if a human is playing black


    '''MAIN LOOP'''
    while running:
        PlayerTurn = (game.Player1Move and WhitePlayer) or (not game.Player1Move and BlackPlayer)  #If it's not a player's turn, we will disable inputs from the human
        GAMEOVER = False

        # '''
        # Print legal_moves At Each Move
        # '''
        # if not PrintMoves:
        #     #print(legal_moves) "MOVE object", needs converting first
        #     print()
        #     print("LEGAL MOVES FOR " + ("WHITE:" if game.Player1Move else "BLACK:"))
        #     for move in legal_moves:
        #         print(move.getChessNotation())
        #     print()
        #     PrintMoves = True

        # '''
        # Print BoardState At Each Move
        # '''
        # if not PrintBoard:  
        #     for line in game.boardstate:
        #         print(line)
        #     print()
        #     PrintBoard = True
        
        '''
        Print Winner at end of game
        '''
        if game.checkmate or game.stalemate and not PrintWinner:  #Flag for if the game is over
            GAMEOVER = True
            if game.stalemate: #Stalemate
                Winner = 2
            elif game.Player1Move: #Black wins!
                Winner = 1
            else:
                Winner = 0 #White wins!
            print(Winner) #To stop it from printing indefinitely consider using a flag variable external to the loop
            PrintWinner = True
            
        '''
        Event Handler 
        '''
        for event in ChessEvents.events:
            
            #### TAKE THIS LIST OF EVENTS AND REDIRECT THE EVENTS ACCORDING TO WHAT NEEDS TO HAPPEN NEXT.
            pass

        for event in pygame.event.get(): #grabs each event in a queue
            if (event.type == pygame.QUIT): # or QUIT: #Pygame default quitting handler (i.e. closing the application in any way will trigger this.) 
                running = False #This will end the loop.

                '''
                Mouse Events 
                '''
            
            elif event.type == pygame.MOUSEBUTTONDOWN: #This is True when any part of the UI is clicked once.

                if PlayerTurn and not GAMEOVER:
                    x_y_loc = pygame.mouse.get_pos() #This takes the immediate (x,y) location of the mouse after clicking
                    col = int(x_y_loc[0]/tile_size) #The row and columns can be determined by dividing by the tile size.
                    row = int(x_y_loc[1]/tile_size)
                    
                    if selected_tile == (row,col): #We want to store our row and col values, if they are already being stored, then it means the same square was clicked twice.
                        selected_tile = () #Since it's a double click, we want to empty selected_tile in order to undo the first click.
                        tile_sequence = [] #Our tile sequence should be reset as well
                        print(selected_tile)
                    else:
                        selected_tile = (row,col)
                        tile_sequence.append(selected_tile) # If it was a unique click, we store that coordinate in a tuple
                    # for move in legal_moves:
                    #     if str(move.getChessNotation()[:2]) == "a2":
                    #         potential_moves = move
                    # print(potential_moves.getChessNotation())
                    
                    if len(tile_sequence) == 2: #after we store 2 unique clicks, a path has been created to successfully make a move. 
                        move = engine.Move(tile_sequence[0],tile_sequence[1],game.boardstate)
                        #print(move.getChessNotation())
                        
                        #Check if the move sent to tile_sequence was a legal move or not!
                        for i in range(len(legal_moves)):
                            if move == legal_moves[i]:
                                game.make_move(legal_moves[i]) #If it's a legal move, make it!
                                #print(selected_tile)
                                PrintBoard = False
                                if game.in_check(): 
                                    print('Check!')                                

                                selected_tile = () #reset user clicks if it was a legal move
                                tile_sequence = [] #only reset if it was a legal move
                                get_new_legal_moves = True #set our flag to true so we can recalculate new legal moves
                        if not get_new_legal_moves: 
                            #MIGHT NEED TO ALTER THIS BASED ON THE PHYSICAL ENVIRONMENT
                            tile_sequence = [tile_sequence[0]] #if a valid move was not made, our program will ignore the second click and retain the first click. (purely for improving UI experience)

                '''
                Keyboard Events
                '''
            elif event.type == pygame.KEYDOWN:
                if event.key == pygame.K_BACKSPACE: #for now, if backspace is pressed in game, undo will be called. This will later be mapped to a physical button on the board
                    if len(game.Log) == 0: #if there are no moves in our log to undo, then ignore this key handle.
                        pass
                    else:
                        game.undo_move() #run the undo function
                        get_new_legal_moves = True 
                        GAMEOVER = False
                # if event.key == pygame.K_r: #reset the game when r is pressed. THIS NEEDS TO BE FIXED.
                #     game = engine.Game()
                #     legal_moves = game.legal_move_generation()
                #     selected_tile = ()
                #     tile_sequence = []
                #     GAMEOVER = False    
                

            '''
            AI.py Integrates Here
            '''
        if not PlayerTurn and not GAMEOVER:
            AIMove = AI.FindBestMoveMinMax(game, legal_moves)
            if AIMove is None:
                AIMove = AI.findRandomMove(legal_moves)
            game.make_move(AIMove)
            PrintBoard = False
            if game.in_check(): 
                print('Check!')
            get_new_legal_moves = True
            #print(AIMove)


            '''
            Legal Move Generation
            '''
        #If at any point a legal move is made, we must regenerate a new set of legal moves to be played.
        if get_new_legal_moves: 
            legal_moves = game.legal_move_generation()
            PrintMoves = False
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


'''Standard Python convention for dealing with multiple scripts in one project. Main should be identified.'''
if __name__ == "__main__":
    main()