import picamera
from time import sleep

#create object for PiCamera class
camera = picamera.PiCamera()
#set resolution
camera.resolution = (1024, 768)
camera.brightness = 60
count=0
def CaptureImage(fileName):
    
    name = 'Image/'+ fileName+'.jpeg'
    camera.start_preview()
    #add text on image
    #camera.annotate_text = 'Hi Pi User'
    #sleep(2)
    #store image
    camera.capture(name)
    camera.stop_preview()
