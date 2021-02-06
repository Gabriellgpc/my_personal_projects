import cv2
import time
import numpy as np

path = 'featureDetector_tutorial/ImagesQuery'
frameWidth = 640
frameHeight= 480

video = cv2.VideoCapture('/dev/video0')

imgCount = 0
while True:
    ok, img = video.read()
    assert(ok == True)
    img = cv2.resize(img, (frameWidth, frameHeight))
    
    cv2.imshow('Video', img)
    key = cv2.waitKey(1)
    if key == ord(' '):
        cv2.imshow('Capture', img)
        if cv2.waitKey(0) == ord('s'):
            name = '{}/{}_{}.png'.format(path, imgCount, time.asctime())
            name = name.replace(' ', '_')
            cv2.imwrite(name, img)
            imgCount += 1
            print(name)
        else:
            print('cancel')
        cv2.destroyWindow('Capture')
    if key == ord('q'):
        break