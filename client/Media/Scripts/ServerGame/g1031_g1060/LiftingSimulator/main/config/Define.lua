Define = {}

Define.Area = {
    ShopArea = 1, --器材售卖区
    SellArea = 2, --肌肉售卖区
    AutoArea = 3, --自动锻炼区
    SafeArea = 4, --安全区
    AdvancedArea = 5, --进阶战斗区
    BackHallArea = 6, --返回大厅区
    StartAdvancedArea = 7, --开始进阶检测区

    CommonArea = 10000,
}

Define.AreaRule = {
    NotAllowAttack = 1,           --不能攻击
    NotAllowWorkout = 2,          --不能锻炼
    NotAllowHurt = 3,             --不会受伤
    NotAllowTeleport = 4,         --不能传送
}

Define.ActionId =
{
    quanji = 100000,
    jiaocai = 100001,
    zhuaqu = 100002,
    juzhong = 100003
}

Define.AdvancedCoeType =
{
    Attack = 1,
    Efficiency = 2,
    MoveSpeed = 3,
    Hp = 4
}

Define.ShopKind = {
    NotKind = 0,
    FitnessEquip = 1,
    Gene = 2,
    Advanced = 3,
    Skill = 4,
}

Define.ItemBuyStatus = {
    Lock = 1, --未解锁
    Unlock = 2, --解锁
    Buy = 3, --购买
    Used = 4, --使用
}

--0.金魔方
--1.金魔方
--2.平台金币
--3.游戏绿钞
--4.健身金币
--5.代币

Define.MoneyType = {

    DiamondGolds1 = 0,
    DiamondGolds2 = 1,
    Gold = 2,
    currey = 3,
    SLGolds = 4,
    Token = 5,
}

Define.SkillType = {
    Throw = 1,
    Beat = 2,
    Toy = 3,
    Common = 4,
}

Define.SkillStage = {
    SkillStart = 1, --技能开始
    PlayAnim = 2, --播放动作
    SpecialEffect = 3, --特殊效果
    DoDamage = 4, --造成伤害
    SkillEnd = 5, --技能结束
}

Define.HurtType = {
    Attack = 1,
    Skill = 2,
}

Define.AreaEventType =
{
    Wrestling = 1, --摔跤擂台
}

Define.RewardType = {
    Muscle = 1,
    Gold = 2,
}


Define.DefaultHave ={
    Have = 0,
    NotHave = 1,
}

Define.PayStoreItem =
{
    PayItem = 1, --特权
    Skin = 2, --皮肤
}

Define.PayItemType =
{
    AutoWorkout = 1, --自动锻炼
    DoubleGold = 2, --双倍金币
    DoubleMuscle = 3, --双倍肌肉
    ConcealPower = 4, --伪装弱者
    InfinityMuscle = 5, --无限肌肉
    DoubleDamage = 6, --双倍伤害
    DoubleSpeed = 7, --双倍速度
    DoubleHp = 8, --双倍血量
}

Define.SelectSkinType =
{
    Default = 1, --默认
    Free = 2, --免费
}

Define.AdType = {
    AutoWork = 1 --自动锻炼
}

--选中状态1-器材，2-拳击，3-脚踩
Define.SelectMode = {
    Equip = 1,
    Punch = 2,
    Step = 3,
    NotSelect =4
}

Define.TokenShopKind = {
    NotKind = 0,
    FitnessEquip = 1,
    Gene = 2,
    Skin = 3,
    Skill = 4,
}

Define.TokenItemBuyStatus = {
    Lock = 1, --未解锁
    Unlock = 2, --解锁
    Buy = 3, --购买
    Used = 4, --使用
}
return Define