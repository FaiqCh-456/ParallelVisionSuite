#include "gui.h"
#include "orchestrator.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "implot.h"
extern "C" {
    #include <glad/glad.h>
}
#include <GLFW/glfw3.h>
#include "panels/image_preview_panel.h"
#include "panels/video_preview_panel.h"
#include "panels/filter_panel.h"
#include "panels/mode_panel.h"
#include "panels/benchmark_panel.h"
#include "panels/log_panel.h"
#include "panels/thread_viz_panel.h"
#include "panels/arch_anim_panel.h"
#include <stdexcept>
#include <iostream>


static std::unique_ptr<ImagePreviewPanel>  g_imgPanel;
static std::unique_ptr<VideoPreviewPanel>  g_vidPanel;
static std::unique_ptr<FilterPanel>        g_filterPanel;
static std::unique_ptr<ModePanel>          g_modePanel;
static std::unique_ptr<BenchmarkPanel>     g_benchPanel;
static std::unique_ptr<LogPanel>           g_logPanel;
static std::unique_ptr<ThreadVizPanel>     g_threadPanel;
static std::unique_ptr<ArchAnimPanel>      g_archPanel;
 
GUI::GUI(Orchestrator& orch) : m_orch(orch) {}
GUI::~GUI() { close(); }
 
bool GUI::init(int w, int h, const std::string& title) {
    if (!glfwInit()) { std::cerr << "GLFW init failed\n"; return false; }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
    if (!m_window) { glfwTerminate(); return false; }
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD init failed\n"; return false;
    }
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.IniFilename = "imgui_layout.ini";
    applyImGuiStyle();
    loadFonts();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    g_imgPanel    = std::make_unique<ImagePreviewPanel>(m_orch);
    g_vidPanel    = std::make_unique<VideoPreviewPanel>(m_orch);
    g_filterPanel = std::make_unique<FilterPanel>(m_orch);
    g_modePanel   = std::make_unique<ModePanel>(m_orch);
    g_benchPanel  = std::make_unique<BenchmarkPanel>(m_orch);
    g_logPanel    = std::make_unique<LogPanel>();
    g_threadPanel = std::make_unique<ThreadVizPanel>(m_orch);
    g_archPanel   = std::make_unique<ArchAnimPanel>();
    m_running = true;
    return true;
}
 
void GUI::run() {
    while (!glfwWindowShouldClose(m_window) && m_running) {
        glfwPollEvents();
        beginFrame();
        renderDockspace();
        renderMainMenuBar();
        renderAllPanels();
        endFrame();
    }
}
 
void GUI::close() {
    if (!m_window) return;
    g_imgPanel.reset(); g_vidPanel.reset(); g_filterPanel.reset();
    g_modePanel.reset(); g_benchPanel.reset(); g_logPanel.reset();
    g_threadPanel.reset(); g_archPanel.reset();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_window);
    glfwTerminate();
    m_window = nullptr;
}
 
void GUI::beginFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
 
void GUI::endFrame() {
    ImGui::Render();
    int dw, dh;
    glfwGetFramebufferSize(m_window, &dw, &dh);
    glViewport(0, 0, dw, dh);
    glClearColor(0.10f, 0.10f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(m_window);
}
 
void GUI::renderDockspace() {
    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(vp->WorkPos);
    ImGui::SetNextWindowSize(vp->WorkSize);
    ImGui::SetNextWindowViewport(vp->ID);
    ImGuiWindowFlags hf =
        ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus |
        ImGuiWindowFlags_NoNavFocus;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,   0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,    ImVec2(0,0));
    ImGui::Begin("##DockHost", nullptr, hf);
    ImGui::PopStyleVar(3);
 
    ImGuiID dockID = ImGui::GetID("MainDock");
    if (!ImGui::DockBuilderGetNode(dockID)) {
        ImGui::DockBuilderRemoveNode(dockID);
        ImGui::DockBuilderAddNode(dockID, ImGuiDockNodeFlags_None);
        ImGui::DockBuilderSetNodeSize(dockID, vp->WorkSize);
 
        ImGuiID left, center, right, bottom, rtop, rbot;
        ImGui::DockBuilderSplitNode(dockID,   ImGuiDir_Left,  0.18f, &left,   &center);
        ImGui::DockBuilderSplitNode(center,   ImGuiDir_Right, 0.32f, &right,  &center);
        ImGui::DockBuilderSplitNode(center,   ImGuiDir_Down,  0.28f, &bottom, &center);
        ImGui::DockBuilderSplitNode(right,    ImGuiDir_Down,  0.52f, &rbot,   &rtop);
 
        ImGui::DockBuilderDockWindow("Filter Controls",       left);
        ImGui::DockBuilderDockWindow("Execution Mode",        left);
        ImGui::DockBuilderDockWindow("Image Preview",         center);
        ImGui::DockBuilderDockWindow("Video Preview",         center);
        ImGui::DockBuilderDockWindow("Debug Log",             bottom);
        ImGui::DockBuilderDockWindow("Parallel Architecture", bottom);
        ImGui::DockBuilderDockWindow("Thread Activity",       rtop);
        ImGui::DockBuilderDockWindow("Benchmark",             rbot);
        ImGui::DockBuilderFinish(dockID);
    }
 
    ImGui::DockSpace(dockID, ImVec2(0,0), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
}
 
void GUI::renderMainMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open Image",  "Ctrl+O")) m_orch.openImageDialog();
            if (ImGui::MenuItem("Open Video",  "Ctrl+V")) m_orch.openVideoDialog();
            if (ImGui::MenuItem("Save Result", "Ctrl+S")) m_orch.saveResultDialog();
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) m_running = false;
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            ImGui::MenuItem("Filter Controls",       nullptr, &g_filterPanel->visible);
            ImGui::MenuItem("Execution Mode",        nullptr, &g_modePanel->visible);
            ImGui::MenuItem("Image Preview",         nullptr, &g_imgPanel->visible);
            ImGui::MenuItem("Video Preview",         nullptr, &g_vidPanel->visible);
            ImGui::MenuItem("Thread Activity",       nullptr, &g_threadPanel->visible);
            ImGui::MenuItem("Parallel Architecture", nullptr, &g_archPanel->visible);
            ImGui::MenuItem("Debug Log",             nullptr, &g_logPanel->visible);
            ImGui::MenuItem("Benchmark",             nullptr, &g_benchPanel->visible);
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Benchmark")) {
            if (ImGui::MenuItem("Run Full Suite")) g_benchPanel->startBenchmark();
            if (ImGui::MenuItem("Export CSV"))     g_benchPanel->exportCSV();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) ImGui::OpenPopup("AboutDlg");
            ImGui::EndMenu();
        }
        const char* gpuStr = m_orch.gpuAvailable()
            ? m_orch.gpuName().c_str() : "None (OpenMP mode)";
        float tw = ImGui::CalcTextSize(gpuStr).x + 70;
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - tw);
        ImGui::TextDisabled("GPU: %s", gpuStr);
        ImGui::EndMainMenuBar();
    }
    if (ImGui::BeginPopupModal("AboutDlg", nullptr,
                               ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("ParallelVisionSuite v1.0");
        ImGui::Separator();
        ImGui::Text("Parallel & Distributed Computing Project");
        ImGui::Text("Filters: Gaussian, Sobel, Median, Laplacian, Sharpen");
        ImGui::Text("Modes:   Serial, OpenMP, MPI, MPI+OpenMP");
        if (ImGui::Button("Close", ImVec2(120,0))) ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}
 
void GUI::renderAllPanels() {
    m_orch.runDialogs();
    if (g_filterPanel->visible) g_filterPanel->render();
    if (g_modePanel->visible)   g_modePanel->render();
    if (g_imgPanel->visible)    g_imgPanel->render();
    if (g_vidPanel->visible)    g_vidPanel->render();
    if (g_benchPanel->visible)  g_benchPanel->render();
    if (g_logPanel->visible)    g_logPanel->render();
    if (g_threadPanel->visible) g_threadPanel->render();
    if (g_archPanel->visible)   g_archPanel->render();
}
 
void GUI::applyImGuiStyle() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 6.0f; s.FrameRounding = 4.0f;
    s.ScrollbarRounding = 4.0f; s.GrabRounding = 4.0f;
    s.TabRounding = 4.0f;
    s.WindowPadding = ImVec2(10,10); s.FramePadding = ImVec2(6,4);
    s.ItemSpacing   = ImVec2(8,6);
    ImVec4* c = s.Colors;
    c[ImGuiCol_WindowBg]         = ImVec4(0.13f,0.14f,0.17f,1.00f);
    c[ImGuiCol_TitleBg]          = ImVec4(0.08f,0.08f,0.10f,1.00f);
    c[ImGuiCol_TitleBgActive]    = ImVec4(0.15f,0.30f,0.55f,1.00f);
    c[ImGuiCol_MenuBarBg]        = ImVec4(0.10f,0.10f,0.13f,1.00f);
    c[ImGuiCol_Header]           = ImVec4(0.20f,0.40f,0.70f,0.55f);
    c[ImGuiCol_HeaderHovered]    = ImVec4(0.26f,0.50f,0.85f,0.80f);
    c[ImGuiCol_Button]           = ImVec4(0.20f,0.40f,0.70f,0.65f);
    c[ImGuiCol_ButtonHovered]    = ImVec4(0.30f,0.55f,0.90f,0.80f);
    c[ImGuiCol_ButtonActive]     = ImVec4(0.15f,0.35f,0.65f,1.00f);
    c[ImGuiCol_FrameBg]          = ImVec4(0.18f,0.19f,0.23f,1.00f);
    c[ImGuiCol_FrameBgHovered]   = ImVec4(0.24f,0.26f,0.32f,1.00f);
    c[ImGuiCol_SliderGrab]       = ImVec4(0.30f,0.55f,0.90f,1.00f);
    c[ImGuiCol_CheckMark]        = ImVec4(0.30f,0.55f,0.90f,1.00f);
    c[ImGuiCol_Tab]              = ImVec4(0.15f,0.17f,0.22f,1.00f);
    c[ImGuiCol_TabActive]        = ImVec4(0.20f,0.40f,0.70f,1.00f);
    c[ImGuiCol_TabHovered]       = ImVec4(0.26f,0.50f,0.85f,0.80f);
    c[ImGuiCol_Separator]        = ImVec4(0.25f,0.27f,0.35f,1.00f);
    c[ImGuiCol_DockingPreview]   = ImVec4(0.20f,0.40f,0.70f,0.70f);
    c[ImGuiCol_DockingEmptyBg]   = ImVec4(0.10f,0.10f,0.12f,1.00f);
}
 
void GUI::loadFonts() {
    ImGuiIO& io = ImGui::GetIO();
    ImFont* f = io.Fonts->AddFontFromFileTTF("assets/fonts/Roboto-Regular.ttf", 15.0f);
    if (!f) io.Fonts->AddFontDefault();
}