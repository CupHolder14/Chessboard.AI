import serial
import ChessEvents

serialport = serial.Serial('COM3', 9600)
serialport.timeout = 1

def ReadSerial():
    while True:
        DATA = serialport.readline().decode('ascii')
        if DATA:
            if "StartGame:" in DATA:
                ParseAppend(DATA,bool)
                return
                # ChessEvents.events.append("StartGame")
                # x = DATA.strip("StartGame: ")
                # ChessEvents.values.append(x)
                # return
            
            if "Difficulty:" in DATA:
                ParseAppend(DATA,int)
                # ChessEvents.events.append("Difficulty")
                # x = DATA.strip("Difficulty: ")
                # difficulty = int(x)
                # ChessEvents.values.append(difficulty)
                return

            if "InitialTile:" in DATA:
                ChessEvents.events.append("InitialTile")
                x = DATA.strip("InitialTile:")
                col = int(x[3:4])
                row = int(x[1:2])
                ChessEvents.values.append((col,row))
                return

            if "NextTile:" in DATA:
                ChessEvents.events.append("NextTile")
                x = DATA.strip("NextTile:")
                col = int(x[3:4])
                row = int(x[1:2])
                ChessEvents.values.append((col,row))
                return

            if "GameOver:" in DATA:
                ParseAppend(DATA,bool)
                return
                # ChessEvents.events.append("GameOver")
                # x = DATA.strip("GameOver: ")
                # ChessEvents.values.append(x)
                # return
            if "PRINT:" in DATA:
                ParseAppend(DATA,str)
                return
def ReadTest():
    while True:
        DATA = serialport.readline().decode('ascii')
        return DATA

def WriteSerial(OPCODE, DATA):
    serialport.write((OPCODE+":"+DATA).encode())
    

def ParseAppend(DATA, type):
    DATA = DATA.split(':')
    ChessEvents.events.append(DATA[0])
    ChessEvents.values.append(type(DATA[1]))