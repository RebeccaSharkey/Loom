import subprocess

def run_cmake_configure(project_root):
    subprocess.run(["cmake", "--preset", "msvc"], cwd=project_root, check=True)
    print("Engine Updated")

if __name__ == "__main__":
    import sys
    if len(sys.argv) == 3  and sys.argv[1] == "remake":
        run_cmake_configure(sys.argv[2])
    else:
        print("Usage: python LoomUpdateEngine.py remake <DestinationPath>")