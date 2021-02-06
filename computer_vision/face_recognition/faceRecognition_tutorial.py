import cv2
import numpy as np
import face_recognition
import os

#########################
path = "ImageAttendance"
#########################

images = []
encodeknownList = []
classNames = []
myListDir  = os.listdir(path)

print(myListDir)

# Import images and compute face encoders
for cl in myListDir:
    img = cv2.imread(f'{path}/{cl}')
    images.append(img)
    classNames.append(os.path.splitext(cl)[0])

    encode = face_recognition.face_encodings(img)[0]
    encodeknownList.append(encode)

print(classNames)

video = cv2.VideoCapture('/dev/video0')
assert(video.isOpened() == True)

while True:
    ok, img = video.read()
    assert(ok == True)
    imgS = cv2.resize(img, (0,0), None, 0.25, 0.25)

    facesLoc = face_recognition.face_locations(imgS)
    encodeUnknownList = face_recognition.face_encodings(imgS)

    for faceLoc, encodeUnknown in zip(facesLoc, encodeUnknownList):
        dist    = face_recognition.face_distance(encodeknownList, encodeUnknown)
        mindist = np.min(dist)

        if mindist < 0.6:
            name = classNames[np.argmin(dist)]
            y1, x2, y2, x1 = np.array(faceLoc) * 4
            cv2.rectangle(img, (x1,y1), (x2,y2), (0,255,0), 2)
            cv2.rectangle(img, (x1,y2-35), (x2,y2), (0,255,0), cv2.FILLED)
            cv2.putText(img, name, (x1+6,y2-6), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255,255,255))

    cv2.imshow('Camera', img)
    if cv2.waitKey(1) == ord('q'):
        break
