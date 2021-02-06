import cv2
import numpy as np
from pyzbar.pyzbar import decode

video = cv2.VideoCapture('/dev/video0')
assert(video.isOpened() == True)

while True:
    ok, img = video.read()

    assert(ok == True)

    for code in decode(img):
        
        pts = np.array([code.polygon], dtype=np.int32)
        pts = pts.reshape((-1,1,2))
        cv2.polylines(img, [pts], True, (255,0,255), thickness=2)
        
        x, y, w, h = code.rect
        # pt1 = x, y
        # pt2 = x+w, y+h
        # cv2.rectangle(img, pt1, pt2, (0,255,0), thickness=2)

        msg = code.data.decode('utf-8')
        cv2.putText(img, msg, (x, y - 20), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (255,0,255), thickness=2)

    cv2.imshow('Video', img)
    if cv2.waitKey(1) == (0xFF & ord('q')):
        break

