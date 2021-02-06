import cv2
import numpy as np

myWidth, myHeight = 360, 480
pts_orig = np.zeros((4,2), dtype='float32')
count = 0

def mousePoint(event, x, y, flags, params):
    global pts_orig
    global count
    global img
    if event == cv2.EVENT_LBUTTONDOWN and count != 4:
        pts_orig[count] = x,y
        count = count + 1 
        print(pts_orig)

img = cv2.imread('resources/foto_3x4.jpg')
img = cv2.resize(img, (myWidth, myHeight))

cv2.imshow('Image', img)
cv2.setMouseCallback('Image', mousePoint)

while count < 4:
    for i in range(4):
        cv2.circle(img, (pts_orig[i][0], pts_orig[i][1]), 3, (0,255,0), thickness=cv2.FILLED)
    cv2.waitKey(2)
    cv2.imshow('Image', img)

pts_dest = np.array([[0,0],
                     [myWidth,0],
                     [0,myHeight],
                     [myWidth,myHeight]
                    ], dtype = 'float32')

M = cv2.getPerspectiveTransform(pts_orig, pts_dest)
imgWarped = cv2.warpPerspective(img, M, (myWidth, myHeight), flags=cv2.INTER_CUBIC)

cv2.imshow('Image Warped', imgWarped)

cv2.waitKey(0)