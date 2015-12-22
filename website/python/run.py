#sudo apt-get install python-serial
import os, sys
import glob, time
import serial


def log(inp,reset=0):
	if(reset==1):
		log_f=open("/tmp/macs_log.txt", "w")
	else:
		log_f=open("/tmp/macs_log.txt", "a")
	out="["+time.strftime("%H:%M:%S")+"] "+str(inp)
	log_f.write(out+"\r\n")
	log_f.close()	
	print(out)

try:	
	log("Reading WiFi config file",1)
except:
	exit("could not start logging")


	
# open file
try:
	dirname, filename = os.path.split(os.path.abspath(sys.argv[0]))
	fileh = open("/tmp/macs_config.txt", 'r')
except:
	exit("file not found")

data=fileh.readlines()


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
	log("no connection found")

for port in result:
	for a in range(0,len(data)):
		# read and prepare data
		data_line=data[a]
		data_ar=data_line.split("	")

		# check length
		if(len(data_ar)!=4):
			log("config invalid")
			exit("config invalid")

		# generate msg
		chk=0x00
		msg=bytearray()

		# array to loop over field (the fields are separated by tabs in the file
		for i in range(0,len(data_ar)):
			b = bytearray()
			b.extend(map(ord, data_ar[i]))
			# field 0 (id) and 3 (wifi type) are asci nr, convert them to nr only
			if((i==3 or i==0) and b[0]>=ord('0') and b[0]<=ord('4')):
				#print("converted type")
				b[0]-=ord('0')
			# loop over all chars in the current field
			for ii in range(0,len(b)):
				if(b[ii]!=10 and b[ii]!=13):
					# id field is not part of checksum
					if(i>0):
						chk^=b[ii]
					msg.append(b[ii])
			msg.append(0x09)
		msg.append(chk)
		msg.append(0x09)

		for i in range(0,len(msg)):
			print(hex(msg[i]))

		if(a==0):
			log("Port "+port+" found")

		try:
			s = serial.Serial(port, baudrate=115200, timeout=2.0)
			if(a==0):
				send=bytearray()
				send.append(ord('i'));
				send.append(0x09);
				s.write(send)
				recv=s.read(4) # waits up to 3 sec
				if(recv.decode()=="MACS"):
					log("MACS unit found on port "+port)
					send=1

			if(send==1 or a>0):
				s.write(msg)
				recv=s.read(20)
				recv=recv.decode()
				#recv=recv[0:5]
				if(recv.find("saved")>-1):
					log("Stored WiFi-config #"+str(a)+" '"+data_ar[1]+"' to unit")
				else:
					log("Resp: '"+recv+"'!='saved'")
					log("Please try again")
					break

			else:
				log("No MACS found on port "+port+" - resp ("+str(len(recv))+") '"+recv.decode()+"'!='MACS'")
				log("Please try again")
				break
		except:
			log("Exception")
			log("Please try again")
	
log("Thanks for using MACS")


