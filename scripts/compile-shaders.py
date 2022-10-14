import os
from subprocess import call

compiler_path = 'glslangValidator'

shaders_path = '../shaders'
shaders_include_path = shaders_path + '/include'

shader_extensions = ('.vert.glsl', '.tesc.glsl', '.tese.glsl', '.geom.glsl', '.frag.glsl', '.comp.glsl')

for root, dirs, files in os.walk(shaders_path):
    if 'include' in dirs:
        dirs.remove('include')

    for file in files:
        if file.endswith(shader_extensions):
            in_path = os.path.join(root, file)
            out_path = os.path.join(root, 'bin', file)
            out_path = out_path.replace('.glsl', '.spv')

            dirpath = os.path.dirname(out_path)
            if not os.path.exists(dirpath):
                os.makedirs(dirpath)

            call([compiler_path, '-G', '-I' + shaders_include_path, in_path, '-o', out_path])
