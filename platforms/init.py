#   SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
#   SPDX-License-Identifier: MIT License
#=========#=========#=========#=========#=========#=========#=========#=========#=========#=========


import os
import shutil


def remove_if_exists(dir):
    if os.path.exists(dir):
        shutil.rmtree(dir)
        print(dir + " was removed")    
    else:
        print("cannot find " + dir + ". Nothing was removed.")


def main():
    remove_if_exists("GPP/build")
    remove_if_exists("Clang/build")
    remove_if_exists("VCPP/build")
    remove_if_exists("VisualStudio2019/x64")
    remove_if_exists("VisualStudio2019/VisualStudio2019/x64")  
    remove_if_exists("VisualStudio2019/All_Build/x64")


main()
