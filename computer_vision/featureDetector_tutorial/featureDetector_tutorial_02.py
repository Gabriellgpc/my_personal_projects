import cv2
import numpy as np

video = cv2.VideoCapture('/dev/video0')

img1 = cv2.imread('featureDetector_tutorial/ImagesQuery/rei.png',0)

orb = cv2.ORB_create(nfeatures=500)
kp1, des1 = orb.detectAndCompute(img1, None)

bf = cv2.BFMatcher()
while True:
    ok, img2 = video.read()
    assert(ok == True)
    img2 = cv2.cvtColor(img2, cv2.COLOR_BGR2GRAY)

    kp2, des2 = orb.detectAndCompute(img2, None)
    matches = bf.knnMatch(des1, des2, k = 2)
    
    good = []
    for m,n in matches:
        if m.distance < 0.5*n.distance:
            good.append([m])
    
    cv2.imshow('Video', img2)
    
    if len(good) > 0:
        imgMath = cv2.drawMatchesKnn(img1, kp1, img2, kp2, good, None, flags=2)
        cv2.imshow('Match', imgMath)
    
    if cv2.waitKey(1) == ord('q'):
        break