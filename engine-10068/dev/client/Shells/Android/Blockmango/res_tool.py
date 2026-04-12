#!/usr/bin/python
# -*- coding: utf-8 -*-
import os, shutil
import json

def get_root_path():
    return os.getcwd().replace('\\', '/') + '/../../../../../'

def get_game_res_path():
    return os.getcwd().replace('\\', '/') + '/resources'

def get_app_res_path():
    return os.getcwd().replace('\\', '/') + '/app/src/main/assets/resources'

def get_game_media_path():
    return get_root_path() + 'res/client/Media'

def get_server_script_path():
    return os.getcwd().replace('\\', '/') + '/app/src/main/assets/resources/Media/Scripts/ServerGame'

def get_app_media_path():
    return os.getcwd().replace('\\', '/') + '/app/src/main/assets/resources/Media'

def get_game_shader_path():
    return get_root_path() + 'engine-core/res/client/Media/LordCore/shader'

def get_app_shader_path():
    return os.getcwd().replace('\\', '/') + '/app/src/main/assets/resources/Media/LordCore/shader'

def remove_dir(dir):
    if os.path.exists(dir):
        for root, dirs, files in os.walk(dir):
            for file in files:
                file_path = root + '/' + file
                os.remove(file_path)
        shutil.rmtree(dir)


def get_game_resource_config():
    file = open('game_res_config.json')
    content = file.read()
    return content and json.loads(content) or None


def get_need_game_ids(ids, game_id, game_res_config):
    ids.append(game_id)
    for game in game_res_config:
        if game['id'] == game_id:
            for id in game['child']:
                get_need_game_ids(ids, id, game_res_config)
    return ids


def get_setting_by_game_id(game_id, game_res_config):
    ids = get_need_game_ids([], game_id, game_res_config)
    setting = {'dirs': [], 'files': [], 'game_id': game_id}
    if game_id == 'Blockymods':
        return setting
    for game in game_res_config:
        if game['id'] in ids:
            setting['dirs'] = setting['dirs'] + game['dirs']
            setting['files'] = setting['files'] + game['files']
    return setting


def zip_dir(zfile, path, root_path):
    for root, dirs, files in os.walk(path):
        for file in files:
            zfile.write(os.path.join(root, file), os.path.relpath(os.path.join(root, file), root_path))


def zip_file(zfile, file_path, root_path):
    if os.path.exists(file_path):
        zfile.write(file_path, os.path.relpath(file_path, root_path))
