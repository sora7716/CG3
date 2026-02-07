#pragma once
#include "engine/math/RenderingData.h"
#ifdef USE_IMGUI
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
#endif // USE_IMGUI
#include <string>
//前方宣言
class DirectXBase;
class SRVManager;
class WinApi;

/// <summary>
/// ImGuiの管理
/// </summary>
class ImGuiManager {
public://メンバ関数
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ImGuiManager();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="winApi">ウィンドウズアプリケーション</param>
	/// <param name="directXBase">DirectXの基盤部分</param>
	/// <param name="srvManager">SRVマネージャー</param>
	void Initialize(WinApi* winApi,DirectXBase* directXBase,SRVManager*srvManager);

	/// <summary>
	/// ImGuiの受付開始
	/// </summary>
	void Begin();

	/// <summary>
	/// ImGuiの受付終了
	/// </summary>
	void End();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// トランスフォームデータ用のImGui
	/// </summary>
	/// <param name="transfromData">トランスフォームデータ</param>
	static void DragTransform(TransformData& transfromData);

	/// <summary>
	/// int型でcheckBoxを表示する
	/// </summary>
	/// <param name="label">ラベル</param>
	/// <param name="frag">フラグ</param>
	static void CheckBoxToInt(const std::string& label,int32_t& frag);
public://PressKey
	class ConstructorKey {
	private:
		ConstructorKey() = default;
		friend class Core;
	};

	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="">PressKeyを受け取る</param>
	explicit ImGuiManager(ConstructorKey);
private://メンバ関数
	//デストラクタの封印
	//コピーコンストラクタ禁止
	ImGuiManager(const ImGuiManager&) = delete;
	//代入演算子の禁止
	ImGuiManager operator=(const ImGuiManager&) = delete;
private://メンバ変数
	//DirectXの基盤部分
	DirectXBase* directXBase_ = nullptr;
	//SRVの管理
	SRVManager* srvManager_ = nullptr;
};

