from PIL import Image, ImageDraw, ImageFont
import math
import re
 
print "####### INITIALIZING PYTHON IMAGE DRAWING #######"
# GLOBAL VARS
SIZE = (4500, 750)
AAS = 4
LINEWIDTH = 10
TRANSLATEX = 2800 * AAS
TRANSLATEY = (SIZE[1] * AAS) / 2
PADDING = 10 * AAS
SCALE = 85
font = ImageFont.truetype('/Users/zeno/Library/Fonts/Inconsolata.otf', 16 * AAS)
WHITE = (255, 255, 255)
RED = (int(0.9 * 255), int(0.4 * 255), int(0.5 * 255))
BLUE = (36, 71, 91)
GREEN = (int(0.5 * 255), int(0.7 * 255), int(0.25 * 255))
ORANGE = (int(0.8 * 255), int(0.4 * 255), int(0.15 * 255))
GREY = (150, 150, 150)
DARKGREY = (120, 120, 120)


FOCUSDISTANCE = 100
 
 
# DATA VARS
print "PYTHON: Reading data file"
dataFile = open('/Users/zeno/pota/tests/draw.pota', 'r')
 
 
RAYSSTRING = dataFile.readline()
RAYSSTRING = re.sub('[RAYS{}]', '', RAYSSTRING)
RAYSLIST = [float(n) for n in RAYSSTRING.split()]
 
dataFile.close()


list1 = []
list2 = []
lenscounter = 0
 
 
 
# IMAGE WITH ACTUAL RAYS
print "PYTHON: Creating actual rays image"
img = Image.new('RGB', (SIZE[0] * AAS, SIZE[1] * AAS), BLUE)
d = ImageDraw.Draw(img)
 
# ORIGIN LINES
print "PYTHON: ---- Drawing origin lines"
d.line([-5000 + TRANSLATEX, 0 + TRANSLATEY, 15000 + TRANSLATEX, 0 + TRANSLATEY],  DARKGREY, 5)
d.line([0 + TRANSLATEX, 9999 + TRANSLATEY, 0 + TRANSLATEX, -9999 + TRANSLATEY],  DARKGREY, 5)
d.text([0 + TRANSLATEX + PADDING, 0 + TRANSLATEY + PADDING + 1000], "(0, 0)", DARKGREY, font)
d.text([0 + TRANSLATEX + PADDING, 0 + TRANSLATEY + PADDING + 1100], "ORIGIN", DARKGREY, font)
 
# TEMPORARY FOCUS LINE, REMOVE!
d.line([(200 * AAS * SCALE + TRANSLATEX, 9999 * AAS * SCALE + TRANSLATEY), (200 * AAS * SCALE + TRANSLATEX, - 9999 * AAS * SCALE + TRANSLATEY)], ORANGE, 1)
 

# RAYS
print "PYTHON: ---- Drawing rays"
for count in range (0, len(RAYSLIST) / 6):
    d.line([RAYSLIST[(count * 6)] * AAS * SCALE + TRANSLATEX, RAYSLIST[(count * 6) + 1] * AAS * SCALE + TRANSLATEY, RAYSLIST[(count * 6) + 2] * AAS * SCALE + TRANSLATEX,  (RAYSLIST[(count * 6) + 3] * AAS * SCALE) + TRANSLATEY], WHITE, 1)
 

# FOCUS DISTANCE
print "PYTHON: ---- Drawing focus line"
d.line([(FOCUSDISTANCE * AAS) * SCALE + TRANSLATEX, 9999 + TRANSLATEY, (FOCUSDISTANCE * AAS) * SCALE + TRANSLATEX, -9999 + TRANSLATEY],  ORANGE, LINEWIDTH)
d.text([(FOCUSDISTANCE * AAS) * SCALE + TRANSLATEX + PADDING, 0 + TRANSLATEY + PADDING + 1000], "(" + str(FOCUSDISTANCE) + ", 0)", ORANGE, font)
d.text([(FOCUSDISTANCE * AAS) * SCALE + TRANSLATEX + PADDING, 0 + TRANSLATEY + PADDING + 1100], "FOCUSDISTANCE", ORANGE, font)


print "PYTHON: ---- Anti-aliasing image"
img.thumbnail(SIZE, Image.ANTIALIAS)
 
print "PYTHON: Saving image"
img.save('/Users/zeno/pota/tests/lensDrawing.png','png')
