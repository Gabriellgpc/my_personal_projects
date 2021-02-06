import cv2
import numpy as np

img = cv2.imread('resources/foto_3x4.jpg')
height, width = img.shape[:2]
height, width = 480, int(480*(width/height))

img = cv2.resize(img, (width, height))

print(img.shape)

pts_orig = np.array([[92,124],
                     [182,90],
                     [167,198],
                     [267,154]
                    ], dtype = 'float32')


pts_dest = np.array([[0,0],
                     [width,0],
                     [0,height],
                     [width,height]
                    ], dtype = 'float32')
M = cv2.getPerspectiveTransform(pts_orig, pts_dest)

print(M)

imgWarped = cv2.warpPerspective(img, M, (width, height), flags=cv2.INTER_CUBIC)

for pt in pts_orig:
    cv2.circle(img, (pt[0],pt[1]), 3, (0,255,0), thickness=cv2.FILLED)
cv2.imshow('Image', img)

cv2.imshow('Image warped', imgWarped)
cv2.waitKey(0)