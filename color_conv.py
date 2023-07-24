from PIL import Image
from numpy import asarray
img = Image.open('filesystem/os/img.bmp')
numdata=asarray(img)
out=open('filesystem/os/img.ri', 'wb')
for i in numdata:
    for j in i:
        t=0
        for k in range(len(j)):
            t|=(j[k]<<k*8)
        t=int(t)
        out.write(t.to_bytes(len(j), "big"))
out.close()