import SerialCommunication as SC
import time 
# x = input()
# SC.WriteSerial("LegalMoves:","[(1,2), (2,2), (3,2), (4,2), (5,2)]")
# print("sleep now")
# time.sleep(2)
# print("woke up")
# SC.WriteSerial("IllegalMove:","[(5,6), (5,7)]")


# x = input()
# SC.WriteSerial("AIMove:","[(1,2), (2,2)]")

# x = input()
# SC.WriteSerial("Check:","True")

# x = input()
# SC.WriteSerial("Winner:","[1]")

# while True:
#     x = input()

#     SC.WriteSerial("Test:","[1]")
#     print("Sent")

MoveQueue = [(0,2),(3,3)]
value = (2,2)
MoveQueue.insert(0,value)
print(MoveQueue)

# SC.WriteSerial("LegalMoves:",str(GreenMoves)) 
# SC.WriteSerial("Check:","True") 
# SC.WriteSerial("AIMove:",str(SendAIMove))
# SC.WriteSerial("IllegalMove:",str()) #Sends Illegal Moves to light up red