#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import argparse
import json
import time
import objstorage
import hashlib
import zip_tool
import rm_zip
import sys
import room_tool
import zipfile
import imp

imp.reload(sys)

def get_file_count(path):
    count = 0
    zip_file = zipfile.ZipFile(path)
    zip_list = zip_file.namelist()
    for f in zip_list:
        count = count + 1
    zip_file.close()
    return  count

class Tool:
    obj_manager = None
    package = "Plugins"
    config_folder = "sandbox-test/config/server/"
    config_prefix = "game-resource-config_"
    min_version = "10029"

    def __init__(self, engines, games, area):
        self.package = "Plugins"
        self.work_dir = os.getcwd()
        self.engines = engines
        self.games = games
        self.release = area.find('test') == -1
        self.game_list = self.get_games_config()
        self.min_version = self.get_min_version()
        self.obj_manager = objstorage.S3ObjManager(area)
        self.config_folder = self.obj_manager.s3client.config_obj.folder
        self.room_util = room_tool.RoomTool(area)
        self.errors = []

    def get_games_config(self):
        file = open('games.json')
        content = file.read()
        return content and json.loads(content) or None

    def get_min_version(self):
        file = open("../../../Media/engineVersion.json")
        content = file.read()
        if content:
            version_config = json.loads(content)
            return version_config['engineVersion']

    def new_file_name(self, name):
        parts = name.split('.')
        now = int(round(time.time() * 1000))
        ext = parts[-1]
        parts[-1] = str(now)
        parts.append(ext)
        return '.'.join(parts)

    def get_game_id_by_name(self, name):
        game_ids = []
        for game in self.game_list:
            if game['name'] == name:
                game_ids.append(game['id'])
        return game_ids

    def upload_plugins(self):
        if len(self.room_util.dispatcher_urls) == 0:
            return
        plugins = []
        filters = []
        names = []
        if self.games:
            for game in self.game_list:
                if game['id'] in self.games:
                    if game['name'] not in names or game['name'] == 'All':
                        filters.append(game)
                        names.append(game['name'])

        for filter in filters:
            group_path = os.path.join(self.work_dir, filter['group'])
            for file in os.listdir(group_path):
                if filter and file != filter['name'] and filter['name'] != 'All':
                    continue
                filepath = os.path.join(group_path, file)
                if not os.path.isdir(filepath):
                    continue
                name = file + ".zip"
                filepath = os.path.join(self.package, name)
                if not os.path.isfile(filepath):
                    print (u'请先打包插件 ' + file)
                    continue
                plugin_file = open(filepath, 'rb')
                md5 = hashlib.md5()
                md5.update(plugin_file.read())
                plugin_file.close()
                url = self.obj_manager.s3client.plugin_obj.putFileObject(filepath, self.new_file_name(name))
                hash = md5.hexdigest()
                game_ids = self.get_game_id_by_name(file)
                for game_id in game_ids:
                    plugins.append({
                        'gameId': game_id,
                        'url': url,
                        'hash': hash,
                        'fileCount': get_file_count(filepath),
                        'fileSize': os.path.getsize(filepath)
                    })
        self.update_plugins_config(plugins)

    def update_plugins_config(self, plugins):
        for plugin in plugins:
            self.update_plugin_config(plugin)

        print ("------------------------------------------------------------------------")
        for error in self.errors:
            print (u'脚本未更新成功 ' + error)

        if len(self.errors) > 0:
            raise Exception(u'Existing game script has not been updated successfully!')

    def get_plugin_config(self, game_id):
        file = self.config_folder + self.config_prefix + game_id
        print (u'获取配置文件 ' + file)
        content = self.obj_manager.s3client.config_obj.getObject(file)
        return content and json.loads(content) or None

    def update_plugin_config(self, plugin):
        game_id = plugin['gameId']
        if not game_id:
            return
        print ("------------------------------------------------------------------------")
        plugin_config = self.get_plugin_config(game_id)
        if not plugin_config:
            return
        engines = []
        for engine in plugin_config:
            if engine in self.engines:
                if self.release:
                    if int(engine) >= int(self.min_version):
                        engines.append(engine)
                else:
                    engines.append(engine)
        if 'All' in self.engines and not self.release:
            engines.append('All')

        if len(engines) == 0:
            self.errors.append(game_id)
            return

        notifies = []

        for engine in engines:
            if engine == 'All':
                for c_engine in plugin_config:
                    engine_config = plugin_config[c_engine]
                    engine_config['plugin'] = plugin['url']
                    engine_config['pluginHash'] = plugin['hash']
                    engine_config['fileSize'] = plugin['fileSize']
                    engine_config['fileCount'] = plugin['fileCount']
                    notifies.append(c_engine)
            else:
                engine_config = plugin_config[engine]
                engine_config['plugin'] = plugin['url']
                engine_config['pluginHash'] = plugin['hash']
                engine_config['fileSize'] = plugin['fileSize']
                engine_config['fileCount'] = plugin['fileCount']
                notifies.append(engine)

        file = self.config_folder + self.config_prefix + game_id
        result = self.obj_manager.s3client.config_obj.putStringObject(file, json.dumps(plugin_config, indent=4))
        if result:
            for notify in notifies:
                self.room_util.notify_game_update(notify, game_id)


def parse_args():
    description = u'自动上传游戏插件脚本'

    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('-e', '--engines', default='All',
                        required=False, help=u'需要上传的引擎版本，默认所有版本，多个版本用‘,’隔开')
    parser.add_argument('-g', '--games', default='g1001_g1030',
                        required=False, help=u'需要上传的游戏，默认g1001_g1030，多个游戏用‘,’隔开')
    parser.add_argument('-a', '--area', default='test',
                        required=False, help=u'服务器，默认是测试服(test/oversea/vanguard/china)')
    parser.add_argument('-p', '--password', default='',
                        required=False, help=u'操作正式环境(oversea/vanguard/china)需要的密码')
    parser.add_argument('--release', action="store_true",
                        help=u'是否为正式环境(True会将area参数改为oversea)')
    parser.add_argument('--unzip', action="store_true",
                        help=u'是否不打包插件')
    parser.add_argument('--remove', action="store_true",
                        help=u'是否需要删除插件')
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

    engines = None
    if args.engines != '':
        engines = args.engines.split(',')

    games = None
    if args.games != '':
        games = args.games.split(',')

    if not args.unzip:
        zip = zip_tool.Tool(games)
        zip.zip_plugins()

    tool = Tool(engines, games, args.area)
    tool.upload_plugins()

    if args.remove:
        rm_zip.remove_all_file()


if __name__ == '__main__':
    main()
