import os
from ctypes import *
dir_struct_size=32
file_struct_size=32
buff=[]
buff_ptr=0
next_dir=[0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]
sz=[dir_struct_size]
level=-1
fsfmt_size=4*256*2
folder="./filesystem"
fs_start=0xE00000
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
    item=(c_int8*sz[0])(*buff)
    test_files=open("kernel/fat.bin", "+ab")
    test_files.write(item)
    test_files.close()

def list_files(path):
    f1=[]
    for i in os.listdir(path):
        if(i.endswith(".o") or i.endswith(".bmp") or i.endswith(".txt") or i.endswith(".ri")):
            f1.append(i)
    return f1

def parse_fs(path, ptr, ind):
    global buff
    global buff_ptr
    global next_dir
    global sz
    global fs_start
    global level
    level+=1
    d=next(os.walk(path))[1]
    f=list_files(path)
    ptr2=buff_ptr
    t=buff_ptr+fs_start
    dir=path.split("/")[-1]
    append_dir([".",dir, 0, 0, (ptr+fs_start), 0])
    buff_ptr+=dir_struct_size
    if(ind==0):
        for i in range(4):
            try:
                buff[ptr+20+i]=t%0x100
            except IndexError:
                print(f"Index Error!! {len(buff)} {next_dir[level]} {buff_ptr}")
                exit()
            t//=0x100

    elif(ind>0):
        for i in range(4):
            try:
                buff[next_dir[level]+28+i]=t%0x100
            except IndexError:
                print(f"Index Error!! {len(buff)} {next_dir[level]} {buff_ptr}")
                exit()
            t//=0x100
    next_dir[level]=buff_ptr-dir_struct_size
    
    if(len(f)>0):
        t=buff_ptr+fs_start
        for i in range(4):
            buff[(ptr2)+16+i]=t%0x100
            t//=0x100
        sz[1]=os.path.getsize(path+"/"+f[0])
        if(sz[1]%512>0):
            sz[1]=sz[1]//512
            sz[1]=sz[1]*512+512
        append_file([",",f[0], sz[0]+fsfmt_size, sz[1], 0])
        sz[0]+=sz[1]
        buff_ptr+=file_struct_size
        temp=open(path+"/"+f[0],"rb")
        f1=temp.read()
        temp.close()
        temp=open("files.bin","+ab")
        temp.write(f1)
        s=sz[1]-os.path.getsize(path+"/"+f[0])
        for i in range(s):
            temp.write(b'\0')
        temp.close()
    for j in range(1, len(f)):
        t=buff_ptr+fs_start
        for i in range(4):
            buff[(buff_ptr-file_struct_size)+28+i]=t%0x100
            t//=0x100
        sz[1]=os.path.getsize(path+"/"+f[j])
        if(sz[1]%512>0):
            sz[1]=sz[1]//512
            sz[1]=sz[1]*512+512
        append_file([",",f[j], sz[0]+fsfmt_size, sz[1], 0])
        sz[0]+=sz[1]
        buff_ptr+=file_struct_size
        temp=open(path+"/"+f[j],"rb")
        f1=temp.read()
        temp.close()
        temp=open("files.bin","+ab")
        temp.write(f1)
        s=sz[1]-os.path.getsize(path+"/"+f[j])
        for i in range(s):
            temp.write(b'\0')
        temp.close()
    for i in d:
        parse_fs(path+"/"+i, ptr2, d.index(i))
    level-=1

def calc_fat_size(path):
    global sz
    d=next(os.walk(path))[1]
    f=list_files(path)
    sz[0]+=(len(d)*dir_struct_size + len(f)*file_struct_size)
    for i in d:
        if('.' not in i):
            new_path=path+"/"+i
            calc_fat_size(new_path)

calc_fat_size(folder)
if(sz[0]%512>0):
    sz[0]=sz[0]//512
    sz[0]=sz[0]*512+512
sz[0]+=os.path.getsize("./temp.bin")
sz.append(0)
test_files=open("./kernel/fat.bin", "w")
test_files.close()
test_files=open("files.bin", "w")
test_files.close()
parse_fs(folder, 0, 0)
sz=[dir_struct_size]
calc_fat_size(folder)
t=sz[0]+fs_start
if(sz[0]%512>0):
    sz[0]=sz[0]//512
    sz[0]=sz[0]*512+512
append_item()
test_files=open("./kernel/fat.bin", "+ab")
test_files.write(t.to_bytes(4, "little"))
t=fs_start+(fsfmt_size-1)
test_files.write(t.to_bytes(4, "little"))
for i in range(8, fsfmt_size):
    test_files.write(b"\0")
test_files.close()