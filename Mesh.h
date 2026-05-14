#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include <cstdint>
#include <memory>
#include <vector>

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"

/**
 * \struct VertexData
 * \brief
 */
struct VertexData {
	Vector4 position;
	Vector2 texCoord;
	Vector3 normal;
};

/**
 * \class Mesh
 * \brief
 */
class Mesh {
public:
	//コンストラクタ・デストラクタ
	Mesh() = default;
	~Mesh();

	static std::unique_ptr<Mesh> CreateSphere(uint32_t divisionVertical = 16, uint32_t divisionHorizontal = 16);

	//初期化
	void Initialize(const std::vector<VertexData>& vertices, const std::vector<uint32_t>& indices);

	//描画準備
	void Bind(ID3D12GraphicsCommandList* commandList) const;

	size_t GetVertexCount() const { return vertexCount_; }
	size_t GetIndexCount() const { return indexCount_; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_ = nullptr;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
	uint32_t vertexCount_ = 0;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_ = nullptr;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};
	uint32_t indexCount_ = 0;
};
