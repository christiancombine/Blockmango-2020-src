#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import argparse
import json
import objstorage
import room_tool
import sys
import imp

imp.reload(sys)

def get_games_config():
    file = open('games.json')
    content = file.read()
    return content and json.loads(content) or None


def get_min_version():
    file = open("../../../Media/engineVersion.json")
    content = file.read()
    if content:
        version_config = json.loads(content)
        return version_config['engineVersion']

class Tool:
    config_prefix = "game-resource-config_"
    min_version = "10029"
    room_util = room_tool.RoomTool('test')

    def __init__(self, games, engines):
        self.games = games
        self.engines = engines
        self.work_dir = os.getcwd()
        self.game_list = get_games_config()
        self.min_version = get_min_version()

        self.test_obj_manager = objstorage.S3ObjManager('test')
        self.test_config_folder = self.test_obj_manager.s3client.config_obj.folder
        self.release_obj_manager = objstorage.S3ObjManager('oversea')
        self.release_config_folder = self.release_obj_manager.s3client.config_obj.folder

    def get_game_config(self, game_id, release):
        if not release:
            file = self.test_config_folder + self.config_prefix + game_id
            print (u'获取配置文件 ' + file)
            content = self.test_obj_manager.s3client.config_obj.getObject(file)
            return content and json.loads(content) or None
        else:
            file = self.release_config_folder + self.config_prefix + game_id
            print (u'获取配置文件 ' + file)
            content = self.release_obj_manager.s3client.config_obj.getObject(file)
            return content and json.loads(content) or None

    def start(self):
        games = []
        for game in self.game_list:
            if game['name'] != 'All':
                games.append(game)
        for game in games:
            if game['id'] in self.games or 'All' in self.games:
                self.copy_release_game(game)

    def copy_release_game(self, game):
        print '------------------------------------------------------'
        config = self.get_game_config(game['id'], True)
        if not config:
            print '找不到正式服游戏配置文件：' + game['id']
            return

        max_version = '0'
        for version in config.keys():
            if int(max_version) < int(version):
                max_version = version
        game_config = config[max_version]

        config = self.get_game_config(game['id'], False)
        if not config:
            print '找不到测试服游戏配置文件：' + game['id']
            return

        changed = False
        for version in config.keys():
            if version in self.engines or 'All' in self.engines:
                changed = True
                config[version] = game_config
        if not changed:
            print '测试服游戏配置文件没有改变：' + game['id']
            return

        file = self.test_config_folder + self.config_prefix + game['id']
        result = self.test_obj_manager.s3client.config_obj.putStringObject(file, json.dumps(config, indent=4))
        if result:
            for version in config.keys():
                if version in self.engines or 'All' in self.engines:
                    self.room_util.notify_game_update(version, game['id'])
            print (u'游戏[' + game['id'] + u']同步正式服配置成功')
        else:
            print (u'游戏[' + game['id'] + u']同步正式服配置失败')


def parse_args():
    description = u'一键同步正式服游戏配置到测试服脚本'
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('-g', '--games', default='All',
                        required=False, help=u'需要同步的游戏，默认所有游戏，多个游戏用‘,’隔开')
    parser.add_argument('-e', '--engines', default='All',
                        required=False, help=u'测试服引擎版本号，默认所有，多个版本用‘,’隔开')
    return parser.parse_args()


def main():
    args = parse_args()
    games = None
    if args.games != '':
        games = args.games.split(',')

    engines = None
    if args.engines != '':
        engines = args.engines.split(',')

    tool = Tool(games, engines)
    tool.start()


if __name__ == '__main__':
    main()
