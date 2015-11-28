#!/usr/bin/python

import sys
import time
import serial
import threading
import readline

class SerialConnection:
    def __init__(self, port = None):
        self.ser = None

        if port is not None:
            # The user specified a port, so let's open it.
            self.open(port)

    def open(self, port, baudrate = 9600):
        """ Open a connection and check if it's open. """
        self.ser = serial.Serial(port, baudrate = baudrate, timeout = 1)

        if not self.ser.isOpen():
            raise Exception("Could not open the port '" + port + "'")

    def close(self):
        """ Close the connection """
        self.ser.close()

    def send(self, command):
        """ Send a command to the device """
        self.ser.write(command)

    def read(self, bytes = None):
        """ Reads a line for the serial port if `bytes` is not specified """
        if bytes is not None:
            return self.ser.read(bytes)

        return self.ser.readline().replace("\r\n", "")

class Logging(threading.Thread):
    def __init__(self, interval, log_function):
        """Initializes the `Logging` class.

            Note:
                Do not include the `self` parameter in the ``Args`` section.

            Args:
                interval (int): Interval in seconds between each sample.
                log_function (function): A function that will be called every time the timer decides it is time to take a sample. You should use this function to log.
        """
        threading.Thread.__init__(self)

        self.stopped = threading.Event()
        self.interval = interval
        self.log_function = log_function

    def run(self):
        """ What to do while logging. """
        while not self.stopped.wait(self.interval):
            if self.log_function():
                print "Logging finished. Cutoff voltage reached."
                self.stop()

    def stop(self):
        """ Stops logging. """
        self.stopped.set()

def fetch(conn, value):
    conn.send(value + "\n")
    reading = conn.read()

    return reading

def grab_sample(conn, csv, count, current, cutoff, interval):
    tmp_data = [ count[0], time.strftime("%c"),
            fetch(conn, "SENSEV"), fetch(conn, "USBV") ]

    line = str(tmp_data[0]) + "," + tmp_data[1] + "," + tmp_data[2] + "," + tmp_data[3] + ",\n"
    csv.write(line.encode('ascii', 'ignore').decode('ascii'))

    print "Reading %d (%.1fmAh): Battery %sV - USB %sV" % (tmp_data[0], (current * (tmp_data[0] * interval) / 3600), tmp_data[2], tmp_data[3])

    if (float(tmp_data[2]) < cutoff):
        print fetch(conn, "ISET 0")
        return True

    count[0] += 1
    return False

def start_log(conn, current, cutoff):
    count = [ 0 ]
    filename = time.strftime("%c") + ".log"
    csv = open(filename, "w")
    print "Logging data to " + filename
    print "The cutoff voltage is set to %.3fV" % cutoff
    
    thread = Logging(5, lambda: grab_sample(conn, csv, count, current, cutoff, 5))
    thread.start()
    raw_input("Press Enter to stop the logging process.\n")
    thread.stop()

    csv.close()

# Main program.
if __name__ == "__main__":
    port = sys.argv[1]
    conn = SerialConnection(port)
    cutoff = 0.0
    current = 0.0

    print "Connecting to miniLoad in " + port + "..."
    time.sleep(3)  # Arduino hates when you send data right after open...

    while True:
        rinput = raw_input("> ")
        line = rinput.split(' ')
        command = line[0]

        if command == "raw":
            # Send raw data.
            print fetch(conn, rinput[4:])
        elif command == "sv":
            # Sense voltage.
            print fetch(conn, "SENSEV") + "V"
        elif command == "uv":
            # USB voltage.
            print fetch(conn, "USBV") + "V"
        elif command == "is":
            # Sets the current.
            print fetch(conn, "ISET " + line[1])
            current = int(line[1])
        elif command == "rs":
            # Ratio set.
            ratio = float(line[1]) * 1000
            print fetch(conn, "RATSET " + str(ratio))
        elif command == "cs":
            # Set the cutoff voltage.
            cutoff = float(line[1])
            print "Cutoff set to %.3fV" % cutoff
        elif command == "start":
            # Start logging.
            start_log(conn, current, cutoff)
        elif command == "help":
            print "raw <command> - Sends raw data"
            print "sv - Fetches a voltage reading"
            print "uv - Fetches the USB voltage"
            print "is <current> - Sets the current in mA"
            print "rs <ratio> - Sets the ratio"
            print "cs <voltage> - Set the cutoff voltage"
            print "start - Starts the logging process"
            print "help - This"
            print "q - Quits the program"
        elif command == "q":
            # Quits the program.
            break
        else:
            print "Invalid command."

    conn.close()

