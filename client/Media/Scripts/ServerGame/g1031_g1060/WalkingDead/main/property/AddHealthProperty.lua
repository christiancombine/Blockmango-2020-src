AddHealthProperty = class("AddHealthProperty", BaseProperty)

function AddHealthProperty:onPlayerUsed(player,itemId)
       if self.config.probability >= math.random(100) then
              player:addHealth(self.config.value[1])
              player.curHealth = player:getHealth()
       end
end
return AddHealthProperty