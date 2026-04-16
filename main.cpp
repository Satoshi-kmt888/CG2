#include <Windows.h>
#include <cassert>
#include <cstdint>
#include <d3d12.h>
#include <dxcapi.h>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include <format>
#include <strsafe.h>

#include "WinApp.h"
#include "DirectXCommon.h"
#include "D3D12Util.h"
#include "DebugUtil.h"
#include "ShaderCompiler.h"
#include "StringUtil.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Transform.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

#include "imgui.h"
#include "backends/imgui_impl_dx12.h"
#include "backends/imgui_impl_win32.h"

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//誰も捕捉しなかった場合に(Unhandled)、捕捉する関数を登録
	SetUnhandledExceptionFilter(ExportDump);

	WinApp* winApp = new WinApp();
	winApp->Initialize();

	DirectXCommon* dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//==================================================
	//シェーダーコンパイル
	//==================================================

	HRESULT hr;

	//dxcCompilerを初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	//現時点でincludeしないが、includeに対応するための設定を行っておく
	IDxcIncludeHandler* includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

	//==================================================
	//PSO(GraphicsPipelineStateObject)
	//==================================================

	/*
	RootSignature
	------------------------------*/
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//RootParameter作成
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	 //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;  //PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0;					 //レジスタ番号0とバインド
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	 //CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; //レジスタ番号0とバインド
	rootParameters[1].Descriptor.ShaderRegister = 0;					 //レジスタ番号0とバインド
	descriptionRootSignature.pParameters = rootParameters;				 //ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);	 //配列の長さ

	//シリアライズにしてバイナリにする
	ID3DBlob* signatureBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(
		&descriptionRootSignature, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob
	);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリをもとに作成
	ID3D12RootSignature* rootSignature = nullptr;
	hr = dxCommon->GetDevice()->CreateRootSignature(
		0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature)
	);
	assert(SUCCEEDED(hr));

	/*
	InputLayout
	------------------------------*/
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[1] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	/*
	BlendState
	------------------------------*/
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	/*
	RasterizerState
	------------------------------*/
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面(時計回り)を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	/*
	CompileShader
	------------------------------*/
	//vertexShader
	IDxcBlob* vertexShaderBlob = CompileShader(
		L"Object3D.VS.hlsl", L"vs_6_0", dxcUtils, dxcCompiler, includeHandler
	);
	assert(vertexShaderBlob != nullptr);

	//pixelShader
	IDxcBlob* pixelShaderBlob = CompileShader(
		L"Object3D.PS.hlsl", L"ps_6_0", dxcUtils, dxcCompiler, includeHandler
	);
	assert(pixelShaderBlob != nullptr);

	/*
	PSOを作成
	------------------------------*/
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature;
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = {
		vertexShaderBlob->GetBufferPointer(),vertexShaderBlob->GetBufferSize()
	};
	graphicsPipelineStateDesc.PS = {
		pixelShaderBlob->GetBufferPointer(),pixelShaderBlob->GetBufferSize()
	};
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ(形状)のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//実際に生成
	ID3D12PipelineState* graphicsPipelineState = nullptr;
	hr = dxCommon->GetDevice()->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc, IID_PPV_ARGS(&graphicsPipelineState)
	);
	assert(SUCCEEDED(hr));

	//==================================================
	//ImGui
	//==================================================

	//IMGUI_CHECKVERSION();
	//ImGui::CreateContext();
	//::StyleColorsDark();

	//==================================================
	//頂点データの作成とビュー
	//==================================================

	/*
	Resourceの生成
	------------------------------*/
	//頂点リソース
	ID3D12Resource* vertexResource = CreateBufferResource(dxCommon->GetDevice(), sizeof(Vector4) * 3);

	//マテリアルリソース。color1つ分のサイズを用意する
	ID3D12Resource* materialResource = CreateBufferResource(dxCommon->GetDevice(), sizeof(Vector4));
	//マテリアルのデータを書き込む
	Vector4* materialData = nullptr;
	//書き込むためのアドレスを取得
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//赤を書き込む
	*materialData = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

	//WVP用のリソースを作る
	ID3D12Resource* wvpResource = CreateBufferResource(dxCommon->GetDevice(), sizeof(Matrix4x4));
	//データを書き込む
	Matrix4x4* wvpData = nullptr;
	//書き込むためのアドレスを取得
	wvpResource->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));
	//単位行列を書き込んでおく
	*wvpData = Matrix4x4::Identity();

	/*
	VertexBufferViewの作成
	------------------------------*/
	//頂点バッファービューを作成
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//リソースの先頭アドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(Vector4) * 3;
	//1頂点あたりのサイズ
	vertexBufferView.StrideInBytes = sizeof(Vector4);

	/*
	Resourceにデータを書き込む
	------------------------------*/
	//頂点リソースにデータを書き込む
	Vector4* vertexData = nullptr;
	//書き込むためのアドレスを取得
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//左下
	vertexData[0] = { -0.5f, -0.5f, 0.0f, 1.0f };
	//上
	vertexData[1] = { 0.0f, 0.5f, 0.0f, 1.0f };
	//右下
	vertexData[2] = { 0.5f, -0.5f, 0.0f, 1.0f };

	/*
	ViewportとScissor
	------------------------------*/
	//ビューポート
	D3D12_VIEWPORT viewport{};
	//クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = winApp->kClientWidth;
	viewport.Height = winApp->kClientHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//シザー矩形
	D3D12_RECT scissorRect{};
	//基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = winApp->kClientWidth;
	scissorRect.top = 0;
	scissorRect.bottom = winApp->kClientHeight;

	//Transform変数を作る
	Transform transform{ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f} };

	//ウィンドウの×ボタンが押されるまでループ
	while (winApp->ProcessMessage()!=0) {
			//ゲームの処理

			//==================================================
			//三角形の更新
			//==================================================

			//回転
			transform.rotation.y += 0.01f;

			//カメラのワールド変換データ
			Transform cameraTransform = { {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, -5.0f} };

			//ワールド行列更新
			Matrix4x4 worldMatrix = Matrix4x4::MakeAffineMatrix(transform.scale, transform.rotation, transform.translation);
			Matrix4x4 cameraMatrix =
				Matrix4x4::MakeAffineMatrix(
					cameraTransform.scale, cameraTransform.rotation, cameraTransform.translation
				);
			Matrix4x4 viewMatrix = cameraMatrix.Inversed();
			Matrix4x4 projectionMatrix =
				Matrix4x4::MakeProjectionFovMatrix(
				0.45f, static_cast<float>(winApp->kClientWidth) / static_cast<float>(winApp->kClientHeight),0.1f, 100.0f
				);
			Matrix4x4 worldViewProjectionMatrix = worldMatrix * viewMatrix * projectionMatrix;
			*wvpData = worldViewProjectionMatrix;

			//==================================================
			//三角形の描画
			//==================================================

			dxCommon->PreDraw();

			/*
			コマンドを積む
			------------------------------*/
			dxCommon->GetCommandList()->RSSetViewports(1, &viewport);
			dxCommon->GetCommandList()->RSSetScissorRects(1, &scissorRect);
			//RootSignatureを設定。PSOとは別途設定が必要
			dxCommon->GetCommandList()->SetGraphicsRootSignature(rootSignature);
			dxCommon->GetCommandList()->SetPipelineState(graphicsPipelineState);
			dxCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
			//形状を設定。PSOとは別途設定。同じものを設定
			dxCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//マテリアルCBufferの場所を設定(RootParameter配列の0番目)
			dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
			//wvp用のCBufferの場所を設定
			dxCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, wvpResource->GetGPUVirtualAddress());
			//描画(DrawCall)
			dxCommon->GetCommandList()->DrawInstanced(3, 1, 0, 0);

			dxCommon->PostDraw();
	}

	//出力ウィンドウへの文字出力
	OutputDebugStringA("Hello,DirectX!\n");

	//==================================================
	//解放作業
	//==================================================

	/*
	三角形の描画に利用したもの
	------------------------------*/
	wvpResource->Release();
	materialResource->Release();
	vertexResource->Release();
	graphicsPipelineState->Release();
	signatureBlob->Release();
	if (errorBlob) {
		errorBlob->Release();
	}
	rootSignature->Release();
	pixelShaderBlob->Release();
	vertexShaderBlob->Release();

	dxCommon->Finalize();
	delete dxCommon;

	winApp->Finalize();
	delete winApp;

	/*
	ウィンドウ生成やデバッグに利用したもの
	------------------------------*/

	//リソースリークチェック
	IDXGIDebug1* debug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
		debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		debug->Release();
	}

	return 0;
}
