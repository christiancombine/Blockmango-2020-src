local baseActor = require "actor.BaseActor"

SupplyActor = class("SupplyActor", baseActor)

function SupplyActor:ctor(pos, itemId, count, meta, actorName, effectName)
    self.super:ctor(pos, 0, actorName or "", "", effectName or "", self)
    self.ItemId = itemId or 0
    self.Count = count or 0
    self.Meta = meta or 0
end

function SupplyActor:clickedActor()
    self:destroyActor()
    local actor = ItemActor.new(self.position, self.ItemId, self.Count, self.Meta)
    ActorManager:addActorToCache(actor)
    SupplyManager:onOpenSupply(self.position)
end