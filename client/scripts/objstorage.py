# coding:utf-8
import os
from ks3.connection import Connection
import boto3

bucket_name = "sandbox-region"
s3_bucket_name = "sandboxol-region"


class S3ObjectStorage:
    def __init__(self):
        self.folder = 'sandbox/'
        self.s3 = boto3.resource('s3')

    def getObject(self, file):
        key_name = file
        try:
            ret = self.s3.Object(s3_bucket_name, self.folder + file).get()
            content = ret['Body'].read()
            return content
        except:
            pass  # 异常处理
        return None

    def putFileObject(self, file):
        key = self.folder + file
        try:
            ret = self.s3.meta.client.upload_file(
                file, s3_bucket_name,  key, ExtraArgs={'ACL': 'public-read'})
            print u"上传文件", s3_bucket_name,  key
            if ret and ret['ResponseMetadata']['HTTPStatusCode'] == 200:
                print u"上传成功"
        except:
            pass  # 异常处理

    def putStringObject(self, file, content, object_key=None):
        key_name = object_key or (self.folder + file)
        try:
            ret = self.s3.Object(s3_bucket_name,  key_name).put(
                ACL='public-read',
                Body=content,
                ContentType='application/json'
            )
            print u"上传文件", key_name
            if ret and ret['ResponseMetadata']['HTTPStatusCode'] == 200:
                print u"上传成功"
        except:
            pass  # 异常处理


class KS3ObjectStorage:
    def __init__(self, ak, sk):
        self.c = Connection(ak, sk, host='ks3-cn-shanghai.ksyun.com',
                            is_secure=False, domain_mode=False)

    def getObject(self, file):
        key_name = file
        b = self.c.get_bucket(bucket_name)
        try:
            k = b.get_key(key_name)
            s = k.get_contents_as_string()
            print s
            return s
        except:
            pass  # 异常处理
        return None

    def putFileObject(self, file):
        key_name = file
        try:
            b = self.c.get_bucket(bucket_name)
            k = b.new_key(key_name)
            print u"上传文件", key_name
            # object policy : 'private' or 'public-read'
            ret = k.set_contents_from_filename(file, policy="public-read")
            if ret and ret.status == 200:
                print u"上传成功"
        except:
            pass  # 异常处理

    def putStringObject(self, file, content, object_key=None):
        key_name = object_key or file
        try:
            b = self.c.get_bucket(bucket_name)
            k = b.new_key(key_name)
            print u"上传文件", key_name
            # object policy : 'private' or 'public-read'
            ret = k.set_contents_from_string(content, policy="public-read")
            if ret and ret.status == 200:
                print u"上传成功"
        except:
            pass  # 异常处理
