import serial
import time
import ChessEvents

serialport = serial.Serial('COM3', 9600)
serialport.timeout = 1

def ReadSerial():
    while True:
        DATA = serialport.readline().decode('ascii')
        if DATA:
            if "InitialTile:" in DATA:
                ChessEvents.events.append("InitialTile")
                x = DATA.strip("InitialTile:")
                col = int(x[3:4])
                row = int(x[1:2])
                return col,row

            if "NextTile:" in DATA:
                ChessEvents.events.append("NextTile")
                y = DATA.strip("NextTile:")
                col = int(y[3:4])
                row = int(y[1:2])
                return col,row

            
