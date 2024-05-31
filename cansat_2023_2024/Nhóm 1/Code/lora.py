import time

def SendData(data,ser_):
    ser_.write(bytearray(data, 'utf-8'))
    ser_.flush()