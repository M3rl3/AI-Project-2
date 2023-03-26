#include "cRenderReticle.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

cRenderReticle::cRenderReticle() {

}

cRenderReticle::~cRenderReticle() {

}

void cRenderReticle::Initialize(GLFWwindow* window, const char* glsl_version) {

    // Init dear ImGUI	
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImGui::StyleColorsDark();		//dark theme
}

void cRenderReticle::Update() {

    // Start a new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImVec2 window_size = ImGui::GetIO().DisplaySize;
    ImVec2 window_center = ImVec2(window_size.x * 0.5f, window_size.y * 0.5f);

    // Crosshair
    ImGui::GetForegroundDrawList()->AddCircle(
                                              window_center,    // window center
                                              window_size.y * 0.01f, // window radius
                                              IM_COL32(255, 255, 255, 255), // color
                                              0.f,      // number of sides
                                              2.f       // thickness of each side
                                             );

    // Render a gui here
    /*ImGui::Begin("");
    ImGui::End();*/

    // Render imgui stuff to screen
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void cRenderReticle::Shutdown() {

    // Gracefully close everything down
    // ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}