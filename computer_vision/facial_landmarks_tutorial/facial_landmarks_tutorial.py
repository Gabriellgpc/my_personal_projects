# Jaw line = 0 to 16
# Left Eyebrow = 17 to 21
# Right Eyebrow = 22 to 26
# Nose = 27 to 35
# Left Eye = 36 to 41
# Right Eye = 42 to 47
# Lips = 48 to 60
# Mouth Opening = 61 to 67
import cv2
import numpy as np
import dlib

#####################################
webcam = True

rangeLeftEye  = np.arange(36,42)
rangeRightEye = np.arange(42,48)
rangeLips = np.arange(48,61)
#####################################


def getMask(img, points, color=(255,255,255)):
    mask = np.zeros_like(img)
    cv2.fillPoly(mask, [points], color)
    return mask

def getROI(img, points):
    bbox = cv2.boundingRect(points)
    x, y, w, h = bbox
    roi = img[y:y+h,x:x+w]
    # roi = cv2.resize(roi, (0,0), None, scale, scale)
    return roi

def empty(a):
    pass

detector = dlib.get_frontal_face_detector()
predictor= dlib.shape_predictor("shape_predictor_68_face_landmarks.dat")

cv2.namedWindow('Output')
cv2.createTrackbar('R', 'Output', 0, 255, empty)
cv2.createTrackbar('G', 'Output', 0, 255, empty)
cv2.createTrackbar('B', 'Output', 0, 255, empty)

video = None
if webcam:
    video = cv2.VideoCapture('/dev/video0')
    assert(video.isOpened() == True)

while True:
    if webcam:
        ok, img = video.read()
        assert(ok == True)
    else:
        img = cv2.imread('Gabriel.png')
    
    imgGray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    faces = detector(imgGray)
    points= []
    for face in faces:
        x1, y1 = face.left(), face.top()
        x2, y2 = face.right(), face.bottom()
        
        # cv2.rectangle(img, (x1,y1), (x2,y2), (0,255,0), 2)
        landmarks = predictor(imgGray, face)
        for n in range(68):
            x, y = landmarks.part(n).x, landmarks.part(n).y
            points.append([x,y])
            # cv2.circle(img, (x,y), 2, (0,0,255), cv2.FILLED)
            # cv2.putText(img, f'{n}', (x,y-5), cv2.FONT_HERSHEY_PLAIN, 0.5, (0,0,255), 1)
        points = np.array(points)

        # Lips colored
        b = cv2.getTrackbarPos('B', 'Output')
        g = cv2.getTrackbarPos('G', 'Output')
        r = cv2.getTrackbarPos('R', 'Output')
        
        imgGray3C = cv2.cvtColor(imgGray, cv2.COLOR_GRAY2BGR)

        mask = getMask(img, points[rangeLeftEye], (b,g,r))
        mask = cv2.GaussianBlur(mask, (5,5), 0)
        output = cv2.addWeighted(imgGray3C, 1, mask, 0.5, 0)

        mask = getMask(img, points[rangeRightEye], (r,g,b))
        mask = cv2.GaussianBlur(mask, (5,5), 0)
        output = cv2.addWeighted(output, 1, mask, 0.5, 0)

        mask = getMask(img, points[rangeLips], (b,g,r))
        mask = cv2.GaussianBlur(mask, (5,5), 0)
        output = cv2.addWeighted(output, 1, mask, 0.5, 0)
        
        output = np.hstack([img, output])
        cv2.imshow('Output', output)
        cv2.destroyWindow('Webcam')
    
    if(len(faces) == 0):
        cv2.imshow('Webcam', img)
        cv2.destroyWindow('output')
    if cv2.waitKey(1) == ord('q'):
        break
# imgJawLine = getROI(img, points[0:17])
# imgLeftEyebrow = getROI(img, points[17:22])
# imgRightEyebrow = getROI(img, points[22:27])
# imgNose    = getROI(img, points[27:36])
# imgLeftEye = getROI(img, points[36:42])
# imgRightEye= getROI(img, points[42:48])
# imgLips    = getROI(img, points[rangeLips])
# imgMouth   = getROI(img, points[61:68])

# cv2.imshow('Jaw line', imgJawLine)
# cv2.imshow('Left Eyebrown', imgLeftEyebrow)
# cv2.imshow('Right Eyebrown', imgRightEyebrow)
# cv2.imshow('Left Eye', imgLeftEye)
# cv2.imshow('Right Eye', imgRightEye)
# cv2.imshow('Nose', imgNose)
# cv2.imshow('Lips', imgLips)
# cv2.imshow('Mouth Opening', imgMouth)

