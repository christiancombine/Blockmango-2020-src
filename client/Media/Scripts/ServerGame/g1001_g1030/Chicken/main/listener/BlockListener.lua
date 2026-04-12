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
    return false
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    return false
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    attr.isBreakBlock = false
    --local max = math.sqrt(8 * 8 + 8 * 8 + 8 * 8)
    --for x = -8, 8 do
    --    for y = -8, 8 do
    --        for z = -8, 8 do
    --            local blockPos = VectorUtil.toBlockVector3(pos.x + x, pos.y + y, pos.z + z)
    --            local blockId = EngineWorld:getBlockId(blockPos)
    --            local abs_x = math.abs(x)
    --            local abs_y = math.abs(y)
    --            local abs_z = math.abs(z)
    --            local distance = math.sqrt(abs_x * abs_x + abs_y * abs_y + abs_z * abs_z)
    --
    --        end
    --    end
    --end
    return false
end

return BlockListener
--endregion
