#!/usr/bin/python

import sys
import time
import serial
import threading

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
        self.ser.write(command + "\r\n")

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
			self.log_function()

	def stop(self):
		""" Stops logging. """
		self.stopped.set()

def grab_sample(dmm, csv, count):
    tmp_data = [ count[0], time.strftime("%c"),
            dmm.fetch(True, False), dmm.fetch(False, False) ]

    line = str(tmp_data[0]) + "," + tmp_data[1] + "," + tmp_data[2][:-1] + "," + tmp_data[3][:-1] + ",\n"
    csv.write(line.encode('ascii', 'ignore').decode('ascii'))

    print("Reading %d:" % tmp_data[0])
    print tmp_data[2]
    print tmp_data[3]
    print "-----------"

    count[0] += 1

# Main program.
if __name__ == "__main__":
    port = sys.argv[1]
    conn = DMM.SerialConnection(port)
    dmm  = DMM.Agilent.DMM(conn)

    # Prints the identification stuff.
    idn = dmm.identify()
    print "Connected to", idn["oem"], idn["model"]

    count = [ 0 ]
    csv = open(time.strftime("%c") + ".log", "w")

    thread = DMM.Logging(1, lambda: grab_sample(dmm, csv, count))
    thread.start()
    raw_input("Press Enter to continue...\n")
    thread.stop()

    csv.close()
    conn.close()

