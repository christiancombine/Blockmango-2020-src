CannonConfig = {}
CannonConfig.cannons = {}

function CannonConfig:init(configs)
	for _, v in pairs(configs) do
		local cannon = {}
		cannon.id = tonumber(v.id)
		cannon.entityId = 0
		cannon.actor = v.actor
		cannon.body = v.body
		cannon.bodyId = v.bodyId
		cannon.x = tonumber(v.x) 
		cannon.y = tonumber(v.y)
		cannon.z = tonumber(v.z)
		cannon.x1 = tonumber(v.x1)
		cannon.y1 = tonumber(v.y1)
		cannon.z1 = tonumber(v.z1)
        cannon.tipNor = v.tipNor
        cannon.tipPre = v.tipPre
        cannon.imageNor = v.imageNor
        cannon.imagePre = v.imagePre
        if cannon.imageNor == "@@@" then
            cannon.imageNor = ""
        end
        if cannon.imagePre == "@@@" then
            cannon.imagePre = ""
        end
		cannon.addHeight = tonumber(v.addHeight)
		table.insert(self.cannons, cannon)
	end
end

function CannonConfig:prepareCannons()
    for _, v in pairs(self.cannons) do
        local beginPos = VectorUtil.newVector3(v.x, v.y, v.z)
        local fallOnPos = VectorUtil.newVector3(v.x1, v.y1, v.z1)
        v.entityId = EngineWorld:getWorld():addActorCannon(beginPos, fallOnPos, v.actor, v.body, v.bodyId, v.addHeight)

        local areaBegin = VectorUtil.newVector3i(beginPos.x - 2, beginPos.y, beginPos.z - 2)
        local areaEnd = VectorUtil.newVector3i(beginPos.x + 2, beginPos.y + 2, beginPos.z + 2)
        AreaInfoManager:push(v.entityId, AreaType.Cannon, v.tipNor, v.tipPre, v.imageNor, v.imagePre, false, areaBegin, areaEnd)
    end
end

function CannonConfig:getCannonIdByEntityId(entityId)
    entityId = tonumber(entityId)
    for _, v in pairs(self.cannons) do
        if v.entityId == entityId then
            return v.id
        end
    end

    return 0
end

return CannonConfig