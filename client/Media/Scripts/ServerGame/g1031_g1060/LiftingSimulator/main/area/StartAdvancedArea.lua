StartAdvancedArea = class("StartAdvancedArea", BaseArea)

function StartAdvancedArea:onPlayerIn(player)
    local scene = SceneManager:getAdvancedScene(player.advancingId)
    if not scene then
        return
    end
    scene:startAdvanced()
end

function StartAdvancedArea:onPlayerStay(player)

end

function StartAdvancedArea:onPlayerOut(player)

end