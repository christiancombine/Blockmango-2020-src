#!/usr/bin/python
# -*- coding: utf-8 -*-
import os, shutil
import res_tool
import argparse
import json
import requests
import sys

reload(sys)
sys.setdefaultencoding('utf8')


def get_min_version():
    path = res_tool.get_root_path() + '.git/HEAD'
    git_file = open(path)
    git_branch = git_file.read().replace('ref: refs/heads/', '')
    if git_branch.find('dev_') == -1: # release
        file = open(res_tool.get_game_media_path() + "/engineVersion.json")
        content = file.read()
        if content:
            version_config = json.loads(content)
            return version_config['engineVersion']
    else: # dev
        return 90000 + int(git_branch.replace('dev_', ''))
    return -1


headers = {
    'userId': '0',
    'X-Shahe-Timestamp': '1499049522339',
    'X-Shahe-Nonce': '154719',
    'X-Shahe-Signature': '88df1aad4acf445c1a396caf15fd8c0a24ad5725'
}

def get_game_res_config(domain, engine, game_id):
    get_game_res_url = domain + 'game/api/v1/inner/games/app-engine/check-update'
    params = {
        'commonResVersion': 0,
        'gameResVersion': 0,
        'gameType': game_id,
        'engineVersion': engine,
        'engineType': 'v1'
    }
    response = requests.get(get_game_res_url, params=params, headers=headers)
    if response.status_code == 200:
        result = json.loads(response.text)
        if result['code'] == 1 \
                and result['data'][0]['resVersion'] is not None\
                and len(result['data'][0]['updates']) > 0:
            return {
                'res_version': result['data'][0]['resVersion'],
                'url': result['data'][0]['updates'][0]['url']
            }
        return None
    else:
        raise Exception('[GameResVerTool]:get game res failed! response.status_code=' + str(response.status_code))

def write_res(domain, engine, game_id):
    print("------------------write game resource version-------------------" + game_id)
    game_res_config = get_game_res_config(domain, engine, game_id)
    if game_res_config is None:
        return
    path = res_tool.get_app_res_path() + "/game_res_version.json"
    if not os.path.isfile(path):
        file = open(path, mode="w")
        file.close()
    file = open(path, "r")
    content = file.read()
    file.close()
    versions = {}
    if content:
        versions = json.loads(content)
    versions[game_id] = game_res_config['res_version']
    file = open(path, "w")
    content = json.dumps(versions, indent=4)
    file.write(content)
    file.close()
    print ("------------------write game resource version success-------------------" + game_id)

class GameResVerTool:

    def __init__(self, domain):
        self.work_dir = os.getcwd() + '/resources'
        self.domain = domain
        self.engine = get_min_version()

    def write_res(self, game_id):
        path = res_tool.get_app_res_path() + "/game_res_version.json"
        file = open(path, "w")
        file.write('{}')
        file.close()

        game_res_config = res_tool.get_game_resource_config()
        game_ids = res_tool.get_need_game_ids([], game_id, game_res_config)
        print("------------------start write game resource version-------------------")
        for game_id in game_ids:
            write_res(self.domain, self.engine, game_id)

        file = open(path, "r")
        content = file.read()
        file.close()
        print("--------------------------------------------------------------------------")
        print(content)
        print("--------------------------------------------------------------------------")

def parse_args():
    description = u'auto output game resources version script.'
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument('-d', '--domain', default='http://mods.sandboxol.com/',
                        required=False, help=u'Host, eg:http://mods.sandboxol.com/, default http://mods.sandboxol.com/')
    parser.add_argument('-g', '--game', default='All',
                        required=False, help=u'GameId, eg:g1001, default All')
    return parser.parse_args()

def main():
    args = parse_args()

    tool = GameResVerTool(args.domain)
    tool.write_res(args.game)

if __name__ == '__main__':
    main()
