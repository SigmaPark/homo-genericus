#   SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
#   SPDX-License-Identifier: MIT License
#=========#=========#=========#=========#=========#=========#=========#=========#=========#=========


import os
import platform


def execute_if_exist(file_dir):
    if os.path.exists(file_dir):
        if os.name == "posix":
            os.system(file_dir)
        elif os.name == "nt":
            file_name = file_dir.split('.')[0]
            os.system("powershell ./" + file_name)

    else:
        print("cannot find " + file_dir)


def main():
    if os.name == "nt":
        execute_if_exist("VCPP/build/Release/VCPP_Solution.exe")

    if os.name == "posix":
        if platform.system() == "Linux":
            execute_if_exist("GPP/build/GPP_Solution")
            execute_if_exist("Clang/build/Clang_Solution")
        elif platform.system() == "Darwin":
            execute_if_exist("Apple_Clang/build/Apple_Clang_Solution")

main()