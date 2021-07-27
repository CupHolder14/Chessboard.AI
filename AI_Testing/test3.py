# # # import time
# # # import ChessEvents

# # #TEST 1: Reading from ARduino
# # # while True:
# # #     c,r = SC.ReadSerial()
# # #     x = ChessEvents.get(ChessEvents.events)
# # #     print(str(c)+str(r))
# # #     print(x)

# # #TEST 2: Parsing
# # # DATA = "opcode:True"
# # # x = []
# # # def ParseAppend(DATA, type):
# # #     DATA = DATA.split(':')
# # #     x.append(DATA[0])
# # #     x.append(type(DATA[1]))

# # # ParseAppend(DATA,bool)
# # # print(x)

# # #TEST 3: Writing to Arduino
# # import serial
# # import time
# # import ChessEvents
# # import SerialCommunication as SC
# # flag = False
# # time.sleep(2) #GIVE THE ARDUINO TIME TO WAKE UP
# # while True:
# #     print("Writing:")
# #     SC.WriteSerial("OPCODE","Value")
# #     print("Writing Complete")
# #     #DATA = SC.serialport.read(SC.serialport.in_waiting).decode('ascii')
# #     #DATA = SC.serialport.readline().decode('ascii') #Takes forever! Why?!?!?! (After figuring that out, replace with the SC version of read)
# #     flag = True
# #     while flag:
# #         #DATA = SC.serialport.read(SC.serialport.in_waiting).decode('ascii')
# #         DATA = SC.serialport.readline().decode('ascii') #Takes forever! Why?!?!?! (After figuring that out, replace with the SC version of read)
# #         if DATA:
# #             print("Read Successful")
# #             print(DATA)
# #             print()
# #             break
# #     break
# # print("absolutely done")

# #     # SC.WriteSerial("InitialMove", "(5,3)")
# #     # SC.ReadSerial() THIS DOES NOT WORK FOR SOME REASON
# #     #event, value = ChessEvents.get(ChessEvents.events, ChessEvents.values)
# #     # print(event)
# #     # print(value)
# #     # print('done')

# x = '(1,2)'
# print(x)

LegalMoves = [[(1,2),(2,3)],[(4,5),(6,7)]]
value = (1,29)
#print(range(len(LegalMoves)))
if value in [LegalMoves[i][0] for i in range(len(LegalMoves))]:
    print('true')
else:
    print('false')

