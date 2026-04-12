
require "Match"

ChestListener = {}
ChestListener.chest = {}

function ChestListener:init()
    ChestOpenSpHandleEvent.registerCallBack(function(vec3i, entityId)
        self:onChestOpenSpHandle(vec3i, entityId)
    end)
end

function ChestListener:onChestOpenSpHandle(vec3i, entityId)
    local player = PlayerManager:getPlayerByEntityId(entityId)

    if player == nil then
        return true
    end

    local roomId = RoomConfig:getRoomId(vec3i)
    if RoomConfig.room[roomId] ~= nil then
        local config = RoomConfig.room[roomId]
        local sign = VectorUtil.hashVector3(vec3i)

        if config.items and config.range then
            if self.chest[sign] == nil then
                self.chest[sign] = {}

                -- first person add achievement
                if self.chest[sign].players == nil then
                    self.chest[sign].players = {}

                    if self.chest[sign].players[entityId] == nil then
                        local chest_item = GameConfig:getChestItem(config)
                        if chest_item then
                            local tip_items = {}
                            for k, v in pairs(chest_item) do
                                player:onAddRanchGoods(v.id, v.num)
                                local tip_item = RanchCommon.new()
                                tip_item.itemId = v.id
                                tip_item.num = v.num
                                table.insert(tip_items, tip_item)
                            end

                            local money = GameConfig:getBoxGetMoney()
                            HostApi.log("onChestOpenSpHandle " .. money)
                            if money > 0 then
                                player:addMoney(money)
                            end

                            HostApi.sendRanchGain(player.rakssid, tip_items)

                            player:onAddAchievement(GamePlayer.Achievement.FindBox, 1)

                            self.chest[sign].players[entityId] = true
                            GameMatch:onFindBox()
                            EngineWorld:setBlockToAir(vec3i)
                            return false
                        end
                    end
                end
            end
        end
    end

    return true
end


return ChestListener