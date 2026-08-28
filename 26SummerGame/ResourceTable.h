#pragma once
#include <array>
#include <DxLib.h>

#include "ResourceType.h"
#include "ResourceName.h"

// リソースの読み込みパラメータをまとめた静的テーブル。
// ここに1行追加し、対応するenum(ResourceType.h)にも1つ値を追加すれば
// ResourceManager::Init()が自動的に読み込む。
namespace ResourceTable
{
	namespace
	{
		constexpr const char* kMainFont = "Noto Sans JP Black";
	}

	struct FontInfo
	{
		FontId Id;
		const char* FontName;
		int Size;
		int Thick;
		int FontType;
	};

	inline const std::array<FontInfo, static_cast<size_t>(FontId::kCount)> kFontTable = { {
		{ FontId::kFactoryUITitle,    kMainFont, 40, 3, DX_FONTTYPE_ANTIALIASING_EDGE_16X16 },
		{ FontId::kFactoryUIText,     kMainFont, 20, 3, DX_FONTTYPE_ANTIALIASING_EDGE_16X16 },
		{ FontId::kFactoryUITextMini, kMainFont, 15, 3, DX_FONTTYPE_ANTIALIASING_EDGE_16X16 },
		{ FontId::kQuestUIText,       kMainFont, 30, 3, DX_FONTTYPE_ANTIALIASING_EDGE_16X16 },
	} };

	struct GraphicInfo
	{
		GraphicId Id;
		const char* Path;
	};

	inline const std::array<GraphicInfo, static_cast<size_t>(GraphicId::kCount)> kGraphicTable = { {
		{ GraphicId::kIronOreIcon,		"image/icon/iron_ore.png" },
		{ GraphicId::kCoalOreIcon,      "image/icon/coal_ore.png" },
		{ GraphicId::kRockIcon,			"image/icon/rock.png" },
		{ GraphicId::kCopperOreIcon,	"image/icon/copper_ore.png" },
		{ GraphicId::kIronPlateIcon,	"image/icon/iron_plate.png" },
		{ GraphicId::kCopperPlateIcon,	"image/icon/copper_plate.png" },

		{ GraphicId::kBeltconveyorIcon, ResourceName::kBeltConveyorGraph.c_str() },
		{ GraphicId::kFurnaceIcon,      ResourceName::kFurnaceGraph.c_str() },
		{ GraphicId::kMiningMachineIcon,ResourceName::kMiningMachineGraph.c_str() },
		{ GraphicId::kInserterIcon, "image/icon/inserter.png"},

		{ GraphicId::kInserterTop, ResourceName::kInserterTop.c_str() },
		{ GraphicId::kFurnaceTop,      ResourceName::kFurnaceTop.c_str() },
		{ GraphicId::kMiningMachineTop,ResourceName::kMiningMachineTop.c_str()},

		{ GraphicId::kButtonA, ResourceName::kButtonA.c_str() },
		{ GraphicId::kButtonY, ResourceName::kButtonY.c_str() },
		{ GraphicId::kButtonX, ResourceName::kButtonX.c_str() },
		{ GraphicId::kButtonB, ResourceName::kButtonB.c_str() },

		{ GraphicId::kOutlineButtonA, ResourceName::kMonochroOutLineButtonA.c_str() },
		{ GraphicId::kOutlineButtonY, ResourceName::kMonochroOutLineButtonY.c_str() },
		{ GraphicId::kOutlineButtonX, ResourceName::kMonochroOutLineButtonX.c_str() },
		{ GraphicId::kOutlineButtonB, ResourceName::kMonochroOutLineButtonB.c_str() },

		{ GraphicId::kNoneButton, ResourceName::kNoneButton.c_str() },

		{ GraphicId::kRightTrigger,   ResourceName::kRightTriger.c_str() },
		{ GraphicId::kRightButton,    ResourceName::kRightButton.c_str() },
		{ GraphicId::kLeftButton,     ResourceName::kLeftButton.c_str() },
		{ GraphicId::kDpadHorizontal, ResourceName::kDpadHorizontal.c_str() },

		{ GraphicId::kKeyboardF,   ResourceName::kKeyboardButtonF.c_str() },
		{ GraphicId::kKeyboardQ,   ResourceName::kKeyboardButtonQ.c_str() },
		{ GraphicId::kKeyboardE,   ResourceName::kKeyboardButtonE.c_str() },
		{ GraphicId::kKeyboardC,   ResourceName::kKeyboardButtonC.c_str() },
		{ GraphicId::kKeyboardR,   ResourceName::kKeyboardButtonR.c_str() },
		{ GraphicId::kKeyboard1,   ResourceName::kKeyboardButton1.c_str() },
		{ GraphicId::kKeyboard2,   ResourceName::kKeyboardButton2.c_str() },
		{ GraphicId::kKeyboardTab, ResourceName::kKeyboardButtonTab.c_str() },
		{ GraphicId::kKeyboardZ, "image/icon/key/keyboard_z.png" },
		{ GraphicId::kKeyboardX, "image/icon/key/keyboard_x.png" },

		{ GraphicId::kUIClose, "image/icon/close.png" },
		{ GraphicId::kArrow, "image/icon/arrow.png" },
		{ GraphicId::kPlus, "image/icon/plus.png" },

		{ GraphicId::kTitleImage,"image/tenohira_factory_logo.png"},

		{ GraphicId::kMouseLeft, ResourceName::kMouseLeft.c_str() },
	} };

	struct ModelInfo
	{
		ModelId Id;
		const char* Path;
		float Scale;
	};

	inline const std::array<ModelInfo, static_cast<size_t>(ModelId::kCount)> kModelTable = { {
		{ ModelId::kIronOre,      ResourceName::kIronOreModel.c_str(),      0.1f },
		{ ModelId::kIronPlate,    ResourceName::kIronPlateModel.c_str(),    0.3f },
		{ ModelId::kCopperPlate,   "model/copper_plate.mv1",    0.3f },
		{ ModelId::kBeltConveyor, ResourceName::kBeltConveyorModel.c_str(), 1.0f },
		{ ModelId::kSkyDome, "model/skyDome.mv1",200.0f},
		{ ModelId::kSkyBox, "model/skyBox.mv1",80.0f }
	} };

}
