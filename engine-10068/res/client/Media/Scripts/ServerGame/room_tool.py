#!/usr/bin/env python
# -*- coding: utf-8 -*-
import json
import requests
import sys
import imp
import web_config

imp.reload(sys)

class RoomTool:
    dispatcher_urls = []
    update_map_api = '/v1/updatemaps'
    headers = {
        'X-Shahe-Timestamp': '1499049522339',
        'X-Shahe-Nonce': '154719',
        'X-Shahe-Signature': '88df1aad4acf445c1a396caf15fd8c0a24ad5725'
    }

    def __init__(self, area):
        self.area = area
        self.init_dispatcher_urls(area)

    def init_dispatcher_urls(self, area):
        dispatch_list_url = web_config.web_domain[area] + '/game/api/v1/inner/region/disp-cluster-list'
        params = {
            'timestamp': '1499049522339',
            'nonce': '154719',
            'signature': '88df1aad4acf445c1a396caf15fd8c0a24ad5725'
        }
        response = requests.get(dispatch_list_url, params=params)
        if response.status_code == 200:
            result = json.loads(response.text)
            if result['code'] == 1:
                data = result['data']
                for item in data:
                    if len(item['dispUrl']) > 0 and "v1" in item['supportedEngineType']:
                        self.dispatcher_urls.append(item['dispUrl'])
        else:
            raise Exception('[RoomTool]:init dispatcher urls failed! response.status_code=' + str(response.status_code))

    def notify_game_update(self, engine, game_id):
        params = {
            'type': 'plugin',
            'force': 0,
            'gameType': game_id,
            'engineVersion': engine
        }
        try:
            for url in self.dispatcher_urls:
                response = requests.post(url + self.update_map_api, params=params, headers=self.headers)
                if response.status_code == requests.codes.ok:
                    print (url + u' 更新插件成功 gameId:' + game_id + ' engine:' + engine)
                else:
                    print (url + u' 更新插件失败 gameId:' + game_id + ' engine:' + engine)
                    self.notify_game_update(engine, game_id)
        except:
            print (u'更新插件失败 gameId:' + game_id + ' engine:' + engine)
            self.notify_game_update(engine, game_id)