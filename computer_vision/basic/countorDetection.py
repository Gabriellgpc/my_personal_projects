import cv2
import numpy as np

# myWidth, myHeight = 320, 240
video = cv2.VideoCapture('/dev/video0')

def empty(v):
    pass

cv2.namedWindow('Canny Threshold')
cv2.createTrackbar('T1', 'Canny Threshold', 0, 200, empty)
cv2.createTrackbar('T2', 'Canny Threshold', 200, 200, empty)
cv2.createTrackbar('Min Area', 'Canny Threshold', 1000, 10000, empty)

def getCountours(imgEdge, imgContours):
    # cv.RETR_EXTERNAL
    contours, hierarchy = cv2.findContours(imgEdge, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)

    for contour in contours:
        area = cv2.contourArea(contour)
        if area > cv2.getTrackbarPos('Min Area', 'Canny Threshold'):
            
            cv2.drawContours(imgCountour, contour, -1, (200,0,200), 2)
            perimeter = cv2.arcLength(contour, True)
            approx = cv2.approxPolyDP(contour, 0.02*perimeter, True)    
            
            cv2.drawContours(imgCountour, approx, -1, (0,255,0), 5)

            x, y, w, h = cv2.boundingRect(approx)
            
            cv2.rectangle(imgCountour, (x,y), (x + w, y + h), (0,255,0), thickness=2)
            cv2.putText(imgContours, 'Area: {:d}'.format(int(area)), (x+w+5, y+20), cv2.FONT_HERSHEY_COMPLEX, 0.7, (0,255,0), 2)
            cv2.putText(imgContours, 'Points: {:d}'.format(int(len(approx))), (x+w+5, y+45), cv2.FONT_HERSHEY_COMPLEX, 0.7, (0,255,0), 2)
            # print(len(approx))
            

while True:
    _, img = video.read()
    # img = cv2.resize(img, (myWidth, myHeight))
    imgCountour = img.copy()
    imgGray= cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    T1 = cv2.getTrackbarPos('T1', 'Canny Threshold')
    T2 = cv2.getTrackbarPos('T2', 'Canny Threshold')

    imgGray = cv2.GaussianBlur(imgGray, (7,7), 0)
    imgEdge = cv2.Canny(imgGray, T1, T2)
    
    kernel = np.ones((3,3))
    imgEdge = cv2.dilate(imgEdge, kernel)

    getCountours(imgEdge, imgCountour)

    imgEdge = cv2.cvtColor(imgEdge, cv2.COLOR_GRAY2BGR)
    imgStaked = np.hstack((img, imgEdge, imgCountour))

    cv2.imshow('', imgStaked)
    # cv2.imshow('Video', img)
    # cv2.imshow('Edge', imgEdge)
    if cv2.waitKey(1) == (0xFF & ord('q')):
        break