import os
from ctypes import *
dir_struct_size=32
file_struct_size=32
buff=[]
buff_ptr=0
next_dir=0
def append_file(f):
    global buff
    buff.append(ord(f[0]))
    s=len(f[1])
    for i in range(19):
        if(i<s):
            buff.append(ord(f[1][i]))
        else:
            buff.append(0)
    for i in range(4):
        buff.append(f[2]%0x100)
        f[2]//=0x100
    for i in range(4):
        buff.append(f[3]%0x100)
        f[3]//=0x100
    for i in range(4):
        buff.append(f[4]%0x100)
        f[4]//=0x100

def append_dir(f):
    global buff
    buff.append(ord(f[0]))
    s=len(f[1])
    for i in range(15):
        if(i<s):
            buff.append(ord(f[1][i]))
        else:
            buff.append(0)
    for i in range(4):
        buff.append(f[2]%0x100)
        f[2]//=0x100
    for i in range(4):
        buff.append(f[3]%0x100)
        f[3]//=0x100
    for i in range(4):
        buff.append(f[4]%0x100)
        f[4]//=0x100
    for i in range(4):
        buff.append(f[5]%0x100)
        f[5]//=0x100
    
def append_item():
    global buff
    item=(c_int8*len(buff))(*buff)
    test_files=open("kernel/fat.bin", "+ab")
    test_files.write(item)
    test_files.close()

def list_files(path):
    f1=[]
    for i in os.listdir(path):
        if(i.endswith(".o") or i.endswith(".bmp") or i.endswith(".txt")):
            f1.append(i)
    return f1

def parse_fs(path, ptr, ptr1, ind):
    global buff
    global buff_ptr
    global next_dir
    d=next(os.walk(path))[1]
    f=list_files(path)
    ptr2=buff_ptr
    t=buff_ptr+ptr1
    if(ind==0):
        dir=path.split("/")[-1]
        append_dir([".",dir, 0, 0, (ptr+ptr1), 0])
        buff_ptr+=dir_struct_size
        for i in range(4):
            try:
                buff[ptr+20+i]=t%0x100
            except IndexError:
                print(f"Index Error!! {len(buff)} {next_dir} {buff_ptr}")
                exit()
            t//=0x100

    elif(ind>0):
        dir=path.split("/")[-1]
        append_dir([".",dir, 0, 0, (ptr+ptr1), 0])
        buff_ptr+=dir_struct_size
        for i in range(4):
            try:
                buff[next_dir+28+i]=t%0x100
            except IndexError:
                print(f"Index Error!! {len(buff)} {next_dir} {buff_ptr}")
                exit()
            t//=0x100
    next_dir=buff_ptr-dir_struct_size
    
    if(len(f)>0):
        t=buff_ptr+ptr1
        for i in range(4):
            buff[(ptr2)+16+i]=t%0x100
            t//=0x100
        append_file([",",f[0], 0, 0, 0])
        buff_ptr+=file_struct_size
    
    for j in range(1, len(f)):
        t=buff_ptr+ptr1
        for i in range(4):
            buff[(buff_ptr-file_struct_size)+28+i]=t%0x100
            t//=0x100
        append_file([",",f[j], 0, 0, 0])
        buff_ptr+=file_struct_size
        
    for i in d:
        parse_fs(path+"/"+i, ptr2, ptr1, d.index(i))
    
folder="./filesystem"
test_files=open("./kernel/fat.bin", "w")
test_files.close()
test_files=open("files.bin", "w")
test_files.close()
parse_fs(folder, 0, 0xE00000, 0)
append_item()