--region *.lua
--Date
--此文件由[BabeLua]插件自动生成

require "Match"

BlockListener = {}

function BlockListener:init()
    BlockBreakEvent.registerCallBack(self.onBlockBreak)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
end

function BlockListener.onBlockBreak(rakssid, blockId, vec3)
    if GameMatch.curStatus ~= GameMatch.Status.GameTime then
        return false
    end
    if blockId == 78 then --雪片
        local player = PlayerManager:getPlayerByRakssid(rakssid)
        if player then
            player:addItem(332, math.random(1, 3), 0)
        end
        GameMatch:addSnowPos(vec3)
        return true
    end
    return GameConfig.enableBreak
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    if GameMatch:isGameStart() == false then
        return false
    end
    if itemId ~= 80 then
        return false
    end
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        if player.team:inBlockPlaceArea(toPlacePos) then
            return true
        end
    end
    return false
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    attr.isBreakBlock = false
    return false
end

return BlockListener
--endregion
