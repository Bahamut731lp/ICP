#include "gui.hpp"
#include "video.hpp"

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

    ImGui::Separator();

    cv::Mat frame;
    if (Video::getFrame(frame) && !frame.empty()) {
        // 1. Prepare the data (OpenCV BGR -> OpenGL RGB)
        cv::cvtColor(frame, frame, cv::COLOR_BGR2RGB);

        // 2. Bind the existing texture ID
        GLuint texID = Video::getTextureId();
        glBindTexture(GL_TEXTURE_2D, texID);

        // 3. Ensure the texture state is valid (Important for some drivers)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        // 4. Update the texture pixels
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        
        // We use glTexImage2D if the frame size might change, 
        // or glTexSubImage2D for better performance if size is constant.
        glTexImage2D(
            GL_TEXTURE_2D, 0, GL_RGB, 
            frame.cols, frame.rows, 0, 
            GL_RGB, GL_UNSIGNED_BYTE, 
            frame.data
        );
    }

    // 5. Draw the final result
    ImTextureID imguiID = (ImTextureID)(intptr_t)Video::getTextureId();
    ImGui::Image(imguiID, ImVec2(320, 240));

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());    
}