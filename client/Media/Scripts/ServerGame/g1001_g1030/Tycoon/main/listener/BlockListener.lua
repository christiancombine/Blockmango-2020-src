--region *.lua
require "messages.Messages"

BlockListener = {}

function BlockListener:init()
    BlockBreakEvent.registerCallBack(self.onBlockBreak)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
    BlockSwitchEvent.registerCallBack(self.onBlockSwitch)
end

function BlockListener.onBlockBreak(rakssid, blockId, vec3)
    if blockId == 65 then
        return true
    end
    return false
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    if itemId == 65 then
        return true
    end
    return false
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    attr.isBreakBlock = false
    attr.isHurtEntityItem = false
    return false
end

-- 自定义开关类方块（压力板，按钮）事件
-- return false屏蔽默认对周围方块影响（如开门）
local lastPlayer = nil
function BlockListener.onBlockSwitch(entityId, downOrUp, blockId, vec3)
    local player = PlayerManager:getPlayerByEntityId(entityId)
    if downOrUp == true and player ~= nil then
        lastPlayer = player
        if lastPlayer.domain ~= nil and lastPlayer.domain.gate ~= nil then
            lastPlayer.domain.gate.isCanClose = false
        end
    end
    if lastPlayer == nil then
        return false
    end
    if lastPlayer.domain == nil then
        return false
    end
    if lastPlayer.domain.gate == nil then
        return false
    end
    if lastPlayer.domain.area:inArea(vec3) then
        if lastPlayer.domain.gate.isOpen == false then
            lastPlayer.domain.gate:toggleGate()
        end
        if downOrUp == false then
            lastPlayer.domain.gate.isCanClose = true
            lastPlayer = nil
        end
    end
    return true
end

return BlockListener
--endregion
