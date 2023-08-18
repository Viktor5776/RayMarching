#pragma once
#ifndef UNICODE
#define UNICODE
#endif 

#define NOMINMAX

#include <Windows.h>
#include <optional>
#include <memory>
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"

namespace Hydro
{
    class Window
    {
    class WindowClass
    {
    public:
        static const wchar_t* GetName();
        static HINSTANCE GetInstance();
    private:
        WindowClass();
        ~WindowClass();
        WindowClass( const WindowClass& ) = delete;
        WindowClass& operator=( const WindowClass& ) = delete;
        static constexpr const wchar_t* wndClassName = L"Hydro";
        static WindowClass wndClass;
        HINSTANCE hInst;
    };


    public:
        Window( int width, int height, const wchar_t* name);
        ~Window();
        Window( const Window& ) = delete;
        Window& operator=( const Window& ) = delete;
        static std::optional<int> ProcessMessages();
        Graphics& Gfx();
        HWND GetWindowHandle() const;
        void EnableCursor();
        void DisableCursor();
    private:
        void HideCursor();
        void ShowCursor();
        void EnableImGuiMouse();
        void DisableImGuiMouse();
        void ConfineCursor();
        void FreeCursor();
        static LRESULT CALLBACK HandleMsgSetup( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
        static LRESULT CALLBACK HandleMsgThunk( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
        LRESULT HandleMsg( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    public:
        Keyboard keyboard;
        Mouse mouse;
    private:
        bool cursorEnabled = true;
        int height;
        int width;
        HWND hWnd;
        std::unique_ptr<Graphics> pGfx;
        std::vector<BYTE> rawBuffer;
    };

}