--region *.lua
--Date
--此文件由[BabeLua]插件自动生成

require "Match"

BlockListener = {}

function BlockListener:init()
    BlockBreakEvent.registerCallBack(self.onBlockBreak)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
    SchemeticPlaceBlockBeginEvent.registerCallBack(self.onSchemeticPlaceBlockBegin)
    SchemeticPlaceBlockEvent.registerCallBack(self.onSchemeticPlaceBlock)
    SchemeticPlaceBlockFinishEvent.registerCallBack(self.onSchemeticPlaceBlockFinish)
end

function BlockListener.onBlockBreak(rakssid, blockId, vec3)
    return false
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    return false
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    attr.isBreakBlock = false
    return false
end

function BlockListener.onSchemeticPlaceBlockBegin(rakssid)
    return false
end

function BlockListener.onSchemeticPlaceBlock(rakssid, vec3, blockId, placeBlockParam)
    return BlockListener.onBlockPlace(rakssid, blockId, placeBlockParam, vec3)
end

function BlockListener.onSchemeticPlaceBlockFinish(rakssid, blockId, _, costNum)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end
    local itemId = 0;
    for _, block in pairs(player.equip_blocks) do
        if block.ItemId == blockId then
            itemId = block.ItemId
        end
    end
    return player:removeItem(itemId, costNum)
end

return BlockListener
--endregion
