#define COBJMACROS
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment(lib, "d3d11")
#pragma comment(lib, "dxguid")
#pragma comment(lib, "d3dcompiler")

#include "../yo/yo.h"

#define ASSERT_HR(hr) assert(SUCCEEDED(hr))

#define countof(x) (sizeof(x) / sizeof(x[0]))

typedef yo_v4f_t color4f_t;

#if 0
struct vertex_t
{
    float     x, y, z;
    color4f_t color;
};
#endif

struct vs_uniform_t
{
    yo_v2f_t resolution;
};

static inline color4f_t color4f(float r, float g, float b, float a)
{
    color4f_t ret = { r, g, b, a };
    return ret;
}

static inline yo_v4f_t yo_v4f_from_argb32(uint32_t argb32)
{
    yo_v4f_t ret =
    {
        .r = (float)((argb32 >> 16) & 0xff) / (float)(0xff),
        .g = (float)((argb32 >> 8)  & 0xff) / (float)(0xff),
        .b = (float)((argb32 >> 0)  & 0xff) / (float)(0xff),
        .a = (float)((argb32 >> 24) & 0xff) / (float)(0xff),
    };

    return ret;
}

static inline yo_v2f_t make_vec2f(float x, float y)
{
    yo_v2f ret = { x, y };
    return ret;
}

static inline yo_v2i_t yo_make_vec2i(int32_t x, int32_t y)
{
    yo_v2i ret = { x, y };
    return ret;
}

struct
{
    IDXGISwapChain         *swap_chain;
    ID3D11Device           *device;
    ID3D11DeviceContext    *device_context;
    ID3D11RenderTargetView *render_target_view;
    ID3D11VertexShader     *vertex_shader;
    ID3D11PixelShader      *pixel_shader;
    ID3D11Buffer           *vertex_buffer;
    ID3D11Buffer           *index_buffer;
    ID3D11Buffer           *constant_buffer;
    ID3D11InputLayout      *layout;
    vs_uniform_t            uniform;
    HWND                    window;
    bool                    running;
} global;

static void render_frame();

static LRESULT CALLBACK window_proc(HWND window, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (msg == WM_DESTROY)
    {
        global.running = false;
    }

    if (msg == WM_SIZE)
    {
        //
        // https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/d3d10-graphics-programming-guide-dxgi#handling-window-resizing
        //
        {
            global.device_context->OMSetRenderTargets(0, 0, 0);

            // Release all outstanding references to the swap chain's buffers.
            global.render_target_view->Release();

            HRESULT hr;
            // Preserve the existing buffer count and format.
            // Automatically choose the width and height to match the client rect for HWNDs.
            hr = global.swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);

            // Perform error handling here!

            // Get buffer and create a render-target-view.
            ID3D11Texture2D* pBuffer;
            hr = global.swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D),
                                              (void**)&pBuffer);
            // Perform error handling here!

            hr = global.device->CreateRenderTargetView(pBuffer, NULL,
                                                       &global.render_target_view);
            // Perform error handling here!
            pBuffer->Release();

            global.device_context->OMSetRenderTargets(1, &global.render_target_view, NULL);

            // Set up the viewport.
            D3D11_VIEWPORT vp;
            vp.Width    = (float)LOWORD(lparam);
            vp.Height   = (float)HIWORD(lparam);
            vp.MinDepth = 0.0f;
            vp.MaxDepth = 1.0f;
            vp.TopLeftX = 0;
            vp.TopLeftY = 0;
            global.device_context->RSSetViewports(1, &vp);
        }

        render_frame();
    }

    return DefWindowProcA(window, msg, wparam, lparam);
}

static void render_frame()
{
    RECT window_client_rect;
    GetClientRect(global.window, &window_client_rect);
    global.uniform.resolution.x = (float)window_client_rect.right;
    global.uniform.resolution.y = (float)window_client_rect.bottom;

    //
    // Fill constant buffer
    //
    {
        D3D11_MAPPED_SUBRESOURCE ms;

        global.device_context->Map(global.constant_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
        memcpy(ms.pData, &global.uniform, sizeof(global.uniform));
        global.device_context->Unmap(global.constant_buffer, NULL);
    }

    //
    // NOTE(rune): Viewport
    //
    {
        D3D11_VIEWPORT viewport = { 0 };

        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width    = global.uniform.resolution.x;
        viewport.Height   = global.uniform.resolution.y;

        global.device_context->RSSetViewports(1, &viewport);
    }

    color4f_t clear_color = color4f(0.0f, 0.0f, 0.0f, 0.0f);
    global.device_context->ClearRenderTargetView(global.render_target_view, clear_color.rgba);

    UINT stride = sizeof(yo_vert_t);
    UINT offset = 0;
    global.device_context->IASetVertexBuffers(0, 1, &global.vertex_buffer, &stride, &offset);
    global.device_context->IASetIndexBuffer(global.index_buffer, DXGI_FORMAT_R32_UINT, 0);
    global.device_context->VSSetConstantBuffers(0, 1, &global.constant_buffer);
    global.device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    global.device_context->DrawIndexed(6, 0, 0);

    global.swap_chain->Present(1, 0);
}

int main()
{
    HRESULT hr = 0;

    //
    // NOTE(rune): Initialize Win32 window
    //
    {
        WNDCLASSA window_class = { 0 };
        window_class.lpszClassName = "my_window_class";
        window_class.lpfnWndProc = &window_proc;

        if (!RegisterClassA(&window_class))
        {
            printf("RegisterClassA failed (%i)\n", GetLastError());
            return 0;
        }

        global.window = CreateWindowExA(0, window_class.lpszClassName, "My Window Title",
                                        WS_OVERLAPPEDWINDOW,
                                        CW_USEDEFAULT,
                                        CW_USEDEFAULT,
                                        800,
                                        600,
                                        0,
                                        0,
                                        0,
                                        0);

        if (!global.window)
        {
            printf("CreateWindowA failed (%i)\n", GetLastError());
            return 0;
        }
    }

    //
    // NOTE(rune): Initialize D3D11
    //
    {
        DXGI_SWAP_CHAIN_DESC scd = { 0 };

        scd.BufferCount       = 1;
        scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        scd.BufferUsage       = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        scd.OutputWindow      = global.window;
        scd.SampleDesc.Count  = 4; // TODO(rune): Multi-sampling?
        scd.Windowed          = TRUE;


        UINT create_flags = 0;

#if _DEBUG
        create_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE,
                                      0,
                                      create_flags,
                                      0,
                                      0,
                                      D3D11_SDK_VERSION,
                                      &scd,
                                      &global.swap_chain,
                                      &global.device,
                                      NULL,
                                      &global.device_context);
    }

    //
    // NOTE(rune): Get back buffer render target
    //
    {
        ID3D11Texture2D *swap_chain_buffer;

        hr = global.swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&swap_chain_buffer);
        hr = global.device->CreateRenderTargetView(swap_chain_buffer, NULL, &global.render_target_view);
        global.device_context->OMSetRenderTargets(1, &global.render_target_view, 0);

        swap_chain_buffer->Release();
    }

    //
    // NOTE(rune): Shaders
    //
    {
        ID3DBlob *vs_blob, *ps_blob;
        hr = D3DCompileFromFile(L"yo.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, &vs_blob, 0);
        hr = D3DCompileFromFile(L"yo.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, &ps_blob, 0);

        void  *vs_buffer      = vs_blob->GetBufferPointer();
        size_t vs_buffer_size = vs_blob->GetBufferSize();
        void  *ps_buffer      = ps_blob->GetBufferPointer();
        size_t ps_buffer_size = ps_blob->GetBufferSize();

        hr = global.device->CreateVertexShader(vs_buffer, vs_buffer_size, NULL, &global.vertex_shader);
        hr = global.device->CreatePixelShader(ps_buffer, ps_buffer_size, NULL, &global.pixel_shader);

        global.device_context->VSSetShader(global.vertex_shader, 0, 0);
        global.device_context->PSSetShader(global.pixel_shader, 0, 0);

        D3D11_INPUT_ELEMENT_DESC ied[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT,       0, offsetof(yo_vert_t, corner),       D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(yo_vert_t, corner_color), D3D11_INPUT_PER_VERTEX_DATA, 0},
        };

        hr = global.device->CreateInputLayout(ied, countof(ied), vs_buffer, vs_buffer_size, &global.layout);
        global.device_context->IASetInputLayout(global.layout);

        // TODO(rune): Blob release is not in the turorial. Should the be released here?
        vs_blob->Release();
        ps_blob->Release();
    }

    yo_vert_t vertices[4] = {};

    //
    // NOTE(rune): Vertices
    //
    {
#if 0
        yo_v2f_t p0 = make_vec2f(0.0f, 0.0f);
        yo_v2f_t p1 = make_vec2f(1.0f, 1.0f);
#else
        yo_v2f_t p0 = make_vec2f(10.0f, 20.0f);
        yo_v2f_t p1 = make_vec2f(30.0f, 40.0f);
#endif

        yo_corners_v4f_t color =
        {
            .top_left     = { 1.0f, 0.0f, 0.0f, 1.0f },
            .top_right    = { 0.0f, 1.0f, 0.0f, 1.0f },
            .bottom_left  = { 0.0f, 0.0f, 1.0f, 1.0f },
            .bottom_right = { 1.0f, 1.0f, 1.0f, 1.0f },
        };

        yo_corners_f32_t corner_radius = { 5, 5, 5, 5 };

        float    border_thickness = 5;
        yo_v4f_t border_color = { 1.0f, 0.5f, 0.0f, 1.0f };

        yo_vert_t vertex_data = { 0 };
        vertex_data.rect.p0.x = (float)(p0.x);
        vertex_data.rect.p0.y = (float)(p0.y);
        vertex_data.rect.p1.x = (float)(p1.x);
        vertex_data.rect.p1.y = (float)(p1.y);
        vertex_data.border_thickness = border_thickness;
        vertex_data.border_color = border_color;

        vertices[0] = vertex_data;
        vertices[1] = vertex_data;
        vertices[2] = vertex_data;
        vertices[3] = vertex_data;

        vertices[0].corner_radius = corner_radius.corner[YO_CORNER_TOP_LEFT];
        vertices[1].corner_radius = corner_radius.corner[YO_CORNER_TOP_RIGHT];
        vertices[2].corner_radius = corner_radius.corner[YO_CORNER_BOTTOM_LEFT];
        vertices[3].corner_radius = corner_radius.corner[YO_CORNER_BOTTOM_RIGHT];
        vertices[0].corner_color  = color.corner[YO_CORNER_TOP_LEFT];
        vertices[1].corner_color  = color.corner[YO_CORNER_TOP_RIGHT];
        vertices[2].corner_color  = color.corner[YO_CORNER_BOTTOM_LEFT];
        vertices[3].corner_color  = color.corner[YO_CORNER_BOTTOM_RIGHT];

        vertices[0].corner        = make_vec2f((float)(p0.x), (float)(p0.y));
        vertices[1].corner        = make_vec2f((float)(p0.x), (float)(p1.y));
        vertices[2].corner        = make_vec2f((float)(p1.x), (float)(p0.y));
        vertices[3].corner        = make_vec2f((float)(p1.x), (float)(p1.y));
        //vertices[0].tex_coord     = yo_make_vec2f(uv0.x, uv0.y);
        //vertices[1].tex_coord     = yo_make_vec2f(uv0.x, uv1.y);
        //vertices[2].tex_coord     = yo_make_vec2f(uv1.x, uv0.y);
        //vertices[3].tex_coord     = yo_make_vec2f(uv1.x, uv1.y);
    }

    //
    // Indicies
    //
    yo_idx_t indicies[6] = { 0, 1, 2, 3, 2, 1 };

    //
    // Create vertex buffer
    //
    {
        D3D11_BUFFER_DESC bd = { 0 };

        bd.Usage          = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth      = sizeof(vertices);
        bd.BindFlags      = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        hr = global.device->CreateBuffer(&bd, NULL, &global.vertex_buffer);

        assert(global.vertex_buffer);
    }

    //
    // Create index buffer
    //
    {
        D3D11_BUFFER_DESC bd = {};

        bd.Usage          = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth      = sizeof(indicies);
        bd.BindFlags      = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        hr = global.device->CreateBuffer(&bd, NULL, &global.index_buffer);

        assert(global.index_buffer);
    }

    //
    // Create contant buffer
    //
    {
        global.uniform.resolution = make_vec2f(800.0f, 600.0f);

        D3D11_BUFFER_DESC bd = {};
        bd.Usage          = D3D11_USAGE_DYNAMIC;
        bd.ByteWidth      = 64; // sizeof(global.uniform);
        bd.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

        hr = global.device->CreateBuffer(&bd, NULL, &global.constant_buffer);

        assert(bd.ByteWidth >= sizeof(global.uniform));
        assert(global.constant_buffer);
    }

    //
    // Fill vertex buffer
    //
    {
        D3D11_MAPPED_SUBRESOURCE ms;

        global.device_context->Map(global.vertex_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
        memcpy(ms.pData, vertices, sizeof(vertices));
        global.device_context->Unmap(global.vertex_buffer, NULL);
    }

    //
    // Fill index buffer
    //
    {
        D3D11_MAPPED_SUBRESOURCE ms;

        global.device_context->Map(global.index_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &ms);
        memcpy(ms.pData, indicies, sizeof(indicies));
        global.device_context->Unmap(global.index_buffer, NULL);
    }

    //
    // Run message loop
    //
    {
        global.running =  true;

        ShowWindow(global.window, SW_SHOW);

        MSG msg = { 0 };
        while (global.running)
        {
            while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }

            render_frame();
        }
    }

    //
    // NOTE(rune): Clean up
    //
    {
        global.swap_chain->Release();
        global.device->Release();
        global.device_context->Release();
        global.render_target_view->Release();
        global.vertex_shader->Release();
        global.pixel_shader->Release();
        global.vertex_buffer->Release();
        global.index_buffer->Release();
        global.constant_buffer->Release();
    }

    return 0;
}
