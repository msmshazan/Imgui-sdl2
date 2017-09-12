#include "main.h"

typedef struct {
sqlite3* Database;
}sqldata;

internal PLATFORM_WORK_QUEUE_CALLBACK(InitilizeSqlite)
{
    char Buffer[256]; 
    wsprintf(Buffer, "Thread: %u\n", GetCurrentThreadId());
    OutputDebugStringA(Buffer);
    if(IsSqliteThreadSafe()){
       wsprintf(Buffer,"Sqlite API is thread Safe.");
    }else{
       wsprintf(Buffer,"Sqlite API is not thread Safe.");
    }
    OutputDebugStringA(Buffer);
    sqldata * Sql = (sqldata *)Data;
    Sql->Database = CreateOrOpenDatabase("Database.db");
}

internal PLATFORM_WORK_QUEUE_CALLBACK(CreateTableSql)
{
    char Buffer[256]; 
    wsprintf(Buffer, "Thread: %u\n", GetCurrentThreadId());
    OutputDebugStringA(Buffer);
    sqldata * Sql = (sqldata *)Data;
    CreateTable(Sql->Database,"Testing","ID INT PRIMARY KEY,AGE INT");
}

internal void ImGuiClipboard(char* text,char* ctx){
    if (ImGui::BeginPopupContextItem(ctx))
    {
        if (ImGui::Selectable("Copy")) ImGui::SetClipboardText(text);
        if (ImGui::Selectable("Paste")){
            strcpy(text,ImGui::GetClipboardText());
        }
        ImGui::EndPopup();
    }
}
                
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
    SDL_Window *window = SDL_CreateWindow("App Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, current.w, current.h, SDL_WINDOW_OPENGL|SDL_WINDOW_ALLOW_HIGHDPI|SDL_WINDOW_RESIZABLE|SDL_WINDOW_BORDERLESS|SDL_WINDOW_FULLSCREEN);
    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(-1);

    // Setup ImGui binding
    ImGui_ImplSdl_Init(window);
    win32_thread_startup Threads[1] = {};
    platform_work_queue SQLQueue = {};
    Win32MakeQueue(&SQLQueue,ArrayCount(Threads),&Threads[0]);
    // Load Fonts
    // (there is a default font, this is only if you want to change it. see extra_fonts/README.txt for more details)
    ImGuiIO& io = ImGui::GetIO();
    io.Fonts->AddFontFromFileTTF("font/LiberationMono.ttf", 20.0f);

    //DATA
    bool AlwaysShowTime = false;
    float NPV=0,KPI =0,DCF=0,FV=0,Year=0;
    bool show_test_window = false;
    bool show_another_window = false;
    bool show_login_window = false;
    bool show_payroll_window = false;
    bool show_guide_window = false;
    bool show_style_window = false;
    bool show_buisness_window = false;
    bool Loggedin = false;
    bool pressed = false;
    bool LoggedinPressed = false;
    char name[256] = {};
    char pass[256] = {};
    ImageData Hello ={};
    float GrossPay=0,Tax=0,NetPay=0;
    sqldata SQLData={};
    // Main loop
    LoadImage("Hello.png",&Hello);
    ImGui::SetupImGuiStyle(true,true);            
    bool running = true;
    while (running)
    {
        Win32DoNextWorkQueueEntry(&SQLQueue);
        io = ImGui::GetIO();
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSdl_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
        }
        ImGui_ImplSdl_NewFrame(window);
        {
            if(ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if(ImGui::MenuItem("Login")) show_login_window = true;
                    if(ImGui::MenuItem("Gamedev Payroll")) show_payroll_window = true;
                    if(ImGui::MenuItem("Gamedev Business")) show_buisness_window = true;
                    if(ImGui::MenuItem("Exit","ALT+F4")) running = false;
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Data"))
                {
                    ImGui::Checkbox("Show Time", &AlwaysShowTime );
                    if(!SQLData.Database){
                    if(ImGui::MenuItem("Sqlinit")) Win32AddEntry(&SQLQueue,InitilizeSqlite,&SQLData);
                    }else
                    {
                    if(ImGui::MenuItem("SqlTable")) Win32AddEntry(&SQLQueue,CreateTableSql,&SQLData);    
                    }
                    ImGui::Checkbox("Login State",&Loggedin);
                    const char* listbox_items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };
                    static int listbox_item_current = 1;
                    ImGui::ListBox("listbox\n(single select)", &listbox_item_current, listbox_items, IM_ARRAYSIZE(listbox_items), 4);
                    ImGui::MenuItem("Checkboxes2");
                    ImGui::EndMenu();
                }
#if INTERNAL 
                if (ImGui::BeginMenu("Debug"))
                {
                    ImGui::Text("UpTime: %f",ImGui::GetTime());
                    if(ImGui::MenuItem("Show test")) show_test_window = true;
                    if(ImGui::MenuItem("Show window")) show_another_window = true;
                    ImGui::Checkbox("help", &show_guide_window );
                    ImGui::Checkbox("style",&show_style_window );
                    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
                    ImGui::EndMenu();
                }          
#endif
                    SYSTEMTIME Win32Time;
                    GetLocalTime(&Win32Time);
                    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - ImGui::CalcTextSize("TIME:00:00:00:000 DATE:00:00:0000").x);
                    ImGui::TextColored(ImColor(0,255,0),"TIME:%02i:%02i:%02i:%03i",Win32Time.wHour,Win32Time.wMinute,Win32Time.wSecond,Win32Time.wMilliseconds);
                    ImGui::SameLine();
                    ImGui::TextColored(ImColor(255,0,0),"DATE:%02i:%02i:%04i",Win32Time.wDay,Win32Time.wMonth,Win32Time.wYear);
                    ImGui::EndMainMenuBar();

            }
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
            ImGui::SetNextWindowPosCenter();
            ImGui::SetNextWindowSize(ImVec2(400,200),0);
            ImGui::Begin("Hello",NULL,ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_AlwaysAutoResize|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoFocusOnAppearing|ImGuiWindowFlags_NoBringToFrontOnFocus|ImGuiWindowFlags_NoInputs);
            ImGui::Image((GLuint *)Hello.ID,ImVec2((float)Hello.w,(float)Hello.h));
            ImGui::End();
            ImGui::PopStyleVar();
            
        }

        if(show_guide_window){ImGui::ShowUserGuide();}
        if(show_style_window){ImGui::ShowStyleEditor();}
        
        // 2. Show another simple window, this time using an explicit Begin/End pair
        if (show_another_window)
        {
            ImGui::SetNextWindowPosCenter();
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        if (show_test_window)
        {
            ImGui::SetNextWindowPosCenter();
            ImGui::SetNextWindowSize(ImVec2(400,200),0);
            ImGui::ShowTestWindow(&show_test_window);
        }        
        if (show_payroll_window)
        {
            ImGui::SetNextWindowPosCenter();
            ImGui::SetNextWindowSize(ImVec2(400,200),0);
            ImGui::Begin("Gamedev Payroll",&show_payroll_window,ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("Payroll Calculator");
            ImGui::InputFloat("GrossPay",&GrossPay,0,0,2);
            ImGui::InputFloat("Income Tax",&Tax,0,0,2);
            ImGui::Spacing();
            if(ImGui::Button("Calculate : NetPay")){
                NetPay = GrossPay - Tax ;
                
            }
            if(NetPay){
                char* buf[256];
                snprintf(buf[0],IM_ARRAYSIZE(buf),"Netpay: $.%.2f/=",NetPay);
                ImGui::Text(buf[0]);
            }
            ImGui::End();
        }        
        if (show_buisness_window)
        {
            ImGui::SetNextWindowPosCenter();
            ImGui::Begin("Gamedev Business",&show_buisness_window,ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_AlwaysAutoResize);
            ImGui::Text("Gamedev Business");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("NPV Calculate");
            ImGui::InputFloat("Future Value",&FV,0,0,2);
            ImGui::InputFloat("DCF in %:",&DCF,0,0,0);
            ImGui::InputFloat("Period (Years):",&Year,0,0,2);
            ImGui::Spacing();
            NPV = FV*powf((1-(DCF/100)),Year);
            char* buf[256];
            snprintf(buf[0],256,"NPV: $ %.2f/=",NPV);
            ImGui::Text((const char *)buf);
            ImGui::End();
        }        
        if (show_login_window)
        {
            if(Loggedin){
                ImGui::SetNextWindowPosCenter();
                ImGui::Begin("Login Window", &show_login_window,ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::Text("Hello Developer,You are logged in.");
                ImGui::End();
            }
            else{
                ImGui::SetNextWindowPosCenter();
                ImGui::Begin("Login Window", &show_login_window,ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_AlwaysAutoResize);
                ImGui::Text("Hello");
                ImGui::Text("Name :");
                ImGui::InputText("##1",&name[0] ,sizeof(name),ImGuiInputTextFlags_AlwaysInsertMode|ImGuiInputTextFlags_AutoSelectAll|ImGuiInputTextFlags_NoHorizontalScroll);
                ImGuiClipboard(&name[0],"name");
                ImGui::Spacing();
                ImGui::Text("Password :");
                ImGui::InputText("##2",&pass[0] ,sizeof(pass),ImGuiInputTextFlags_Password);
                ImGuiClipboard(&pass[0],"pass");
                ImGui::Columns(2,0,false);
                if(ImGui::Button("LOGIN")) {
                    LoggedinPressed = true;
                    Loggedin = ( (strcmp(name, "Shazan") == 0) && (strcmp(pass, "Shazanman123")==0));
                    }
                ImGui::Columns();
                if(LoggedinPressed && !Loggedin)ImGui::Text("Sorry Wrong Credentials.");
                ImGui::End();
                
            }
        }
        // Rendering
        {
            ImVec4 clear_color = ImColor(0, 115, 206);
            glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
            ImGui::Render();
            SDL_GL_SwapWindow(window);
        }
    }
    Win32CompleteAllWork(&SQLQueue);
    FreeImage(&Hello);
    // Cleanup
    ImGui_ImplSdl_Shutdown();
    SDL_GL_DeleteContext(glcontext);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

