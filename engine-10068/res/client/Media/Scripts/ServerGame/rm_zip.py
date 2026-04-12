import os
import shutil
import sys
import imp

imp.reload(sys)


def remove_all_file():
    path = os.path.join(os.getcwd(), "Plugins")
    if not os.path.exists(path):
        return
    for file in os.listdir(path):
        os.remove(os.path.join(path, file))
    shutil.rmtree(path)


def main():
    remove_all_file()


if __name__ == '__main__':
    main()
