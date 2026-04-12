#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import argparse
import json
import objstorage
import hashlib
import sys
import imp

imp.reload(sys)

class Tool:
    config_folder = "sandbox-test/config/server/"
    config_prefix = "game-resource-config_"

    def __init__(self, games, engines, area):
        self.work_dir = os.getcwd()
        self.games = games
        self.engines = engines
        self.game_list = self.get_games_config()
        self.obj_manager = objstorage.S3ObjManager(area)
        self.config_folder = self.obj_manager.s3client.config_obj.folder

    def get_games_config(self):
        file = open('games.json')
        content = file.read()
        return content and json.loads(content) or None

    def get_game_config(self, game_id):
        file = self.config_folder + self.config_prefix + game_id
        print (u'获取配置文件 ' + file)
        content = self.obj_manager.s3client.config_obj.getObject(file)
        return content and json.loads(content) or None

    def delete_games_versions(self):
        delete_games = []

        for game in self.game_list:
            if (game['id'] in self.games or self.games[0] == 'All') and game['name'] != 'All':
                delete_games.append(game)

        for copy_game in delete_games:
            self.delete_game_versions(copy_game)

    def delete_game_versions(self, game):
        config = self.get_game_config(game['id'])
        if not config:
            return
        success_versions = []
        for version in self.engines:
            if version not in config.keys():
                print (u'游戏[' + game['id'] + u']目标版本[' + version + u']不存在')
                continue
            del config[version]
            success_versions.append(version)

        if len(success_versions) == 0:
            return
        file = self.config_folder + self.config_prefix + game['id']
        result = self.obj_manager.s3client.config_obj.putStringObject(file, json.dumps(config, indent=4))
        if result:
            for version in success_versions:
                print (u'游戏[' + game['id'] + u']目标版本[' + version + u']删除成功')
        else:
            for version in success_versions:
                print (u'游戏[' + game['id'] + u']目标版本[' + version + u']删除失败')


def parse_args():
    description = u'一键删除引擎版本插件脚本'

    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('-g', '--games', default='All',
                        required=False, help=u'需要删除的游戏，默认所有游戏，多个游戏用‘,’隔开')
    parser.add_argument('-e', '--engines', default='90000',
                        required=False, help=u'需要删除的引擎版本，默认90000')
    parser.add_argument('-a', '--area', default='test',
                        required=False, help=u'服务器(test/oversea/vanguard/china)')
    parser.add_argument('-p', '--password', default='',
                        required=False, help=u'操作正式环境(oversea/vanguard/china)需要的密码')
    parser.add_argument('--release', action="store_true",
                        help=u'是否为正式环境(True会将area参数改为oversea)')
    return parser.parse_args()


def main():
    args = parse_args()
    if args.release:
        args.area = 'oversea'

    password_md5 = "3bcd4a5027e51ff2be50560584631100"
    if args.area.find('test') == -1:
        md5 = hashlib.md5()
        md5.update(args.password.encode("utf-8"))
        if password_md5 != md5.hexdigest():
            print (u'正式服需要校验密码，密码错误或未输入密码')
            return

    games = None
    if args.games != '':
        games = args.games.split(',')

    engines = None
    if args.engines != '':
        engines = args.engines.split(',')

    tool = Tool(games, engines, args.area)
    tool.delete_games_versions()


if __name__ == '__main__':
    main()
