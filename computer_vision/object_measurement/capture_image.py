import cv2

video = cv2.VideoCapture('/dev/video0')

while True:
    ok, img = video.read()

    cv2.imshow('Camera', img)
    key = cv2.waitKey(1) & 0xFF
    if key == ord('q'):
        break
    if key == ord('s'):
        cv2.imwrite('imagem.png', img)