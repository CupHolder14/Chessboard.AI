import serial
import time

serialport = serial.Serial('COM3', 9600)
serialport.timeout = 1

def ReadSerial():
    while True:
        DATA = serialport.readline().decode('ascii')
        if DATA:
            if "InitalTile:" in DATA:
                x = DATA.strip("InitialTile:")
                col = int(x[3:4])
                row = int(x[1:2])
                return col,row

            if "NextTile:" in DATA:
                y = DATA.strip("NextTile:")
                col = int(y[3:4])
                row = int(y[1:2])
                return col,row
