Messages = {}
Messages.aButtonToDeleteAll = "BlockCity.aButtonToDeleteAll"
Messages.upgradeManor = "BlockCity.upgradeManor"
Messages.buyFly = "BlockCity.buyFly"
Messages.buyDraw = "BlockCity.buyDraw"
Messages.buyBlock = "BlockCity.buyBlock"
Messages.buyDecoration = "BlockCity.buyDecoration"
Messages.buyTigerLottery = "BlockCity.buyTigerLottery"
Messages.buyLackItems = "BlockCity.buyLackItems"
Messages.buyShopHouseItem = "BlockCity.buyShopHouseItem"
Messages.buyArchive = "BlockCity.buyArchive"

function Messages:gameName()
    return TextFormat:getTipType(1047001)
end

function Messages:msgHeldItem()
	return 1047002
end

function Messages:msgEditMode()
    return 1047004
end

function Messages:msgPlaceManorLevelUp()
    return 1047005
end

return Messages