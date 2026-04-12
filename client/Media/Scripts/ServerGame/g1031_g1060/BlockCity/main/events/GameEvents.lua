BlockCityOpenTigerLotteryEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BlockCityOpenTigerLottery", BlockCityOpenTigerLotteryEvent)

BlockCitySelectTigerLotteryEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BlockCitySelectTigerLottery", BlockCitySelectTigerLotteryEvent)

BlockCityOperationEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BlockCityOperation", BlockCityOperationEvent)

BlockCityBuyGoodsEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BlockCityBuyGoods", BlockCityBuyGoodsEvent)

BlockCityChangeEditModeEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BlockCityChangeEditMode", BlockCityChangeEditModeEvent)

RotateOperationEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("RotateOperation", RotateOperationEvent)

BlockCityBuyLackingItemsEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BlockCityBuyLackingItems", BlockCityBuyLackingItemsEvent)

BlockCityManorBuyOperationEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BlockCityManorBuyOperation", BlockCityManorBuyOperationEvent)

BlockCityChoosePropEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BlockCityChooseProp", BlockCityChoosePropEvent)

BlockCityGetFreeFlyEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BlockCityGetFreeFly", BlockCityGetFreeFlyEvent)

BlockCityHouseShopBuyGoodsEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BlockCityHouseShopBuyGoods", BlockCityHouseShopBuyGoodsEvent)

BlockCityChangeToolStatusEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BlockCityChangeToolStatus", BlockCityChangeToolStatusEvent)

MoveElevatorEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("MoveElevator", MoveElevatorEvent)

BlockCityTeleportToShopEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BlockCityTeleportToShop", BlockCityTeleportToShopEvent)

BlockCityOperateArchiveDataEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BlockCityOperateArchiveData", BlockCityOperateArchiveDataEvent)

BlockCityUnlockNewArchiveEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BlockCityUnlockNewArchive", BlockCityUnlockNewArchiveEvent)

BlockCityChangeArchiveNameEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BlockCityChangeArchiveName", BlockCityChangeArchiveNameEvent)

BlockCityTossBackEvent = IScriptEvent.new()
CommonDataEvents:registerEvent("BlockCityTossBack", BlockCityTossBackEvent)

local BlockCityPortal = class("BlockCityPortalEvent", IScriptEvent)
BlockCityPortalEvent = BlockCityPortal.new()
CommonDataEvents:registerEvent("BlockCityPortal", BlockCityPortalEvent)

local BlockCityToRace = class("BlockCityToRaceOrNotEvent", IScriptEvent)
BlockCityToRaceOrNotEvent = BlockCityToRace.new()
CommonDataEvents:registerEvent("BlockCityToRaceOrNot", BlockCityToRaceOrNotEvent)