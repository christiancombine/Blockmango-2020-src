#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import argparse
import json
import objstorage
import upload_tool
import zip_tool
import rm_zip
import hashlib
import sys
import imp

imp.reload(sys)

class Tool:
    config_folder = "sandbox-test/config/server/"
    config_prefix = "game-resource-config_"
    min_version = "10029"

    def __init__(self, games, source, targets, area, upload, replace):
        self.work_dir = os.getcwd()
        self.games = games
        self.source = source
        self.targets = targets
        self.release = area.find('test') == -1
        self.upload = upload
        self.replace = replace
        self.wait_uploads = {}
        self.game_list = self.get_games_config()
        self.min_version = self.get_min_version()
        self.obj_manager = objstorage.S3ObjManager(area)
        self.config_folder = self.obj_manager.s3client.config_obj.folder

    def get_min_version(self):
        file = open("../../../Media/engineVersion.json")
        content = file.read()
        if content:
            version_config = json.loads(content)
            return version_config['engineVersion']

    def get_games_config(self):
        file = open('games.json')
        content = file.read()
        return content and json.loads(content) or None

    def get_game_id_by_name(self, name):
        for game in self.game_list:
            if game['name'] == name:
                return game['id']
        return None

    def get_game_config(self, game_id):
        file = self.config_folder + self.config_prefix + game_id
        print (u'获取配置文件 ' + file)
        content = self.obj_manager.s3client.config_obj.getObject(file)
        return content and json.loads(content) or None

    def copy_games_versions(self):
        if len(self.targets) > 1 and (len(self.games) > 1 or self.games[0] == 'All'):
            print (u'不能同时拷贝多个游戏和多个引擎版本，避免误操作')
            print (u'拷贝规则 [单游戏多版本，单版本多游戏，源版本低于目标版本]')
            return

        copy_targets = []
        copy_games = []
        for target in self.targets:
            if not self.release:
                copy_targets.append(target)
            else:
                if int(target) > int(self.source):
                    if int(target) >= int(self.min_version):
                        copy_targets.append(target)

        for game in self.game_list:
            if (game['id'] in self.games or self.games[0] == 'All') and game['name'] != 'All':
                copy_games.append(game)

        for copy_game in copy_games:
            self.copy_game_versions(copy_game, copy_targets)

        if not self.upload:
            return

        games = []
        engines = self.wait_uploads.keys()
        if len(engines) > 1:
            games = self.games[0]
        else:
            if len(engines) == 1:
                games = self.wait_uploads[engines[0]]

        if len(engines) > 0 and len(games) > 0:
            zip = zip_tool.Tool(games)
            zip.zip_plugins()

            tool = upload_tool.Tool(engines, games, self.release)
            tool.upload_plugins()

            rm_zip.remove_all_file()

    def copy_game_versions(self, game, versions):
        if len(versions) == 0 and self.release:
            return
        config = self.get_game_config(game['id'])

        if not config:
            return

        if (not self.release) and (not self.source in config.keys()):
            print (u'游戏[' + game['id'] + u']源版本[' + self.source + u']不存在')
            return

        success_versions = []
        for version in versions:
            if not self.replace:
                if version in config.keys():
                    print (u'游戏[' + game['id'] + u']目标版本[' + version + u']已存在')
                    continue

            if not self.release:
                config[version] = config[self.source]
            else:
                old_max_version = '0'
                for key in config.keys():
                    if int(old_max_version) < int(key):
                        old_max_version = key
                config[version] = config[old_max_version]
            success_versions.append(version)

        if len(success_versions) == 0:
            return

        file = self.config_folder + self.config_prefix + game['id']
        result = self.obj_manager.s3client.config_obj.putStringObject(file, json.dumps(config, indent=4))
        if result:
            for version in success_versions:
                print (u'游戏[' + game['id'] + u']目标版本[' + version + u']拷贝成功')
                if not version in self.wait_uploads.keys():
                    self.wait_uploads[version] = []
                self.wait_uploads[version].append(game['id'])
        else:
            for version in success_versions:
                print (u'游戏[' + game['id'] + u']目标版本[' + version + u']拷贝失败')


def parse_args():
    description = u'自动拷贝引擎版本插件脚本'

    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('-g', '--games', default='All',
                        required=False, help=u'需要拷贝的游戏，默认所有游戏，多个游戏用‘,’隔开')
    parser.add_argument('-s', '--source', default='10028',
                        required=False, help=u'拷贝的源引擎版本，默认10028')
    parser.add_argument('-t', '--targets', default='10029',
                        required=False, help=u'拷贝的目标引擎版本，默认10029，多个版本用‘,’隔开')
    parser.add_argument('-a', '--area', default='test',
                        required=False, help=u'服务器(test/oversea/vanguard/china)')
    parser.add_argument('-p', '--password', default='',
                        required=False, help=u'操作正式环境(oversea/vanguard/china)需要的密码')
    parser.add_argument('--release', action="store_true",
                        help=u'是否为正式环境(True会将area参数改为oversea)')
    parser.add_argument('--replace', action="store_true",
                        help=u'是否替换已有的版本')
    parser.add_argument('--upload', action="store_true",
                        help=u'是否上传当前插件到目标版本')

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

    source = None
    if args.source != '':
        source = args.source

    targets = None
    if args.targets != '':
        targets = args.targets.split(',')

    tool = Tool(games, source, targets, args.area, args.upload, args.replace)
    tool.copy_games_versions()


if __name__ == '__main__':
    main()
