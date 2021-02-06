import cv2
import numpy as np

tracker = cv2.TrackerKCF_create()
# tracker = cv2.TrackerCSRT_create()
# tracker = cv2.TrackerGOTURN_create()
# tracker = cv2.TrackerMIL_create()

myWidth, myHeight = 640, 480
video = cv2.VideoCapture('/dev/video0')

ok, img = video.read()
bbox = cv2.selectROI('Camera', img, showCrosshair=False, fromCenter=False)

tracker.init(img, bbox)

def drawBbox(img, bbox):
    x, y, w, h = bbox
    pt1 = x, y
    pt2 = x + w, y + h
    cv2.rectangle(img, pt1, pt2, (0,255,0), thickness=2)
    cv2.putText(img, 'Tracking', (50,40), cv2.FONT_HERSHEY_PLAIN, 1, (0,255,0), thickness=2)    

while True:
    timer = cv2.getTickCount()
    
    ok, img = video.read()
    img = cv2.resize(img, (myWidth, myHeight))

    ok, bbox = tracker.update(img)

    if ok:
        drawBbox(img, bbox)
    else:
        cv2.putText(img, 'Lost', (50,40), cv2.FONT_HERSHEY_PLAIN, 1, (0,0,200), thickness=2)    

    fps = cv2.getTickFrequency() / (cv2.getTickCount() - timer)

    cv2.putText(img, 'FPS: {}'.format(int(fps)), (50,20), cv2.FONT_HERSHEY_PLAIN, 1, (0,0,200), thickness=2)

    cv2.imshow('Camera', img)
    if cv2.waitKey(1) == (0xFF & ord('q')):
        break