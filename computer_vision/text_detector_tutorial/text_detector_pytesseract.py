import cv2
import pytesseract
import time


print(pytesseract.get_tesseract_version())
print(pytesseract.get_languages())

tik = time.time()
image = cv2.imread('image_texto_teste.png')
string= pytesseract.image_to_string(image)
tok = time.time()

print(string)

print(f'Time = {tok - tik} s')