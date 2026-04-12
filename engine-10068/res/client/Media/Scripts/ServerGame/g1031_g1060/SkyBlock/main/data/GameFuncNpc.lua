-- @Author: tanjianyong
-- @Date:   2019-08-23 11:24:53
-- @Last Modified by:   tanjianyong
-- @Last Modified time: 2019-11-04 17:03:02

GameFuncNpc = class()

function GameFuncNpc:ctor(config)
    self.id = tonumber(config.id) or 1
    self.func = tostring(Define.Npc_Func[self.id])
    self.type = tonumber(config.type)
    self.name = tostring(config.name)
    self.actor = tostring(config.actor) or ""
    self.pos = self:initPosFromConfig(config.pos)
    self.point_LB = self:initPosFromConfig(config.point_LB)
    self.point_LT = self:initPosFromConfig(config.point_LT)
    self.point_RT = self:initPosFromConfig(config.point_RT)
    self.point_RB = self:initPosFromConfig(config.point_RB)
    self.door_H = tonumber(config.door_H) or 3
    self.yaw = tonumber(config.yaw) or 0
    self:onCreate()
end

function GameFuncNpc:onCreate()
    local entityId = EngineWorld:addSessionNpc(self.pos, self.yaw, self.type, self.actor, function(entity)
        entity:setNameLang("")
        entity:setCanCollidedByUser(0, false)
    end)
    self.entityId = entityId
end

function GameFuncNpc:onPlayerMove(player, x, y, z)
    if self:inDoorArea( x, y, z) then
        self:onPlayerEnter(player)
    end
end

function GameFuncNpc:inDoorArea(x, y, z)
    -- 原理：凸多边形内部的点都在凸多边形的边所在的向量的同一侧
    -- 配置必须为凸多边形
    local posA = self.point_LB
    local posB = self.point_LT
    local posC = self.point_RT
    local posD = self.point_RB
    local a = (posB.x - posA.x)*(z - posA.z) - (posB.z - posA.z)*(x - posA.x)
    local b = (posC.x - posB.x)*(z - posB.z) - (posC.z - posB.z)*(x - posB.x)
    local c = (posD.x - posC.x)*(z - posC.z) - (posD.z - posC.z)*(x - posC.x)
    local d = (posA.x - posD.x)*(z - posD.z) - (posA.z - posD.z)*(x - posD.x)
    local e = y > posA.y and y < posA.y + self.door_H
    if  e and ( a > 0 and b > 0 and c > 0 and d > 0) or (a < 0 and b < 0 and c < 0 and d < 0 ) then
        return true
    end
    return false
end

function GameFuncNpc:onPlayerEnter(player)
    if player.isNextGameStatus == "initGameStatus" then
        local nextGame = ""
        if self.func == Define.Npc_Func[1] then
            nextGame = "g1048"
        end
        if self.func == Define.Npc_Func[2] then
            nextGame = "g1049"
            player.isNextGameStatus = self.func
        end
        if self.func == Define.Npc_Func[3] then
            nextGame = "g1050"
            player.isNextGameStatus = self.func
        end
        if nextGame ~= "" then
            player:gotoNextGame(nextGame)
        end
    end
end

function GameFuncNpc:initPosFromConfig(configPos)
    local vec3 = StringUtil.split(configPos, "#")
    return VectorUtil.newVector3(tonumber(vec3[1] or 0), tonumber(vec3[2] or 0), tonumber(vec3[3]) or 0)
end

return GameFuncNpc