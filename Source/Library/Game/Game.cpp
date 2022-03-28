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
	Game::Game(PCWSTR pszGameName) {
		m_pszGameName = pszGameName;
		m_mainWindow = std::unique_ptr<MainWindow>(new MainWindow);
		m_renderer = std::unique_ptr<Renderer>(new Renderer);
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
      TODO: Game::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/
	HRESULT Game::Initialize(_In_ HINSTANCE hinstance, _In_ INT nCmdShow) {
		HRESULT hr = m_mainWindow->Initialize(hinstance, nCmdShow, GetGameName());
		if (FAILED(hr))
		{
			return 0;
		}
		if (FAILED(m_renderer->Initialize(m_mainWindow->GetWindow())))
		{
			return 0;
		}

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
		MSG msg = { 0 };

		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
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
	PCWSTR Game::GetGameName() const{
		return this->m_pszGameName;
	}

}