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
}
