import MySQLdb
import serial
import time

print "\n            Welcome to Allflowlift client server\n"

ser = serial.Serial ('/dev/ttyS0', 9600)
ser.flushInput()
ser.flushOutput()

conn = MySQLdb.connect("localhost","root","0000","allflowlift")
cursor = conn.cursor()

insert_query = ''

def read_vars():
	ser.flushInput()
	ser.flushOutput()

	global insert_query
	insert_query = 'REPLACE INTO pompen (last_error_1, last_error_2, last_error_3, z_looptijd_pomp1, z_looptijd_pomp2, z_inschakelingen_pomp1, z_inschakelingen_pomp2, c_nivo_bovenste, c_nivo_onderste, c_nivo_uitschakel, c_nadraai, c_nadraai_hoogwater, c_looptijd, id) VALUES ('

	ser.write('X')

	read = 0
	count = 0

	read_list = range(18)

	while read == 0:
		while (ser.inWaiting() == 0):
			pass
		readbyte = ser.read()
		# print 'Read Byte: ' + readbyte
		read_list[count] = readbyte
		if count == 17:
			read = 1
		count += 1

	count = 0
	temp2 = ''

	while count < 3: # First 3 elements
		if ord(read_list[count]) == 1:
			temp2 = "Nivo breuk"
		if ord(read_list[count]) == 2:
			temp2 = "Nivo kortsluiting"
		if ord(read_list[count]) == 3:
			temp2 = "Pomp 1 stroom"
		if ord(read_list[count]) == 4:
			temp2 = "Pomp 2 stroom"
		if ord(read_list[count]) == 5:
			temp2 = "Pomp 1 temp"
		if ord(read_list[count]) == 6:
			temp2 = "Pomp 2 temp"
		if ord(read_list[count]) == 7:
			temp2 = "Pomp 1 fase"
		if ord(read_list[count]) == 8:
			temp2 = "Pomp 2 fase"
		if ord(read_list[count]) == 9:
			temp2 = "Hoogwater"

		insert_query += "'" + str(temp2) + "', " # Mysql insert
		count += 1

	temp = str( (ord(read_list[count]) << 8) + ord(read_list[count+1]) )
	insert_query += "'" + temp + "', " # Mysql insert
	count += 2

	temp = str( (ord(read_list[count]) << 8) + ord(read_list[count+1]) )
	insert_query += "'" + temp + "', " # Mysql insert
	count += 2

	temp = str( (ord(read_list[count]) << 8) + ord(read_list[count+1]) )
	insert_query += "'" + temp + "', " # Mysql insert
	count += 2

	temp = str( (ord(read_list[count]) << 8) + ord(read_list[count+1]) )
	insert_query += "'" + temp + "', " # Mysql insert
	count += 2

	# Count = 11

	while count < 17:
		insert_query += "'" + str(ord(read_list[count])) + "', " # Mysql insert
		count += 1

	insert_query += "'" + str(ord(read_list[count])) + "', " # Mysql insert

	# Finish mysql query
	insert_query = insert_query[:-2] # Strip last comma
	insert_query += ')'



alivebyte1 = ''
alivebyte2 = ''

while (1):
	#time.sleep (3)

	ser.flushInput()
	ser.flushOutput()
	
	ser.write('A') # Alive byte

	while (ser.inWaiting() == 0):
		pass
	alivebyte1 = ser.read()
	while (ser.inWaiting() == 0):
		pass
	alivebyte2 = ser.read()
	alivebyte2 = str(ord(alivebyte2))

	if (alivebyte1 == 'A'): # Ack byte
		print "========================================="
		print "Received alive byte with ID: " + alivebyte2
		print "\nSQL Query:\n"
		read_vars()
		print insert_query
		cursor.execute (insert_query)
		print "\nData received from allflowlift system with ID: " + alivebyte2
		print "========================================="
	
	time.sleep(4)



ser.close()
