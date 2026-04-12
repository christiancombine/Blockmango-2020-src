SoundConfig = {}

function SoundConfig:init()
    local config = FileUtil.getConfigFromYml("setting/SoundSetting", true)


    -- UI相关音效Id
    -- 背包界面
    SoundConfig.openInv = config.openInv or 0
    SoundConfig.closeInv = config.closeInv or 0
    SoundConfig.selectItem = config.selectItem or 0

    --设置界面
    SoundConfig.openSetting = config.openSetting or 0
    SoundConfig.closeSetting = config.closeSetting or 0
    SoundConfig.switchSetting = config.switchSetting or 0

    --对阵信息界面
    SoundConfig.openBattle = config.openBattle or 0
    SoundConfig.closeBattle = config.closeBattle or 0
    SoundConfig.selectTabBattle = config.selectTabBattle or 0
    SoundConfig.banChatBattle = config.banChatBattle or 0
    SoundConfig.addFriendBattle = config.addFriendBattle or 0
    SoundConfig.agreeFriendBattle = config.agreeFriendBattle or 0
    SoundConfig.refuseFriendBattle = config.refuseFriendBattle or 0

    --结算界面
    SoundConfig.openSettlement = config.openSettlement or 0
    SoundConfig.previousSettlement = config.previousSettlement or 0
    SoundConfig.nextSettlement = config.nextSettlement or 0
    SoundConfig.againSettlement = config.againSettlement or 0
    SoundConfig.watchSettlement = config.watchSettlement or 0
    SoundConfig.backHallSettlement = config.backHallSettlement or 0
    SoundConfig.sendGoodSettlement = config.sendGoodSettlement or 0
    SoundConfig.addFriendSettlement = config.addFriendSettlement or 0

    --物品商店,包括定制,升级,魔方商店,
    SoundConfig.openStore = config.openStore or 0
    SoundConfig.closeStore = config.closeStore or 0
    SoundConfig.selectTabStore = config.selectTabStore or 0
    SoundConfig.selectGoodsStore = config.selectGoodsStore or 0
    SoundConfig.buyGoodsStore = config.buyGoodsStore or 0
    SoundConfig.useGoodsStore = config.useGoodsStore or 0
    SoundConfig.rechargeCubeStore = config.rechargeCubeStore or 0

    --升级商店(全队播放)
    SoundConfig.levelUpStore = config.levelUpStore or 0

    --发射火焰弹
    SoundConfig.shootFire = config.shootFire or -1

    --信号弹
    SoundConfig.shootSignal = config.shootSignal or -1
    SoundConfig.boomSignal =  config.boomSignal or -1

    --防御塔
    SoundConfig.buildTower = config.buildTower or -1

    --指令相关
    SoundConfig.attackCommand = config.attackCommand or -1
    SoundConfig.defenseCommand = config.defenseCommand or -1
    SoundConfig.togetherCommand = config.togetherCommand or -1
    SoundConfig.callHelpCommand = config.callHelpCommand or -1
    SoundConfig.buyCommand = config.buyCommand or -1
    SoundConfig.closeBuyCommand = config.closeBuyCommand or -1
    SoundConfig.clickCommand = config.clickCommand or -1
    SoundConfig.switchTab = config.switchTab or -1
    SoundConfig.sendMessage = config.sendMessage or -1
    SoundConfig.selfBedBreak = config.selfBedBreak or -1
    SoundConfig.breakOtherBed = config.breakOtherBed or -1
    SoundConfig.teamMateKillOther = config.teamMateKillOther or -1
    SoundConfig.otherKillTeamMate = config.otherKillTeamMate or -1
    SoundConfig.otherKillOther = config.otherKillOther or -1

    --抽奖
    SoundConfig.openLottery = config.openLottery or -1
    SoundConfig.closeLottery = config.closeLottery or -1
    SoundConfig.refreshReward = config.refreshReward or -1
    SoundConfig.showRewardDetail = config.showRewardDetail or -1
    SoundConfig.closeRewardDetail = config.closeRewardDetail or -1
    SoundConfig.clickLottery = config.clickLottery or -1
    SoundConfig.startLottery = config.startLottery or -1
    SoundConfig.getLotteryReward = config.getLotteryReward or -1
end

SoundConfig:init()