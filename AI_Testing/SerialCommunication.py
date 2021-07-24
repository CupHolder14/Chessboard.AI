import serial
import ChessEvents

serialport = serial.Serial('COM4', 9600)
serialport.timeout = 1

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
                print(DATA)
                ParseAppend(DATA,eval)
                return

            if "NextTile:" in DATA:
                print(DATA)
                ParseAppend(DATA,eval)
                return

            if "TimeOut:" in DATA:
                ParseAppend(DATA,bool)
                return

            if "Quit:" in DATA:
                ParseAppend(DATA,bool)
                return

def ReadTest(): #Reading is a work in progress still
    while True:
        DATA = serialport.readline().decode('ascii')
        return DATA

def WriteSerial(OPCODE, DATA):
    serialport.write((OPCODE+":"+DATA).encode())
    

def ParseAppend(DATA, type):
    DATA = DATA.strip().split(':')
    ChessEvents.events.append(DATA[0])
    ChessEvents.values.append(type(DATA[1]))