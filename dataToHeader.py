import os
import sys
from tkinter import messagebox, Tk

input = sys.argv[1]
output = os.path.join(os.getcwd(), "Arduino", "Main", "Payload.h")

allowedValues = [0, 1, 255]
removeFFArr = []
finalArr = []

def errorOut(str1, str2, str3):
    root = Tk()
    root.withdraw()
    messagebox.showerror(title=str1, message=str2)
    root.destroy()
    sys.exit(str3)

if os.path.splitext(input)[1] != ".data":
    errorOut("Error", 'Incorrect file format (expected ".data", got "{}").'.format(
        os.path.splitext(input)[1]), "Image is not file format, program terminated.")

def bytesToC(data):
    return [format(b, '#04x') for b in data]

with open(input, mode='rb') as file:
    fileContent = file.read()

if len(fileContent) != 76800:
    errorOut("Error", "Image is not correct size.",
             "Image is not correct size, program terminated.")

for x in range(0, len(fileContent)):
    if fileContent[x] not in allowedValues:
        errorOut("Error", "Image provided was not a 1 bit .data file.",
                 "Image provided was not a 1 bit .data file, program terminated.")
    if fileContent[x] != 255:
        removeFFArr.append(fileContent[x])

i = 0
while i <= len(removeFFArr) - 8:
    finalArr.append(int("{}{}{}{}{}{}{}{}".format(removeFFArr[i], removeFFArr[i + 1], removeFFArr[i + 2], removeFFArr[i + 3],
                                                  removeFFArr[i + 4], removeFFArr[i + 5], removeFFArr[i + 6], removeFFArr[i + 7]), 2))

    i += 8


finalArr = bytearray(finalArr)

finalWrite = ("#include <stdint.h>\n#include <avr/pgmspace.h>\n\nconst uint8_t PAYLOAD[4800] PROGMEM = {{{}}};".format(
    ", ".join(bytesToC(finalArr))))


f = open(output, "w")
f.write(finalWrite)
f.close()
