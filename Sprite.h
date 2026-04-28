#pragma once

#include "BaseObject.h"

/**
 * .
 */
class Sprite : public BaseObject{
public:

public:
	/**
	 * \brief 初期化処理
	 * \param[in] descriptorHeap SRVを管理するディスクリプターヒープ
	 */
	void Initialize(ID3D12DescriptorHeap* descriptorHeap) override;

	/**
	 * \brief 更新処理
	 * \param[in] viewProjectionMatrix ビュー・プロジェクション行列
	 */
	void Update(const Matrix4x4& viewProjectionMatrix) override;

	/** \brief 描画処理 */
	void Draw() override;
};

