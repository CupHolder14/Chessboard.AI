from pygame.time import delay
import serial
import ChessEvents
import time 

serialport = serial.Serial('COM4', 9600)
serialport.timeout = 1
lasttime = 0

def ReadSerial():
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
                ParseAppend(DATA,eval)
                return

            if "NextTile:" in DATA:
                ParseAppend(DATA,eval)
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

def WriteSerial(OPCODE, DATA, lasttime):
    thistime = time.time()
    while True:
        if thistime - lasttime >= 3:
            serialport.write((OPCODE+DATA+'9').encode())
            lasttime = thistime
            break
    
    

def ParseAppend(DATA, type):
    DATA = DATA.strip().split(':')
    ChessEvents.events.append(DATA[0])
    ChessEvents.values.append(type(DATA[1]))