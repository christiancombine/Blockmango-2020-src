package com.sandboxol.blockmango;

import com.sandboxol.web.Dispatch;

public class UserInfo {
    public int clz;
    public long userId;
    public long targetId;
    public long engineVer;
    public String token;
    public String nickName;
    public String gameType;
    public String mapId;
    public String ip = "";
    public Dispatch dispatch;
    public boolean isRestart = false;

    private static UserInfo me;

    public static UserInfo newInstance() {
        if (me == null) {
            me = new UserInfo();
        }
        return me;
    }

    private UserInfo() {
        this.mapId = "";
        this.gameType = "g1047";
        this.clz = 0;
        this.userId = 144;
        this.targetId = 144;
//        this.userId = 734374880;
//        this.targetId = 734374880;
        this.nickName = "Mr.luo-144";
        this.token = "eyJhbGciOiJIUzI1NiJ9.eyJqdGkiOiIxNDQiLCJpYXQiOjE1NjgwODI1NjMsInN1YiI6IjIwMTktMDktMTAgMTA6Mjk6MjMiLCJpc3MiOiJTYW5kYm94LVNlY3VyaXR5LUJhc2ljIiwiZXhwIjoxNTY4OTcxNTk2fQ.9rM-kyT8V5IGtRTUGd_oyWhMnwWvjpq_WvSO9TDDB8Y";
        this.engineVer = BuildConfig.ENGINE_VERSION;
    }

    public void setGameType() {
        gameType = gameType.equals("g1042") ? "g1008" : "g1042";
    }
}
