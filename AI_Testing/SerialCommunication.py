from pygame.time import delay
import serial
import ChessEvents
import time 

serialport = serial.Serial('COM4', 9600)
serialport.timeout = 1
lasttime = 0

def ReadSerial(stack=True):
    while True:
        DATA = serialport.readline().decode('ascii')
        if DATA:
            if "StartGame:" in DATA:
                ParseAppend(DATA,bool)
                return
            
            if "VsHuman:" in DATA:
                ParseAppend(DATA,bool)
                return

            if "Difficulty:" in DATA:
                ParseAppend(DATA,int)
                return

            if "InitialTile:" in DATA:
                if stack:
                    ParseAppend(DATA,eval)
                else:
                    AppendParse(DATA,eval)
                return

            if "NextTile:" in DATA:
                if stack:
                    ParseAppend(DATA,eval)
                else:
                    AppendParse(DATA,eval)
                return

            if "TimeOut:" in DATA:
                ParseAppend(DATA,bool)
                return

            if "Quit:" in DATA:
                ParseAppend(DATA,bool)
                return

def ReadTest(): #work in progress still
    while True:
        DATA = serialport.readline().decode('ascii')
        return DATA
    
def WriteSerial(OPCODE, DATA):
    thistime = time.time()
    while thistime - lasttime <= 2:
        thistime = time.time()
    serialport.write((OPCODE+DATA+'9').encode())
    globals()['lasttime'] = time.time()

def ParseAppend(DATA, type):
    DATA = DATA.strip().split(':')
    ChessEvents.events.append(DATA[0])
    ChessEvents.values.append(type(DATA[1]))

def AppendParse(DATA,type):
    DATA = DATA.strip().split(':')
    ChessEvents.events.insert(0,DATA[0])
    ChessEvents.values.insert(0,type(DATA[1]))