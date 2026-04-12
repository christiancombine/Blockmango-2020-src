# coding:utf-8

from ks3.connection import Connection
import boto3
import sys
import os
import imp

imp.reload(sys)

class ObjectStorage:
    domain = None
    bucket = None
    folder = None
    def __init__(self, bucket, folder, domain):
        self.domain = domain
        self.bucket = bucket
        self.folder = folder

    def getObject(self, file):
        return None

    def putFileObject(self, file, key=None):
        return False

    def putStringObject(self, file, content):
        return False

class S3ObjectStorage(ObjectStorage):
    def __init__(self, bucket, folder, domain="http://static.sandboxol.com/"):
        ObjectStorage.__init__(self, bucket, folder, domain)
        ak = os.environ.get('AWS_ACCESS_KEY')
        sk = os.environ.get('AWS_SECRET_KEY')
        if not ak or not sk:
            raise Exception(u'Please add environment variables AWS_ACCESS_KEY AWS_SECRET_KEY')
        self.c = boto3.client('s3', aws_access_key_id=ak, aws_secret_access_key=sk)

    def getObject(self, file):
        key_name = file
        try:
            res = self.c.get_object(
                Bucket=self.bucket,
                Key=key_name
            )
            s = res['Body'].read()
            return s
        except:
            pass  # 异常处理
        return None

    def putFileObject(self, file, key=None):
        if key:
            key_name = self.folder + key
        else:
            key_name = self.folder + file
        print (u"上传文件 " + key_name)
        try:
            with open(file, 'rb') as data:
                self.c.upload_fileobj(data, self.bucket, key_name)
            print (u"上传成功")
            return self.domain + key_name
        except:
            pass

        return ''

    def putStringObject(self, file, content):
        key_name = file
        try:
            print (u"更新配置文件 " + key_name)
            res = self.c.put_object(
                Bucket=self.bucket,
                Key=key_name,
                Body=content,
                ACL='public-read'
            )
            if res['ResponseMetadata']['HTTPStatusCode'] == 200:
                print (u"更新配置成功")
                return True
        except:
            pass
            return False


class KS3ObjectStorage(ObjectStorage):
    def __init__(self, bucket, folder, domain="http://static.sandboxol.cn/"):
        ObjectStorage.__init__(self, bucket, folder, domain)
        ak = os.environ.get('KS3_ACCESS_KEY')
        sk = os.environ.get('KS3_SECRET_KEY')
        if not ak or not sk:
            raise Exception(u'Please add environment variables KS3_ACCESS_KEY KS3_SECRET_KEY')
        self.c = Connection(ak, sk, host='ks3-cn-shanghai.ksyun.com',
                            is_secure=False, domain_mode=False)

    def getObject(self, file):
        key_name = file
        b = self.c.get_bucket(self.bucket)
        try:
            k = b.get_key(key_name)
            s = k.get_contents_as_string()
            return s
        except:
            pass  # 异常处理
        return None

    def putFileObject(self, file, key=None):
        if key:
            key_name = self.folder + key
        else:
            key_name = self.folder + file

        try:
            print (u"上传文件 " + key_name)
            b = self.c.get_bucket(self.bucket)
            k = b.new_key(key_name)
            ret = k.set_contents_from_filename(file, policy="public-read")
            if ret and ret.status == 200:
                print (u"上传成功")
            return self.domain + key_name
        except:
            pass

        return ''

    def putStringObject(self, file, content):
        key_name = file
        try:
            print (u"更新配置文件 " + key_name)
            b = self.c.get_bucket(self.bucket)
            k = b.new_key(key_name)
            ret = k.set_contents_from_string(content, policy="public-read")
            if ret and ret.status == 200:
                print (u"更新配置成功")
                return True
        except:
            pass  # 异常处理
            return False

class S3ObjMap:
    config_obj = None
    map_obj = None
    plugin_obj = None
    def __init__(self, config_obj, map_obj, plugin_obj):
        self.config_obj = config_obj
        self.map_obj =  map_obj
        self.plugin_obj =  plugin_obj

class S3ObjManager:
    s3client = None
    def __init__(self, area):
        if area == 'test':
            self.s3client = S3ObjMap(KS3ObjectStorage('sandbox-region', 'sandbox-test/config/server/'),
                                     KS3ObjectStorage('sandbox-region', 'sandbox-test/games/maps/'),
                                     KS3ObjectStorage('sandbox-region', 'sandbox-test/games/plugins/'))
        if area == 'oversea':
            self.s3client = S3ObjMap(S3ObjectStorage('sandboxol-region', 'sandbox/config/server/'),
                                     S3ObjectStorage('sandboxol-region', 'sandbox/games/maps/'),
                                     S3ObjectStorage('sandboxol-region', 'sandbox/games/plugins/'))
        if area == 'vanguard':
            self.s3client = S3ObjMap(S3ObjectStorage('vanguard-static', 'vanguard/config/server/'),
                                     S3ObjectStorage('vanguard-static', 'vanguard/public/games/maps/'),
                                     S3ObjectStorage('vanguard-static', 'vanguard/public/games/plugins/'))
        if area == 'china':
            self.s3client = S3ObjMap(KS3ObjectStorage('sandbox-region', 'sandbox/config/server/'),
                                     KS3ObjectStorage('sandbox-region', 'dl/games/maps/'),
                                     KS3ObjectStorage('sandbox-region', 'dl/games/plugins/'))
        if area == 'china_test':
            self.s3client = S3ObjMap(KS3ObjectStorage('sandbox-region', 'sandbox-test/config/server/'),
                                     KS3ObjectStorage('sandbox-region', 'dl-test/games/maps/'),
                                     KS3ObjectStorage('sandbox-region', 'dl-test/games/plugins/'))