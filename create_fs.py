import os
from ctypes import *


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
    file=(c_int8*32)(*file_list)
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
    file=(c_int8*32)(*dir_list)
    test_files=open("kernel/fat.bin", "+ab")
    test_files.write(file)
    test_files.close()

def list_files(path):
    f1=[]
    for i in os.listdir(path):
        if(i.endswith(".o")):
            f1.append(i)
    return f1

def parse_fs(path, prev_dirs):
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
                if(len(prev_dirs)>0):
                    delim=';'
                else:
                    delim='/'
            append_dir([delim, i])
    if(len(f)>0):
        delim=','
        for i in f:
            if(f.index(i)==len(f)-1):
                if(len(prev_dirs)>0):
                    delim=' '
                else:
                    delim='|'
            append_file([delim, i, 0, 0])
    for i in d:
        if('.' not in i):
            new_path=path+"/"+i
            parse_fs(new_path, d)
    

test_files=open("./kernel/fat.bin", "w")
test_files.close()
parse_fs("./os",[])
test_files=open("kernel/fat.bin", "+ab")
test_files.write(b"!")
test_files.close()