import random

#Worst level AI
def findRandomMove(legal_moves):
    return legal_moves[random.randint(0,len(legal_moves)-1)]

#Black wants the score to be as negative as possible
#White wants the score to be as positive as possible


'''
NON-RECURSIVE BRUTE FORCE METHOD, CAN ONLY EVALUATE DEPTH = 2 BEFORE NEEDING MOVE CODE
'''
# def FindBestMove(game, legal_moves):
#     Multiplier = 1 if game.Player1Move else -1
#     MinMaxGrade = checkmateValue #Start at the highest value 
#     BestMove = None #Variable to store the best move after the algorithm is done
#     random.shuffle(legal_moves) #This is to make sure the bot doesn't follow the same sequence every game
#     for move in legal_moves: 
#         game.make_move(move) #Game will make each move (First layer)
#         opponentsMoves = game.legal_move_generation() #For each move, generate all possible opponent's moves (second layer)
#         '''
#         Get opponent's moves and find their max score (their best possible move given the circumstance)
#         '''
#         if game.checkmate:
#             opponentsMaxScore = stalemateValue
#         elif game.stalemate:
#             opponentsMaxScore = -checkmateValue
#         else:
#             opponentsMaxScore = -checkmateValue #Flip the sign (we want to maximize opponent's moves)
#             for OppMove in opponentsMoves:
#                 game.make_move(OppMove)
#                 game.legal_move_generation() #we have to generate valid moves after each move to check checkmate. This slows the code down significantly, but is necessary
#                 if game.checkmate:
#                     grade = checkmateValue
#                 elif game.stalemate:
#                     grade = stalemateValue
#                 else:
#                     grade = -Multiplier*ScoreBoardstate(game.boardstate)
#                 if grade > opponentsMaxScore:
#                     opponentsMaxScore = grade
#                 game.undo_move()
#             '''
#             If our move leads to a lower max score from the opponent, it is our new best move.
#             '''
#         if opponentsMaxScore < MinMaxGrade:
#             MinMaxGrade = opponentsMaxScore
#             BestMove = move
#         game.undo_move()
#     return BestMove
#     #Calculate the grade for each legal move in the first layer.


#Variables
DEPTH = 2

'''
Recursive form of the minmax algorithm, can evaluate at any value of depth
'''
def FindBestMoveMinMax(game, legal_moves): #this is a helper function, it is responsible for calling the initial recursive value to nextMove, and then returning the result
    global nextMove
    nextMove = None
    MinMaxMove(game, legal_moves, DEPTH, 1 if game.Player1Move else -1)
    return nextMove


'''
Need to implement alpha beta pruning next
'''
def MinMaxMove(game, legal_moves, depth, Multiplier): #this incorporates recursion (depth or number of layers)
    global nextMove
    if depth == 0:
        return Multiplier*ScoreBoardstate(game)
    random.shuffle(legal_moves) #Might be needed, not sure where it goes yet though      
    MaxScore = -checkmateValue
    for move in legal_moves:
        game.make_move(move)
        NewMoves = game.legal_move_generation()
        score = -MinMaxMove(game, NewMoves, depth - 1, -Multiplier) #Enter the next layer
        if score > MaxScore:
            MaxScore = score
            if depth == DEPTH:
                nextMove = move
        game.undo_move()
    return MaxScore



'''
More lines to do it
'''

# def MinMaxMove(game, legal_moves, depth, Player1Move): #this incorporates recursion (depth or number of layers)
#     global nextMove #create a global variable 
#     if depth == 0:
#         return ScoreBoardstate(game)
#     random.shuffle(legal_moves) #Might be needed, not sure where it goes yet though      
#     if Player1Move:
#         MaxScore = -checkmateValue
#         for move in legal_moves:
#             game.make_move(move)
#             NewMoves = game.legal_move_generation()
#             score = MinMaxMove(game, NewMoves, depth - 1, not Player1Move) #Enter the next layer
#             if score > MaxScore:
#                 MaxScore = score
#                 if depth == DEPTH:
#                     nextMove = move
#             game.undo_move()
#         return MaxScore

#     else: #black's move
#         MinScore = checkmateValue
#         for move in legal_moves:
#             game.make_move(move)
#             NewMoves = game.legal_move_generation()
#             score = MinMaxMove(game, NewMoves, depth - 1, Player1Move)
#             if score < MinScore:
#                 MinScore = score
#                 if depth == DEPTH:
#                     nextMove = move
#             game.undo_move()
#         return MinScore

def ScoreBoardstate(game):
    '''
    Scan the board and grade the amount of material leftover for each side
    '''
    if game.checkmate:   #PRUNING OUT CHECKMATE/STALEMATE MOVES
        if game.Player1Move:
            return -checkmateValue #black wins
        else:
            return checkmateValue #white wins
    elif game.stalemate:
        return stalemateValue #stalemate
    #If there is a checkmate, skip the scoring, we don't need to evaluate further    
    points = 0 
    
    for row in game.boardstate:
        for tile in row:
            if tile[0] == 'w':
                points += ValueDict[tile[1]]
            elif tile[0] == 'b':
                points -= ValueDict[tile[1]]
    return points

#Google's Deepmind Values
KingValue = 10000
QueenValue = 9.5
RookValue = 5.65
KnightValue = 3.05
BishopValue = 3.33
PawnValue = 1

ValueDict = {'K': KingValue, 'Q': QueenValue, 'R': RookValue, 'N': KnightValue, 'B': BishopValue, 'P': PawnValue}

checkmateValue = 10000
stalemateValue = 0