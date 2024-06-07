import serial
import time
import string
import pynmea2
def readDataGPS(ser):
	dataout = pynmea2.NMEAStreamReader()
	newdata=ser.readline().decode('unicode_escape')
	res=""
	if newdata[0:6] == "$GPRMC":
		newmsg=pynmea2.parse(newdata)
		lat=newmsg.latitude
		lng=newmsg.longitude
		res += "Latitude=" + str(lat) + "and Longitude=" + str(lng)
	return res