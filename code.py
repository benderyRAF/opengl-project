
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
redOffset = 0
blueOffset = 2

file = open('image_pixels.txt', 'r')

alltext = file.read()

lines = alltext.split("*")
img = []
for line in lines:
    for pixel in line.split("\n"):
        if pixel != "":
            img.append(pixel.split(" ")[0])
            img.append(pixel.split(" ")[1])
            img.append(pixel.split(" ")[2])

plt.imshow(img)