#   SPDX-FileCopyrightText: (c) 2019 Jin-Eon Park <greengb@naver.com> <sigma@gm.gist.ac.kr>
#   SPDX-License-Identifier: MIT License
#=========#=========#=========#=========#=========#=========#=========#=========#=========#=========


import os
import subprocess
import shutil
import json


def to_dir(dirs):
    if len(dirs) == 0:
        return ""

    first_non_empty_idx = -1

    for i in range( 0, len(dirs) ):
        if dirs[i] != "":
            first_non_empty_idx = i
            break
    
    if first_non_empty_idx == -1:
        return ""

    res = dirs[first_non_empty_idx]

    for i in range( first_non_empty_idx + 1, len(dirs) ):
        if dirs[i] != "":
            res += '/' + dirs[i]

    return res


def is_dir(x):
    return isinstance(x, list)


def remote_os_name():
    if os.name == "nt":
        return "posix"
    elif os.name == "posix":
        return "nt"
    else:
        return "unknown" 
#---------#---------#---------#---------#---------#---------#---------#---------#---------#---------


class Temporary_Copy:
    original_root_to_title_dir = ""
    target_root_to_title_dir = ""
    project_name = ""

    def __init__(self, root_to_title_dir, project_name):
        self.original_root_to_title_dir = root_to_title_dir
        self.target_root_to_title_dir = root_to_title_dir + '/_Temp'
        self.project_name = project_name

        os.mkdir(self.target_root_to_title_dir)


    def __del__(self):
        shutil.rmtree(self.target_root_to_title_dir)


    def copy_to_temp_dir(self, title_to_dir, file):
        src_file = to_dir([self.original_root_to_title_dir, title_to_dir, file])
        tgt_dir = to_dir([self.target_root_to_title_dir,  title_to_dir])
        
        subprocess.call(['cp', src_file, tgt_dir])


    def sync(self, title_to_dir, file_tree):
        sub_file_tree = to_dir([ title_to_dir, file_tree[0] ])

        os.mkdir( to_dir([self.target_root_to_title_dir, sub_file_tree]) )

        if len(file_tree) <= 1: # empty directory
            return

        for i in range( 1, len(file_tree) ):
            if is_dir(file_tree[i]):
                dir = file_tree[i]
                self.sync(sub_file_tree, dir)
            else:
                file = file_tree[i]
                self.copy_to_temp_dir(sub_file_tree, file)
#---------#---------#---------#---------#---------#---------#---------#---------#---------#---------


def main():
    print("transfer.py starts. It may take a few minutes. . .")

    file = open("_private_config.json")
    js = json.load(file)

    proj_name = js["project_name"]

    proj_tree = \
    [proj_name
    ,   ["include"
        ,   ["SGM"
            ,   ["Abbreviable"
                ,   'Abbreviable.hpp'
                ,   'Avatar.hpp'
                ,   'Test_Avatar.hpp'
                ,   'Test_Avatar.cpp'
                ,   'Maybe.hpp'
                ,   'Test_Maybe.hpp'
                ,   'Test_Maybe.cpp'
                ,   'Pinweight.hpp'
                ,   'Test_Pinweight.hpp'
                ,   'Test_Pinweight.cpp'
                ]
            ,   ["Allocator"
                ,   'Allocator.hpp'
                ]
            ,   ["Array"
                ,   'Array.hpp'
                ,   'Test_Array.hpp'
                ,   'Test_Array.cpp'
                ]
            ,   ["Boomerang"
                ,   'Boomerang.hpp'
                ,   'Test_Boomerang.hpp'
                ,   'Test_Boomerang.cpp'
                ]
            ,   ["Concurrency"
                ,   'Concurrency.hpp'
                ,   'Test_Concurrency.hpp'
                ,   'Test_Concurrency.cpp'
                ]
            ,   ["Exception"
                ,   'Exception.hpp'
                ]
            ,   ["Family"
                ,   'Family.hpp'
                ,   'Test_Family.hpp'
                ,   'Test_Family.cpp'
                ]
            ,   ["Flag_Set"
                ,   'Flag_Set.hpp'
                ,   'Test_Flag_Set.hpp'
                ,   'Test_Flag_Set.cpp'
                ]
            ,   ["Functor"
                ,   'Parameters.hpp'
                ,   'Test_Parameters.hpp'
                ,   'Test_Parameters.cpp'
                ,   'Functor.hpp'
                ,   'Test_Functor.hpp'
                ,   'Test_Functor.cpp'
                ]
            ,   ["High_Templar"
                ,   'Countable.hpp'
                ,   'High_Templar.hpp'
                ,   'Test_High_Templar.hpp'
                ,   'Test_High_Templar.cpp'
                ]
            ,   ["interface_Traits"
                ,   'interface_Traits.hpp'
                ,   'Test_interface_Traits.hpp'
                ,   'Test_interface_Traits.cpp'
                ]
            ,   ["iterable"
                ,   'iterable.hpp'
                ,   'Test_iterable.hpp'
                ,   'Test_iterable.cpp'
                ]
            ,   ["List"
                ,   'List.hpp'
                ,   'Test_List.hpp'
                ,   'Test_List.cpp'
                ]
            ,   ["Mathexpr"
                ,   'Mathexpr.hpp'
                ,   'Test_Mathexpr.hpp'
                ]
            ,   ["Operators_of"
                ,   'Operators_of.hpp'
                ,   'Test_Operators_of.hpp'
                ,   'Test_Operators_of.cpp'
                ]
            ,   ["Recursion"
                ,   'Recursion.hpp'
                ,   'Test_Recursion.hpp'
                ,   'Test_Recursion.cpp'
                ]
            ,   ["Span"
                ,   'Span.hpp'
                ,   'Test_Span.hpp'
                ,   'Test_Span.cpp'
                ]
            ,   ["Specification"
                ,   'Specification.hpp'
                ,   'Specification.cpp'
                ,   'Test_Specification.hpp'
                ,   'Test_Specification.cpp'
                ,   'Test_Specification.md'
                ]
            ,   ["Type_Analysis"
                ,   'Type_Analysis.hpp'
                ,   'Test_Type_Analysis.hpp'
                ,   'Test_Type_Analysis.cpp'
                ]
            ]
        ,   ["S3D"
            ,   ["Affine"
                ,   'Affine.hpp'
                ,   'Test_Affine.hpp'
                ,   'Test_Affine.cpp'
                ]
            ,   ["Decomposition"
                ,   'Decomposition.hpp'
                ,   '_Decomposition_by_Eigen.hpp'
                ,   'Test_Decomposition.hpp'
                ,   'Test_Decomposition.cpp'
                ]
            ,   ["Euclid"
                ,   'Euclid.hpp'
                ,   'Test_Euclid.hpp'
                ,   'Test_Euclid.cpp'
                ]
            ,   ["Hamilton"
                ,   'Hamilton.hpp'
                ,   '_Hamilton_by_Eigen.hpp'
                ,   'Test_Hamilton.hpp'
                ,   'Test_Hamilton.cpp'
                ]
            ,   ["Quaternion"
                ,   'Quaternion.hpp'
                ,   'Test_Quaternion.hpp'
                ,   'Test_Quaternion.cpp'
                ]
            ]
        ]
    ,   ["platforms"
        ,   ["Clang"
            ,   'CMakeLists.txt'
            ,   'Clang_main.cpp'
            ]
        ,   ["GPP"
            ,   'CMakeLists.txt'
            ,   'GPP_main.cpp'
            ]
        ,   ["VCPP"
            ,   'CMakeLists.txt'
            ,   'VCPP_main.cpp'
            ]
        ,   ["VisualStudio2019"
            ,   ["All_Build"
                ,   'All_Build.vcxproj'
                ,   'All_Build.vcxproj.filters'
                ,   'main.cpp'
                ]
            ,   ["VisualStudio2019"
                ,   'VisualStudio2019.vcxproj'
                ,   'VisualStudio2019.vcxproj.filters'
                ,   'VS2019_main.cpp'
                ,   'postbuild.bat'
                ]
            ,   'VisualStudio2019.sln'
            ]
        ,   'execute.py'
        ,   'generate.py'
        ,   'init.py'
        ,   'WorldTest.hpp'
        ,   'WorldTest.cpp'
        ]
    ,   'LICENSE'
    ,   '.gitignore'    
    ,   '_private_config.json'
    ,   'transfer.py'
    ]

    remote_id, local_id \
    =   (js["linux_id"], js["windows_id"]) if os.name == "nt" \
        else (js["windows_id"], js["linux_id"])

    remote_root_to_title, local_root_to_title \
    =   (js["linux_dir"], js["windows_dir"]) if os.name == "nt" \
        else (js["windows_dir"], js["linux_dir"])

    temp_copy = Temporary_Copy(local_root_to_title, proj_name)
    temp_copy.sync("", proj_tree)

    subprocess.call \
    (   [   'scp', '-r', to_dir([local_root_to_title, '_Temp', proj_name])
        ,   remote_id + ':' + remote_root_to_title
        ]
    )

    print("transfer.py ends.")
#---------#---------#---------#---------#---------#---------#---------#---------#---------#---------

main()
