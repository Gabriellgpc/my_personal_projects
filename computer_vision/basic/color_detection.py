import cv2
import numpy as np
import time
import myUtils

cap = cv2.VideoCapture('/dev/video0')

def empty(v):
    pass

cv2.namedWindow("HSV Space")
cv2.createTrackbar("HUE Min", "HSV Space", 0, 179, empty)
cv2.createTrackbar("HUE Max", "HSV Space", 179, 179, empty)
cv2.createTrackbar("SAT Min", "HSV Space", 0, 255, empty)
cv2.createTrackbar("SAT Max", "HSV Space", 255, 255, empty)
cv2.createTrackbar("VALUE Min", "HSV Space", 0, 255, empty)
cv2.createTrackbar("VALUE Max", "HSV Space", 255, 255, empty)

fps = 0
while True:
    tik = time.time()
    sucess, img = cap.read()
    img = cv2.resize(img, (320, 240))

    imgHSV = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)

    h_min, h_max = cv2.getTrackbarPos("HUE Min", "HSV Space"), cv2.getTrackbarPos("HUE Max", "HSV Space")
    s_min, s_max = cv2.getTrackbarPos("SAT Min", "HSV Space"), cv2.getTrackbarPos("SAT Max", "HSV Space")
    v_min, v_max = cv2.getTrackbarPos("VALUE Min", "HSV Space"), cv2.getTrackbarPos("VALUE Max", "HSV Space")

    lower = np.array([h_min, s_min, v_min])
    upper = np.array([h_max, s_max, v_max])
    mask  = cv2.inRange(img, lower, upper)
    result= cv2.bitwise_and(img, img, None, mask)

    tok = time.time()
    fps = 1.0/(tok - tik)
    
    cv2.putText(img, "FPS:{0:.2f}".format(fps), (0,15), cv2.FONT_HERSHEY_COMPLEX, 0.5, (0,0,200), thickness=1) 
    
    mask = cv2.cvtColor(mask, cv2.COLOR_GRAY2BGR)
    imageStacked = np.hstack((img, mask, result))
    
    cv2.imshow('Video', imageStacked)
    if cv2.waitKey(1) == (0xFF & ord('q')):
        break