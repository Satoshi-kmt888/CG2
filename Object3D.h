#pragma once

#include <d3d12.h>
#include <wrl/client.h>

#include "Vector2.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Transform.h"

struct VertexData {
	Vector4 position;
	Vector2	texCoord;
};

/// <summary>
/// オブジェクト3D
/// </summary>
class Object3D {
public:
	void Initialize(ID3D12Device* device, ID3D12DescriptorHeap* descriptorHeap);
	void Update(const Matrix4x4& viewProjectionMatrix);
	void Draw(ID3D12GraphicsCommandList* commandList);
	void Finalize();

	D3D12_CPU_DESCRIPTOR_HANDLE GetTextureSrvHandleCPU() const { return textureSrvHandleCPU; }
	Vector4& GetMaterialData() const { return *materialData; }

private:
	//リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;

	//ビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

	//
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;

	//データポインタ
	VertexData* vertexData = nullptr;
	Vector4* materialData = nullptr;
	Matrix4x4* wvpData = nullptr;

	//ワールド変換データ
	Transform transform{};
};
