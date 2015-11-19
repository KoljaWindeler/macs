#sudo apt-get install python-serial
import os, sys
import glob
import serial

# open file
try:
	dirname, filename = os.path.split(os.path.abspath(sys.argv[0]))
	fileh = open(dirname+"/config.txt", 'r')
except:
	exit("file not found")

# read and prepare data
data=fileh.readlines()[0]
data_ar=data.split("	")

# check length
if(len(data_ar)!=4):
	exit("config invalid")

# generate msg
chk=0x00
msg=bytearray()

for i in range(0,len(data_ar)):
	b = bytearray()
	b.extend(map(ord, data_ar[i]))
	if((i==3 or i==0) and b[0]>=ord('0') and b[0]<=ord('3')):
		#print("converted type")
		b[0]-=ord('0')
	for ii in range(0,len(b)):
		if(i>0):
			chk^=b[ii]
		msg.append(b[ii])
	msg.append(0x09)
msg.append(chk)
msg.append(0x09)

# try to get all serial ports starting with /dev/ttyACM
ports = glob.glob('/dev/ttyACM*')
result = []
for port in ports:
	try:
		s = serial.Serial(port)
		s.close()
		result.append(port)
	except (OSError, serial.SerialException):
		pass


if(len(result)==0):
	print("no connection found")

for port in result:
	s = serial.Serial(port, baudrate=115200, timeout=3.0)
	send=bytearray()
	send.append(ord('i'));
	send.append(0x09);
	s.write(send)
	recv=s.read(4) # waits up to 3 sec
	if(recv=="MACS"):
		s.write(msg)
		delay(2000)
		recv=s.read(20)
		if(recv[:5]=="saved"):
			print("saved on port "+port)
		else:
			print("port "+port+" resp: "+recv.decode()+"!='saved'")
	else:
		print("port "+port+" resp ("+str(len(recv))+") '"+recv.decode()+"'!='MACS'")
	


