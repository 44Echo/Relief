#include <iostream>
#include <windows.h>
#include <d3d11.h>
#include <wrl/client.h>

// Bring in Imgui
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

// Our own header files
#include "TextFile.h"
#include "KeyValues.h"
#include "Displacement.h"



using namespace Microsoft::WRL;

UINT resizeWidth = 0;
UINT resizeHeight = 0;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hwnd, message, wParam, lParam))
        return true;

    switch (message)
    {
    case WM_DESTROY:
        // Destroying window, post quit message
        PostQuitMessage(0);
        return 0;

    case WM_SIZE:
        if (wParam == SIZE_MINIMIZED)
        {
            return 0;
        }
        resizeWidth = LOWORD(lParam);
        resizeHeight = HIWORD(lParam);
        return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}

int main(int argc, char *argv[])
{
    // Just testing -- Remove this later?
    std::cout << "Relief" << '\n';

    if (argc >= 2)
    {
        std::string text;
        if (readTextFile(argv[1], text))
        {
            int lines = 0;
            for (char c : text)
            {
                if (c == '\n')
                {
                    lines++;
                }
            }

            std::cout << "Read " << text.size() << " bytes, " << lines << " lines" << std::endl;

            std::vector<Token> tokens;
            if (tokenize(text, tokens))
            {
                // Counting tokens on load, this is how we actually read the .vmf
                int strings = 0;
                int openBraces = 0;
                int closeBraces = 0;
                for (const Token &t : tokens)
                {
                    if (t.type == TokenKind::String)
                    {
                        strings++;
                    }
                    else if (t.type == TokenKind::OpenBrace)
                    {
                        openBraces++;
                    }
                    else if (t.type == TokenKind::CloseBrace)
                    {
                        closeBraces++;
                    }
                }
                std::cout << tokens.size() << " tokens: "
                          << strings << " strings, "
                          << openBraces << " {, "
                          << closeBraces << " }\n";

                for (size_t i = 0; i < 8 && i < tokens.size(); i++)// Here we print out first 8 tokens, just to see if it works -- Cleanup later
                {
                    std::cout << tokens[i].text << ' ';
                }
                std::cout << '\n';

                // Here we start reporting what we found in the vmf file
                KvNode root;
                if (parseKeyValues(tokens, root))
                {
                    std::cout << "Parsed " << root.children.size() << " top-level blocks:\n";
                    for (const KvNode &child : root.children){
                        std::cout << child.key << ' ';
                    }
                    std::cout << '\n';
                    std::cout << countBlocks(root, "solid") << " solids, "
                                << countBlocks(root, "dispinfo") << " displacements\n";

                     // Here we start reporting and collecting what the displacements are actually made out of
                    std::vector<Displacement> displacements;
                    if (collectDisplacements(root, displacements)){
                        std::cout << "Collected " << displacements.size() << " displacements:\n";
                        if (!displacements.empty()){
                            const Displacement& first = displacements[0];
                            KvNode* id = findChild(*first.side, "id");
                            if (id != nullptr){
                                std::cout << "First: side " << id->value << ", power " << first.power << '\n';
                            }
                        }
                
                    }
                    else {
                        std::cout << "Broken displacements in " << argv[1] << '\n';
                    }   
                }

                else {
                    std::cout << "Could not parse Keyvalues in " << argv[1] << '\n';
                }
            }
            else
            {
                std::cout << "Could not Tokenize " << argv[1] << '\n';
            }
        }
        else
        {
            std::cout << "Failed to open " << argv[1] << std::endl;
        }
    }

    // Define the window class
    WNDCLASSEXW wb{};
    wb.cbSize = sizeof(WNDCLASSEXW);
    wb.lpfnWndProc = WindowProc;
    wb.hInstance = GetModuleHandleW(nullptr);
    wb.lpszClassName = L"ReliefWindowClass";

    RegisterClassExW(&wb);

    // Now create a window
    HWND hwnd = CreateWindowExW(
        0,
        L"ReliefWindowClass",
        L"Relief",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        nullptr,
        nullptr,
        GetModuleHandleW(nullptr),
        nullptr);
    ShowWindow(hwnd, SW_SHOW);

    // Initialize d3d11 device and swap chain
    DXGI_SWAP_CHAIN_DESC sd{};
    // Use two buffers, one shown, one being drawn to
    sd.BufferCount = 2;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hwnd;
    sd.SampleDesc.Count = 1;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

    ComPtr<ID3D11Device> device;
    ComPtr<ID3D11DeviceContext> context;
    ComPtr<IDXGISwapChain> swapChain;
    // Create the device and swap chain
    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        swapChain.GetAddressOf(),
        device.GetAddressOf(),
        nullptr,
        context.GetAddressOf());
    // Device creation failed, print error and exit
    if (FAILED(hr))
    {
        std::cout << "Could not create Direct3D device\n";
        return 1;
    }
    // Set up the backbuffer render target
    ComPtr<ID3D11Texture2D> backBuffer;
    swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

    // Create the Render Target View
    ComPtr<ID3D11RenderTargetView> renderTarget;
    device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTarget.GetAddressOf());

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(device.Get(), context.Get());

    // New message loop
    MSG msg{};
    bool running = true;
    float clearColor[4] = {0.10f, 0.12f, 0.16f, 1.0f};
    while (running)
    {

        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                running = false;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (!running)
        {
            break;
        }
        // This needs to be inside an If statement, otherwise it'll run on every frame and tank performance. Especially if we have a displacement heavy scene.
        if (resizeWidth != 0 && resizeHeight != 0)
        {
            renderTarget.Reset();
            backBuffer.Reset();
            HRESULT resizeResult = swapChain->ResizeBuffers(0, resizeWidth, resizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            if (FAILED(resizeResult))
            {
                std::cout << "Failed to resize swap chain buffers\n";
            }
            // Set the globals back to 0, so we don't keep resizing every frame
            resizeWidth = 0;
            resizeHeight = 0;

            // If this lives outside the if statement, they'll create a memory leak, as the old backbuffer and render target view will never be released.
            swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
            device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTarget.GetAddressOf());
        }

        // Start ImGui frames
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Add the Imgui Demo, remove later
        ImGui::ShowDemoWindow();

        // Our own ImGui window
        ImGui::Begin("Relief");
        ImGui::Text("Hello says Relief!");
        ImGui::ColorEdit3("Background", clearColor);
        ImGui::End();

        ImGui::Render();

        // Here we draw our frame
        context->ClearRenderTargetView(renderTarget.Get(), clearColor);

        context->OMSetRenderTargets(1, renderTarget.GetAddressOf(), nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
        swapChain->Present(1, 0); // Present with vsync
    }
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    return 0;
}