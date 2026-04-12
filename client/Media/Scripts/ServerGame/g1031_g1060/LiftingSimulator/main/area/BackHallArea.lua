BackHallArea = class("BackHallArea", BaseArea)

function BackHallArea:onPlayerIn(player)
    local scene = SceneManager:getAdvancedScene(player.advancingId)
    if not scene then
        return
    end
    if scene:isGameStart() then
        return
    end
    scene:onBackHall(player)
end

function BackHallArea:onPlayerStay(player)

end

function BackHallArea:onPlayerOut(player)
    player.curArea = Define.Area.CommonArea
end