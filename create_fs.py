import os
from ctypes import *
dir_struct_size=32
file_struct_size=32

def append_file(f):
    file_list=[ord(f[0])]
    s=len(f[1])
    for i in range(19):
        if(i<s):
            file_list.append(ord(f[1][i]))
        else:
            file_list.append(0)
    for i in range(4):
        file_list.append(f[2]%0x100)
        f[2]//=0x100
    for i in range(4):
        file_list.append(f[3]%0x100)
        f[3]//=0x100
    file=(c_int8*file_struct_size)(*file_list)
    test_files=open("kernel/fat.bin", "+ab")
    test_files.write(file)
    test_files.close()

def append_dir(f):
    dir_list=[ord(f[0])]
    s=len(f[1])
    for i in range(15):
        if(i<s):
            dir_list.append(ord(f[1][i]))
        else:
            dir_list.append(0)
    dir=(c_int8*dir_struct_size)(*dir_list)
    test_files=open("kernel/fat.bin", "+ab")
    test_files.write(dir)
    test_files.close()

def list_files(path):
    f1=[]
    for i in os.listdir(path):
        if(i.endswith(".o") or i.endswith(".bmp") or i.endswith(".txt")):
            f1.append(i)
    return f1

def calc_file_dir_num(c, path):
    d=[]
    f=[]
    try:
        d=next(os.walk(path))[1]
    except:
        pass
    f=list_files(path)
    if(len(d)==0 and len(f)==0):
        return
    c[0]+=len(d)
    c[1]+=len(f)
    for i in d:
        calc_file_dir_num(c, path+"/"+i)

def parse_fs(path, prev_dirs, fat_sz):
    d=next(os.walk(path))[1]
    f=list_files(path)
    delim=''
    if(len(d)>1):
        if(len(next(os.walk(path+"/"+d[0]))[1])==0):
            i=1
            while(len(next(os.walk(path+"/"+d[i]))[1])==0 and len(list_files(path))==0):
                i+=1
            t=d[0]
            d[0]=d[i]
            d[i]=t
    if(len(d)>0):
        delim='.'
        for i in d:
            if(d.index(i)==len(d)-1 and len(f)==0):
                if(len(prev_dirs)>1):
                    delim=';'
                else:
                    delim='/'
            append_dir([delim, i])
    if(len(f)>0):
        delim=','
        for i in f:
            if(f.index(i)==len(f)-1):
                if(len(prev_dirs)>1):
                    new_path=path.split("/")
                    for k in prev_dirs:
                        if(k==path.split("/")[-1]):
                            continue
                        c=[0,0]
                        new_path[-1]=k
                        calc_file_dir_num(c, "/".join(new_path))
                    if(c[1]>0):
                        delim=' '
                    else:
                        delim='|'
                else:
                    delim='|'
            fat_sz[1]=os.path.getsize(path+"/"+i)
            if(fat_sz[1]%512>0):
                fat_sz[1]=fat_sz[1]//512
                fat_sz[1]=fat_sz[1]*512+512
            append_file([delim, i, fat_sz[0], fat_sz[1]])
            fat_sz[0]+=fat_sz[1]
            temp=open(path+"/"+i,"rb")
            f1=temp.read()
            temp.close()
            temp=open("files.bin","+ab")
            temp.write(f1)
            s=fat_sz[1]-os.path.getsize(path+"/"+i)
            for i in range(s):
                temp.write(b'\0')
            temp.close()
    for i in d:
        if('.' not in i):
            new_path=path+"/"+i
            parse_fs(new_path, d, fat_sz)
    
def calc_fat_size(path, s):
    d=next(os.walk(path))[1]
    f=list_files(path)
    s[0]+=(len(d)*dir_struct_size + len(f)*file_struct_size)
    for i in d:
        if('.' not in i):
            new_path=path+"/"+i
            calc_fat_size(new_path, s)

sz=[0]
folder="./filesystem"
calc_fat_size(folder,sz)
if(sz[0]%512>0):
    sz[0]=sz[0]//512
    sz[0]=sz[0]*512+512
sz[0]+=os.path.getsize("./temp.bin")
sz.append(0)
test_files=open("./kernel/fat.bin", "w")
test_files.close()
test_files=open("files.bin", "w")
test_files.close()
parse_fs(folder, [], sz)
test_files=open("kernel/fat.bin", "+ab")
test_files.write(b"!")
test_files.close()