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

def WriteSerial(OPCODE, DATA, lasttime):
    while True:
        thistime = time.time()
        if thistime - lasttime >= 3:
            print("Send!")
            lasttime = thistime
            break

lasttime = time.time()
while True:
    WriteSerial("Test", "test", lasttime)
    

# print('done') 

# SC.WriteSerial("LegalMoves:",str(GreenMoves)) 
# SC.WriteSerial("Check:","True") 
# SC.WriteSerial("AIMove:",str(SendAIMove))
# SC.WriteSerial("IllegalMove:",str()) #Sends Illegal Moves to light up red