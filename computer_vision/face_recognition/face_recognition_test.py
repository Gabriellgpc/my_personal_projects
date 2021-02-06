import cv2
import face_recognition

imgGabriel = cv2.imread('base_images/1.png')
imgTest = cv2.imread('base_images/5.png')

faceLoc = face_recognition.face_locations(imgGabriel)[0]
encodeGabriel = face_recognition.face_encodings(imgGabriel)[0]
cv2.rectangle(imgGabriel, (faceLoc[3], faceLoc[0]), (faceLoc[1], faceLoc[2]), (0,255,0), 2)

faceLocTest = face_recognition.face_locations(imgTest)[0]
encodeTest = face_recognition.face_encodings(imgTest)[0]
cv2.rectangle(imgTest, (faceLocTest[3], faceLocTest[0]), (faceLocTest[1], faceLocTest[2]), (0,255,0), 2)


results = face_recognition.compare_faces([encodeGabriel], encodeTest)
print(results)

distance = face_recognition.face_distance([encodeGabriel, encodeGabriel], encodeTest)
print(distance)

cv2.imshow('Gabriel', imgGabriel)
cv2.imshow('Image Test', imgTest)
cv2.waitKey(0)