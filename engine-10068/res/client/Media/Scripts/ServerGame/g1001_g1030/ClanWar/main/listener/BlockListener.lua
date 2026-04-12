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
    if GameMatch:isGameStart() then
        for i, v in pairs(GameMatch.Teams) do
            if v:locInBornArea(vec3) or v:locInFlagArea(vec3) then
                return false
            end
        end
        return true
    else
        return false
    end
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player ~= nil then
        local canPlace1, isPlaceFlag1 = GameMatch.Teams[1]:onBlockPlace(itemId, meta, toPlacePos)
        local canPlace2, isPlaceFlag2 = GameMatch.Teams[2]:onBlockPlace(itemId, meta, toPlacePos)
        if player.team.id == 1 then
            if canPlace1 and isPlaceFlag1 then
                player:onPlaceFlag()
                return true
            end
        else
            if canPlace2 and isPlaceFlag2 then
                player:onPlaceFlag()
                return true
            end
        end
        if canPlace1 and canPlace2 then
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
