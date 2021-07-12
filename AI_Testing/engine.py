'''
This is our game engine file. It handles move generation and special rules. It will output board states for the AI to accept, 
as well as possible move highlighting for the microcontroller and physical board to interpret.
'''

class Game():
    '''
    Keeps track of the board state, all possible moves to be made, and all moves already made. 
    '''
    def __init__(self): #Constructor
        '''List of lists that contains each row of the board. Board is initialized as shown.
        b or w = colour,
        'R', 'N', 'B', 'Q', 'K', 'P' = type of piece,
        '..' = empty space (no piece on that square)
        '''
        self.boardstate = [
            ["bR", "bN", "bB", "bQ", "bK", "bB", "bN", "bR"],
            ["bP", "bP", "bP", "bP", "bP", "bP", "bP", "bP"],
            ["..", "..", "..", "..", "..", "..", "..", ".."],
            ["..", "..", "..", "..", "..", "..", "..", ".."],
            ["..", "..", "..", "..", "..", "..", "..", ".."],
            ["..", "..", "..", "..", "..", "..", "..", ".."],
            ["wP", "wP", "wP", "wP", "wP", "wP", "wP", "wP"],
            ["wR", "wN", "wB", "wQ", "wK", "wB", "wN", "wR"]]

        '''Board State Conditions and variables'''
        self.Log = [] #This will be a log to keep track of every move made
        self.piece_rules = {'P':self.pawn_rules,'R':self.rook_rules,'N':self.knight_rules, 
                            'B':self.bishop_rules,'Q':self.queen_rules,'K':self.king_rules} #Dictionary to make it easier to reference the rules functions written below.       
        self.Player1Move = True #Player 1 is white. The AI will always be Black for simplicity's sake. This variable will keep track of who's turn it is.
        self.checkmate = False #we want to set checkmate and stalemate variables. By default they should be set to a boolean false.
        self.stalemate = False
        self.white_king_loc = (7,4) #We want to keep track of where the kings are at at all times, in order to anticipate and register any checks/checkmates
        self.black_king_loc = (0,4)


        self.currentCastlingRight = CastleRights(True,True,True,True)
        self.CastlingRightsLog = [CastleRights(self.currentCastlingRight.wks, self.currentCastlingRight.bks, self.currentCastlingRight.wqs, self.currentCastlingRight.bqs)]

    '''Make a move function'''
    def make_move(self, move): 
        self.boardstate[move.end_tile[0]][move.end_tile[1]] = move.start_piece_name #Replace end square with the piece we moved
        self.boardstate[move.start_tile[0]][move.start_tile[1]] = ".." #Replace start square with an empty block
        self.Log.append(move) #Add the move to our log. Necessary for "undo" to work.
        self.Player1Move = not self.Player1Move #After the move is made, swap turn state.

        #We want to keep track of the king's location at all times.
        if move.start_piece_name == 'wK':
            self.white_king_loc = (move.end_tile[0], move.end_tile[1])
        elif move.start_piece_name == 'bK':
            self.black_king_loc = (move.end_tile[0], move.end_tile[1])

        #Pawn Promotion
        if move.PawnPromotion:
            self.boardstate[move.end_tile[0]][move.end_tile[1]] = move.start_piece_name[0] + 'Q'
        #Castling - whenever a rook or king moves for the first time.
        if move.isCastleMove:
            if move.end_tile[1] - move.start_tile[1] == 2: #kingside castle move
                self.boardstate[move.end_tile[0]][move.end_tile[1]-1] = self.boardstate[move.end_tile[0]][move.end_tile[1] + 1] #moves rook
                self.boardstate[move.end_tile[0]][move.end_tile[1]+1] = '..' #erase old rook
            else: #queenside castle
                self.boardstate[move.end_tile[0]][move.end_tile[1]+1] = self.boardstate[move.end_tile[0]][move.end_tile[1] - 2] #moves rook
                self.boardstate[move.end_tile[0]][move.end_tile[1]-2] = '..' #erase old rook
        self.updateCastleRights(move)
        self.CastlingRightsLog.append(CastleRights(self.currentCastlingRight.wks, self.currentCastlingRight.bks, self.currentCastlingRight.wqs, self.currentCastlingRight.bqs))


    '''Undo last move function. Note: A physical button on the board may be required if we choose to take this approach. For now, it works in the UI.'''
    def undo_move(self):
        move = self.Log.pop() #remove last element from move log list and remporarily stores it in 'move'.
        self.boardstate[move.start_tile[0]][move.start_tile[1]] = move.start_piece_name #Replace preivious start square with the piece we moved
        self.boardstate[move.end_tile[0]][move.end_tile[1]] = move.end_piece_name #Replace previous end square with the piece we covered. (potentially empty)
        self.Player1Move = not self.Player1Move #We must switch the turns back.

        #We want to keep track of the king's location at all times, when we undo a move we want to revert king's positions.
        if move.start_piece_name == 'wK':
            self.white_king_loc = (move.start_tile[0], move.start_tile[1])
        elif move.start_piece_name == 'bK':
            self.black_king_loc = (move.start_tile[0], move.start_tile[1])

        #undo castling rights
        self.CastlingRightsLog.pop()
        newRights = self.CastlingRightsLog[-1]
        self.currentCastlingRight = CastleRights(newRights.wks,newRights.bks,newRights.wqs,newRights.bqs)

        if move.isCastleMove:
            if move.end_tile[1] - move.start_tile[1] == 2: #kingside
                self.boardstate[move.end_tile[0]][move.end_tile[1]+1]= self.boardstate[move.end_tile[0]][move.end_tile[1]-1]
                self.boardstate[move.end_tile[0]][move.end_tile[1]-1]= '..'
            else:
                self.boardstate[move.end_tile[0]][move.end_tile[1]-2]= self.boardstate[move.end_tile[0]][move.end_tile[1]+1]
                self.boardstate[move.end_tile[0]][move.end_tile[1]+1]= '..'
        self.checkmate = False
        self.stalemate = False

    def updateCastleRights(self, move):
        if move.start_piece_name == 'wK':
            self.currentCastlingRight.wks = False
            self.currentCastlingRight.wqs = False
        elif move.start_piece_name == 'bK':
            self.currentCastlingRight.bks = False
            self.currentCastlingRight.bqs = False
        elif move.start_piece_name == 'wR':
            if move.start_tile[0] == 7:
                if move.start_tile[1] == 0: #left rook
                    self.currentCastlingRight.wqs = False
                elif move.start_tile[1] == 7: #right rook
                    self.currentCastlingRight.wks = False
        elif move.start_piece_name == 'bR':
            if move.start_tile[0] == 0:
                if move.start_tile[1] == 0: #left rook
                    self.currentCastlingRight.bqs = False
                elif move.start_tile[1] == 7: #right rook
                    self.currentCastlingRight.bks = False          


    '''Check our list of all possible moves and determine which ones are legal to make. Illegal moves are moves that 
    will put the player's king in check or moves that simply do not follow the set rules for each piece.
    The algorithm we came accross to completing this essentially attempts to perform each move, checks if it puts the player's 
    king in check, and if not, adds it as a possible move.'''
    def legal_move_generation(self):
        tempCastleRights = CastleRights(self.currentCastlingRight.wks, self.currentCastlingRight.bks, self.currentCastlingRight.wqs, self.currentCastlingRight.bqs) #copy current castling rights
        
        moves = self.all_moves_generation() #First we generate all possible moves.
        
        #Removing moves that put you in checkmate from the possible moves list:
        for i in range(len(moves)-1,-1,-1): #Start at the end of the list, and remove any invalid moves. (Start at the end to avoid indexing issues in Python)
            self.make_move(moves[i])  #Attempt to make the move
            self.Player1Move = not self.Player1Move #For the moment, switch them player's turn in order to see the opponent's moves
            if self.in_check(): #If any of these moves puts the original player is in check, remove the move from the legal list
                moves.remove(moves[i])
            self.Player1Move = not self.Player1Move #Switch back the turn
            self.undo_move() #Undo the move that we just made

        #If there are no moves to be made, then the player is in either checkmate or stalemate.
        if len(moves) == 0:
            if self.in_check(): #If in check and there are no other moves to make, it's checkmate.
                self.checkmate = True
                print('Checkmate!')
            else: #If not in check and there are no other moves to make, it's stalemate.
                self.stalemate = True
                print('Stalemate!') 
        else: 
            self.checkmate = False
            self.stalemate = False



        if self.Player1Move:
            self.getCastleMoves(self.white_king_loc[0],self.white_king_loc[1],moves)
        else:
            self.getCastleMoves(self.black_king_loc[0],self.black_king_loc[1],moves)
        self.currentCastlingRight = tempCastleRights #reset to temp rights.
        return moves


    def in_check(self):
        #Using the square_being_attacked function, we can test if our king's location that we are tracking at any given time is being attacked.
        if self.Player1Move: #If king's location is being attacked, True is output, else false.
            return self.square_being_attacked(self.white_king_loc[0],self.white_king_loc[1])
        else:
            return self.square_being_attacked(self.black_king_loc[0],self.black_king_loc[1])

    
    def square_being_attacked(self,r,c):
        #A function to see if any particular square is under attack. Will be used for checks!
        self.Player1Move = not self.Player1Move #switch to opponent's point of view in order to see their moves
        opponents_moves = self.all_moves_generation() #now that it is the opponent's POV, generate all of their moves
        self.Player1Move = not self.Player1Move #switch back to the original player's turn
        for move in opponents_moves: #if any of the moves are attacking r,c then it is True
            if move.end_tile[0] == r and move.end_tile[1] == c: 
                return True
        return False



    '''For each player's turn, we will generate every possible move they can make. This is key for understanding positional and future
    conditions like checks and checkmates, and will also be necessary for the AI.'''
    def all_moves_generation(self):
        moves = [] #All possible moves for the current player's turn will be stored in this list. 
        for r in range(8): #number of rows
            for c in range(len(self.boardstate[r])): #number of cols in given row
                piece_color,piece_type = self.boardstate[r][c] #Takes the first identifier of the piece name, i.e. 'b','w' or '.' as color and the second as type
                if (piece_color == 'w' and self.Player1Move) or (piece_color == 'b' and not self.Player1Move): #If the piece idenfitied is the same as the player's turn
                    self.piece_rules[piece_type](r,c,moves) #Redirect our piece_type to each respective piece_rules function, where things can get more specific.
        return moves                                                    


    '''Define the different ways a pawn can move on the board'''    
    def pawn_rules(self,r,c,moves):
        if self.Player1Move: #White's turn
            if self.boardstate[r-1][c] == "..": #If the square ahead of the white pawn is empty
                moves.append(Move((r,c),(r-1,c), self.boardstate)) #Add this as a possible move
                if r==6 and self.boardstate[r-2][c] == "..": #If the square two tiles ahead of the white pawn is empty and it's sitting in it's original tile
                    moves.append(Move((r,c),(r-2,c), self.boardstate)) #Add this as a possible move
            if c-1 >= 0: #Specify range of the board
                if self.boardstate[r-1][c-1][0] == "b": #If a black piece is on the left, capture is available
                    moves.append(Move((r,c),(r-1,c-1), self.boardstate))
            if c+1 <= 7: 
                if self.boardstate[r-1][c+1][0] == "b": #If a black piece is on the right, capture is available
                    moves.append(Move((r,c),(r-1,c+1), self.boardstate))

        else: #Black's turn
            if self.boardstate[r+1][c] == "..": #If the square ahead of the black pawn is empty
                moves.append(Move((r,c),(r+1,c), self.boardstate)) #Add this as a possible move
                if r==1 and self.boardstate[r+2][c] == "..": #If the square two tiles ahead of the black pawn is empty and it's sitting in it's original tile
                    moves.append(Move((r,c),(r+2,c), self.boardstate))
            if c-1 >= 0: #Bug fix off board error
                if self.boardstate[r+1][c-1][0] == "w": #If a black piece is on the left, capture is available
                    moves.append(Move((r,c),(r+1,c-1), self.boardstate))
            if c+1 <= 7: 
                if self.boardstate[r+1][c+1][0] == "w": #If a black piece is on the right, capture is available
                    moves.append(Move((r,c),(r+1,c+1), self.boardstate))


    '''Define the different ways a rook can move on the board'''         
    def rook_rules(self,r,c,moves):
        unit_vectors = ((-1,0),(0,-1),(1,0),(0,1)) #Define the cartesian directions that a rook can travel through.
        enemy_color = "b" if self.Player1Move else "w" #Quick way to define what pieces can be taken or not.
        for path in unit_vectors:
            for n in range(1,8):
                end_row = r + path[0]*n  #Multiply our cartesian vectors by the number of potential tiles ahead of each castle (max 7). 
                end_col = c + path[1]*n  #Then add it as a possible move, iterate until we hit a friendly piece or an enemy piece.
                if 0 <= end_row < 8 and 0 <= end_col < 8: #Check board range
                    end_piece = self.boardstate[end_row][end_col]
                    if end_piece == "..":
                        moves.append(Move((r,c),(end_row,end_col),self.boardstate)) #if landing on empty spot, append move.
                    elif end_piece[0] == enemy_color:
                        moves.append(Move((r,c),(end_row,end_col),self.boardstate)) #if landing on enemy piece, append move.
                        break #if we hit an enemy piece, stop looking for moves in this direction.
                    else:
                        break #if we hit a friendly piece it's invalid
                else:
                    break #if we hit the edge of the board it's invalid

    '''Define the different ways a bishop can move on the board'''         
    def bishop_rules(self,r,c,moves):
        #Very similar to rooks, just with different direction vectors.
        unit_vectors = ((-1,-1),(-1,1),(1,-1),(1,1)) #Define the cartesian directions that a rook can travel through.
        enemy_color = "b" if self.Player1Move else "w" #Quick way to define what pieces can be taken or not.
        for path in unit_vectors:
            for n in range(1,8):
                end_row = r + path[0]*n
                end_col = c + path[1]*n
                if 0 <= end_row < 8 and 0 <= end_col < 8: #Check board range
                    end_piece = self.boardstate[end_row][end_col]
                    if end_piece == "..":
                        moves.append(Move((r,c),(end_row,end_col),self.boardstate)) #if landing on empty spot, append move.
                    elif end_piece[0] == enemy_color:
                        moves.append(Move((r,c),(end_row,end_col),self.boardstate)) #if landing on enemy piece, append move.
                        break #if we hit an enemy piece, stop looking for moves in this direction.
                    else:
                        break #if we hit a friendly piece it's invalid
                else:
                    break #if we hit the edge of the board it's invalid

    '''Define the different ways a queen can move on the board'''         
    def queen_rules(self,r,c,moves):
        #A queen's moves consist of a rook and a bishop's moves, so we can simply apply the other two functions.
        self.rook_rules(r,c,moves)
        self.bishop_rules(r,c,moves)

    '''Define the different ways a knight can move on the board'''         
    def knight_rules(self,r,c,moves):
        knight_moves = ((-2,-1),(-2,1),(-1,-2),(-1,-2),(-1,2),(1,-2),(1,2),(2,-1),(2,1)) #For knight's we must define all the different jumps they can make.
        enemy_color = "b" if self.Player1Move else "w" #Quick way to define what pieces can be taken or not.
        for coordinate in knight_moves: 
            end_row = r + coordinate[0] #Add all of these coordinates to the row and column values (r,c) and then check if it is a valid move. 
            end_col = c + coordinate[1] #Note, we don't check for friendly collisions here because we are checking each jump vector independently, and not in a for loop.
            if 0 <= end_row < 8 and 0 <= end_col < 8: #Check board range
                end_piece = self.boardstate[end_row][end_col]
                if end_piece == "..":
                    moves.append(Move((r,c),(end_row,end_col),self.boardstate)) #if landing on empty spot, append move.
                elif end_piece[0] == enemy_color:
                    moves.append(Move((r,c),(end_row,end_col),self.boardstate)) #if landing on enemy piece, append move.
        
    '''Define the different ways a king can move on the board'''         
    def king_rules(self,r,c,moves):
        king_moves =((-1,-1),(-1,0),(-1,1),(0,-1),(0,1),(1,-1),(1,0),(1,1)) #For king's we must define all the different jumps they can make.
        enemy_color = "b" if self.Player1Move else "w" #Quick way to define what pieces can be taken or not.
        for coordinate in king_moves:
            end_row = r + coordinate[0] #Add all of these coordinates to the row and column values (r,c) and then check if it is a valid move.
            end_col = c + coordinate[1] #Note, similar to knights we don't check for friendly collisions.
            if 0 <= end_row < 8 and 0 <= end_col < 8: #Check board range
                end_piece = self.boardstate[end_row][end_col]
                if end_piece == "..":
                        moves.append(Move((r,c),(end_row,end_col),self.boardstate)) #if landing on empty spot, append move.
                elif end_piece[0] == enemy_color:
                    moves.append(Move((r,c),(end_row,end_col),self.boardstate)) #if landing on enemy piece, append move.
        

    '''Define how castling works'''
    def getCastleMoves(self,r,c,moves):
        if self.square_being_attacked(r,c):
            return #can't castle while we are in check
        if (self.Player1Move and self.currentCastlingRight.wks) or (not self.Player1Move and self.currentCastlingRight.bks):
            self.getKingsideCastleMoves(r,c,moves)
        if (self.Player1Move and self.currentCastlingRight.wqs) or (not self.Player1Move and self.currentCastlingRight.bqs):
            self.getQueensideCastleMoves(r,c,moves)

    def getKingsideCastleMoves(self,r,c,moves):
        if self.boardstate[r][c+1] == '..' and self.boardstate[r][c+2] == '..':
            if not self.square_being_attacked(r,c+1) and not self.square_being_attacked(r,c+2):
                moves.append(Move((r,c),(r,c+2),self.boardstate, isCastleMove = True))

    def getQueensideCastleMoves(self,r,c,moves):
        if self.boardstate[r][c-1] == '..' and self.boardstate[r][c-2] == '..' and self.boardstate[r][c-3] == '..':
            if not self.square_being_attacked(r,c-1) and not self.square_being_attacked(r,c-2):
                moves.append(Move((r,c),(r,c-2),self.boardstate,isCastleMove = True))

'''
A separate class will be made to constantly track whether castling is allowed at any given time.
'''
class CastleRights():
    def __init__(self,wks,bks,wqs,bqs):
        self.wks = wks
        self.bks = bks
        self.wqs = wqs
        self.bqs = bqs


class Move():
    '''This basically just logs our start moves from main and logs what pieces move and to where (capture).
    The inputs start_tile and end_tile will be tuples in the form of (row,col)
    '''
    def __init__(self, start_tile, end_tile, boardstate, isCastleMove = False):
        self.start_tile = start_tile #Start and end tiles should be initialized with "self." in order to call them in other functions or classes.
        self.end_tile = end_tile
        self.start_piece_name = boardstate[start_tile[0]][start_tile[1]] #If a successful move is being called upon, we want to grab the piece identity at our start tile coordinates
        self.end_piece_name = boardstate[end_tile[0]][end_tile[1]] #For move-undo and our movelog, we would like to know which pieces are being landed on/taken as well.

        #PAWN PROMOTION AND CASTLING
        self.PawnPromotion = False
        if self.start_piece_name == 'wP' and end_tile[0] == 0:
            self.PawnPromotion = True
        elif  self.start_piece_name == 'bP' and end_tile[0] == 7:
            self.PawnPromotion = True
        self.isCastleMove = isCastleMove
        
        self.moveID = start_tile[0] * 1000 + start_tile[1] * 100 + end_tile[0] * 10 + end_tile[1] #This is a common method of assigning an ID number to each move based on the path it takes

    '''Overriding the equals method. Compare moveID's. Because we create a class for Move, we need to use this to make sure pieces don't start and end on the same pieces.'''
    def __eq__(self,other):
        if isinstance(other,Move):
            return self.moveID == other.moveID
        return False

    '''
    Note: This section deals with rankfile notation (Ex. MOVE: a2a3 (move piece on a2 to a3.)). This is temporary and will not appear in the final project.
    Further coordination must be done with the Board State Decoder before this can be completed properly.
    '''
    #maps keys to values (RankFile notation)
    #key : value
    ranksToRows = {"1":7, "2":6, "3":5,"4":4,"5":3,"6":2,"7":1,"8":0}
    rowToRanks = {v: k for k,v in ranksToRows.items()} #quick way to reverse the pairs for ranksToRows
    filesToCols = {"a":0,"b":1,"c":2,"d":3,"e":4,"f":5,"g":6,"h":7}
    colsToFiles = {v: k for k,v in filesToCols.items()} #quick way to reverse the pairs for ranksToRows

    def getChessNotation(self):
        return self.getRankFile(self.start_tile[0], self.start_tile[1]) + self.getRankFile(self.end_tile[0], self.end_tile[1])
    
    def getRankFile(self,r,c):
        return self.colsToFiles[c] + self.rowToRanks[r]