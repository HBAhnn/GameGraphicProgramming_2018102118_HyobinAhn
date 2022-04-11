#include "Window/MainWindow.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::Initialize

      Summary:  Initializes main window

      Args:     HINSTANCE hInstance
                  Handle to the instance
                INT nCmdShow
                    Is a flag that says whether the main application window
                    will be minimized, maximized, or shown normally
                PCWSTR pszWindowName
                    The window name

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: MainWindow::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT MainWindow::Initialize(_In_ HINSTANCE hInstance, _In_ INT nCmdShow, _In_ PCWSTR pszWindowName)
    {
        m_directions.bBack = 0;
        m_directions.bDown = 0;
        m_directions.bFront = 0;
        m_directions.bLeft = 0;
        m_directions.bRight = 0;
        m_directions.bUp = 0;
        
        m_mouseRelativeMovement.X = 0;
        m_mouseRelativeMovement.Y = 0;

        static bool RI_initialize = false;
        if (RI_initialize == false)
        {
            RAWINPUTDEVICE rid;

            rid.usUsagePage = 0x01;
            rid.usUsage = 0x02;
            rid.dwFlags = 0;
            rid.hwndTarget = NULL;

            if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == false)
            {
                return E_FAIL;
            }

        }

        return initialize(hInstance, nCmdShow, pszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 800, 600, nullptr, nullptr);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::GetWindowClassName

      Summary:  Returns the name of the window class

      Returns:  PCWSTR
                  Name of the window class
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: MainWindow::GetWindowClassName definition (remove the comment)
    --------------------------------------------------------------------*/
    PCWSTR MainWindow::GetWindowClassName() const
    {
        return L"Window Class";
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::HandleMessage

      Summary:  Handles the messages

      Args:     UINT uMessage
                  Message code
                WPARAM wParam
                    Additional data the pertains to the message
                LPARAM lParam
                    Additional data the pertains to the message

      Returns:  LRESULT
                  Integer value that your program returns to Windows
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: MainWindow::HandleMessage definition (remove the comment)
    --------------------------------------------------------------------*/
    LRESULT MainWindow::HandleMessage(_In_ UINT uMessage, _In_ WPARAM wParam, _In_ LPARAM lParam)
    {
        
        switch (uMessage)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_INPUT:
        {
            RECT rc;
            POINT p1, p2;

            GetClientRect(m_hWnd, &rc);

            p1.x = rc.left;
            p1.y = rc.top;
            p2.x = rc.right;
            p2.y = rc.bottom;

            ClientToScreen(m_hWnd, &p1);
            ClientToScreen(m_hWnd, &p2);

            rc.left = p1.x;
            rc.top = p1.y;
            rc.right = p2.x;
            rc.bottom = p2.y;

            ClipCursor(&rc);

            UINT dataSize;

            GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, NULL, &dataSize, sizeof(RAWINPUTHEADER));

            if (dataSize > 0)
            {
                std::unique_ptr<BYTE[]> rawdata = std::make_unique<BYTE[]>(dataSize);

                if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, rawdata.get(), &dataSize, sizeof(RAWINPUTHEADER)) == dataSize)
                {
                    RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(rawdata.get());
                    if (raw->header.dwType == RIM_TYPEMOUSE)
                    {
                        m_mouseRelativeMovement.X = raw->data.mouse.lLastX;
                        m_mouseRelativeMovement.Y = raw->data.mouse.lLastY;
                    }
                }
            }
            
            return DefWindowProc(this->GetWindow(), uMessage, wParam, lParam);
        }
        return 0;

        case WM_KEYUP:
        {
            //'Shift'
            if (wParam == VK_SHIFT)
                m_directions.bDown = 0;
            //'Space'
            if (wParam == VK_SPACE)
                m_directions.bUp = 0;
            //'W'
            if (wParam == 0x57)
                m_directions.bFront = 0;
            //'A'
            if (wParam == 0x41)
                m_directions.bLeft = 0;
            //'S'
            if (wParam == 0x53)
                m_directions.bBack = 0;
            //'D'
            if (wParam == 0x44)
                m_directions.bRight = 0;
        }
        return 0;

        case WM_KEYDOWN:
        {
            //'Shift'
            if (wParam == VK_SHIFT)
                m_directions.bDown = 1;
            //'Space'
            if (wParam == VK_SPACE)
                m_directions.bUp = 1;
            //'W'
            if(wParam == 0x57)
                m_directions.bFront = 1;
            //'A'
            if (wParam == 0x41)
                m_directions.bLeft = 1;
            //'S'
            if (wParam == 0x53)
                m_directions.bBack = 1;
            //'D'
            if (wParam == 0x44)
                m_directions.bRight = 1;
        }
        return 0;

        default:
            return DefWindowProc(this->GetWindow(), uMessage, wParam, lParam);
        }
        return TRUE;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::GetDirections
      Summary:  Returns the keyboard direction input
      Returns:  const DirectionsInput&
                  Keyboard direction input
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: MainWindow::GetDirections definition (remove the comment)
    --------------------------------------------------------------------*/
    const DirectionsInput& MainWindow::GetDirections() const {
        return m_directions;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::GetMouseRelativeMovement
      Summary:  Returns the mouse relative movement
      Returns:  const MouseRelativeMovement&
                  Mouse relative movement
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: MainWindow::GetMouseRelativeMovement definition (remove the comment)
    --------------------------------------------------------------------*/
    const MouseRelativeMovement& MainWindow::GetMouseRelativeMovement() const {
        return m_mouseRelativeMovement;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   MainWindow::ResetMouseMovement
      Summary:  Reset the mouse relative movement to zero
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: MainWindow::ResetMouseMovement definition (remove the comment)
    --------------------------------------------------------------------*/
    void MainWindow::ResetMouseMovement()
    {
        m_mouseRelativeMovement.X = 0;
        m_mouseRelativeMovement.Y = 0;
    }
}
