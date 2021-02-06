import cv2
import numpy as np
import myUtils




img = cv2.imread('resources/me.jpg')
img = cv2.resize(img, (306,408))

imgGray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
imgEdge = cv2.Canny(img, 100, 200)
imgBlur = cv2.GaussianBlur(img, (5,5), 0)

kernel = np.ones((2,2), np.dtype('uint'))

imgDilation = cv2.dilate(imgEdge, kernel, iterations=2)
imgEration  = cv2.erode(imgDilation, kernel, iterations=2)

stackedImages = myUtils.stackImages([[img, imgGray, imgEdge], 
                                     [imgBlur, imgDilation, imgEration]], 
                                     0.01)

cv2.imshow('Image', stackedImages)

# cv2.imshow('GrayScale', imgGray)
# cv2.imshow('Canny Result', imgEdge)
# cv2.imshow('Blur Image', imgBlur)
# cv2.imshow('Dilation Image', imgDilation)
# cv2.imshow('Eration Image', imgEration)

cv2.waitKey(0)