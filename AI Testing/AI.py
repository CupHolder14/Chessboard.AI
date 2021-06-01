#This will take the list of valid moves and grade each one.
import random
#Worst level AI
def findRandomMove(legal_moves):
    return legal_moves[random.randint(0,len(legal_moves)-1)]

#Black wants the score to be as negative as possible
#White wants the score to be as positive as possible

def FindBestMove(game, legal_moves):
    Multiplier = 1 if game.Player1Move else -1
    MinMaxGrade = checkmateValue #Start at the highest value 
    BestMove = None #Variable to store the best move after the algorithm is done
    random.shuffle(legal_moves) #This is to make sure the bot doesn't follow the same sequence every game
    for move in legal_moves: 
        game.make_move(move) #Game will make each move (First layer)
        opponentsMoves = game.legal_move_generation() #For each move, generate all possible opponent's moves (second layer)
        '''
        Get opponent's moves and find their max score (their best possible move given the circumstance)
        '''
        if game.checkmate:
            opponentsMaxScore = stalemateValue
        elif game.stalemate:
            opponentsMaxScore = -checkmateValue
        else:
            opponentsMaxScore = -checkmateValue #Flip the sign (we want to maximize opponent's moves)
            for OppMove in opponentsMoves:
                game.make_move(OppMove)
                game.legal_move_generation()
                if game.checkmate:
                    grade = checkmateValue
                elif game.stalemate:
                    grade = stalemateValue
                else:
                    grade = -Multiplier*ScoreBoardstate(game.boardstate)
                if grade > opponentsMaxScore:
                    opponentsMaxScore = grade
                game.undo_move()
            '''
            If our move leads to a lower max score from the opponent, it is our new best move.
            '''
        if opponentsMaxScore < MinMaxGrade:
            MinMaxGrade = opponentsMaxScore
            BestMove = move
        game.undo_move()
    return BestMove
    #Calculate the grade for each legal move in the first layer.

# def FindBestMoveMinMax(game, legal_moves): #this is a helper function, it is responsible for calling the initial recursive value to nextMove, and then returning the result
#     global nextMove
#     nextMove = None
#     MinMaxMove(game, legal_moves, DEPTH, game.WhitesTurn)
#     return nextMove

# def MinMaxMove(game, legal_moves, depth, WhitesTurn): #this incorporates recursion (depth or number of layers)
#     global nextMove #create a global variable 
#     if depth == 0:
#         return ScoreBoardstate(game.board)
    

#Variables
DEPTH = 2

#Pretty standard
KingValue = 10000
QueenValue = 9
RookValue = 5
KnightValue = 3
BishopValue = 3
PawnValue = 1

ValueDict = {'K': KingValue, 'Q': QueenValue, 'R': RookValue, 'N': KnightValue, 'B': BishopValue, 'P': PawnValue}

checkmateValue = 10000
stalemateValue = 0


def ScoreBoardstate(board):
    '''
    Scan the board and grade the amount of material leftover for each side
    '''
    # if game.checkmate:   #PRUNING OUT CHECKMATE/STALEMATE MOVES
    #     if game.Player1Move:
    #         return -checkmateValue #black wins
    #     else:
    #         return checkmateValue #white wins
    # elif game.stalemate:
    #     return stalemateValue #stalemate
    points = 0 
    for row in board:
        for tile in row:
            if tile[0] == 'w':
                points += ValueDict[tile[1]]
            elif tile[0] == 'b':
                points -= ValueDict[tile[1]]
    return points


#Implement piece and position values. We will let them be STATIC values (not dynamic). These are Google's "deepmind" AI values. But adjust if needed.
#P_Value = 1
#N_Value = 3.05
#B_Value = 3.33
#R_Value = 5.65
#Q_Value = 9.5
#Figure out how they derived these.