import time
from time import sleep
import bme280_basic
import mpu6050
import mq135
import Lora
import gps11
import serial
import camera
from time import strftime
from datetime import datetime
port="/dev/ttyAMA0"
ser=serial.Serial(port, baudrate=9600, timeout=0.5)
gpsVal=""
gpsNewVal=""
while True:
    full_datetime = strftime("%d/%m/%y%_I:%M%p")
    val="\r\n========"+full_datetime+"=========\r\n"
    val+=bme280_basic.readSensorData()
    val+=mpu6050.readSensorData()
    val+=mq135.readSensorData()
    gpsVal=gps11.readDataGPS(ser)
    if(gpsVal != ""):
        gpsNewVal=gpsVal
    val+="\r\n-----GPS DATA-----\r\n"
    val += gpsNewVal
    print(val)
    Lora.SendData(val,ser)
    print("sent")
    fileName = "Log/"+datetime.today().strftime('%Y-%m-%d_%H:%M:%S')+".txt"
    myfile = open(fileName, 'w')
    myfile.write(val)
    myfile.close()
    
    camera.CaptureImage(datetime.today().strftime('%Y-%m-%d_%H:%M:%S'))
    sleep(5)