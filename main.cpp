#include "SubSystems/FocalEngine/FEngine.h"
using namespace FocalEngine;

FEScene* CurrentScene = nullptr;

void mouseButtonCallback(int button, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse)
	{
		CAMERA_SYSTEM.SetIsIndividualInputActive(CAMERA_SYSTEM.GetMainCameraEntity(CurrentScene), false);
		return;
	}

	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_PRESS)
	{
		CAMERA_SYSTEM.SetIsIndividualInputActive(CAMERA_SYSTEM.GetMainCameraEntity(CurrentScene), true);
	}


	if (button == GLFW_MOUSE_BUTTON_2 && action == GLFW_RELEASE)
	{
		CAMERA_SYSTEM.SetIsIndividualInputActive(CAMERA_SYSTEM.GetMainCameraEntity(CurrentScene), false);
	}
}

void keyButtonCallback(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		ENGINE.Terminate();
}

void LoadSampleEntity()
{
	FEMesh* LoadedMesh = RESOURCE_MANAGER.LoadFEMesh("Resources//Cardboard.model");

	FEMaterial* NewMaterial = RESOURCE_MANAGER.CreateMaterial();
	NewMaterial->Shader = RESOURCE_MANAGER.GetShader("0800253C242B05321A332D09"/*"FEPBRShader"*/);
	NewMaterial->SetAlbedoMap(RESOURCE_MANAGER.LoadFETexture("Resources//Albedo.texture"));
	NewMaterial->SetNormalMap(RESOURCE_MANAGER.LoadFETexture("Resources//NormalMap.texture"));
	NewMaterial->SetAOMap(RESOURCE_MANAGER.LoadFETexture("Resources//AO.texture"));
	NewMaterial->SetRoughnessMap(RESOURCE_MANAGER.LoadFETexture("Resources//Roughness.texture"));

	FEGameModel* NewGameModel = RESOURCE_MANAGER.CreateGameModel(LoadedMesh, NewMaterial);

	FEEntity* Result = CurrentScene->CreateEntity("Cardboard");
	Result->AddComponent<FEGameModelComponent>(NewGameModel);
}

void SetSimpleScene()
{
	ENGINE.AddMouseButtonCallback(mouseButtonCallback);
	ENGINE.AddKeyCallback(keyButtonCallback);

	CurrentScene = SCENE_MANAGER.CreateScene();

	FEEntity* SkyDome = CurrentScene->CreateEntity("SkyDome");
	SkyDome->GetComponent<FETransformComponent>().SetScale(glm::vec3(100.0f));
	SkyDome->AddComponent<FESkyDomeComponent>();

	FEEntity* Sun = CurrentScene->CreateEntity("Sun");
	Sun->AddComponent<FELightComponent>(FE_DIRECTIONAL_LIGHT);
	Sun->GetComponent<FELightComponent>().SetCastShadows(true);
	Sun->GetComponent<FETransformComponent>().SetRotation(glm::vec3(0.0f, 45.0f, 45.0f));

	FEEntity* Camera = CurrentScene->CreateEntity("Camera");
	Camera->AddComponent<FECameraComponent>();
	CAMERA_SYSTEM.SetMainCamera(Camera);
	CAMERA_SYSTEM.SetCameraViewport(Camera, ENGINE.GetDefaultViewport()->GetID());

	LoadSampleEntity();
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