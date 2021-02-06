import cv2
import numpy as np
import sys

path = '../resources/haarcascades/haarcascade_frontalface_default.xml'

video = cv2.VideoCapture('/dev/video0')

if not video.isOpened():
    print('Open video device fail')
    sys.exit()

def empty(p):
    pass

cv2.namedWindow('Camera')
cv2.createTrackbar('Scale', 'Camera', 100, 1000, empty)
cv2.createTrackbar('Neig', 'Camera', 1, 10, empty)
cv2.createTrackbar('Min Area', 'Camera', 1000, 100000, empty)

# load the classifier 
cascade = cv2.CascadeClassifier(path)

while True:
    timer = cv2.getTickCount()
    ok, img = video.read()
    
    if not ok:
        print('Image capture fail')
        break
    
    imgGray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    
    scale = 1 + cv2.getTrackbarPos('Scale', 'Camera') / 1000
    minNeig = cv2.getTrackbarPos('Neig', 'Camera')
    
    objects = cascade.detectMultiScale(imgGray, scale, minNeig)

    fps = cv2.getTickFrequency() / (cv2.getTickCount() - timer)
    cv2.putText(img, 'FPS: {}'.format(int(fps)), (50,20), cv2.FONT_HERSHEY_PLAIN, 1, (0,0,200), thickness=2)

    for x, y, w, h in objects:
        pt1 = x, y
        pt2 = x + w, y + h
        area = w * h
        minArea = cv2.getTrackbarPos('Min Area', 'Camera')
        if area >= minArea:
            cv2.rectangle(img, pt1, pt2, (0,255,0), thickness=2)
            cv2.putText(img, 'Face', (x,y-10), cv2.FONT_HERSHEY_PLAIN, 0.7, (0,255,0))

    imgGray = cv2.cvtColor(imgGray, cv2.COLOR_GRAY2BGR)
    cv2.imshow('Camera', img)
    if cv2.waitKey(1) == (0xFF & ord('q')):
        break