import cv2
import numpy as np

imgKing  = cv2.imread('featureDetector_tutorial/ImagesQuery/rei.png',0)
imgQueen = cv2.imread('featureDetector_tutorial/ImagesQuery/rainha.png',0)
imgKnave = cv2.imread('featureDetector_tutorial/ImagesQuery/valete.png',0)

orb = cv2.ORB_create(nfeatures=500)

kp_king, des_king = orb.detectAndCompute(imgKing, None)
kp_queen, des_queen = orb.detectAndCompute(imgQueen, None)
kp_knave, des_knave = orb.detectAndCompute(imgKnave, None)

imgKing  = cv2.drawKeypoints(imgKing, kp_king, None)
imgQueen = cv2.drawKeypoints(imgQueen, kp_queen, None)
imgKnave = cv2.drawKeypoints(imgKnave, kp_knave, None)

stacked = np.hstack((imgKing, imgQueen, imgKnave))
cv2.imshow('Features Detected', stacked)
# cv2.imshow('Features Detected',imgKing)
cv2.waitKey(0)