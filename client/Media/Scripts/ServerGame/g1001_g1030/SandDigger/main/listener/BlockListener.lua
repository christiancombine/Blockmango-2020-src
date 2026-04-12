--region *.lua
--Date
--此文件由[BabeLua]插件自动生成

require "Match"
require "config.MerchantConfig"
require "config.BlockConfig"
require "config.ToolConfig"

BlockListener = {}

function BlockListener:init()
    BlockStartBreakEvent.registerCallBack(self.onBlockStartBreak)
    BlockAbortBreakEvent.registerCallBack(self.onBlockAbortBreak)
    BlockBreakEvent.registerCallBack(self.onBlockBreak)
    PlayerPlaceBlockEvent.registerCallBack(self.onBlockPlace)
    BlockTNTExplodeEvent.registerCallBack(self.onBlockTNTExplode)
end

function BlockListener.onBlockStartBreak(rakssid, blockId, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    if BlockConfig.area:inArea(vec3) == true then
        local meta = EngineWorld:getWorld():getBlockMeta(vec3)
        for i, v in pairs(BlockConfig.block) do
            if v.id == blockId and v.meta == meta then
                local hashKey = VectorUtil.hashVector3(vec3)
                if BlockConfig.blockHp[hashKey] == nil then
                    BlockConfig.blockHp[hashKey] = v.hp
                end
                player.digStatus = true
                GameMatch:showGameData(BlockConfig.blockHp[hashKey], player)
            end
        end
    end
end

function BlockListener.onBlockAbortBreak(rakssid, blockId, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return
    end
    if BlockConfig.area:inArea(vec3) == true then
        local meta = EngineWorld:getWorld():getBlockMeta(vec3)
        for i, v in pairs(BlockConfig.block) do
            if v.id == blockId and v.meta == meta then
                local hashKey = VectorUtil.hashVector3(vec3)
                if BlockConfig.blockHp[hashKey] == nil then
                    BlockConfig.blockHp[hashKey] = v.hp
                end
                player.digStatus = false
                GameMatch:showGameData()
            end
        end
    end

end

function BlockListener.onBlockBreak(rakssid, blockId, vec3)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if player == nil then
        return false
    end

    if BlockConfig.area:inArea(vec3) == true then
        local meta = EngineWorld:getWorld():getBlockMeta(vec3)
        for i, v in pairs(BlockConfig.block) do
            if v.id == blockId and v.meta == meta then
                local hashKey = VectorUtil.hashVector3(vec3)
                if BlockConfig.blockHp[hashKey] == nil then
                    BlockConfig.blockHp[hashKey] = v.hp
                end

                local damage = ToolConfig:getDamageByToolId(player:getHeldItemId())
                if BlockConfig.blockHp[hashKey] - damage > 0 then
                    BlockConfig.blockHp[hashKey] = BlockConfig.blockHp[hashKey] - damage
                    player.breakBlockCounts = player.breakBlockCounts + 1
                    player:addScore(damage)
                    GameMatch:showGameData(BlockConfig.blockHp[hashKey], player)
                    player.digStatus = false
                    return false
                else
                    player.breakBlockCounts = player.breakBlockCounts + 1
                    player:addScore(BlockConfig.blockHp[hashKey])
                    BlockConfig.blockHp[hashKey] = nil
                    local money, name = ChestConfig:getChestMoneyById(v.id)
                    if money ~= 0 then
                        player.breakChestCounts = player.breakChestCounts + 1
                        player:addMoney(ChestConfig:getChestMoneyById(v.id))
                        MsgSender.sendMsg(Messages:openChest(player.name, name))
                    end
                    player.digStatus = false
                    GameMatch:showGameData()
                    return true
                end
            end
        end
    end

    return false
end

function BlockListener.onBlockPlace(rakssid, itemId, meta, toPlacePos)
    return false
end

function BlockListener.onBlockTNTExplode(entityId, pos, attr)
    attr.isBreakBlock = false
    return false
end

return BlockListener
--endregion
