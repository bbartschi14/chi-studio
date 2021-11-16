#include "Application.h"
#include "Scene.h"
#include "ChiGraphics/Shaders/ShaderProgram.h"
#include "ChiGraphics/Shaders/OutlineShader.h"
#include "ChiGraphics/Shaders/PointShader.h"
#include "ChiGraphics/Shaders/EdgeShader.h"
#include "ChiGraphics/Shaders/SelectedFaceShader.h"
#include "ChiGraphics/Shaders/TextureBlendShader.h"
#include "ChiGraphics/Shaders/TextureBlurShader.h"
#include "ChiGraphics/Components/ShadingComponent.h"
#include <cassert>
#include <iostream>
#include <glad/glad.h>
#include <glm/gtx/string_cast.hpp>
#include "ChiGraphics/Meshes/PrimitiveFactory.h"
#include "ChiGraphics/InputManager.h"

namespace CHISTUDIO {
	Renderer::Renderer(Application& InApplication) : Application_(InApplication)
	{
		UNUSED(Application_);

		OutlineShader_ = std::make_shared<OutlineShader>();
		PointShader_ = std::make_shared<PointShader>();
		EdgeShader_ = std::make_shared<EdgeShader>();
		SelectedFaceShader_ = std::make_shared<SelectedFaceShader>();
		TextureBlendShader_ = std::make_shared<TextureBlendShader>();
		TextureBlurShader_ = std::make_shared<TextureBlurShader>();

		SceneRenderQuad = PrimitiveFactory::CreateQuad();
	}

	void Renderer::Render(const Scene& InScene)
	{
		SetRenderingOptions();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2{ 0, 0 });
		ImGui::Begin("Scene");
		ImGui::BeginChild("ViewportArea");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
		if (viewportPanelSize.x != CurrentWidth || viewportPanelSize.y != CurrentHeight)
		{
			CurrentWidth = (uint32_t)viewportPanelSize.x;
			CurrentHeight = (uint32_t)viewportPanelSize.y;

			InvalidateFrameBuffer();
		}

		RenderScene(InScene);

		// Add rendered texture to ImGUI scene window
		uint64_t textureID = SceneOutputTexture->GetHandle();
		ImGui::Image(reinterpret_cast<void*>(textureID), ImVec2{ (float)CurrentWidth, (float)CurrentHeight }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
		ImGui::EndChild();


		ImGui::BeginChild("ViewportArea");
		ImGui::SetCursorPos({ 6,6 });

		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4{ 0.1, 0.1, 0.1, 1 });
		ImGui::BeginChild("TopLeft");

		std::vector<SceneNode*> selection = Application_.GetSelectedNodes();
		// Draw mode dropdown
		std::vector<std::string> viewportModeStrings;
		viewportModeStrings.push_back("Object");
		if (selection.size() == 1 && selection[0]->GetComponentPtr<RenderingComponent>() != nullptr)
			viewportModeStrings.push_back("Edit");

		int selectedMode = (int)Application_.GetSceneMode();
		if (selectedMode < viewportModeStrings.size())
		{
			const char* currentSceneModeString = viewportModeStrings[selectedMode].c_str();
			ImGui::Text("Mode");
			ImGui::SetNextItemWidth(200);
			if (ImGui::BeginCombo("##Mode", currentSceneModeString))
			{
				for (int i = 0; i < viewportModeStrings.size(); i++)
				{
					bool isSelected = currentSceneModeString == viewportModeStrings[i];
					if (ImGui::Selectable(viewportModeStrings[i].c_str(), isSelected))
					{
						currentSceneModeString = viewportModeStrings[i].c_str();
						Application_.SetSceneMode((ESceneMode)i);
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}

				ImGui::EndCombo();
			}
		}
		
		InputManager::GetInstance().SetInputBlocked(!ImGui::IsWindowHovered());

		if (ImGui::IsWindowHovered())
		{
			// Viewport hotkeys
			if (ImGui::IsKeyPressed(GLFW_KEY_TAB))
			{
				Application_.TryToggleEditMode();
			}
			if (ImGui::IsMouseClicked(0))
			{
				glm::vec2 mousePos = { ImGui::GetMousePos().x - ImGui::GetWindowPos().x + 8, ImGui::GetMousePos().y - ImGui::GetWindowPos().y + 8}; // Adding default padding manually
				glm::vec2 viewportSize = { CurrentWidth, CurrentHeight };
				std::cout << "Mouse clicked in scene. Mouse Pos: " << glm::to_string(mousePos) << ", Viewport: " << glm::to_string(viewportSize) << std::endl;
				Application_.OnClick(0, mousePos, viewportSize);
			}
		}
		
		ImGui::EndChild();
		ImGui::PopStyleColor();

		ImGui::EndChild();

		ImGui::End();
		ImGui::PopStyleVar();

	}

	uint64_t Renderer::GetSceneOutputTextureHandle() const
	{
		return SceneOutputTexture->GetHandle();
	}

	void Renderer::RenderScene(const Scene& InScene) const
	{
		GL_CHECK(glDisable(GL_BLEND));

		GL_CHECK(glViewport(0, 0, CurrentWidth, CurrentHeight));

		// Clear Window
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		BindGuard FrameBufferBindGuard(SceneColorFrameBuffer.get());

		GL_CHECK(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
		GLenum buffers[] = { GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		GL_CHECK(glDrawBuffers(2, buffers));
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));

		GLenum buffers2[] = { GL_COLOR_ATTACHMENT0 };

		GL_CHECK(glDrawBuffers(1, buffers2));
		GL_CHECK(glClearColor(0.2f, 0.2f, 0.2f, 0.0f));
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		const SceneNode& root = InScene.GetRootNode();
		auto renderingInfo = RetrieveRenderingInfo(InScene);
		auto lightComponentPointers = root.GetComponentPtrsInChildren<LightComponent>();

		if (lightComponentPointers.size() == 0) {
			return;
		}

		CameraComponent* activeCamera = InScene.GetActiveCameraPtr();
		activeCamera->SetAspectRatio(CurrentWidth, CurrentHeight);

		// First pass: depth buffer and stencil buffer
		// Remaining passes - 1: one per light source.
		size_t totalNumberOfPasses = 1 + lightComponentPointers.size();

		for (size_t currentPass = 0; currentPass < totalNumberOfPasses; currentPass++) {

			GL_CHECK(glDepthMask((currentPass == 0) ? GL_TRUE : GL_FALSE));
			bool color_mask = (currentPass == 0) ? GL_FALSE : GL_TRUE;
			GL_CHECK(glColorMask(color_mask, color_mask, color_mask, color_mask));

			for (const auto& NodeAndMatrixPair : renderingInfo) {
				auto renderingComponentPointer = NodeAndMatrixPair.first;
				SceneNode& renderingNode = *renderingComponentPointer->GetNodePtr();

				bool isSelected = renderingNode.IsSelected(); // Get selected info for stencil rendering

				if (isSelected && renderingComponentPointer->bRenderSolid)
				{
					GL_CHECK(glStencilFunc(GL_ALWAYS, 1, 0xFF));
					GL_CHECK(glStencilMask(0xFF));
				}
				else
				{
					GL_CHECK(glStencilFunc(GL_ALWAYS, 1, 0xFF));
					GL_CHECK(glStencilMask(0x00));
				}

				if (renderingComponentPointer->bIsDebugRender)
				{
					// For debug rendering, we use whatever mode is already specified
					auto shadingPointer = renderingNode.GetComponentPtr<ShadingComponent>();
					if (shadingPointer == nullptr) {
						std::cerr << "Some mesh is not attached with a shader during rendering!"
							<< std::endl;
						continue;
					}
					ShaderProgram* shader = shadingPointer->GetShaderPtr();
					if (shader == nullptr) {
						std::cerr << "Shader program is null"
							<< std::endl;
						continue;
					}
					BindGuard shader_bg(shader);

					// Set various uniform variables in the shaders.
					shader->SetTargetNode(renderingNode, NodeAndMatrixPair.second);
					shader->SetCamera(*activeCamera);

					if (currentPass > 0) {
						LightComponent& light = *lightComponentPointers.at(totalNumberOfPasses - currentPass - 1);
						shader->SetLightSource(light);
					}

					renderingComponentPointer->Render();
				}
				else
				{
					// Non debug rendering components have solid vs wireframe vs point modes
					if (renderingComponentPointer->bRenderSolid || (renderingNode.IsSelected() && Application_.AreEditModeFacesSelectable() && Application_.GetSceneMode() == ESceneMode::Edit))
					{
						renderingComponentPointer->SetDrawMode(EDrawMode::Triangles);
						renderingComponentPointer->SetPolygonMode(EPolygonMode::Fill);

						auto shadingPointer = renderingNode.GetComponentPtr<ShadingComponent>();
						if (shadingPointer == nullptr) {
							std::cerr << "Some mesh is not attached with a shader during rendering!"
								<< std::endl;
							continue;
						}
						ShaderProgram* shader = shadingPointer->GetShaderPtr();
						if (shader == nullptr) {
							std::cerr << "Shader program is null"
								<< std::endl;
							continue;
						}
						BindGuard shader_bg(shader);

						// Set various uniform variables in the shaders.
						shader->SetTargetNode(renderingNode, NodeAndMatrixPair.second);
						shader->SetCamera(*activeCamera);

						if (currentPass > 0) {
							LightComponent& light = *lightComponentPointers.at(totalNumberOfPasses - currentPass - 1);
							shader->SetLightSource(light);
						}

						if (isSelected && renderingComponentPointer->bRenderSolid)
						{
							GLenum buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
							GL_CHECK(glDrawBuffers(2, buffers));
						}
						else
						{
							GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
							GL_CHECK(glDrawBuffers(1, buffers));
						}

						renderingComponentPointer->Render();
					}

					{
						// POINTS
						GL_CHECK(glStencilFunc(GL_ALWAYS, 1, 0xFF));
						GL_CHECK(glStencilMask(0x00));
						renderingComponentPointer->SetDrawMode(EDrawMode::Points);

						BindGuard pointShaderBg(PointShader_.get());

						// Set various uniform variables in the shaders.
						PointShader_->SetTargetNode(renderingNode, NodeAndMatrixPair.second);
						PointShader_->SetCamera(*activeCamera);
						PointShader_->SetSelectedStyle(false);

						if (renderingComponentPointer->bRenderPoints || (renderingNode.IsSelected() && Application_.AreEditModeVerticesSelectable() && Application_.GetSceneMode() == ESceneMode::Edit))
						{
							renderingComponentPointer->Render();
						}

						if (renderingComponentPointer->GetVertexObjectPtr()->GetSelectedVertices().size() > 0 && renderingNode.IsSelected() && Application_.GetSceneMode() == ESceneMode::Edit)
						{
							GL_CHECK(glDisable(GL_DEPTH_TEST)); // Disable depth test and write to stencil buffer to get the points on top
							GL_CHECK(glStencilFunc(GL_ALWAYS, 1, 0xFF));
							GL_CHECK(glStencilMask(0xFF));

							PointShader_->SetSelectedStyle(true);
							PointShader_->AssociateVertexArray(renderingComponentPointer->GetVertexObjectPtr()->GetSelectedVertexVertexArray());
							renderingComponentPointer->GetVertexObjectPtr()->GetSelectedVertexVertexArray().Render();
							GL_CHECK(glEnable(GL_DEPTH_TEST));
						}
					}

					{
						// EDGES
						GL_CHECK(glStencilFunc(GL_ALWAYS, 1, 0xFF));
						GL_CHECK(glStencilMask(0x00));

						BindGuard edgeShaderBg(EdgeShader_.get());

						// Set various uniform variables in the shaders.
						EdgeShader_->SetSelectedStyle(false);
						EdgeShader_->SetTargetNode(renderingNode, NodeAndMatrixPair.second);
						EdgeShader_->AssociateVertexArray(renderingComponentPointer->GetVertexObjectPtr()->GetEdgeVertexArray());
						EdgeShader_->SetCamera(*activeCamera);
						if (renderingComponentPointer->bRenderWireframe || (renderingNode.IsSelected() && Application_.AreEditModeEdgesSelectable() && Application_.GetSceneMode() == ESceneMode::Edit))
						{
							renderingComponentPointer->GetVertexObjectPtr()->GetEdgeVertexArray().Render();
						}

						if (renderingComponentPointer->GetVertexObjectPtr()->GetSelectedEdges().size() > 0 && renderingNode.IsSelected() && Application_.GetSceneMode() == ESceneMode::Edit)
						{
							GL_CHECK(glDisable(GL_DEPTH_TEST)); // Disable depth test and write to stencil buffer to get the edges on top
							GL_CHECK(glStencilFunc(GL_ALWAYS, 1, 0xFF));
							GL_CHECK(glStencilMask(0xFF));

							EdgeShader_->SetSelectedStyle(true);
							EdgeShader_->AssociateVertexArray(renderingComponentPointer->GetVertexObjectPtr()->GetSelectedEdgeVertexArray());
							renderingComponentPointer->GetVertexObjectPtr()->GetSelectedEdgeVertexArray().Render();
							GL_CHECK(glEnable(GL_DEPTH_TEST));
						}
					}


					if (renderingComponentPointer->GetVertexObjectPtr()->GetSelectedFaces().size() > 0 && renderingNode.IsSelected() && Application_.GetSceneMode() == ESceneMode::Edit)
					{
						// Selected Faces
						GL_CHECK(glDisable(GL_DEPTH_TEST)); // Disable depth test and write to stencil buffer to get the edges on top
						GL_CHECK(glStencilFunc(GL_ALWAYS, 1, 0xFF));
						GL_CHECK(glStencilMask(0xFF));

						GL_CHECK(glEnable(GL_BLEND));
						GL_CHECK(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

						BindGuard selectedFaceShaderBg(SelectedFaceShader_.get());
						SelectedFaceShader_->SetTargetNode(renderingNode, NodeAndMatrixPair.second);
						SelectedFaceShader_->AssociateVertexArray(renderingComponentPointer->GetVertexObjectPtr()->GetSelectedFaceVertexArray());
						SelectedFaceShader_->SetCamera(*activeCamera);

						renderingComponentPointer->GetVertexObjectPtr()->GetSelectedFaceVertexArray().Render();
						GL_CHECK(glEnable(GL_DEPTH_TEST));
						GL_CHECK(glDisable(GL_BLEND));
					}


					if (renderingComponentPointer->GetVertexObjectPtr()->IsDebugNormals())
					{
						GL_CHECK(glStencilFunc(GL_ALWAYS, 1, 0xFF));
						GL_CHECK(glStencilMask(0x00));

						BindGuard pointShaderBg(EdgeShader_.get());
						EdgeShader_->SetSelectedStyle(false);
						EdgeShader_->SetTargetNode(renderingNode, NodeAndMatrixPair.second);
						EdgeShader_->AssociateVertexArray(renderingComponentPointer->GetVertexObjectPtr()->GetDebugNormalsVertexArray());
						EdgeShader_->SetCamera(*activeCamera);

						renderingComponentPointer->GetVertexObjectPtr()->GetDebugNormalsVertexArray().Render();
					}
				}
			}
		}
		// Post processing
		if (Application_.GetSceneMode() != ESceneMode::Edit)
		{
			GL_CHECK(glDisable(GL_DEPTH_TEST));
			GL_CHECK(glDisable(GL_BLEND));
			BindGuard textureBlurShader(TextureBlurShader_.get());
			TextureBlurShader_->SetVertexObject(*SceneRenderQuad);
			TextureBlurShader_->SetTexture(*customMaskTexture);
			GLenum newBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
			GL_CHECK(glDrawBuffers(3, newBuffers));
			SceneRenderQuad->GetVertexArray().Render();

			BindGuard textureShader(TextureBlendShader_.get());
			TextureBlendShader_->SetVertexObject(*SceneRenderQuad);
			TextureBlendShader_->SetTextures(*SceneOutputTexture, *OutlineTexture);
			SceneRenderQuad->GetVertexArray().Render();
		}
		

		// Re-enable writing to depth buffer.
		GL_CHECK(glDepthMask(GL_TRUE));

		GL_CHECK(glStencilMask(0xFF));
		GL_CHECK(glStencilFunc(GL_ALWAYS, 1, 0xFF));
	}

	void Renderer::SetRenderingOptions() const
	{
		// Enable depth test.
		GL_CHECK(glEnable(GL_DEPTH_TEST));
		GL_CHECK(glDepthFunc(GL_LEQUAL));

		// Enable stencil test
		GL_CHECK(glEnable(GL_STENCIL_TEST));
		GL_CHECK(glStencilFunc(GL_NOTEQUAL, 1, 0xFF));
		GL_CHECK(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));

		// Enable blending for multi-pass forward rendering.
		GL_CHECK(glEnable(GL_BLEND));
		GL_CHECK(glBlendFunc(GL_ONE, GL_ONE));

		GL_CHECK(glEnable(GL_PROGRAM_POINT_SIZE));

	}

	Renderer::RenderingInfo Renderer::RetrieveRenderingInfo(const Scene& InScene) const
	{
		RenderingInfo info;
		const SceneNode& root = InScene.GetRootNode();
		// Efficient implementation without redundant matrix multiplcations.
		RecursiveRetrieve(root, info, glm::mat4(1.0f));
		return info;
	}

	void Renderer::RecursiveRetrieve(const SceneNode& InNode, RenderingInfo& InInfo, const glm::mat4& InModelMatrix)
	{
		// model_matrix is parent to world transformation.
		glm::mat4 new_matrix = InModelMatrix * InNode.GetTransform().GetLocalToParentMatrix();
		auto robj_ptr = InNode.GetComponentPtr<RenderingComponent>();
		if (robj_ptr != nullptr && InNode.IsActive())
			InInfo.emplace_back(robj_ptr, new_matrix);

		size_t child_count = InNode.GetChildrenCount();
		for (size_t i = 0; i < child_count; i++) {
			RecursiveRetrieve(InNode.GetChild(i), InInfo, new_matrix);
		}
	}

	void Renderer::InvalidateFrameBuffer()
	{
		SceneOutputTexture = make_unique<FTexture>();
		SceneOutputTexture->Reserve(GL_RGB, CurrentWidth, CurrentHeight, GL_RGBA, GL_UNSIGNED_BYTE);

		SceneColorFrameBuffer = make_unique<Framebuffer>();
		SceneColorFrameBuffer->AssociateTexture(*SceneOutputTexture, GL_COLOR_ATTACHMENT0);

		SceneDepthStencilTexture = make_unique<FTexture>();
		SceneDepthStencilTexture->Reserve(GL_DEPTH24_STENCIL8, CurrentWidth, CurrentHeight, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
		SceneColorFrameBuffer->AssociateTexture(*SceneDepthStencilTexture, GL_DEPTH_STENCIL_ATTACHMENT);

		PostProcessOutputTexture = make_unique<FTexture>();
		PostProcessOutputTexture->Reserve(GL_RGB, CurrentWidth, CurrentHeight, GL_RGBA, GL_UNSIGNED_BYTE);

		PostProcessFrameBuffer = make_unique<Framebuffer>();
		PostProcessFrameBuffer->AssociateTexture(*PostProcessOutputTexture, GL_COLOR_ATTACHMENT0);

		// Custom depth buffer
		customMaskTexture = make_unique<FTexture>(TextureConfig{{GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},{GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE},{GL_TEXTURE_MIN_FILTER, GL_LINEAR },{GL_TEXTURE_MAG_FILTER, GL_LINEAR }});
		customMaskTexture->Reserve(GL_RGB, CurrentWidth, CurrentHeight, GL_RGBA, GL_UNSIGNED_BYTE);
		SceneColorFrameBuffer->AssociateTexture(*customMaskTexture, GL_COLOR_ATTACHMENT1);

		OutlineTexture = make_unique<FTexture>(TextureConfig{ {GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE},{GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE},{GL_TEXTURE_MIN_FILTER, GL_LINEAR },{GL_TEXTURE_MAG_FILTER, GL_LINEAR } });
		OutlineTexture->Reserve(GL_RGB, CurrentWidth, CurrentHeight, GL_RGBA, GL_UNSIGNED_BYTE);
		SceneColorFrameBuffer->AssociateTexture(*OutlineTexture, GL_COLOR_ATTACHMENT2);
	}
}