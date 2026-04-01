<<<<<<< HEAD
#pragma once
#include "orchestrator.h"
#include <memory>
#include <string>

struct GLFWwindow;

class GUI {
public:
    explicit GUI(Orchestrator& orch);
    ~GUI();

    // Initialise GLFW + OpenGL + Dear ImGui
    bool init(int windowW = 1600, int windowH = 900,
              const std::string& title = "ParallelVisionSuite");

    // Main render loop — blocks until window is closed
    void run();

    // Gracefully close
    void close();

private:
    void beginFrame();
    void endFrame();
    void renderMainMenuBar();
    void renderDockspace();
    void renderAllPanels();

    void applyImGuiStyle();
    void loadFonts();

    GLFWwindow* m_window    = nullptr;
    Orchestrator& m_orch;
    bool          m_running = false;
=======
#pragma once
#include "orchestrator.h"
#include <memory>
#include <string>

struct GLFWwindow;

class GUI {
public:
    explicit GUI(Orchestrator& orch);
    ~GUI();

    // Initialise GLFW + OpenGL + Dear ImGui
    bool init(int windowW = 1600, int windowH = 900,
              const std::string& title = "ParallelVisionSuite");

    // Main render loop — blocks until window is closed
    void run();

    // Gracefully close
    void close();

private:
    void beginFrame();
    void endFrame();
    void renderMainMenuBar();
    void renderDockspace();
    void renderAllPanels();

    void applyImGuiStyle();
    void loadFonts();

    GLFWwindow* m_window    = nullptr;
    Orchestrator& m_orch;
    bool          m_running = false;
>>>>>>> 7d871911432ff019623cab0b3e39a8bf91592573
};