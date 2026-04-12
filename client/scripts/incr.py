# coding:utf-8
import os
import sys
import commands
import subprocess
import datetime
import re
import gittool
import json
import tarfile
import argparse
import objstorage
import requests
import hashlib
import time


class Incr:
    repo_path = ''
    git = None
    version_file = ""
    version = None
    history_file = ""
    history = None
    tar_path = ''
    branch = ''

    def __init__(self, repo_path=".",
                 tar_path="resupdate/files",
                 s3client=None,
                 domain="",
                 version_path="resupdate/version/",
                 ws_host=None,
                 branch='master',
                 config_path="",
                 without_partial_update=False):
        if not os.path.exists(repo_path):
            raise Exception(repo_path + " not exist")

        os.chdir(repo_path)
        self.s3client = s3client
        self.domain = domain
        self.repo_path = repo_path
        self.git = gittool.gittool(repo_path)
        self.version_file = version_path + 'version.json'
        self.history_file = version_path + 'history.json'
        self.tar_path = tar_path
        self.ws_host = ws_host
        self.branch = branch
        self.config_path = config_path
        self.without_partial_update = without_partial_update

        self.local_tar_path = os.getcwd()
        dirs = tar_path.split('/')
        for d in dirs:
            self.local_tar_path = os.path.join(self.local_tar_path, d)

        now = datetime.datetime.now()
        timeNow = now.strftime("%Y%m%d-%H%M%S")
        self.delfile_path = os.path.join(
            self.local_tar_path, 'dellist_' + timeNow)

        if not os.path.exists(self.local_tar_path):
            print 'make dirs', self.local_tar_path
            os.makedirs(self.local_tar_path)

        self.git.checkout(self.branch)
        # pull_res = self.git.pull(self.branch, 'origin')
        # for l in pull_res:
        #     print l
        self.loadVersion()

    def loadVersion(self):
        ver = self.s3client.getObject(self.version_file)
        his = self.s3client.getObject(self.history_file)
        if ver:
            self.version = json.loads(ver)
        if his:
            self.history = json.loads(his)

        if not self.version:
            print self.version
            self.initPackage()

    def initPackage(self):
        version = 1
        pak_info = self.packFull(version)
        self.version = {
            "version": version,
            "commit": self.git.current_commit(),
            "full_update": {},
            "partial_update": {}
        }
        for p, v in pak_info.items():
            self.version['full_update'][p] = {
                'url': self.domain + v,
                'md5': self.md5(v)
            }
        self.uploadVersionInfo()
        return pak_info

    def packFull(self, version):
        actor_dir = os.path.join(os.getcwd(), 'Media')
        skins_dir = os.path.join(os.getcwd(), 'recipe')
        libs_dir = os.path.join(os.getcwd(), 'libs')
        resource_cfg = os.path.join(os.getcwd(), 'resource.cfg')
        self.saveLocalVersion(version)
        result = {}
        for d in os.listdir(libs_dir):
            tarname = 'full-update.%s.%s.%d.tar.gz' % (
                version, d, time.time() * 1000)
            obj_key = self.tar_path + '/' + tarname
            filename = os.path.join(self.local_tar_path, tarname)
            with tarfile.open(filename, 'w:gz') as tar:
                tar.add(actor_dir, arcname=os.path.basename(actor_dir))
                tar.add(skins_dir, arcname=os.path.basename(skins_dir))
                lib_dir = os.path.join(libs_dir, d)
                tar.add(lib_dir, arcname=os.path.basename(libs_dir) + '/' + d)
                tar.add('version.json')
                tar.add('resource.cfg')

            self.s3client.putFileObject(obj_key)
            result[d] = obj_key
        return result

    def pack_partial_update_for_old_version(self, cur_version, cur_commit, lib_dirs):
        partial_map = {}
        for ver, commit in self.history.items():
            update_ver = '%s.%d' % (ver, cur_version)
            for lib_dir in lib_dirs:
                [file, md5] = self.pack_partial_update(
                    ver, cur_version, cur_commit, commit, lib_dir)

                if os.path.exists(file):
                    if not partial_map.has_key(update_ver):
                        partial_map[update_ver] = {}

                    partial_map[update_ver][lib_dir] = {
                        'url': file,
                        'md5': md5
                    }

        for files in partial_map.values():
            for p in files.values():
                self.s3client.putFileObject(p['url'])
        return partial_map

    def pack_partial_update(self, from_ver, to_ver, cur_commit, last_commit, platform):
        tarname = "partial-update.%s.%s.%s.%d.tar.gz" % (
            str(from_ver), str(to_ver), platform, time.time() * 1000)
        filepath = os.path.join(self.local_tar_path, tarname)

        git_cmd = "git diff %s %s --name-only --diff-filter=d" % (
            cur_commit, last_commit)
        pipe = subprocess.Popen(
            git_cmd, stdout=subprocess.PIPE, shell=True, universal_newlines=True)

        tar = tarfile.open(filepath, 'w:gz')
        has_change_files = False
        lib_dir = os.path.join(os.getcwd(), 'libs', platform)

        for line in pipe.stdout.readlines():
            if 'res/client/Media' in line or 'res/client/recipe' in line or lib_dir in line:
                fp = os.path.normpath(os.path.join(
                    os.getcwd(), '..', '..', os.path.normpath(line.strip())))
                aname = line.replace('res/client', '').strip()
                print 'add file', fp, aname
                if os.path.exists(fp):
                    tar.add(fp, arcname=aname)
                has_change_files = True
        if os.path.exists('version.json'):
            tar.add('version.json')
        tar.close()
        md5 = self.md5(filepath)
        if not has_change_files:
            os.remove(filepath)

        return [self.tar_path + '/' + tarname, md5]

    def uploadVersionInfo(self):
        self.s3client.putStringObject(self.version_file, json.dumps(
            self.version, indent=4, sort_keys=True, ensure_ascii=False))
        self.s3client.putStringObject(self.version_file, json.dumps(
            self.version, indent=4, sort_keys=True, ensure_ascii=False), object_key=self.config_path)
        self.deleteVersionCache()

        if not self.history:
            self.history = {}
        self.history[self.version['version']] = self.version['commit']
        self.s3client.putStringObject(self.history_file, json.dumps(
            self.history, indent=4, sort_keys=True, ensure_ascii=False))

    def saveLocalVersion(self, version):
        with open('version.json', "w+") as f:
            f.write(json.dumps({'version': version, 'commit': self.git.current_commit()},
                               indent=4, sort_keys=True, ensure_ascii=False))

    def updateVersion(self, full_package, partial_package, version):
        for k, _ in partial_package.items():
            for p, _ in partial_package[k].items():
                partial_package[k][p]['url'] = self.domain + \
                    partial_package[k][p]['url']

        self.version = {
            "version": version,
            "commit": self.git.current_commit(),
            "full_update": {},
            "partial_update": partial_package
        }

        for k, v in full_package.items():
            self.version['full_update'][k] = {
                'url': self.domain + v,
                'md5': self.md5(v)
            }

        self.uploadVersionInfo()

    def deleteVersionCache(self):
        if self.ws_host:
            result = requests.delete(
                self.ws_host + "/v1/client/version-check?platform=x86&ver=1&reload=1&timestamp=1512982966972&nonce=477602947&signature=668acec02819fd513f5435db3104db36987e0787")
            print 'delete version cache result:', result

    def incrPush(self):
        ret = u'''
git 增量更新操作
:return:
        '''
        version = self.version['version']
        cur_commit = self.git.current_commit()
        if cur_commit == self.version['commit']:
            ret += u"\n无增量更新"
        else:
            last_commit = self.version['commit']
            lib_dirs = os.listdir(os.path.join(os.getcwd(), 'libs'))
            if not lib_dirs or len(lib_dirs) == 0:
                raise Exception(
                    "can not file and platform dir in res/client/libs")

            [filepath, md5] = self.pack_partial_update(
                version, version + 1, cur_commit, last_commit, lib_dirs[0])
            if not os.path.exists(filepath):
                ret += u"\n<b>查找本次变更的文件: None</b>"
            else:
                # 全量更新包
                full_package = self.packFull(version + 1)
                # 增量更新包
                partial_package = {}
                if not self.without_partial_update:
                    partial_package = self.pack_partial_update_for_old_version(
                        version + 1, cur_commit, lib_dirs)
                self.updateVersion(full_package, partial_package, version + 1)

                listCmd = "git diff %s %s --name-only --diff-filter=d" % (
                    cur_commit, last_commit)
                listRes = self.__run(listCmd)
                ret += u"\n<b>打包变更的文件到</b> ==> %s   SUCCESS " % filepath
                listRes = [
                    line for line in listRes if 'res/client/Media/Actor' in line or 'res/client/Media/Skins' in line]
                ret += u"\n<b>文件变更明细:</b>\n" + "\n".join(listRes)

        return ret

    def __run(self, cmd):
        pipe = subprocess.Popen(
            cmd, stdout=subprocess.PIPE, shell=True, universal_newlines=True)
        print "cmd:" + cmd
        return [x.strip().replace("\t", '    ') for x in pipe.stdout.readlines()]

    def md5(self, fname):
        hash_md5 = hashlib.md5()
        with open(fname, "rb") as f:
            for chunk in iter(lambda: f.read(4096), b""):
                hash_md5.update(chunk)
        return hash_md5.hexdigest()

    def cleanPackages(self):
        for root, dirs, files in os.walk(self.local_tar_path, topdown=False):
            for file in files:
                os.remove(os.path.join(root, file))

        os.removedirs(self.local_tar_path)


def parse_args():
    description = '''
        Auto pack Skin resources, full update package and partial update package to Cloud Storage
    '''

    parser = argparse.ArgumentParser(description=description)
    parser.add_argument(
        '-t', '--type', choices=['s3', 'ks3'], default='ks3', help=u'云存储平台类型')
    parser.add_argument('-p', '--resource_media_path',
                        default='D:\\reps\\blockmango-client\\res\\client', help=u'资源所在目录路径')
    parser.add_argument('--prod', action="store_true",
                        help=u'正式更新包，如果不提供此参数默认是打测试更新包')
    parser.add_argument('--without_partial_update',
                        action="store_true", help=u'不需要打包增量更新包')
    return parser.parse_args()


def main():
    if len(sys.argv) == 1:
        sys.argv.append('--help')

    args = parse_args()
    # class Struct:
    #     def __init__(self, **entries):
    #         self.__dict__.update(entries)
    # args = {
    #     'type': 'ks3',
    #     'resource_media_path': 'D:\\reps\\blockmango-client\\res\\client',
    #     'prod': True
    # }
    # args = Struct(**args)

    work_dir = os.path.dirname(os.path.realpath(__file__))
    media_path = os.path.normpath(
        os.path.join(work_dir, args.resource_media_path))

    ak = os.environ.get('KS3_ACCESS_KEY')
    sk = os.environ.get('KS3_SECRET_KEY')

    objStorage = None
    domain = ""
    ws_host = ""
    if args.type == 'ks3':
        if not ak or not sk:
            print u'请先配置环境变量 KS3_ACCESS_KEY，KS3_SECRET_KEY'
            return
        objStorage = objstorage.KS3ObjectStorage(ak, sk)
        domain = "http://static.sandboxol.cn/"
        ws_host = "http://v3.game.sandboxol.cn:9902"

    else:
        objStorage = objstorage.S3ObjectStorage()
        domain = "http://static.sandboxol.com/sandbox/"
        ws_host = "http://v3.game.sandboxol.com:9902"

    tar_path = "resupdate/files-test"
    version_path = 'resupdate/version-test/'
    config_path = 'sandbox-test/config/server/resource-version'
    branch = 'test'
    if args.prod:
        tar_path = "resupdate/files"
        version_path = 'resupdate/version/'
        config_path = 'sandbox/config/server/resource-version'
        branch = 'release'
    else:
        # debug(test env)
        ws_host = "http://120.92.133.131:9902"
        branch = 'test'

    incr = Incr(media_path, s3client=objStorage,
                domain=domain, tar_path=tar_path,
                version_path=version_path,
                ws_host=ws_host,
                branch=branch,
                config_path=config_path,
                without_partial_update=args.without_partial_update)
    result = incr.incrPush()
    print result
    # incr.cleanPackages()


if __name__ == '__main__':
    main()
