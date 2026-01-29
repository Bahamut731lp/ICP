#include "gui.hpp"

void GUI::render(GlRender* instance)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool sync = instance->isVSynced();
    static bool fullscreen = instance->isFullscreen();
    static bool maximized = instance->isMaximized();
    static bool antialiasing = instance->isAntialiased();

    ImGui::Begin("Sidebar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    ImGui::Text("Version: %s", instance->version.c_str()); 
    ImGui::Text("Profile: %s", instance->profile.c_str());
    ImGui::Text("Shading Language: %s", instance->shadingLanguage.c_str());
    ImGui::Text("Vendor: %s", instance->vendor.c_str());
    ImGui::Text("Renderer: %s", instance->renderer.c_str());
    
    ImGui::Separator();
    
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    
    ImGui::Separator();

    if (ImGui::Checkbox("Enable V-Sync", &sync)) {
        instance->setVsync(sync);
    }

    if (ImGui::Checkbox("Enable Antialiasing (MSAA 4x)", &antialiasing)) {
        instance->setAntialiasing(antialiasing);
    }

    if (ImGui::Checkbox("Enable Maximization", &maximized)) {
        instance->setMaximization(maximized);
    }

    if (ImGui::Checkbox("Enable Fullscreen", &fullscreen)) {
        instance->setFullscreen(fullscreen);
    }
    
    ImGui::Separator();
    
    if (ImGui::Button("Screenshot")) {
        instance->getScreenshot();
    }
    ImGui::SameLine(0.0f, 20.0f);
    if (ImGui::Button("Close App")) {
        glfwSetWindowShouldClose(instance->window, true);
    }
    
    ImGui::End();
    ImGui::Render();
}