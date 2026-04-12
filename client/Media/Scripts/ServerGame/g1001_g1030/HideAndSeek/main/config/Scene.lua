---
--- Created by Jimmy.
--- DateTime: 2017/11/14 0014 16:10
---
require "config.ProduceConfig"

Scene = class()
Scene.lastModel = {}

function Scene:ctor(config)
    self.npcs = {}
    self:init(config)
end

function Scene:init(config)
    self:initPos(config)
    self:initSeek(config.seek)
    self:initHide(config.hide)
    self:initModel(config.model)
    self:initProduceConfig(config.produce)
    self:initActorNpc(config.actorNpcRange, config.actorNpc)
end

function Scene:initPos(config)
    self.initPos = VectorUtil.newVector3(config.initPos[1], config.initPos[2], config.initPos[3])
end

function Scene:initSeek(config)
    self.seek = {}
    self.seek.initPos = {}
    for i, pos in pairs(config.initPos) do
        self.seek.initPos[i] = VectorUtil.newVector3(pos.pos[1], pos.pos[2], pos.pos[3])
    end
    self.seek.name = config.name
    self.seek.hp = tonumber(config.hp)
end

function Scene:initHide(config)
    self.hide = {}
    self.hide.initPos = {}
    for i, pos in pairs(config.initPos) do
        self.hide.initPos[i] = VectorUtil.newVector3(pos.pos[1], pos.pos[2], pos.pos[3])
    end
    self.hide.name = config.name
    self.hide.hp = tonumber(config.hp)
end

function Scene:randomSeekPos()
    math.randomseed(os.clock() * 1000000)
    return self.seek.initPos[math.random(1, #self.seek.initPos)]
end

function Scene:randomHidePos()
    math.randomseed(os.clock() * 1000000)
    return self.hide.initPos[math.random(1, #self.hide.initPos)]
end

function Scene:initModel(config)
    self.model = {}
    for i, v in pairs(config) do
        local m = {}
        m.actor = v.actor
        m.name = v.name
        self.model[i] = m
    end
end

function Scene:randomModel(model)
    local m = self.model[math.random(1, #self.model)]
    while model == m or self.lastModel == m do
        m = self.model[math.random(1, #self.model)]
    end
    self.lastModel = m
    return m
end

function Scene:initActorNpc(range, npcs)
    self.actor_npcs = {}
    self.actor_npcs.range = {}
    self.actor_npcs.range[1] = tonumber(range[1])
    self.actor_npcs.range[2] = tonumber(range[2])
    self.actor_npcs.npcs = {}
    for i, npc in pairs(npcs) do
        local n = {}
        n.actor = npc.actor
        n.initPos = VectorUtil.newVector3(npc.initPos[1], npc.initPos[2], npc.initPos[3])
        n.yaw = tonumber(npc.initPos[4])
        self.actor_npcs.npcs[i] = n
    end
end

function Scene:prepareActorNpc()
    math.randomseed(os.clock() * 1000000)
    local num = math.random(self.actor_npcs.range[1], self.actor_npcs.range[2])
    num = math.min(num, #self.actor_npcs.npcs)
    local record = {}
    local pos = 1
    while num > 0 do
        local index = math.random(1, #self.actor_npcs.npcs)
        if record[index] == nil then
            self.npcs[pos] = {}
            local config = self.actor_npcs.npcs[index]
            self.npcs[pos].config = config
            self.npcs[pos].entityId = EngineWorld:addActorNpc(config.initPos, config.yaw, config.actor)
            record[index] = true
            pos = pos + 1
            num = num - 1
        end
    end
end

function Scene:removeActorNpc()
    for i, npc in pairs(self.npcs) do
        EngineWorld:removeEntity(npc.entityId)
    end
    self.npcs = {}
end

function Scene:initProduceConfig(produce)
    self.produceConfig = ProduceConfig.new(produce)
end

function Scene:reset()
    self.produceConfig:reset()
    self:removeActorNpc()
end

function Scene:getConfig(role)
    if role == GamePlayer.ROLE_SEEK then
        return self.seek
    else
        return self.hide
    end
end

return Scene