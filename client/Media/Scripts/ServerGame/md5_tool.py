#!/usr/bin/env python
# -*- coding: utf-8 -*-
import hashlib
import sys
import imp
imp.reload(sys)

def md5_file(path):
    file = open(path, 'rb')
    md5 = hashlib.md5()
    md5.update(file.read())
    file.close()
    return md5.hexdigest()