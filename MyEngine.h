#pragma once

namespace MyEngine {
	void Initialize();
	void Finalize();

	void BeginFrame();
	void EndFrame();
	bool ProcessMessage();
}//namespace MyEngine
