SkinArea = class("SkinArea", BaseArea)

function SkinArea:init()
    self.skinConfig = SkinStoreConfig:getConfigById(self.config.configId)
    self.luaTime = nil
end

function SkinArea:createActor()
    if isClient and #self.skinConfig.actorName > 7 then
        return EngineWorld:addActorNpc(self.config.centrePos, self.config.yaw, self.skinConfig.actorName, function(entity)
            entity:setIgnoreFrustumCheck(true)
            entity:setRenderDistanceWeight(5)
            entity:setSkillName("g1056_fly_idle")
            self.entity = entity

            self.actorSize = ActorSizeSetting.getActorSize(self.skinConfig.actorName)
            local builder = DataBuilder.new()
            builder:addParam("atlasName", "skin_price.json")
            builder:addParam("spriteName", "price" .. self.skinConfig.price)
            local offsetY = (self.actorSize.height + 0.15) * self.actorSize.scale
            builder:addVector3Param("offset", VectorUtil.newVector3(0, offsetY + 1.6, 0))
            entity:addCustomImage(tostring(self.skinConfig.actorName) .. "price", builder:getData(), -1, 0.5)
        end)
    end
end

function SkinArea:onClick(player)
    if isClient then
        if tostring(player.userId) == tostring(GamePlayer:Instance().userId) then
            ClickSkinActorEvent:invoke(self.skinConfig.id)
        end
    end
end
