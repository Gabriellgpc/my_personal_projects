import cv2
import numpy as np

def getContours(image, cThr=[100,100], minArea = 1000, filter=0,showCanny = False, drawContours=False):
    imgGray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    imgBlur = cv2.GaussianBlur(imgGray, (5,5), 1)
    imgCanny= cv2.Canny(imgGray, cThr[0], cThr[1])

    kernel = np.ones((5,5))
    imgDial = cv2.dilate(imgCanny, kernel, iterations=3)
    imgThre = cv2.erode(imgDial, kernel, iterations=2)

    if showCanny: 
        cv2.imshow('Canny Image', imgCanny)
        # cv2.imshow('Dial Image', imgDial)
        # cv2.imshow('Thre Image', imgThre)

    contours, hierarchy = cv2.findContours(imgThre, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    outContours = []
    for contour in contours:
        area = cv2.contourArea(contour)
        if area > minArea:
            perimeter = cv2.arcLength(contour, True)
            approx = cv2.approxPolyDP(contour, 0.02*perimeter, True)
            bbox = cv2.boundingRect(approx)

            if filter != 0:
              if len(approx) == filter:
                    outContours.append( {'approx_size':len(approx), 
                                        'area':area, 
                                        'approx':approx, 
                                        'bbox':bbox, 
                                        'contour':contour})
            else:
                outContours.append( {'approx_size':len(approx), 
                                    'area':area, 
                                    'approx':approx, 
                                    'bbox':bbox, 
                                    'contour':contour})
    outContours = sorted(outContours, key= lambda x:x['area'], reverse=True)

    if drawContours:
        for contour in outContours:
            cv2.drawContours(image, contour['contour'], -1, (255,0,255), 2)

    return image, outContours


def reorderPoints(points):
    orderedPoints = np.zeros_like(points)
    points = points.reshape((4,2))
    
    add = points.sum(axis=1)
    orderedPoints[0] = points[ np.argmin(add) ]
    orderedPoints[3] = points[ np.argmax(add) ]
    
    diff= np.diff(points, axis=1)

    orderedPoints[1] = points[ np.argmin(diff) ]
    orderedPoints[2] = points[ np.argmax(diff) ]

    return orderedPoints

def warpImage(image, points, w, h, pad=20, horizontal=False):
    points = reorderPoints(points)
    
    pts1 = np.float32(points)

    if horizontal:
        w,h = (h,w)
    
    pts2 = np.float32([[0,0],[w,0],[0,h],[w,h]])
    
    M = cv2.getPerspectiveTransform(pts1, pts2)

    imgWarp = cv2.warpPerspective(image, M, (w,h))
    imgWarp = imgWarp[pad:imgWarp.shape[0]-pad, pad:imgWarp.shape[1]-pad]

    return imgWarp


def distance(pt1, pt2):
    return ((pt2[0] - pt1[0])**2 + (pt2[1] - pt1[1])**2)**0.5