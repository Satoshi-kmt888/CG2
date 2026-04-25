#pragma once
#include <Windows.h>
#include <cstdint>

/**
 * \class WinApp
 * \brief Windowsアプリケーションの基盤管理を行うシングルトンクラス
 * * ウィンドウの生成、メッセージループの制御、および各種ハンドルの管理を担当します。
 */
class WinApp {
public:
	/*==================================================
	 公開定数
	==================================================*/

	static const uint32_t kClientWidth = 1280; //!< クライアント領域の横幅
	static const uint32_t kClientHeight = 720; //!< クライアント領域の縦幅

	/*==================================================
	 インスタンス制御
	==================================================*/

	/**
	 * \brief インスタンスの取得
	 * \return WinAppの唯一のインスタンス
	 */
	static WinApp* GetInstance() {
		static WinApp instance;
		return &instance;
	}

	//コピーガード
	WinApp(const WinApp&) = delete;            //!< コピーコンストラクタを禁止
	WinApp& operator=(const WinApp&) = delete; //!< 代入演算子を禁止

	/*==================================================
	 静的メソッド
	==================================================*/

	/**
	 * \brief ウィンドウプロシージャ
	 * \details OSからのシステムメッセージを処理します
	 * \param[in] hwnd   ウィンドウハンドル
	 * \param[in] msg    メッセージID
	 * \param[in] wparam パラメータ1
	 * \param[in] lparam パラメータ2
	 * \return メッセージ処理の結果
	 */
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	/*==================================================
	 ライフサイクル
	==================================================*/

	/** \brief ウィンドウの初期化(クラス登録及びウィンドウ生成) */
	void Initialize();

	/**
	 * \brief メッセージの受付処理
	 * \return アプリケーションを続行する場合は true、終了する場合は false
	 */
	bool ProcessMessage();

	/** \brief ウィンドウの破棄および後処理 */
	void Finalize();

	/*==================================================
	 ゲッター
	==================================================*/

	HWND GetHwnd() const { return hwnd_; }
	HINSTANCE GetHInstance() const { return hInstance_; }

private:
	/*==================================================
	 コンストラクタ・デストラクタ
	==================================================*/

	WinApp() = default;
	~WinApp() = default;

	/*==================================================
	 メンバ変数
	==================================================*/

	HWND hwnd_ = nullptr;           //!< ウィンドウハンドル
	WNDCLASS wc_ = {};              //!< ウィンドウクラス構造体
	HINSTANCE hInstance_ = nullptr; //!< アプリケーションインスタンスハンドル
};
