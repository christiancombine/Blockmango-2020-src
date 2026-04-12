#!/usr/bin/python
# -*- coding: utf-8 -*-
import os, shutil
import res_tool
import argparse
import game_res_ver_tool

class Tool:

    def __init__(self, domain, game_id):
        self.domain = domain
        self.game_id = game_id
        self.game_media_path = res_tool.get_game_media_path()
        self.server_script_path = res_tool.get_server_script_path()
        self.app_media_path = res_tool.get_app_media_path()
        self.filter_dirs = []
        self.filter_files = []
        self.init_filter_dir_and_file()

    def init_filter_dir_and_file(self):
        game_res_config = res_tool.get_game_resource_config()

        for game in game_res_config:
            for dir in game['dirs']:
                for root, dirs, files in os.walk(self.game_media_path + '/' + dir):
                    root = root.replace('\\', '/')
                    if root not in self.filter_dirs:
                        self.filter_dirs.append(root)
            for file in game['files']:
                path = self.app_media_path + '/' + file
                if path not in self.filter_files:
                    self.filter_files.append(path)

        ids = res_tool.get_need_game_ids([], self.game_id, game_res_config)

        for game in game_res_config:
            if game['id'] in ids:
                for dir in game['dirs']:
                    for root, dirs, files in os.walk(self.game_media_path + '/' + dir):
                        root = root.replace('\\', '/')
                        if root in self.filter_dirs:
                            self.filter_dirs.remove(root)
                for file in game['files']:
                    path = self.app_media_path + "/" + file
                    if path in self.filter_files:
                        self.filter_files.remove(path)

    def copy_resource(self):
        print("------------------start copy game resource-------------------")
        res_tool.remove_dir(self.app_media_path)
        shutil.copytree(self.game_media_path, self.app_media_path)
        shutil.copytree(res_tool.get_game_shader_path(), res_tool.get_app_shader_path())
        res_tool.remove_dir(self.server_script_path)
        for root, dirs, files in os.walk(self.app_media_path):
            root = root.replace('\\', '/')
            for file in files:
                file_path = root + "/" + file
                if file_path in self.filter_files:
                    os.remove(file_path)
            if root.replace(self.app_media_path, self.game_media_path) in self.filter_dirs:
                res_tool.remove_dir(root)

        game_ver_tool = game_res_ver_tool.GameResVerTool(self.domain)
        game_ver_tool.write_res(self.game_id)


def parse_args():
    description = u'auto copy resources script.'
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('-d', '--domain', default='http://mods.sandboxol.com/',
                        required=False, help=u'Host, eg:http://mods.sandboxol.com/, default http://mods.sandboxol.com/')
    parser.add_argument('-g', '--game', default='blockymods',
                        required=False, help=u'GameId, eg:g1001, default blockymods')
    return parser.parse_args()


def main():
    args = parse_args()

    tool = Tool(args.domain, args.game)
    tool.copy_resource()


if __name__ == '__main__':
    main()
