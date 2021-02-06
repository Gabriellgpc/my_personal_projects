import tesserocr
import time

print( tesserocr.tesseract_version() )
print( tesserocr.get_languages() )

tik = time.time()
image = tesserocr.Image.open('image_texto_teste.png')
string= tesserocr.image_to_text(image)
tok = time.time()

print(string)
print(f'Time = {tok - tik} s')