Define = {}

Define.GameStatus = {
    GameWaitPlayer = 1,
    GamePrepare = 2,
    GameFight = 3,
    GameOver = 4,
}

Define.Money = {
    DiamondsGold = 1,
    PlatformGolds = 2,
    GreenMoney = 3,
    GameGold = 4,
    HallGold = 5,
}

Define.EntityType = {
    Monster = 1,
    Tower = 2
}

Define.TowerStatus = {
    Build = 1,
    Formula = 2,
    Destroy = 3,
}

Define.SkillType = {
    Aura = 1,
    Attack = 2,
    Crit = 3,
    Plunder = 4,
}

Define.SkillTarget = {
    Tower = 1,
    Monster = 2,
}

Define.FloatNumberColor = {
    Red = 1,
    Yellow = 2,
    Blue = 3,
    Purple = 4,
    Green = 5,
}

Define.RewardType = {
    Exp = 1,
    Money = 2,
    Tower = 3,
    Skin = 4,
    PayItem = 5,
}

Define.Sex = {
    Boy = 1,
    Girl = 2
}

Define.WaitMessageType = {
    StartShow = 1,
    Update = 2,
    OnBegin = 3,
    Close = 4
}

Define.PayStoreItem = {
    ShortcutBarOne = 1,
    ShortcutBarTwo = 2,
    GameGold = 3,
}

Define.ShortCutBarStatus = {
    Lock = 1,
    NotTower = 2,
    HaveTower = 3
}

Define.MonsterStatus = {
    Normal = 1,
    Controlled = 2,
    Guide = 3,
}

Define.TowerStatus = {
    Normal = 1,
    Controlled = 2,
}

Define.SkillStatus = {
    Ready = 1 ,
    Guide = 2,
    CD = 3,
}

Define.RankKey = {
    KillMonsterTotal = "g1057_MonsterTotal",
    KillMonsterWeek = "g1057_MonsterWeek",
    TowerBook = "g1057_TowerBook",
    CheckPointTotal = "g1057_CheckPointTotal",
    CheckPoint = "g1057_CheckPoint",
}

Define.MapName = {"A1","A2","A3","A4","A5",
                  "B1","B2","B3","B4","B5",
                  "C1","C2","C3","C4","C5",
                  "D1","D2","D3","D4","D5"}

Define.PathManagerTickPeriod = 100

Define.SyncBloodByPercent = 0.2  --同步怪物血量的最小变化量