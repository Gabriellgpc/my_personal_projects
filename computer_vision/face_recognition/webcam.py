import cv2
import time

#####################
fold = 'ImageAttendance'
iW = 640
#####################

video = cv2.VideoCapture('/dev/video0')
assert(video.isOpened() == True)

ok, img = video.read()
h, w, c = img.shape

iH = int(h*640/w)

while True:
    ok, img = video.read()
    assert(ok == True)
    img = cv2.resize(img, (iW, iH))

    cv2.imshow('Camera', img)

    key = cv2.waitKey(1) & 0xFF
    if key == ord('q'):
        break
    if key == ord('s'):
        name = input('Quem é?')
        cv2.imwrite(f'{fold}/{name}.png', img)
        # cv2.imwrite(f'{fold}/image_{int(time.time())}.png', img)