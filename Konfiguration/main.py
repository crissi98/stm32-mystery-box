import serial
import struct

ser = serial.Serial()

# 16 Bit Wert Little Endian codiert schreiben
def write16BitValue(val):
    data = struct.pack(">i", val)
    ser.write(struct.pack(">B", data[2]))
    ser.write(struct.pack(">B", data[3]))


# 8 Bit Wert schreiben
def write8BitValue(val):
    data = struct.pack(">i", val)
    ser.write(struct.pack(">B", data[3]))


if __name__ == '__main__':
    ser.baudrate = 9600
    ser.port = 'COM4'
    ser.open()

    print("Avialable colors:")
    print("1: Pink")
    print("2: Türkis")
    print("3: Orange")
    print("4: Gelb")
    print()

    # Konfiguration für jeweilige Farbe laden
    selected = input("Enter number to select color: ")
    if selected == "1":
        color = b'pinkes     '
        redMin = 820
        redMax = 1024
        greenMin = 0
        greenMax = 10
        blueMin = 20
        blueMax = 800
    elif selected == "2":
        color = b'tuerkises  '
        redMin = 0
        redMax = 50
        greenMin = 500
        greenMax = 1024
        blueMin = 400
        blueMax = 800
    elif selected == "3":
        color = b'oranges    '
        redMin = 800
        redMax = 1024
        greenMin = 50
        greenMax = 200
        blueMin = 0
        blueMax = 10
    elif selected == "4":
        ser.color = b'gelbes     '
        redMin = 800
        redMax = 1024
        greenMin = 200
        greenMax = 600
        blueMin = 0
        blueMax = 10
    else:
        print("Wrong input.", selected, "is not in [1,2,3,4]")
        exit(1)

    # Konfiguration für Annäherung und Helligkeit laden
    proximity = int(input("Annäherung eingeben: "))
    if proximity not in range(0, 256):
        print("Nur Byte-Werte (0-255) erlaubt!")
        exit(1)

    brightness = int(input("Helligkeit eingeben: "))
    if brightness not in range(0, 256):
        print("Nur Byte-Werte (0-255) erlaubt!")
        exit(1)

    # Konfiguration an über Serial-Port schreiben
    ser.write(color)

    write16BitValue(redMin)
    write16BitValue(redMax)
    write16BitValue(greenMin)
    write16BitValue(greenMax)
    write16BitValue(blueMin)
    write16BitValue(blueMax)

    write8BitValue(proximity)
    write8BitValue(brightness)

    print("Data sent successfully")

    ser.close()
