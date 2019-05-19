import sys
import os
from subprocess import call


validator_path = 'glslangValidator'

shaders_dir =  "shaders/"

in_path = "{}shader.comp".format(shaders_dir)
out_path = "{}compute.spv".format(shaders_dir)

if not os.path.exists(out_path):
    open(out_path, 'x').close()

call([validator_path, "-V", in_path, "-o", out_path])
