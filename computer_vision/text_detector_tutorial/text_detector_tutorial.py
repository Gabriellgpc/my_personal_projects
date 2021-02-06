# https://nanonets.com/blog/ocr-with-tesseract/
import cv2
import pytesseract

image = cv2.imread('image_texto_teste.png')
(h,w) = image.shape[:2]

string= pytesseract.image_to_string(image)
print(f'String in image:\n{string}')

boxes = pytesseract.image_to_boxes(image)

# Detection characteres
# for box in boxes.splitlines():
#     (c, x1, y1, x2, y2) = box.split(' ')[:5]
#     x1, y1, x2, y2 =  map(int, (x1, y1, x2, y2))
    
#     cv2.rectangle(image, (x1,h-y1), (x2,h-y2), (0,0,255), 2)
#     cv2.putText(image, c, (x1,h-y1+20) , cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,0,255), 2)

# Detection words
# data = pytesseract.image_to_data(image)
# for d in data.splitlines()[1:]:
#     # print(d)
#     d = d.split()
#     print(d)
#     if len(d) == 12:
#         text = d[-1]
#         (x1, y1, width, height) = map(int, d[6:10])
#         pt1 = (x1, y1)
#         pt2 = (x1 + width, y1 + height)
#         cv2.rectangle(image, pt1, pt2, (0,0,255), 2)
#         cv2.putText(image, text, (x1,y1-5) , cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,0,255), 2)

# Only digits
conf = r'--oem 3 --psm 6 outputbase digits'
data = pytesseract.image_to_data(image, config=conf)
for d in data.splitlines()[1:]:
    # print(d)
    d = d.split()
    print(d)
    if len(d) == 12:
        text = d[-1]
        (x1, y1, width, height) = map(int, d[6:10])
        pt1 = (x1, y1)
        pt2 = (x1 + width, y1 + height)
        cv2.rectangle(image, pt1, pt2, (0,0,255), 2)
        cv2.putText(image, text, (x1,y1-5) , cv2.FONT_HERSHEY_SIMPLEX, 0.7, (0,0,255), 2)

cv2.imshow('Image', image)
cv2.waitKey(0)