import cv2
import numpy as np

####################################
classFile = 'cocoNames/coco.names'
classNames=[]
WH_target = 320
modelConfig = "yolov3.cfg"
modelWeights= "yolov3.weights"

webcam = False
saveimg= True
image_file = 'me.jpeg'
#####################################
if webcam:
    video = cv2.VideoCapture('/dev/video0')
    assert(video.isOpened() == True)


with open(classFile, 'rt') as f:
    classNames = f.read().rstrip('\n').split('\n')

# modelConfig = "yolov3-tiny.cfg"
# modelWeights= "yolov3-tiny.weights"

net = cv2.dnn.readNetFromDarknet(modelConfig, modelWeights)

net.setPreferableBackend(cv2.dnn.DNN_BACKEND_OPENCV)
net.setPreferableTarget(cv2.dnn.DNN_TARGET_CPU)

def findObject(outputs, img, confidenceThr = 0.5, nms_threshold=0.3):
    hT, wT, cT = img.shape
    bboxes = []
    classIds = []
    confs = []

    for output in outputs:
        for det in output:
            scores = det[5:]
            classId= np.argmax(scores)
            confidence = scores[classId]
            if confidence > confidenceThr:
                w, h = int(det[2]*wT), int(det[3]*hT)
                x, y = int(det[0]*wT - w/2.0), int(det[1]*hT - h/2.0)
                bboxes.append([x,y,w,h])
                classIds.append(classId)
                confs.append(float(confidence))
    indices = cv2.dnn.NMSBoxes(bboxes, confs, score_threshold = confidenceThr, nms_threshold=nms_threshold)
    
    for i in indices:
        i = i[0]
        name  = classNames[classIds[i]]
        confi = confs[i]
        x, y, w, h = bboxes[i]
        
        cv2.rectangle(img, (x,y), (x+w,y+h), (0,255,0), 2)
        cv2.rectangle(img, (x,y-20), (x+w,y), (0,255,0), cv2.FILLED)
        cv2.putText(img, ('{}:{:.2f}'.format(name, confi)).upper(), 
                    (x,y-2), 
                    cv2.FONT_HERSHEY_SIMPLEX, 0.6, (0,0,0), 2)

def showDetections(detections, img):
    for i, classId in enumerate(detections[0]):
        name  = classNames[classId]
        confi = detections[1][i]
        x, y, w, h = detections[2][i]
        
        print(detections[2][i])
        cv2.rectangle(img, (x,y), (x+w,y+h), (0,255,0), 2)
        cv2.rectangle(img, (x,y-10), (x+w,y), (0,255,0), cv2.FILLED)
        cv2.putText(img, '{}:{:.2f}'.format(name, confi), (x,y-2), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,0,0), 2)
        print(name)
while True:
    start = cv2.getTickCount()
    if webcam:
        ok, img = video.read()
        assert(ok == True)
    else:
        img = cv2.imread(image_file)
    

    blob = cv2.dnn.blobFromImage(img, 1/255.0, (WH_target, WH_target), [0,0,0], 1, crop=False)
    net.setInput(blob)
    
    layerNames = net.getLayerNames()
    outputNames= [layerNames[i[0] - 1] for i in net.getUnconnectedOutLayers()]
    
    output = net.forward(outputNames)
    
    fps = cv2.getTickFrequency() / (cv2.getTickCount() - start)

    findObject(output, img, 0.3, 0.3)

    # cv2.putText(img, f'FPS = {int(fps)}', (20,20), cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,0,255), 2)
    cv2.imshow('Video', img)
    if saveimg:
        cv2.imwrite('output.jpg', img)
        cv2.waitKey(0)
        break
    if cv2.waitKey(1) == ord('q'):
        break