import subprocess
import os

# Paths
source_dir = "./logic"
build_dir = os.path.join(source_dir, "build")
executable_path = os.path.join(build_dir, "logic.exe")


def compile_cpp():
    os.makedirs(build_dir, exist_ok=True)
    subprocess.run(["cmake", "-G", "Ninja", ".."], cwd=build_dir, check=True)
    subprocess.run(["cmake", "--build", "."], cwd=build_dir, check=True)


def run_cpp_with_args(*args):
    str_args = [str(a) for a in args]
    print("Arguments passed to C++:", str_args)
    result = subprocess.run([executable_path] + str_args, capture_output=True, text=True)
    print(result.stdout)


