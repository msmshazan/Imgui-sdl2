#include "main.h"

int main(int argc ,char** argv)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER) != 0)
    {
        printf("Error: %s", SDL_GetError());
        return -1;
    }

    // Setup window
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    SDL_Window *window = SDL_CreateWindow("App Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, current.w, current.h, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE|SDL_WINDOW_FULLSCREEN);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);

    // Setup ImGui binding
    ImGui_ImplSdl_Init(window);

    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyClean.ttf", 13.0f);
    //io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    io.Fonts->AddFontFromFileTTF("font/LiberationMono.ttf", 15.0f,0, io.Fonts->GetGlyphRangesJapanese());

 bool show_test_window = false;
    bool show_another_window = false;
    bool show_login_window = false;
char name[256] = {};
            char pass[256] = {};
                ImageData Brick ={};
    LoadImage("Hello.png",&Brick);
    
    // Main loop
    bool running = true;
    while (running)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdl_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
        }
        ImGui::PushStyleColor(ImGuiCol_TitleBg,ImVec4 (0,100,255,255));
        //ImGui::PopStyleColor();    
         ImGui_ImplSdl_NewFrame(window);
        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        {
            ImGui::SetNextWindowPosCenter();
            ImGui::SetNextWindowSize(ImVec2(400,200),0);
            ImGui::Begin("Window",NULL,ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoTitleBar| ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoScrollbar);
            //static float f = 0.0f;
            ImGui::Image((GLuint *)Brick.ID,ImVec2((float)Brick.w,(float)Brick.h));
            // ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            // ImGui::ColorEdit3("clear color", (float*)&clear_color);
            //  if (ImGui::Button("Test Window")) show_test_window ^= 1;
            //  if (ImGui::Button("Another Window")) show_another_window ^= 1;
            // if (ImGui::Button("Quit")) running = false;
    if(ImGui::BeginMainMenuBar())
        {
        if (ImGui::BeginMenu("File"))
        {
        if(ImGui::MenuItem("Login")) show_login_window = true;
        if(ImGui::MenuItem("Exit","ALT+F4")) running = false;
        ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
            if (ImGui::MenuItem("Redo", "CTRL+Y")) {}  // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X")) {}
            if (ImGui::MenuItem("Copy", "CTRL+C")) {}
            if (ImGui::MenuItem("Paste", "CTRL+V")) {}
            ImGui::Separator();
            ImGui::EndMenu();
        }
        #if INTERNAL 
        if (ImGui::BeginMenu("Debug"))
        {
        if(ImGui::MenuItem("Show test")) show_test_window = true;
        if(ImGui::MenuItem("Show window")) show_another_window = true;
        if(ImGui::MenuItem("help")) ImGui::ShowUserGuide();
        if(ImGui::MenuItem("style")) ImGui::ShowStyleEditor();
            ImGui::EndMenu();
        }
        #endif
        ImGui::EndMainMenuBar();
        }
  ImGui::End();
        }
        // 2. Show another simple window, this time using an explicit Begin/End pair
        if (show_another_window)
        {
            ImGui::SetNextWindowSize(ImVec2(200,20), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Another Window", &show_another_window);
                        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        if (show_test_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
            ImGui::ShowTestWindow(&show_test_window);
        }        
        if (show_login_window)
        {
            ImGui::SetNextWindowSize(ImVec2(200,20), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Login Window", &show_login_window);
            ImGui::Text("Hello");
         ImGui::Columns(2);
            ImGui::Text("Name :");
            ImGui::NextColumn();
            ImGui::InputText("##1",&name[0] ,sizeof(name),ImGuiInputTextFlags_AlwaysInsertMode);
            ImGui::Spacing();
            ImGui::NextColumn();
            ImGui::Text("Password :");
            ImGui::NextColumn();
            ImGui::InputText("##2",&pass[0] ,sizeof(pass),ImGuiInputTextFlags_Password);
            ImGui::Columns(1);
   
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        // Rendering
        {
        ImVec4 clear_color = ImColor(100, 100, 255);
        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        SDL_GL_SwapWindow(window);
        }
    }

    FreeImage(&Brick);
    // Cleanup
    ImGui_ImplSdl_Shutdown();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
