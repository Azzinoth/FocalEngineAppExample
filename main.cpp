#include "SubSystems/FocalEngine/FEngine.h"
using namespace FocalEngine;

void mouseButtonCallback(int button, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse)
	{
		ENGINE.GetCamera()->SetIsInputActive(false);
		return;
	}

	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
	{
		ENGINE.GetCamera()->SetIsInputActive(true);
	}


	if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE)
	{
		ENGINE.GetCamera()->SetIsInputActive(false);
	}
}

void keyButtonCallback(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		ENGINE.Terminate();

	ENGINE.GetCamera()->KeyboardInput(key, scancode, action, mods);
}

FEEntity* LoadSampleEntity()
{
	FEMesh* LoadedMesh = RESOURCE_MANAGER.LoadFEMesh("Resources//Cardboard.model");

	FEMaterial* NewMaterial = RESOURCE_MANAGER.CreateMaterial();
	NewMaterial->Shader = RESOURCE_MANAGER.GetShader("0800253C242B05321A332D09"/*"FEPBRShader"*/);
	NewMaterial->SetAlbedoMap(RESOURCE_MANAGER.LoadFETexture("Resources//Albedo.texture"));
	NewMaterial->SetNormalMap(RESOURCE_MANAGER.LoadFETexture("Resources//NormalMap.texture"));
	NewMaterial->SetAOMap(RESOURCE_MANAGER.LoadFETexture("Resources//AO.texture"));
	NewMaterial->SetRoughnessMap(RESOURCE_MANAGER.LoadFETexture("Resources//Roughness.texture"));

	FEGameModel* NewGameModel = RESOURCE_MANAGER.CreateGameModel(LoadedMesh, NewMaterial);
	FEPrefab* NewPrefab = RESOURCE_MANAGER.CreatePrefab(NewGameModel);

	return SCENE.AddEntity(NewPrefab);
}

void SetCamera(FEEntity* EntityToLookAt)
{
	FEModelViewCamera* NewCamera = new FEModelViewCamera("ModelView Camera");
	NewCamera->SetAspectRatio(static_cast<float>(ENGINE.GetRenderTargetWidth()) / static_cast<float>(ENGINE.GetRenderTargetHeight()));
	glm::vec3 Position = glm::vec3(0.0f);
	Position = EntityToLookAt->Transform.GetPosition();
	NewCamera->SetTrackingObjectPosition(Position);
	NewCamera->SetDistanceToModel(50.0f);
	NewCamera->SetAzimutAngle(45.0f);
	NewCamera->SetPolarAngle(45.0f);
	NewCamera->SetIsInputActive(false);
	ENGINE.SetCamera(NewCamera);
}

void SetSimpleScene()
{
	ENGINE.AddMouseButtonCallback(mouseButtonCallback);
	ENGINE.AddKeyCallback(keyButtonCallback);

	RENDERER.SetSkyEnabled(true);
	RENDERER.SetDistanceFogEnabled(false);

	FEDirectionalLight* Sun = reinterpret_cast<FEDirectionalLight*>(SCENE.AddLight(FE_DIRECTIONAL_LIGHT, "Sun"));
	Sun->Transform.SetRotation(glm::vec3(0.0f, 45.0f, 45.0f));
	Sun->SetCastShadows(true);

	FEEntity* NewEntity = LoadSampleEntity();
	SetCamera(NewEntity);
}

void RenderSimpleSettingWindow()
{
	if (ImGui::Begin("Example of window"))
	{
		ImGui::Text("FPS : %f", ImGui::GetIO().Framerate);
		ImGui::End();
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	ENGINE.InitWindow();
	THREAD_POOL.SetConcurrentThreadCount(10);

	const int FrameCountTillMeasure = 20;
	double CPUFrameDurations[FrameCountTillMeasure] = { 0.0f };
	double GPUFrameDurations[FrameCountTillMeasure] = { 0.0f };
	int FrameCounter = 0;

	double AverageCpuFrameDuration = 0.0;
	double AverageGpuFrameDuration = 0.0;

	SetSimpleScene();

	while (ENGINE.IsNotTerminated())
	{
		ENGINE.BeginFrame();
		ENGINE.Render();

		RenderSimpleSettingWindow();
		ENGINE.EndFrame();

		// CPU and GPU Time
		CPUFrameDurations[FrameCounter] = ENGINE.GetCpuTime();
		GPUFrameDurations[FrameCounter] = ENGINE.GetGpuTime();
		FrameCounter++;

		if (FrameCounter > FrameCountTillMeasure - 1)
		{
			AverageCpuFrameDuration = 0.0f;
			AverageGpuFrameDuration = 0.0f;
			for (size_t i = 0; i < FrameCountTillMeasure; i++)
			{
				AverageCpuFrameDuration += CPUFrameDurations[i];
				AverageGpuFrameDuration += GPUFrameDurations[i];
			}
			AverageCpuFrameDuration /= FrameCountTillMeasure;
			AverageGpuFrameDuration /= FrameCountTillMeasure;
			
			FrameCounter = 0;
		}

		std::string CPUMs = std::to_string(AverageCpuFrameDuration);
		CPUMs.erase(CPUMs.begin() + 4, CPUMs.end());

		std::string GPUMs = std::to_string(AverageGpuFrameDuration);
		GPUMs.erase(GPUMs.begin() + 4, GPUMs.end());

		std::string FrameMs = std::to_string(AverageCpuFrameDuration + AverageGpuFrameDuration);
		FrameMs.erase(FrameMs.begin() + 4, FrameMs.end());

		std::string caption = "CPU time : ";
		caption += CPUMs;
		caption += " ms";
		caption += "  GPU time : ";
		caption += GPUMs;
		caption += " ms";
		caption += "  Frame time : ";
		caption += FrameMs;
		caption += " ms";

		ENGINE.SetWindowCaption(caption.c_str());
	}
	
	return 0;
}