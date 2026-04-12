#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os, shutil
import sys
import subprocess
import argparse


class SmartFormatter(argparse.ArgumentDefaultsHelpFormatter):
    def _fill_text(self, text, width, indent):
        return ''.join([indent + line for line in text.splitlines(True)])

    def _split_lines(self, text, width):
        return text.splitlines()

def parse_args():
    description = u'编译游戏服脚本'
    parser = argparse.ArgumentParser(description=description, formatter_class=SmartFormatter)
    build_description=u'''
    -b： 编译的对应分支 （必选）
    '''

   

    parser.add_argument('-b', '--branch', required=True,  help=u'branch name,return engineversion')

    return parser.parse_args()

def find_engineVersion(branch="master"):
    if branch == "test" or branch == "master":
        base =  90900
        version = "0"
    elif branch.startswith("test_") or branch.startswith("dev_"):
        base = 90000
        version = branch.split('_')[-1]

    elif branch.startswith("engine2_dev_") or branch.startswith("engine2_test_"):
        base = 80000
        version = branch.split('_')[-1]
    elif branch == "engine2" or branch == "engine2_deploy_test":
        base = 80000
        version = "0"

    return  str(base + int(version))


if __name__ == "__main__":
    if len(sys.argv) == 1:
        sys.argv.append('--help')
    
    args = parse_args()
    '''print args.branch
    branchlists=[]
    for i in range(1,100):
        branchlists.append("engine2_dev_%s" % i)
        branchlists.append("engine2_test_%s"  % i)
        branchlists.append("dev_%s" % i)
        branchlists.append("test_%s" % i)
        

    for branch in branchlists:
        engineVersion = find_engineVersion(branch)
        print "branch:%s engineVersion:%s" % (branch ,engineVersion)'''
    
    engineversion = find_engineVersion(args.branch)
    print engineversion
