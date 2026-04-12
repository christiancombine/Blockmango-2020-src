---
--- Created by Jimmy.
--- DateTime: 2017/10/21 0021 17:30
---

Area = class()

Area.minX = 0
Area.maxX = 0
Area.minZ = 0
Area.maxZ = 0
Area.minY = 0

function Area:ctor(area)
    self:setMatrix(area)
end

function Area:setMatrix(matrix)
    self.minX = matrix[1][1];
    self.maxX = matrix[1][1];
    self.minY = matrix[1][2];
    self.minZ = matrix[1][3];
    self.maxZ = matrix[1][3];
    for i, v in pairs(matrix) do
        if tonumber(v[1]) < tonumber(self.minX) then
            self.minX = v[1]
        end
        if tonumber(v[1]) > tonumber(self.maxX) then
            self.maxX = v[1]
        end
        if tonumber(v[2]) < tonumber(self.minY) then
            self.minY = v[2]
        end
        if tonumber(v[3]) < tonumber(self.minZ) then
            self.minZ = v[3]
        end
        if tonumber(v[3]) > tonumber(self.maxZ) then
            self.maxZ = v[3]
        end
    end
end

function Area:inArea(vec3)
    return vec3.y < tonumber(self.minY) or (vec3.x >= tonumber(self.minX) and vec3.x <= tonumber(self.maxX) and vec3.z >= tonumber(self.minZ) and vec3.z <= tonumber(self.maxZ))
end

function Area:inBlockPlaceArea(vec3)
    return vec3.x >= tonumber(self.minX) and vec3.x <= tonumber(self.maxX) and vec3.z >= tonumber(self.minZ) and vec3.z <= tonumber(self.maxZ)
end

return Area