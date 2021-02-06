import cv2
import numpy as np

img = cv2.imread('resources/me.jpg')
img = cv2.resize(img, (306,408))

h, w, _ = img.shape

imgCropped = img[int(h/2):h,:]


cv2.imshow('Image', img)
cv2.imshow('Image Cropped', imgCropped)
cv2.waitKey(0)