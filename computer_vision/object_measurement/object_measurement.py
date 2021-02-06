import cv2
import numpy as np
import utils

#################
scale = 3
# [mm]
wPaper = 210 * scale
hPaper = 296 * scale
#################

img = cv2.imread('1.png')
h, w, c = img.shape

imgContour, contours = utils.getContours(img, cThr=[50,150], filter=4,showCanny=False, drawContours=False)

if len(contours) != 0:
    biggest = contours[0]
    imgWarp = utils.warpImage(img, biggest['approx'], wPaper, hPaper, horizontal=True)
    
    cv2.imshow('Warped Image', imgWarp)
    
    imgContour2, contours2 = utils.getContours(imgWarp, 
                                               cThr=[15,30],
                                               showCanny=True,
                                               drawContours=False)                                        


    if len(contours2) != 0:
        for contour in contours2:
            print(contour['approx'])
            if contour['approx_size'] == 4:
                points = utils.reorderPoints(contour['approx'])
                p1p2 = utils.distance(points[0][0]//scale, points[1][0]//scale)
                p1p3 = utils.distance(points[0][0]//scale, points[2][0]//scale)
                print(p1p2, p1p3)
            else:
                x, y, w, h = cv2.boundingRect( contour['approx'] )
                print( utils.distance( [x//scale,y//scale], [(x+w)//scale, (y+h)//scale] ))
            cv2.polylines(imgContour2, [contour['approx']], True, (0,255,0), thickness=2)
    
    cv2.imshow('Image Contour 2', imgContour2)

# cv2.imshow('Image', img)
cv2.waitKey(0)