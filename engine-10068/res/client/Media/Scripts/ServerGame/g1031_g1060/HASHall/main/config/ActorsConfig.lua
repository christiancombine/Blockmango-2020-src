---
--- Created by longxiang.
--- DateTime: 2018/11/9  16:15
---

ActorsConfig = {}
ActorsConfig.actors = {}
ActorsConfig.rareActors = {}
ActorsConfig.ordinaryActors = {}
ActorsConfig.rareWeights = {}
ActorsConfig.ordinaryWeights = {}

ActorsConfig.ACTORS_IS_RARE = 1

ActorsConfig.RANDOM_ONE_TIMES_GROUP = 11
ActorsConfig.ORDINARY_WEIGHTS_GROUP = 12
ActorsConfig.RARE_WEIGHTS_GROUP = 13

function ActorsConfig:init(actors)
    self:initActors(actors)
end

function ActorsConfig:initActors(actors)
    local allrareWeights = 0
    local allordinaryWeights = 0
    for k, actor in pairs(actors) do
        local item = {}
        item.id = actor.id
        item.name = actor.name
        item.image = actor.image
        item.price = tonumber(actor.price) or 0
        item.actor_name = actor.actor_name
        item.rare = tonumber(actor.rare)
        item.desc = actor.desc
        item.weights = tonumber(actor.weights)
        self.actors[item.id] = item
        if item.rare == ActorsConfig.ACTORS_IS_RARE then
            allrareWeights = allrareWeights + item.weights
            table.insert(self.rareWeights, item.weights)
            table.insert(self.rareActors, item)
        else
            allordinaryWeights = allordinaryWeights + item.weights
            table.insert(self.ordinaryWeights, item.weights)
            table.insert(self.ordinaryActors, item)
        end
    end


    for k,v in pairs(self.ordinaryWeights) do
        self.ordinaryWeights[k] = v / allordinaryWeights
    end

    for k,v in pairs(self.rareWeights) do
       self.rareWeights[k] = v / allrareWeights
    end
    
    self:initRandomSeed()
end


function ActorsConfig:initRandomSeed()
    LotteryUtil:initRandomSeed(ActorsConfig.ORDINARY_WEIGHTS_GROUP, self.ordinaryWeights)
    LotteryUtil:initRandomSeed(ActorsConfig.RARE_WEIGHTS_GROUP, self.rareWeights)
end

function ActorsConfig:getActors()
    return self.actors or {}
end

function ActorsConfig:getRareActors()
    return self.rareActors
end

function ActorsConfig:getOrdinaryActors()
    return self.ordinaryActors
end

function ActorsConfig:getActorById(id)
    for lv, actor in pairs(self.actors) do
        if actor.id == id then
            return actor
        end
    end
    return nil
end

return ActorsConfig