#   SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
#   SPDX-License-Identifier: MIT License
#=========#=========#=========#=========#=========#=========#=========#=========#=========#=========


import sys
import os
import subprocess


result_dir = "build"


def Report(is_success, name):
    msg = name

    if is_success:
        msg += " build successes"
    else:
        msg += " build failes"

    print()
    print(msg)
    print("--------  --------  --------  --------  --------  ")


def Solution_name(name, configuration):
    return name + '/' + result_dir + '/' + configuration + name + "_solution"


def Build(sub_dir, prefix, tool, make_script, exe_list):
    if os.path.exists(prefix):
        os.chdir(prefix)

        if os.path.exists(result_dir):
            os.chdir(result_dir)
        else:
            os.mkdir(result_dir)
            os.chdir(result_dir)
            
            subprocess.call(["cmake", "..", "-G", tool])

        subprocess.call(make_script.split())
        
        result_file = sub_dir + prefix + "_Solution"

        is_success = os.path.exists(result_file) or os.path.exists(result_file + ".exe")

        Report(is_success, sub_dir + prefix)

        if is_success:
            exe_list += Solution_name(prefix, sub_dir)

        os.chdir("../..")


def Posix_platforms(exe_list):
    Build("", "GPP", "Unix Makefiles", "make", exe_list)
    Build("", "Clang", "Unix Makefiles", "make", exe_list)


def NT_platforms(exe_list, vs_ver):
    null_list = []

    ms_v141_d \
    =   "cmake --build . --config Debug \
        -- /p:CharacterSet=Unicode /p:PlatformToolset=v141 /p:Platform=x64\
        "
    
    ms_v141_r \
    =   "cmake --build . --config Release \
        -- /p:CharacterSet=Unicode /p:PlatformToolset=v141 /p:Platform=x64\
        "  

    Build("Debug/", "VCPP", vs_ver, ms_v141_d, null_list)
    Build("Release/", "VCPP", vs_ver, ms_v141_r, exe_list)       


def Ask_Visual_Studio_Version(vs_ver):
    while \
    (   vs_ver != '1' and vs_ver != '2' and vs_ver != '3' and vs_ver != 'q' \
    and not("Visual Studio" in vs_ver)
    ):
        print("Select Visual Studio Version you have")
        print('[1]   Visual Studio 15 2017')
        print('[2]   Visual Studio 16 2019')
        print('[3]   Visual Studio 17 2022')
        print('[q]   quit')
        print('or type it directly')
        vs_ver=input()

    if vs_ver == '1':
        vs_ver = "Visual Studio 15 2017"
    elif vs_ver == '2':
        vs_ver = "Visual Studio 16 2019"
    elif vs_ver == '3':
        vs_ver = "Visual Studio 17 2022"
    
    return vs_ver



def main():
    exe_list = []

    if os.name == "nt":
        vs_ver = Ask_Visual_Studio_Version("" if len(sys.argv) == 1 else sys.argv[1])
        if vs_ver != 'q':
            NT_platforms(exe_list, vs_ver)


    if os.name == "posix":
        Posix_platforms(exe_list)

    print("generate.py ends.")


main()