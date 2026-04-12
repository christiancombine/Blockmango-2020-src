#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import argparse
import zipfile
import json
import sys
import importlib
import shutil
import md5_tool

importlib.reload(sys)

def md5_game_plugin(name):
    target_path = 'Plugins/' + name
    if not os.path.exists(target_path):
        os.makedirs(target_path)
    # unzip map
    if not os.path.exists(target_path):
        os.makedirs(target_path)
    zip_file = zipfile.ZipFile('Plugins/' + name + '.zip')
    zip_list = zip_file.namelist()
    for f in zip_list:
        zip_file.extract(f, target_path)
    zip_file.close()
    os.remove('Plugins/' + name + '.zip')

    # create md5 sum file
    checksum = ""
    for root, dirs, files in os.walk(target_path):
        for file in files:
            file_path = os.path.join(root, file)
            file_path = file_path.replace('\\', '/')
            file_path = file_path.replace(target_path + '/', '*')
            checksum = checksum + (md5_tool.md5_file(os.path.join(root, file)) + ' ' + file_path + '\n')

    checksum_file = open(target_path + '/checksums.md5', 'w')
    checksum_file.write(checksum)
    checksum_file.close()

class Tool:
    work_dir = '.'

    def __init__(self, games):
        self.work_dir = os.getcwd()
        self.games = games
        self.game_list = self.get_game_config()

    def zip_plugins(self):
        files = []

        filters = []
        if self.games:
            for g in self.game_list:
                if g['id'] in self.games:
                    filters.append(g)

        for filter in filters:
            group_path = os.path.join(self.work_dir, filter['group'])
            for f in os.listdir(group_path):
                if filter and f != filter['name'] and filter['name'] != 'All':
                    continue
                filepath = os.path.join(group_path, f)
                if os.path.isdir(filepath):
                    print (u'打包插件:' + f)
                    files.append(self._zip_plugin(f, filepath))
        return files

    def _zip_plugin(self, name, path):
        plugins_path = os.path.join(self.work_dir, 'Plugins')
        if not os.path.exists(plugins_path):
            os.makedirs(plugins_path)
        filename = "Plugins/" + name + '.zip'
        file = zipfile.ZipFile(filename, 'w', zipfile.ZIP_DEFLATED)
        self._zip_dir(file, os.path.join(path.replace('ServerGame', 'BaseGame'), 'main'))
        self._zip_dir(file, os.path.join(path, 'main'))
        self._zip_dir(file, os.path.join(self.work_dir.replace('ServerGame', 'BaseGame'), 'Common'))
        self._zip_dir(file, os.path.join(self.work_dir, 'Common'))
        file.close()
        md5_game_plugin(name)
        file = zipfile.ZipFile(filename, 'w', zipfile.ZIP_DEFLATED)
        self._zip_dir(file, 'Plugins/' + name)
        file.close()
        if os.path.exists('Plugins/' + name):
            shutil.rmtree('Plugins/' + name)
        return filename

    def _zip_dir(self, zfile, path):
        for root, dirs, files in os.walk(path):
            for file in files:
                if os.path.basename(file).find(".lua") != -1 or os.path.basename(file).find(".md5") != -1:
                    zfile.write(os.path.join(root, file), os.path.relpath(
                        os.path.join(root, file), os.path.join(path)))

    def get_game_config(self):
        file = open('games.json')
        content = file.read()
        return content and json.loads(content) or None

def parse_args():
    description = u'自动打开游戏插件脚本'

    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('-g', '--games', default='g1001_g1030',
                        required=False, help=u'需要打包的游戏，默认g1001_g1030，多个游戏用‘,’隔开')
    return parser.parse_args()


def main():
    args = parse_args()

    games = None
    if args.games != '':
        games = args.games.split(',')

    tool = Tool(games)
    tool.zip_plugins()

if __name__ == '__main__':
    main()
