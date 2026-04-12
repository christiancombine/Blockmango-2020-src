# coding:utf-8
import subprocess
import os
import time
import commands
import sys


class gittool:
    remote = "origin"
    curr_branch = ""

    def __init__(self, repoPath):
        self.path = repoPath
        os.chdir(repoPath)

    def diff(self, commi1, commit2):
        return self.__run("git diff %s  %s --name-status | grep -v .svn | grep -v .idea")

    def checkout(self, branch, isNew=False):
        return self.__run("git checkout " + (' -b ' if isNew else "") + branch)

    def merge(self, branch):
        return self.__run("git merge " + branch)

    def commit(self, commitinfo):
        res = self.__run("git commit -am '%s' " % commitinfo)
        ret = []
        for i in res:
            ret.append(i.strip())
        return ret

    def fetch(self, remote=""):
        return self.__run("git fetch -v " + (remote if remote != '' else " --all"))

    def log(self, full=False):
        if full:
            return self.__run('git log -n 30')
        else:
            return self.__run('git log --pretty=format:"%h | %ad | %s | %ae |%an " --date=local -n 50')

    def status(self):
        return self.__run("git status -s ")

    def push(self, remote, branch):
        cmd = "git push " + remote.strip() + " " + branch.strip()
        return self.__run(cmd)

    def pull(self, branch="", remote=""):
        branch = branch if branch != '' else self.curr_branch
        return self.__run("git pull " + remote.strip() + " " + branch.strip())

    def show(self, tagName):
        res = self.__run(
            "git show --name-status --pretty=format:'%h | %ad | %s | %ae |%an #br' --date=local " + tagName)
        return res

    def tag(self, tagName=None, tagDesc=None, showTag=False):
        if showTag:
            return self.__run("git tag %s -v" % tagName)
        else:
            if None == tagName:
                return self.__run("git tag")
            else:
                if None == tagDesc:
                    raise Exception(u"必须输标签注释")
                cmd = " git tag -a %s -m '%s' " % (tagName, tagDesc)
                self.__run(cmd)
                return ["create tag success"]

    def reset(self, to=None):
        return self.__run("git reset " + "--hard " + to if to != "" else "")

    def branch(self):
        return [x.replace("*", "") for x in self.__run("git branch")]

    def remote_list(self):
        cmd = "git remote -v |  awk '{print $1}' | uniq"
        return self.__run(cmd)

    def current_branch(self):
        branchs = self.__run("git branch")
        for x in branchs:
            if x.find("*") != -1:
                return x.strip().replace('*', '')
        return None

    def current_commit(self):
        commit = self.__run("git rev-parse HEAD")
        return len(commit) and commit[0] or None

    def __run(self, cmd):
        pipe = subprocess.Popen(
            cmd, stdout=subprocess.PIPE, shell=True, universal_newlines=True)
        print "cmd:" + cmd
        return [x.strip().replace("\t", '    ') for x in pipe.stdout.readlines()]
