import struct
import sys


if len(sys.argv) != 3:
    print "Usage python mkhead <file> <load address>"
else:
    mlo_name = sys.argv[1]
    mlo_loadadr = int(sys.argv[2],16)
    print "load at "+hex(mlo_loadadr)+" ..."
    mlo_target = open("MLO","wb")
    mlo_header = open("mlo_header", "rb")
    #one sector = 512bytes
    temp = mlo_header.read(512)
    if len(temp) != 512:
        print "illegal header, error"
    mlo_header.close()
    mlo_target.write(temp)
    mlo_target.flush()
    mlo_img = open(mlo_name,"rb")
    img = ""
    while True:
        img_temp = mlo_img.read(256)
        img = img + img_temp 
        if len(img_temp) == 0:
            break
    #pack as gp_header
    gp_header = struct.pack("<2I",len(img),mlo_loadadr)
    mlo_target.write(gp_header)
    mlo_target.write(img)
    mlo_target.flush()
    mlo_target.close()
    print "done"
    


