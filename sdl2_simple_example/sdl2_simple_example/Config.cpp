#include "Config.h"
#include "MyWindow.h"


Config::Config() {}
Config::~Config() {}

void Config::draw(SDL_Window* _window) {
    ImGui::Begin("Config");
    ImGui::Text("Configuracion general del sistema");

    // Sección de resolución de pantalla
    ImGui::Text("Resolución de pantalla:");

    struct ResolutionItem {
        ivec2 resolution;
        std::string name;
    };

    std::vector<ResolutionItem> resolutions = {
        { {800, 600}, "800 x 600" },
        { {1024, 768}, "1024 x 768" },
        { {1280, 720}, "1280 x 720" },
        { {1360, 768}, "1360 x 768" },
        { {1366, 768}, "1366 x 768" },
        { {1440, 900}, "1440 x 900" },
        { {1600, 900}, "1600 x 900" },
        { {1920, 1080}, "1920 x 1080" }
    };

    static int selectedResolutionIndex = 0;
    if (ImGui::Combo("Resolucion", &selectedResolutionIndex, [](void* data, int idx, const char** out_text) {
        const std::vector<ResolutionItem>* resolutions = static_cast<const std::vector<ResolutionItem>*>(data);
        *out_text = resolutions->at(idx).name.c_str(); // Return resolution name
        return true;
        }, (void*)&resolutions, resolutions.size())) {
        selectedResolution = resolutions[selectedResolutionIndex].resolution;

        // Update window size
        SDL_SetWindowSize(_window, selectedResolution.x, selectedResolution.y);

        // Ensure we apply the new projection and viewport
        if (!isFullscreen) {
            renderer.applyProjectionAndViewport(selectedResolution); // Aplica la resolución seleccionada
        }
    }

    // Sección de pantalla completa
    if (ImGui::Checkbox("Pantalla completa", &isFullscreen)) {
        if (isFullscreen) {
            SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
            logMessage("Pantalla completa activada.");
        }
        else {
            SDL_SetWindowFullscreen(_window, 0);  // 0 means windowed mode
            logMessage("Pantalla completa desactivada.");
        }

        // Get the current window size dynamically after changing fullscreen mode
        int windowWidth, windowHeight;
        SDL_GetWindowSize(_window, &windowWidth, &windowHeight);

        // Apply the new window size for projection and viewport
        renderer.applyProjectionAndViewport(ivec2(windowWidth, windowHeight));
    }

    ImGui::Text("Grafico FPS");
    static float values[90] = {};
    static int values_offset = 0;
    values[values_offset] = fps; // Ejemplo fijo de FPS
    values_offset = (values_offset + 1) % 90;
    char fpsText[16];
    sprintf_s(fpsText, "%d fps", static_cast<int>(fps));
    ImGui::PlotLines("FPS", values, IM_ARRAYSIZE(values), values_offset, fpsText, 0.0f, 100.0f, ImVec2(0, 80));

    ImGui::Text("Consumo de Memoria: ");
    try {
        MemoryInfo memInfo = MemoryUsage::getMemoryInfo();
        ImGui::Separator();
        ImGui::Text("Consumo de Memoria:");
        ImGui::Text("Memoria Total: %llu MB", memInfo.totalMemory);
        ImGui::Text("Memoria Libre: %llu MB", memInfo.freeMemory);
        ImGui::Text("Memoria Usada: %llu MB", memInfo.usedMemory);
        static float totalMemoryValues[90];
        static float freeMemoryValues[90];
        static float usedMemoryValues[90];
        static int memValuesOffset = 0;

        totalMemoryValues[memValuesOffset] = memInfo.totalMemory;
        freeMemoryValues[memValuesOffset] = memInfo.freeMemory;
        usedMemoryValues[memValuesOffset] = memInfo.usedMemory;
        memValuesOffset = (values_offset + 1) % 90;

        ImGui::PlotLines("TotalMem", totalMemoryValues, IM_ARRAYSIZE(totalMemoryValues), memValuesOffset, "TotalMem", 0.0f, 100.0f, ImVec2(0, 80));
        ImGui::PlotLines("freeMem", freeMemoryValues, IM_ARRAYSIZE(freeMemoryValues), memValuesOffset, "FreeMem", 0.0f, 100.0f, ImVec2(0, 80));
        ImGui::PlotLines("UsedMem", usedMemoryValues, IM_ARRAYSIZE(usedMemoryValues), memValuesOffset, "UsedMem", 0.0f, 100.0f, ImVec2(0, 80));
    }
    catch (const std::exception& e) {
        ImGui::Text("Error obteniendo memoria: %s", e.what());
    }
    ImGui::Text("Deteccion de maquinaria i versions de programario:");
    ImGui::Text("SDL, OpenGL, DevIL");
    ImGui::End();
}