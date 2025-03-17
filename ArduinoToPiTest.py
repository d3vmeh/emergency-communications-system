import serial
import time

ser = serial.Serial("/dev/ttyACM0", 115200, timeout=1)

# Reset the Arduino's line. This is key to getting the write to work.
# Without it, the first few writes don't work.
# Clear DTR, wait one second, flush input, then set DTR.
# Without this, the first write fails.
# This trick was learned from:
# https://github.com/miguelasd688/4-legged-robot-model

ser.setDTR(False)
time.sleep(1)
ser.reset_input_buffer()
ser.setDTR(True)
time.sleep(2)


#IF THE SERIAL PORT WONT WORK, RESET THE ARDUINO WHILE IT IS CONNECTED TO THE PI
while True:
    from_arduino = ser.readline().decode(errors='ignore').strip()  # Read full message, decode, and clean up
    if from_arduino:
        print(f"Arduino says: {from_arduino}")
        print("Responding to Arduino")
        ser.write(b"Hello Arduino\n")  # Send bytes with newline
        print("\n\n")
    #time.sleep(1)  # Avoid flooding

