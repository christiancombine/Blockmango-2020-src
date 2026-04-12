--region *.lua

require "Match"
require "messages.Messages"

BlockListener = {}

function BlockListener:init()
    BlockBreakEvent.registerCallBack(self.onBlockBreak)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
    PlayerPlaceBlockBoxJudgeEvent.registerCallBack(self.onPlayerPlaceBlockBoxJudge)
end

function BlockListener.onBlockBreak(rakssid, blockId, blockPos)
    if EngineWorld:isBlockChange(blockPos) then
        return true
    end
    return false
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    if GameMatch:isGameStart() then
        local player = PlayerManager:getPlayerByRakssid(rakssid)
        if player ~= nil then
            return true
        end
    end
    return false
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    attr.isBreakBlock = false
    return false
end

function BlockListener.onPlayerPlaceBlockBoxJudge(rakssid, blockId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return true
    end

    return false
end

return BlockListener
--endregion
