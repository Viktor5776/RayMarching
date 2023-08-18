#include "Window.h"
#include "Resource/resource.h"
#include <assert.h>


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

namespace Hydro
{
    //Window class
    Window::WindowClass Window::WindowClass::wndClass;

    Window::WindowClass::WindowClass()
        :
        hInst( GetModuleHandle( nullptr ) )
    {
        WNDCLASSEX wc = { 0 };
        wc.cbSize = sizeof( wc );
        wc.style = CS_OWNDC;
        wc.lpfnWndProc = HandleMsgSetup;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = GetInstance();
        wc.hIcon = static_cast<HICON>(LoadImage( hInst, MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 32, 32, 0 ));
        wc.hCursor = nullptr;
        wc.hbrBackground = nullptr;
        wc.lpszMenuName = nullptr;
        wc.lpszClassName = GetName();
        wc.hIconSm = static_cast<HICON>(LoadImage( hInst, MAKEINTRESOURCE( IDI_ICON1 ), IMAGE_ICON, 16, 16, 0 ));;
        RegisterClassEx( &wc );
    }

    Window::WindowClass::~WindowClass()
    {
        UnregisterClass( wndClassName, GetInstance() );
    }

    const wchar_t* Window::WindowClass::GetName() 
    {
        return wndClassName;
    }

    HINSTANCE Window::WindowClass::GetInstance() 
    {
        return wndClass.hInst;
    }


    // Window
    Window::Window( int width, int height, const wchar_t* name )
        :
        width( width ),
        height( height )
    {
        //Setup ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        if( io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable )
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        //Calculat Window Size
        RECT wr = {};
        wr.left = 100;
        wr.right = width + wr.left;
        wr.top = 100;
        wr.bottom = height + wr.top;
        
        //Set Window size 
        assert( AdjustWindowRect( &wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME, FALSE ) != 0 );

        //Create Window
        hWnd = CreateWindow(
            WindowClass::GetName(), name,
            WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU | WS_THICKFRAME,
            CW_USEDEFAULT, CW_USEDEFAULT,
            wr.right - wr.left, wr.bottom - wr.top,
            nullptr, nullptr, WindowClass::GetInstance(), this
        );
        
        //Check creation worked
        assert( hWnd != nullptr );

        ShowWindow( hWnd, SW_SHOWDEFAULT );

        ImGui_ImplWin32_Init( hWnd );

        //Set Graphics
        pGfx = std::make_unique<Graphics>( hWnd );

        // register mouse raw input device
        RAWINPUTDEVICE rid;
        rid.usUsagePage = 0x01; // mouse page
        rid.usUsage = 0x02; // mouse usage
        rid.dwFlags = 0;
        rid.hwndTarget = nullptr;
        BOOL b = RegisterRawInputDevices( &rid, 1, sizeof( rid ) ); 
        assert( b != 0 );
    }

    Window::~Window()
    {
        ImGui_ImplWin32_Shutdown();
        DestroyWindow( hWnd );
    }

    void Window::EnableCursor()
    {
        cursorEnabled = true;
        ShowCursor();
        EnableImGuiMouse();
        FreeCursor();
    }

    void Window::DisableCursor()
    {
        cursorEnabled = false;
        HideCursor();
        DisableImGuiMouse();
        ConfineCursor();
    }

    std::optional<int> Window::ProcessMessages()
    {
        MSG msg;

        while( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) )
        {
            if( msg.message == WM_QUIT )
            {
                //Return When Quting
                return (int)msg.wParam;
            }

            TranslateMessage( &msg );
            DispatchMessage( &msg );
        }

        //Return Empty Optinal When Not Quiting
        return {};
    }

    void Window::HideCursor()
    {
        while( ::ShowCursor( FALSE ) >= 0 );
    }

    void Window::ShowCursor()
    {
        while( ::ShowCursor( TRUE ) < 0 );
    }

    void Window::EnableImGuiMouse()
    {
        ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
    }

    void Window::DisableImGuiMouse()
    {
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
    }

    void Window::ConfineCursor()
    {
        RECT rect;
        GetClientRect( hWnd, &rect );
        MapWindowPoints( hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2 );
        ClipCursor( &rect );
    }

    void Window::FreeCursor()
    {
        ClipCursor( nullptr );
    }


    LRESULT CALLBACK Window::HandleMsgSetup( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
    {
        //WM_NCCREATE msg Gets Called When We Use CreateWindow()
        if( msg == WM_NCCREATE )
        {
            //Get ptr To Window Class
            const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
            Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
            //Set WinAPI to store ptr to Window Class
            SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd) );
            //Set Message Proc to Normal (non-setup) Handler
            SetWindowLongPtr( hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk) );
            //Forward message to Window Class Handler
            return pWnd->HandleMsg( hWnd, msg, wParam, lParam );
        }
        //Messages before WM_NCCREATE Get Forwarded to Default Handler
        return DefWindowProc( hWnd, msg, wParam, lParam );
    }

    LRESULT CALLBACK Window::HandleMsgThunk( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) 
    {
        //Retrive ptr to Window Class
        Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr( hWnd, GWLP_USERDATA ));
        //Forward message to Window Class Handler
        return pWnd->HandleMsg( hWnd, msg, wParam, lParam );
    }

    LRESULT Window::HandleMsg( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
    {
        if( ImGui_ImplWin32_WndProcHandler( hWnd, msg, wParam, lParam ) )
        {
            return true;
        }

        switch( msg )
        {
        case WM_CLOSE:
            PostQuitMessage( 0 );
            return 0;
        case WM_SIZE:
            if( pGfx != nullptr && wParam != SIZE_MINIMIZED )
            {
                width = LOWORD( lParam );
                height = HIWORD( lParam );

                pGfx->OnResize( width, height );
			}
			return 0;
        //Clear Keystate When Window Loses Focus
        case WM_KILLFOCUS:
            keyboard.ClearState();
            break;
            /********** KEYBOARD MESSAGES **********/
        case WM_KEYDOWN:
            //SYSKEY Commands Need to be Handeld For ALT ( VM_MENU ) and F10
        case WM_SYSKEYDOWN:
            //Filter Auto Repeat
            if( !(lParam & 0x40000000) || keyboard.AutorepeatIsEnabled() )
            {
                keyboard.OnKeyPressed( static_cast<unsigned char>(wParam) );
            }
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            keyboard.OnKeyReleased( static_cast<unsigned char>(wParam) );
            break;
        case WM_CHAR:
            keyboard.OnChar( static_cast<unsigned char>(wParam) );
            break;
            /******** END KEYBOARD MESSAGES ********/


            /********** Mouse MESSAGES **********/
        case WM_MOUSEMOVE:

            const POINTS pt = MAKEPOINTS( lParam );
            // cursorless exclusive gets first dibs
            if( !cursorEnabled )
            {
                if( !mouse.IsInWindow() )
                {
                    SetCapture( hWnd );
                    mouse.OnMouseEnter();
                    HideCursor();
                }
                break;
            }
            if( GetFocus() == hWnd )
            {
                //In Window
                if( pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height )
                {
                    mouse.OnMouseMove( pt.x, pt.y );
                    if( !mouse.IsInWindow() )
                    {
                        SetCapture( hWnd );
                        mouse.OnMouseEnter();
                    }
                }
                //Not in Window ( Maintain Capture if Button is Down )
                else
                {
                    if( wParam & (MK_LBUTTON | MK_RBUTTON) )
                    {
                        mouse.OnMouseMove( pt.x, pt.y );
                    }
                    //Button Up ( Release Capture )
                    else
                    {
                        ReleaseCapture();
                        mouse.OnMouseLeave();

                    }
                }
            }
            break;
        case WM_LBUTTONDOWN:
            mouse.OnLeftPressed();
            break;
        case WM_LBUTTONUP:
            mouse.OnLeftRelease();
            break;
        case WM_RBUTTONDOWN:
            mouse.OnRightPressed();
            break;
        case WM_RBUTTONUP:
            mouse.OnRightRelease();
            break;
        case WM_MOUSEWHEEL:
            mouse.OnWheelDelta( GET_WHEEL_DELTA_WPARAM( wParam ) );
            break;
        case WM_MBUTTONDOWN:
            mouse.OnWheelPressed();
            break;
            /********** END Mouse MESSAGES **********/

            /************** RAW MOUSE MESSAGES **************/
        case WM_INPUT:
        {
            UINT size = 0u;
            // first get the size of the input data
            if( GetRawInputData(
                reinterpret_cast<HRAWINPUT>(lParam),
                RID_INPUT,
                nullptr,
                &size,
                sizeof( RAWINPUTHEADER ) ) == -1 )
            {
                // bail msg processing if error
                break;
            }
            rawBuffer.resize( size );
            // read in the input data
            if( GetRawInputData(
                reinterpret_cast<HRAWINPUT>(lParam),
                RID_INPUT,
                rawBuffer.data(),
                &size,
                sizeof( RAWINPUTHEADER ) ) != size )
            {
                // bail msg processing if error
                break;
            }
            // process the raw input data
            auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
            if( ri.header.dwType == RIM_TYPEMOUSE &&
                (ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0) )
            {
                mouse.OnRawDelta( ri.data.mouse.lLastX, ri.data.mouse.lLastY );
            }
            break;
        }
        /************** END RAW MOUSE MESSAGES **************/

        }

        return DefWindowProc( hWnd, msg, wParam, lParam );
    }

    Graphics& Window::Gfx()
    {
        assert( pGfx );
        return *pGfx;
    }

    HWND Window::GetWindowHandle() const
    {
        return hWnd;
    }

}

