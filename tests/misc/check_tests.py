# SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

import os
import sys

verbose = 1
verbose_devel = 0
def check_subfolders(root_dir):
    non_compliant_folders = []
    skip_folders = []
    with open('./tests/misc/check_tests_skip.txt', 'r') as f:
        skip_folders = [folder.strip() for folder in f.readlines()]
    if (verbose_devel): print(skip_folders)

    for root_dir in root_dirs:
        for dirpath, dirnames, filenames in os.walk(root_dir):
            if dirpath == root_dir:
                for dirname in dirnames:
                    subfolder_path = os.path.join(dirpath, dirname, 'tests')
                    if (verbose_devel):  print (subfolder_path, skip_folders)
                    if os.path.join(dirpath, dirname) not in skip_folders:
                       if not os.path.exists(subfolder_path):
                          if (verbose): print("Tests NOT found in folder:", os.path.join(dirpath, dirname))
                          non_compliant_folders.append(os.path.join(dirpath, dirname))
                       else:
                          if (verbose): print("Tests found in folder:", os.path.join(dirpath, dirname))
                    else:
                       if (verbose): print("Skipping folder:", os.path.join(dirpath, dirname))
    return non_compliant_folders

root_dirs = ['./src/devices', './src/devices/networkWrappers', './src/devices/fake']

non_compliant_folders = check_subfolders(root_dirs)

if non_compliant_folders:
    print("The following subfolders (",len(non_compliant_folders),") do not contain a 'tests' subfolder:")
    for folder in non_compliant_folders:
        print(folder)
    sys.exit(1)
else:
    print("All subfolders contain a 'test' subfolder.")
    sys.exit(0)
