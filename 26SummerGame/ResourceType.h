#pragma once

// フォントリソースの識別子
enum class FontId
{
	kNone = -1,
	kFactoryUITitle,
	kFactoryUIText,
	kFactoryUITextMini,
	kQuestUIText,
	kCount
};

// グラフィックリソースの識別子
enum class GraphicId
{
	kNone = -1,
	kIronOreIcon,
	kCoalOreIcon,
	kRockIcon,
	kCopperOreIcon,
	kIronPlateIcon,
	kCopperPlateIcon,

	kBeltconveyorIcon,
	kFurnaceIcon,
	kMiningMachineIcon,
	kInserterIcon,

	kFurnaceTop,
	kMiningMachineTop,
	kInserterTop,

	kButtonA,
	kButtonY,
	kButtonX,
	kButtonB,

	kOutlineButtonA,
	kOutlineButtonY,
	kOutlineButtonX,
	kOutlineButtonB,

	kNoneButton,

	kRightTrigger,
	kRightButton,
	kLeftButton,
	kDpadHorizontal,

	kKeyboardF,
	kKeyboardQ,
	kKeyboardE,
	kKeyboardC,
	kKeyboardR,
	kKeyboardZ,
	kKeyboardX,
	kKeyboard1,
	kKeyboard2,
	kKeyboardTab,

	kMouseLeft,

	kUIClose,
	kArrow,
	kPlus,

	kCount
};

// モデルリソースの識別子
enum class ModelId
{
	kNone = -1,
	kIronOre,
	kIronPlate,
	kCopperPlate,
	kBeltConveyor,
	kSkyDome,
	kSkyBox,
	kCount
};
