import cv2
import numpy as np
import argparse

ap = argparse.ArgumentParser()

ap.add_argument("-p", "--prototxt", required=True, 
                help="path to Caffe 'deploy' prototxt file")
ap.add_argument("-m", "--model", required=True, help="path to Caffe pre-treined model")
ap.add_argument("-c", "--confidence", type=float, default=0.5, 
                help="minimum probability to filter weak detections")

args = vars(ap.parse_args())

# load our model from file
print("[INFO] loading model...")
net = cv2.dnn.readNetFromCaffe(args["prototxt"], args["model"])

video = cv2.VideoCapture('/dev/video0')

assert(video.isOpened() == True)
_, image = video.read()
(h,w) = image.shape[:2]

while True:
    tick = cv2.getTickCount()
    ok, image = video.read()
    assert(ok == True)
    # construct the input blob for the image
    # by resizing to a fixed 300x300 and normalizing it
    blob  = cv2.dnn.blobFromImage(cv2.resize(image, (300,300)), 
                                  1.0, 
                                  (300,300), 
                                  (104.0, 177.0, 123.0))
    # print("[INFO] computing object detections...")
    net.setInput(blob)
    detections = net.forward()

    fps = cv2.getTickFrequency() / (cv2.getTickCount() - tick)

    # loop over the detections
    for i in range(detections.shape[2]):
        confidence = detections[0, 0, i,2]

        if confidence > args["confidence"]:
            box = detections[0, 0, i, 3:7] * np.array([w, h, w, h])
            (startX, startY, endX, endY) = box.astype("int")
            
            text = "{:.2f}%".format(confidence * 100)
            y = startY - 10 if startY - 10 > 10 else startY + 10
            cv2.rectangle(image, (startX, startY), (endX, endY), (255, 0, 255), 2)
            cv2.putText(image, text, (startX, y), 
                        cv2.FONT_HERSHEY_SIMPLEX, 0.45, (255, 0, 255), 2)
    cv2.putText(image, f'FPS: {int(fps)}', (20, 20), 
                       cv2.FONT_HERSHEY_SIMPLEX, 0.45, (0, 0, 255), 2)
    cv2.imshow('Output', image)
    if cv2.waitKey(1) == (0xFF & ord('q')):
        break