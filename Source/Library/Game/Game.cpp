#include "Game/Game.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Game::Game

      Summary:  Constructor

      Args:     PCWSTR pszGameName
                  Name of the game

      Modifies: [m_pszGameName, m_mainWindow, m_renderer].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Game::Game definition (remove the comment)
    --------------------------------------------------------------------*/
    Game::Game(_In_ PCWSTR pszGameName) : m_pszGameName(pszGameName), m_mainWindow(std::make_unique<MainWindow>()), m_renderer(std::make_unique<Renderer>())
    {
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Game::Initialize

      Summary:  Initializes the components of the game

      Args:     HINSTANCE hInstance
                  Handle to the instance
                INT nCmdShow
                  Is a flag that says whether the main application window
                  will be minimized, maximized, or shown normally

      Modifies: [m_mainWindow, m_renderer].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Game::Initializes definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Game::Initialize(_In_ HINSTANCE hinstance, _In_ INT nCmdShow) {

        HRESULT hr = m_mainWindow->Initialize(hinstance, nCmdShow, GetGameName());
        if (FAILED(hr))
        {
            return hr;
        }
        hr = m_renderer->Initialize(m_mainWindow->GetWindow());
        if (FAILED(hr))
        {
            return hr;
        }
        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Game::Run

      Summary:  Runs the game loop

      Returns:  INT
                  Status code to return to the operating system
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Game::Run definition (remove the comment)
    --------------------------------------------------------------------*/
    INT Game::Run() {
        MSG msg = {};
        PeekMessage(&msg, nullptr, 0U, 0U, PM_NOREMOVE);

        LARGE_INTEGER StartingTime, EndingTime, ElapsedMicroseconds;
        LARGE_INTEGER Frequency;

        QueryPerformanceFrequency(&Frequency);
        QueryPerformanceCounter(&StartingTime);

        while (WM_QUIT != msg.message)
        {
            if (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE) != 0)
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
            else
            {
                // update the elapsed time
                QueryPerformanceCounter(&EndingTime);
                ElapsedMicroseconds.QuadPart = (FLOAT)(EndingTime.QuadPart - StartingTime.QuadPart);

                ElapsedMicroseconds.QuadPart *= 1000000;
                ElapsedMicroseconds.QuadPart /= (FLOAT)Frequency.QuadPart;

                QueryPerformanceFrequency(&Frequency);
                QueryPerformanceCounter(&StartingTime);

                m_renderer->HandleInput(
                    m_mainWindow->GetDirections(),
                    m_mainWindow->GetMouseRelativeMovement(),
                    (float)ElapsedMicroseconds.QuadPart / 1000000.0f
                );

                // update the renderer
                m_renderer->Update((float)ElapsedMicroseconds.QuadPart / 1000000.0f);


                m_renderer->Render();
            }
        }

        return static_cast<INT>(msg.wParam);
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Game::GetGameName

      Summary:  Returns the name of the game

      Returns:  PCWSTR
                  Name of the game
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Game::GetGameName definition (remove the comment)
    --------------------------------------------------------------------*/
    PCWSTR Game::GetGameName() const {
        return this->m_pszGameName;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Game::GetWindow

      Summary:  Returns the main window

      Returns:  std::unique_ptr<MainWindow>&
                  The main window
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Game::GetWindow definition (remove the comment)
    --------------------------------------------------------------------*/
    std::unique_ptr<MainWindow>& Game::GetWindow() {
        return m_mainWindow;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Game::GetRenderer

      Summary:  Returns the renderer

      Returns:  std::unique_ptr<Renderer>&
                  The renderer
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Game::GetRenderer definition (remove the comment)
    --------------------------------------------------------------------*/
    std::unique_ptr<Renderer>& Game::GetRenderer() {
        return m_renderer;
    }
}
