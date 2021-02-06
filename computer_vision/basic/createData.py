import cv2
import time
import os
####################################
path = 'resources/rawdata'
saveImage = False
showImage = True
stepFrame = 10
imgWidth = 180
imgHeight= 125
minBlur  = 600
maxSavedImg = 5000
####################################

video = cv2.VideoCapture('/dev/video0')

global countFolder
countFolder = 0
while os.path.exists('{}/data{}'.format(path, countFolder)):
    countFolder += 1

folder = '{}/data{}'.format(path, countFolder)
os.makedirs(folder)

imgCount = 0
imgSavedCount = 0
while True:
    ok, img = video.read()
    assert(ok == True)

    img = cv2.resize(img, (imgWidth, imgHeight))    
    imgGray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    # Quanto maior a derivada menos borrada é a imagem (menor blur)
    blur = cv2.Laplacian(imgGray, cv2.CV_64F).var()

    if showImage:
        # img = cv2.resize(img, (640, 480))
        # text = 'Blur: {}'.format(int(blur))
        # cv2.putText(img, text, (5,10), cv2.FONT_HERSHEY_SIMPLEX, 0.4, (0,100,0))
        cv2.imshow('Video', img)

    if saveImage and (blur > minBlur):
        currTime = time.time()
        save_image_name = "{}/{}_{}_{}.png".format(folder, imgSavedCount, blur ,currTime)
        cv2.imwrite(save_image_name, imgGray)
        imgSavedCount += 1
    
    imgCount += 1
    if cv2.waitKey(1) == (0xFF & ord('q')) or imgSavedCount == maxSavedImg:
        break