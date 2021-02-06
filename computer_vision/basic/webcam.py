import cv2
import numpy as np
import time

frameWidth  = 640
frameHeight = 480
cap = cv2.VideoCapture('/dev/video0')
# cap = cv2.VideoCapture('resources/my_room.mp4')

while True:
    tik = time.time()
    
    sucess, img = cap.read()
    if not sucess:
        print('Video finished!')
        break
    img = cv2.resize(img, (frameWidth, frameHeight))
    tok = time.time()
    
    print('FPS:', 1.0/(tok-tik))
    cv2.imshow('Capture', img)
    
    if cv2.waitKey(1) == ord('q'):
        print('Exit')
        break


