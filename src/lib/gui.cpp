#include "gui.hpp"

void GUI::render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool sync = Renderer::isVSynced();
    static bool fullscreen = Renderer::isFullscreen();
    static bool maximized = Renderer::isMaximized();
    static bool antialiasing = Renderer::isAntialiased();

    ImGui::Begin("Sidebar", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    ImGui::Text("Version: %s", Renderer::version.c_str()); 
    ImGui::Text("Profile: %s", Renderer::profile.c_str());
    ImGui::Text("Shading Language: %s", Renderer::shadingLanguage.c_str());
    ImGui::Text("Vendor: %s", Renderer::vendor.c_str());
    ImGui::Text("Renderer: %s", Renderer::renderer.c_str());
    
    ImGui::Separator();
    
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    
    ImGui::Separator();
    ImGui::Text("Press V to make camera static.");
    ImGui::Text("Press C to make camera first-person.");
    ImGui::Separator();

    if (ImGui::Checkbox("Enable V-Sync", &sync)) {
        Renderer::setVsync(sync);
    }

    if (ImGui::Checkbox("Enable Antialiasing (MSAA 4x)", &antialiasing)) {
        Renderer::setAntialiasing(antialiasing);
    }

    if (ImGui::Checkbox("Enable Maximization", &maximized)) {
        Renderer::setMaximization(maximized);
    }

    if (ImGui::Checkbox("Enable Fullscreen", &fullscreen)) {
        Renderer::setFullscreen(fullscreen);
    }
    
    ImGui::Separator();
    
    if (ImGui::Button("Screenshot")) {
        Renderer::getScreenshot();
    }
    ImGui::SameLine(0.0f, 20.0f);
    if (ImGui::Button("Close App")) {
        glfwSetWindowShouldClose(Renderer::window, true);
    }
    
    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());    
}