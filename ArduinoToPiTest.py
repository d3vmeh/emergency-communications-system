import serial
import time

def main():
    ser = serial.Serial("/dev/ttyACM0", 115200, timeout=1)
    time.sleep(2) 

    print("Raspberry Pi interface ready.")
    print("Type your message and press Enter to send to the Tower:")

    try:
        while True:
            while ser.in_waiting:
                incoming = ser.readline().decode('utf-8', errors='ignore').strip()
                if incoming:
                    print("From Arduino:", incoming)

            user_message = input("Message to Tower: ")
            if user_message:
                ser.write((user_message + "\n").encode('utf-8'))
    except KeyboardInterrupt:
        print("Exiting...")
    finally:
        ser.close()

if __name__ == "__main__":
    main()

