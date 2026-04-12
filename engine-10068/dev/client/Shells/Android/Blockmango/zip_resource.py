#!/usr/bin/python
# -*- coding: utf-8 -*-
import os, shutil
import res_tool
import argparse
import zipfile


class Tool:

    def __init__(self, game_id):
        self.work_dir = os.getcwd() + '/resources'
        self.game_id = game_id
        self.game_media_path = res_tool.get_game_media_path()

    def zip_resources(self):
        game_res_config = res_tool.get_game_resource_config()
        settings = []
        if self.game_id == 'All':
            for game in game_res_config:
                setting = res_tool.get_setting_by_game_id(game['id'], game_res_config)
                if len(setting['dirs']) > 0 or len(setting['files']) > 0:
                    settings.append(setting)
        else:
            setting = res_tool.get_setting_by_game_id(self.game_id, game_res_config)
            if len(setting['dirs']) > 0 or len(setting['files']) > 0:
                settings.append(setting)

        for setting in settings:
            self.zip_resource(setting)

    def zip_resource(self, setting):
        if not os.path.exists(self.work_dir):
            os.makedirs(self.work_dir)
        filename = "resources/" + setting['game_id'] + '.zip'
        print ('zip resource game id:' + setting['game_id'])
        z_file = zipfile.ZipFile(filename, 'w', zipfile.ZIP_DEFLATED)
        for dir in setting['dirs']:
            res_tool.zip_dir(z_file, os.path.join(self.game_media_path, dir), self.game_media_path)
        for file in setting['files']:
            res_tool.zip_file(z_file, os.path.join(self.game_media_path, file), self.game_media_path)
        z_file.close()


def parse_args():
    description = u'auto zip resources script.'
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('-g', '--game', default='All',
                        required=False, help=u'GameId, eg:g1001, default All')
    return parser.parse_args()


def main():
    args = parse_args()

    tool = Tool(args.game)
    tool.zip_resources()


if __name__ == '__main__':
    main()
