import SerialCommunication as SC
import ChessEvents

while True:
    c,r = SC.ReadSerial()
    x = ChessEvents.get(ChessEvents.events)
    print(str(c)+str(r))
    print(x)


