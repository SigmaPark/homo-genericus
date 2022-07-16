#   SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
#   SPDX-License-Identifier: MIT License
#=========#=========#=========#=========#=========#=========#=========#=========#=========#=========


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


def NT_platforms(exe_list):
    null_list = []

    ms_v141_d \
    =   "cmake --build . --config Debug \
        -- /p:CharacterSet=Unicode /p:PlatformToolset=v141 /p:Platform=x64\
        "
    
    ms_v141_r \
    =   "cmake --build . --config Release \
        -- /p:CharacterSet=Unicode /p:PlatformToolset=v141 /p:Platform=x64\
        "  

    Build("Debug/", "VCPP", "Visual Studio 16 2019", ms_v141_d, null_list)
    Build("Release/", "VCPP", "Visual Studio 16 2019", ms_v141_r, exe_list)       


def main():
    exe_list = []

    if os.name == "nt":
        NT_platforms(exe_list)

    if os.name == "posix":
        Posix_platforms(exe_list)

    print("generate.py ends.")


main()