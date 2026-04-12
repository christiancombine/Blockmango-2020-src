BaseGameConfig = {}

function BaseGameConfig:init()
    require "common.CommonHeader"
    require "game.config.BlockConfig"
    require "game.config.CommandSettingConfig"
    require "game.config.MessageEffectConfig"
    require "game.config.GameTipSettingConfig"
    require "game.config.KillBroadCastConfig"
    require "game.config.GainsConfig"
    require "game.config.TeamConfig"
    require "game.config.ExpConfig"
    require "game.config.ResourceTypeConfig"
    require "game.config.PublicResourceConfig"
    require "game.config.ResourcePointConfig"
    require "game.config.MonsterConfig"
    require "common.config.TaskConfig"
    require "common.config.TreasureChallengeConfig"
    require "common.config.ActionEmojiConfig"
    require "common.config.BuffConfig"
    if not isClient then
        require "game.config.HonourConfig"
    else
        require "game.config.CustomSceneConfig"
    end
end
