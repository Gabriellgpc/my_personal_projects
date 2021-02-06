import cv2
import numpy as np

img = np.zeros((480, 640, 3), dtype='uint8')

imgBlue = img.copy()
imgBlue[:] = 255,0,0

cv2.line(img, (0,0), (img.shape[1],img.shape[0]), (255,255,0), thickness=2)
cv2.rectangle(img, (200,300), (300,400), (0,255,0), thickness=cv2.FILLED)
cv2.circle(img, (400,50), 50, (0,0,255), thickness=2)
cv2.putText(img, "Draw Shapes", (60,50), cv2.FONT_HERSHEY_COMPLEX, 1, (0,0,200), 1)


cv2.imshow('Image', img)
cv2.imshow('Image Blue', imgBlue)
cv2.waitKey(0)