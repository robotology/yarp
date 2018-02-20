# Copyright (C) 2006-2018 Istituto Italiano di Tecnologia (IIT)
# Copyright (C) 2007 Giacomo Spigler
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

nvidia:
  Driver to interface with NVIDIA GPUs, allowing 
  general purpose computing.

libraries needed:
  opengl (debian/ubuntu: libgl1-mesa-dev)
  glu    (debian/ubuntu: libglu1-mesa-dev)
  glut   (debian/ubuntu: freeglut3-dev)
  glew   (debian/ubuntu: libglew-dev)
  Cg Toolkit 1.5 (http://developer.nvidia.com/object/cg_toolkit.html)
    (or http://ftp.cica.es/debian/pool/contrib/n/nvidia-cg-toolkit/)
