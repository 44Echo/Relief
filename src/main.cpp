#include <iostream>
#include <windows.h>
#include <d3d11.h>
#include <wrl/client.h>

using namespace Microsoft::WRL;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message)
    {
        case WM_DESTROY:
        // Destroying window, post quit message
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, message, wParam, lParam);
}


int main(){
    //Just testing -- Remove this later?
    std::cout << "Relief" << '\n';
    //Define the window class
    WNDCLASSEXW wb{};
    wb.cbSize = sizeof(WNDCLASSEXW);
    wb.lpfnWndProc = WindowProc;
    wb.hInstance = GetModuleHandleW(nullptr);
    wb.lpszClassName = L"ReliefWindowClass";

    RegisterClassExW(&wb);

    //Now create a window
    HWND hwnd = CreateWindowExW(
        0,
        L"ReliefWindowClass",
        L"Relief",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720,
        nullptr,
        nullptr,
        GetModuleHandleW(nullptr),
        nullptr
    );
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
        context.GetAddressOf()
    );
    // Device creation failed, print error and exit
    if (FAILED(hr)) {
        std::cout << "Could not create Direct3D device\n";
        return 1;
    }
    // Set up the backbuffer render target
    ComPtr<ID3D11Texture2D> backBuffer;
    swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));

    // Create the Render Target View
    ComPtr<ID3D11RenderTargetView> renderTarget;
    device->CreateRenderTargetView(backBuffer.Get(), nullptr, renderTarget.GetAddressOf());

    // New message loop
    MSG msg{};
    bool running = true;
    while (running){

        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)){
            if (msg.message == WM_QUIT){
                running = false;
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        if (!running){
            break;
        }

        // Here we draw our frame
        float clearColor[4] = { 0.10f, 0.12f, 0.16f, 1.0f }; 
        context->ClearRenderTargetView(renderTarget.Get(), clearColor);
        swapChain->Present(1, 0); // Present with vsync
    }
    return 0;
}