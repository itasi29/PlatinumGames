#pragma once
#include <unordered_map>

// オブジェクトのタグ
enum class ObjectTag
{
	PLAYER,			// プレイヤー
	CAMERA,			// カメラ
	GATE,			// ゲート
	GATE_BULLET,	// ゲート生成弾
	GATE_DELETE,	// ゲート破壊
	FLOOR,			// 床
	WALL,			// 壁
	ROOF,			// 天井
	NO_GATE_FLOOR,	// ゲート生成不可床
	NO_GATE_WALL,	// ゲート生成不可壁
	NO_GATE_ROOF,	// ゲート生成不可天井
	FLOOR_MOVE,		// 動く床
	PERFORATED_WALL,	// 穴あき壁
	STAIRS,			// 階段
	CATWALK,		// 足場
	CATWALK_FENCE,	// 足場のフェンス
	CATWALK_PILLAR,	// 足場の柱
	HAND_OBJ,		// 手で持てるオブジェクト
	BUTTON,			// ボタン
	DOOR,			// ドア
	DOOR_ARCH,		// ドアフレーム壁
	PISTON,			// ピストン
	TURRET,			// タレット
	TURRET_BULLET,		// タレットの球
	LASER_LAUNCH_PAD,	// レーザーの開始地点
	LASER_CATCHER,		// レーザーの終了地点
	LASER_BULLET,	// レーザーの球
	GIMMICK,		// ギミック
	ELEVATOR,		// エレベータ
	CHECK_POINT,	// チェックポイント
	GLASS,			// ガラス
	NONE,			// その他
};

namespace Tags
{
	const std::unordered_map<ObjectTag, const char* const> TAG_NAME =
	{
		{ ObjectTag::PLAYER, "プレイヤー" },
		{ ObjectTag::CAMERA, "カメラ" },
		{ ObjectTag::GATE, "ゲート" },
		{ ObjectTag::GATE_BULLET, "ゲートの弾" },
		{ ObjectTag::GATE_DELETE, "ゲート破壊" },
		{ ObjectTag::FLOOR, "床" },
		{ ObjectTag::WALL, "壁" },
		{ ObjectTag::ROOF,	 "天井" },
		{ ObjectTag::NO_GATE_FLOOR, "ゲート生成不可床" },
		{ ObjectTag::NO_GATE_WALL, "ゲート生成不可壁" },
		{ ObjectTag::NO_GATE_ROOF, "ゲート生成不可天井" },
		{ ObjectTag::FLOOR_MOVE, "動く床" },
		{ ObjectTag::PERFORATED_WALL, "穴あき壁" },
		{ ObjectTag::STAIRS,	"階段" },
		{ ObjectTag::CATWALK,		"足場" },
		{ ObjectTag::CATWALK_FENCE,	"足場のフェンス" },
		{ ObjectTag::CATWALK_PILLAR,	"足場の柱" },
		{ ObjectTag::HAND_OBJ, "手で持てるオブジェ" },
		{ ObjectTag::BUTTON, "ボタン" },
		{ ObjectTag::DOOR, "ドア" },
		{ ObjectTag::DOOR_ARCH, "ドアフレーム壁" },
		{ ObjectTag::PISTON, "ピストン" },
		{ ObjectTag::TURRET, "タレット" },
		{ ObjectTag::TURRET_BULLET, "タレットの球" },
		{ ObjectTag::LASER_LAUNCH_PAD, "レーザーの開始地点" },
		{ ObjectTag::LASER_CATCHER, "レーザーの終了地点" },
		{ ObjectTag::LASER_BULLET, "レーザーの弾" },
		{ ObjectTag::GIMMICK, "ギミック" },
		{ ObjectTag::ELEVATOR, "エレベータ" },
		{ ObjectTag::CHECK_POINT, "チェックポイント" },
		{ ObjectTag::GLASS, "ガラス" },
		{ ObjectTag::NONE, "何かしら" },
	};
}