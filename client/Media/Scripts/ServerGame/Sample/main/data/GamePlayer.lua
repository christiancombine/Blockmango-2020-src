---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr()
    self:teleInitPos()

    self:addItem(325, 10, 0) -- empty bucket
    self:addItem(326, 10, 0) -- water bucket
    self:addItem(327, 10, 0) -- lava bucket

    self:addItem(3, 64, 0) -- item of dirt block
    self:addItem(4, 64, 0) -- item of stone block
    self:addItem(44, 64, 0) -- item of slab block
    self:addItem(65, 64, 0) -- item of ladder block
    self:addItem(85, 64, 0) -- item of fence block

    
    self:addItem(407, 1, 0) -- unknown item id what allwed placed in JailBreak
    self:addItem(324, 5, 0) -- dorWood

    self:addItem(290, 1, 0) -- hoeWood
end

function GamePlayer:teleInitPos()
    self:teleportPos(GameConfig.initPos)
end

function GamePlayer:reward()

end

return GamePlayer

--endregion


