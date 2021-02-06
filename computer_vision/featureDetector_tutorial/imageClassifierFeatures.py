import cv2
import numpy as np
import os

#### Import Images
path = 'featureDetector_tutorial/ImagesQuery'
images=[]
classNames=[]
myListDir = os.listdir(path)

print(f'Total Classes Detected {len(myListDir)}')
for cl in myListDir:
    imgCurr = cv2.imread(f'{path}/{cl}', cv2.IMREAD_GRAYSCALE)
    images.append(imgCurr)
    classNames.append( os.path.splitext(cl)[0] )

### Computing the descriptors
orb = cv2.ORB_create(nfeatures=1000)
descriptors = []
for image in images:
    kp, des = orb.detectAndCompute(image, None)
    descriptors.append(des)

## Auxilia function
def findID(img, desList, threshold=30):
    kp2, des2 = orb.detectAndCompute(img, None)
    matchList = []
    classID = -1
    bf = cv2.BFMatcher()
    try:
        for des1 in desList:
            matches = bf.knnMatch(des1, des2, k=2)
            good = []
            for m,n in matches:
                if m.distance < 0.75*n.distance:
                    good.append([m])
            matchList.append(len(good))
    except:
        pass
    if len(matchList) != 0:
        print(matchList)
        if max(matchList) > threshold:
            classID = matchList.index( max(matchList) )
    return classID

### Video capturing
video = cv2.VideoCapture('/dev/video0')
assert(video.isOpened() == True)

while True:
    ok, img = video.read()
    assert(ok == True)
    imgOriginal = img.copy()
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    classID = findID(img, descriptors)
    if classID == -1:
        cv2.putText(imgOriginal, 'Nothing', (25,25), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,0,255), 2)    
    else:
        className = classNames[classID]
        cv2.putText(imgOriginal, className, (25,25), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,255,0), 2)

    cv2.imshow('Camera', imgOriginal)
    if cv2.waitKey(1) == (0xFF & ord('q')):
        break