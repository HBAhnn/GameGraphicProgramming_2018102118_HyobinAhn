#include "Renderer/Renderer.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Renderer

      Summary:  Constructor

      Modifies: [m_driverType, m_featureLevel, m_d3dDevice, m_d3dDevice1,
                 m_immediateContext, m_immediateContext1, m_swapChain,
                 m_swapChain1, m_renderTargetView, m_depthStencil,
                 m_depthStencilView, m_cbChangeOnResize, m_camera,
                 m_projection, m_renderables, m_vertexShaders,
                 m_pixelShaders].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Renderer definition (remove the comment)
    --------------------------------------------------------------------*/
    Renderer::Renderer()
        : m_driverType(D3D_DRIVER_TYPE_HARDWARE),
        m_featureLevel(D3D_FEATURE_LEVEL_11_1),
        m_d3dDevice(nullptr),
        m_d3dDevice1(nullptr),
        m_immediateContext(nullptr),
        m_immediateContext1(nullptr),
        m_swapChain(nullptr),
        m_swapChain1(nullptr),
        m_renderTargetView(nullptr),
        m_depthStencil(nullptr),
        m_depthStencilView(nullptr),
        m_cbChangeOnResize(nullptr),
        m_camera(XMVectorSet(0.0f, 1.0f, -5.0f, 0.0f)),
        m_projection(),
        m_renderables(),
        m_vertexShaders(),
        m_pixelShaders(),
        m_cbLights(nullptr),
        m_aPointLights(),
        m_pszMainSceneName(),
        m_scenes(),
        m_models()
    {}

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Initialize
      Summary:  Creates Direct3D device and swap chain
      Args:     HWND hWnd
                  Handle to the window
      Modifies: [m_d3dDevice, m_featureLevel, m_immediateContext,
                 m_d3dDevice1, m_immediateContext1, m_swapChain1,
                 m_swapChain, m_renderTargetView, m_cbChangeOnResize,
                 m_projection, m_cbLights, m_camera, m_vertexShaders,
                 m_pixelShaders, m_renderables].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/

    HRESULT Renderer::Initialize(_In_ HWND hWnd) {

        HRESULT hr = S_OK;

        RECT rc;
        GetClientRect(hWnd, &rc);
        UINT width = rc.right - rc.left;
        UINT height = rc.bottom - rc.top;

        UINT createDeviceFlags = 0;
#ifdef _DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        D3D_DRIVER_TYPE driverTypes[] =
        {
            D3D_DRIVER_TYPE_HARDWARE,
            D3D_DRIVER_TYPE_WARP,
            D3D_DRIVER_TYPE_REFERENCE,
        };
        UINT numDriverTypes = ARRAYSIZE(driverTypes);

        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
        };
        UINT numFeatureLevels = ARRAYSIZE(featureLevels);

        for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
        {
            m_driverType = driverTypes[driverTypeIndex];
            hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                D3D11_SDK_VERSION, &m_d3dDevice, &m_featureLevel, &m_immediateContext);

            if (hr == E_INVALIDARG)
            {
                // DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
                hr = D3D11CreateDevice(nullptr, m_driverType, nullptr, createDeviceFlags, &featureLevels[1], numFeatureLevels - 1,
                    D3D11_SDK_VERSION, &m_d3dDevice, &m_featureLevel, &m_immediateContext);
            }

            if (SUCCEEDED(hr))
                break;
        }
        if (FAILED(hr))
            return hr;

        // Obtain DXGI factory from device (since we used nullptr for pAdapter above)
        //IDXGIFactory1* dxgiFactory = nullptr;
        ComPtr<IDXGIFactory1>           dxgiFactory(nullptr);
        {
            ComPtr<IDXGIDevice>           dxgiDevice(nullptr);
            hr = m_d3dDevice.As(&dxgiDevice);
            if (SUCCEEDED(hr))
            {
                ComPtr<IDXGIAdapter>           adapter(nullptr);

                hr = dxgiDevice->GetAdapter(adapter.GetAddressOf());
                if (SUCCEEDED(hr))
                {
                    hr = adapter->GetParent(__uuidof(IDXGIFactory1), (&dxgiFactory));
                }
            }
        }
        if (FAILED(hr))
            return hr;

        //create swap chain
        ComPtr<IDXGIFactory2>           dxgiFactory2(nullptr);
        hr = dxgiFactory.As(&dxgiFactory2);
        if (dxgiFactory2)
        {
            // DirectX 11.1 or later
            hr = m_d3dDevice.As(&m_d3dDevice1);
            if (SUCCEEDED(hr))
            {
                hr = m_immediateContext.As(&m_immediateContext1);
            }

            DXGI_SWAP_CHAIN_DESC1 sd = {};
            sd.Width = width;
            sd.Height = height;
            sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.BufferCount = 1;

            hr = dxgiFactory2->CreateSwapChainForHwnd(m_d3dDevice.Get(), hWnd, &sd, nullptr, nullptr, m_swapChain1.GetAddressOf());
            if (SUCCEEDED(hr))
            {
                hr = m_swapChain1.As(&m_swapChain);
            }
        }
        else
        {
            // DirectX 11.0 systems
            DXGI_SWAP_CHAIN_DESC sd = {};
            sd.BufferCount = 1;
            sd.BufferDesc.Width = width;
            sd.BufferDesc.Height = height;
            sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            sd.BufferDesc.RefreshRate.Numerator = 60;
            sd.BufferDesc.RefreshRate.Denominator = 1;
            sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            sd.OutputWindow = hWnd;
            sd.SampleDesc.Count = 1;
            sd.SampleDesc.Quality = 0;
            sd.Windowed = TRUE;

            hr = dxgiFactory->CreateSwapChain(m_d3dDevice.Get(), &sd, m_swapChain.GetAddressOf());
        }

        // Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
        dxgiFactory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);


        if(FAILED(hr))
            return hr;

        // Create a render target view
        ComPtr<ID3D11Texture2D> pBackBuffer;

        hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(pBackBuffer.GetAddressOf()));

        if (FAILED(hr))
            return hr;

        hr = m_d3dDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, m_renderTargetView.GetAddressOf());
        if (FAILED(hr))
            return hr;

        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), nullptr);

        //Create Depth stencil texture
        D3D11_TEXTURE2D_DESC descDepth = {
        .Width = width,
        .Height = height,
        .MipLevels = 1,
        .ArraySize = 1,
        .Format = DXGI_FORMAT_D24_UNORM_S8_UINT,
        .SampleDesc = {.Count = 1,
                       .Quality = 0},
        .Usage = D3D11_USAGE_DEFAULT,
        .BindFlags = D3D11_BIND_DEPTH_STENCIL,
        .CPUAccessFlags = 0,
        .MiscFlags = 0
        };
        hr = m_d3dDevice->CreateTexture2D(&descDepth, nullptr, &m_depthStencil);
        if (FAILED(hr))
            return hr;

        // Create the depth stencil view
        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
        descDSV.Format = descDepth.Format;
        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;
        hr = m_d3dDevice->CreateDepthStencilView(m_depthStencil.Get(), &descDSV, &m_depthStencilView);
        if (FAILED(hr))
            return hr;

        m_immediateContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), m_depthStencilView.Get());

        // Setup the viewport
        D3D11_VIEWPORT vp;
        vp.Width = (FLOAT)width;
        vp.Height = (FLOAT)height;
        vp.MinDepth = 0.0f;
        vp.MaxDepth = 1.0f;
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        m_immediateContext->RSSetViewports(1, &vp);

        //Camera initialize
        m_camera.Initialize(m_d3dDevice.Get());

        //Create a projection matrix
        m_projection = XMMatrixPerspectiveFovLH(XM_PIDIV2, (FLOAT)width / (FLOAT)height, 0.01f, 100.0f);    

        //Create the constant buffer
        D3D11_BUFFER_DESC bd = {
            .ByteWidth = sizeof(CBChangeOnResize),
            .Usage = D3D11_USAGE_DEFAULT,
            .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
            .CPUAccessFlags = 0
        };
        hr = m_d3dDevice->CreateBuffer(&bd, nullptr, m_cbChangeOnResize.GetAddressOf());
        if (FAILED(hr))
            return hr;

        //Set CBChangeOnResize constant buffer

        CBChangeOnResize cbChangesOnResize;
        cbChangesOnResize.Projection = XMMatrixTranspose(m_projection);
        m_immediateContext->UpdateSubresource(m_cbChangeOnResize.Get(), 0, nullptr, &cbChangesOnResize, 0, 0);

        //Create Light Constant buffer
        D3D11_BUFFER_DESC bd1 = {
           .ByteWidth = sizeof(CBLights),
           .Usage = D3D11_USAGE_DEFAULT,
           .BindFlags = D3D11_BIND_CONSTANT_BUFFER,
           .CPUAccessFlags = 0
        };

        CBLights cbLights;

        hr = m_d3dDevice->CreateBuffer(&bd1, nullptr, &m_cbLights);
        if (FAILED(hr)) 
            return hr;

        
        //Initialize renderables
        for (auto& renderables : m_renderables)
        {
            hr = renderables.second->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());
            if (FAILED(hr))
                return hr;
        }

        //initialize models
        for (auto& models : m_models)
        {
            hr = models.second->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());
            if (FAILED(hr))
                return hr;
        }

        //Initialize vertexShaders
        for (auto& vertexShaders : m_vertexShaders)
        {
            hr = vertexShaders.second->Initialize(m_d3dDevice.Get());
            if (FAILED(hr)) 
                return hr;
        }

        //Initialize pixelShaders
        for (auto& pixelShaders : m_pixelShaders)
        {
            hr = pixelShaders.second->Initialize(m_d3dDevice.Get());
            if (FAILED(hr)) 
                return hr;
        }
        
        //Initialize Scenes ( create buffer )
        for (auto& scenes : m_scenes)
        {
            hr = scenes.second->Initialize(m_d3dDevice.Get(), m_immediateContext.Get());
            if (FAILED(hr))
                return hr;
        }

        // Set primitive topology
        m_immediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddRenderable

      Summary:  Add a renderable object and initialize the object

      Args:     PCWSTR pszRenderableName
                  Key of the renderable object
                const std::shared_ptr<Renderable>& renderable
                  Unique pointer to the renderable object

      Modifies: [m_renderables].

      Returns:  HRESULT
                  Status code.
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::AddRenderable definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::AddRenderable(_In_ PCWSTR pszRenderableName, _In_ const std::shared_ptr<Renderable>& renderable) {
        if (m_renderables.count(pszRenderableName))
        {
            return E_FAIL;
        }
        else
        {
            m_renderables.insert(std::make_pair(pszRenderableName, renderable));

            if (m_renderables.empty()){}
            return S_OK;
        }
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddPointLight
      Summary:  Add a point light
      Args:     size_t index
                  Index of the point light
                const std::shared_ptr<PointLight>& pointLight
                  Shared pointer to the point light object
      Modifies: [m_aPointLights].
      Returns:  HRESULT
                  Status code.
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::AddPointLight definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::AddPointLight(_In_ size_t index, _In_ const std::shared_ptr<PointLight>& pPointLight)
    {
        if (index >= NUM_LIGHTS)
            return E_FAIL;
        m_aPointLights[index] = pPointLight;
        return S_OK;
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddVertexShader

      Summary:  Add the vertex shader into the renderer

      Args:     PCWSTR pszVertexShaderName
                  Key of the vertex shader
                const std::shared_ptr<VertexShader>&
                  Vertex shader to add

      Modifies: [m_vertexShaders].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::AddVertexShader definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::AddVertexShader(_In_ PCWSTR pszVertexShaderName, _In_ const std::shared_ptr<VertexShader>& vertexShader) {
        if (m_vertexShaders.count(pszVertexShaderName))
        {
            return E_FAIL;
        }
        else
        {
            m_vertexShaders.insert(std::make_pair(pszVertexShaderName, vertexShader));
            return S_OK;
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddPixelShader

      Summary:  Add the pixel shader into the renderer

      Args:     PCWSTR pszPixelShaderName
                  Key of the pixel shader
                const std::shared_ptr<PixelShader>&
                  Pixel shader to add

      Modifies: [m_pixelShaders].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::AddPixelShader definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::AddPixelShader(_In_ PCWSTR pszPixelShaderName, _In_ const std::shared_ptr<PixelShader>& pixelShader)
    {
        if (m_pixelShaders.count(pszPixelShaderName))
        {
            return E_FAIL;
        }
        else
        {
            m_pixelShaders.insert(std::make_pair(pszPixelShaderName, pixelShader));
            return S_OK;
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddScene
      Summary:  Add a scene
      Args:     PCWSTR pszSceneName
                  Key of a scene
                const std::filesystem::path& sceneFilePath
                  File path to initialize a scene
      Modifies: [m_scenes].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::AddScene definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::AddScene(_In_ PCWSTR pszSceneName, const std::filesystem::path& sceneFilePath)
    {
        if (m_scenes.count(pszSceneName))
        {
            return E_FAIL;
        }
        else
        {
            m_scenes.insert(std::make_pair(pszSceneName, std::make_shared<Scene>(sceneFilePath)));
            return S_OK;
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetMainScene
      Summary:  Set the main scene
      Args:     PCWSTR pszSceneName
                  Name of the scene to set as the main scene
      Modifies: [m_pszMainSceneName].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::SetMainScene definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::SetMainScene(_In_ PCWSTR pszSceneName)
    {
        m_pszMainSceneName = pszSceneName;
        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
    Method:   Renderer::HandleInput
    Summary:  Add the pixel shader into the renderer and initialize it
    Args:     const DirectionsInput& directions
                  Data structure containing keyboard input data
                const MouseRelativeMovement& mouseRelativeMovement
                Data structure containing mouse relative input data
    Modifies: [m_camera].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
    TODO: Renderer::HandleInput definition (remove the comment)
    --------------------------------------------------------------------*/
    void Renderer::HandleInput(
        _In_ const DirectionsInput& directions,
        _In_ const MouseRelativeMovement& mouseRelativeMovement,
        _In_ FLOAT deltaTime
    )
    {
        m_camera.HandleInput(
            directions,
            mouseRelativeMovement,
            deltaTime
        );
    }



    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Update

      Summary:  Update the renderables each frame

      Args:     FLOAT deltaTime
                  Time difference of a frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Update definition (remove the comment)
    --------------------------------------------------------------------*/
    void Renderer::Update(_In_ FLOAT deltaTime) {
        for (auto& renderable : m_renderables)
        {
            renderable.second->Update(deltaTime);
        }
        for (auto& point_light : m_aPointLights)
        {
            point_light.get()->Update(deltaTime);
        }
        for (auto& models : m_models)
        {
            models.second->Update(deltaTime);
        }
        m_camera.Update(deltaTime);
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::Render

      Summary:  Render the frame
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::Render definition (remove the comment)
    --------------------------------------------------------------------*/
    void Renderer::Render() {
        //Clear the back buffer
        m_immediateContext->ClearRenderTargetView(m_renderTargetView.Get(), Colors::MidnightBlue);

        //Clear the depth buffer to 1.0 (maximum depth)
        m_immediateContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

        // Create camera constant buffer and update
        CBChangeOnCameraMovement cb = {
               .View = XMMatrixTranspose(m_camera.GetView())
        };
        XMStoreFloat4(&cb.CameraPosition, m_camera.GetEye());

        m_immediateContext->VSSetConstantBuffers(0, 1, m_camera.GetConstantBuffer().GetAddressOf());
        m_immediateContext->PSSetConstantBuffers(0, 1, m_camera.GetConstantBuffer().GetAddressOf());

        m_immediateContext->UpdateSubresource(m_camera.GetConstantBuffer().Get(), 0, nullptr, &cb, 0, 0);

        //Update the lights constant buffer

        CBLights cb1;
        cb1.LightPositions[0] = m_aPointLights[0].get()->GetPosition();
        cb1.LightPositions[1] = m_aPointLights[1].get()->GetPosition();
        cb1.LightColors[0] = m_aPointLights[0].get()->GetColor();
        cb1.LightColors[1] = m_aPointLights[1].get()->GetColor();



        m_immediateContext->VSSetConstantBuffers(3, 1, m_cbLights.GetAddressOf());
        m_immediateContext->PSSetConstantBuffers(3, 1, m_cbLights.GetAddressOf());

        m_immediateContext->UpdateSubresource(m_cbLights.Get(), 0, nullptr, &cb1, 0, 0);


        for(auto& each : m_renderables)
        {
            std::shared_ptr<Renderable> renderable = each.second;
            //Set vertex buffer
            UINT stride = sizeof(SimpleVertex);
            UINT offset = 0;
            m_immediateContext->IASetVertexBuffers(0, 1, renderable->GetVertexBuffer().GetAddressOf(), &stride, &offset);

            //Set Index buffer
            m_immediateContext->IASetIndexBuffer(renderable->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

            //Set input layer
            m_immediateContext->IASetInputLayout(renderable->GetVertexLayout().Get());

            //Create and update constant buffer
            CBChangesEveryFrame cb = {
                .World = XMMatrixTranspose(renderable->GetWorldMatrix()),
                .OutputColor = renderable->GetOutputColor()
            };
            m_immediateContext->UpdateSubresource(renderable->GetConstantBuffer().Get(), 0, NULL, &cb, 0, 0);

            //Rendering triangles
            m_immediateContext->VSSetShader(renderable->GetVertexShader().Get(), nullptr, 0);

            
            m_immediateContext->VSSetConstantBuffers(1, 1, m_cbChangeOnResize.GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(2, 1, renderable->GetConstantBuffer().GetAddressOf());

            m_immediateContext->PSSetShader(renderable->GetPixelShader().Get(), nullptr, 0);

            m_immediateContext->PSSetConstantBuffers(2, 1, renderable->GetConstantBuffer().GetAddressOf());


            if (renderable->HasTexture())
            {
                for (UINT i = 0; i < renderable->GetNumMeshes(); i++)
                {
                    m_immediateContext->PSSetShaderResources(0, 1, renderable->GetMaterial(renderable->GetMesh(i).uMaterialIndex).pDiffuse->GetTextureResourceView().GetAddressOf());
                    m_immediateContext->PSSetSamplers(0, 1, renderable->GetMaterial(renderable->GetMesh(i).uMaterialIndex).pDiffuse->GetSamplerState().GetAddressOf());

                    m_immediateContext->DrawIndexed(renderable->GetMesh(i).uNumIndices, renderable->GetMesh(i).uBaseIndex, renderable->GetMesh(i).uBaseVertex);
                }
            }
            else {
                m_immediateContext->DrawIndexed(renderable->GetNumIndices(), 0, 0);
            }

        }
        for (auto& scene : m_scenes)
        {
            //m_scenes[m_pszMainSceneName]와 동일
            for (auto& voxel : scene.second->GetVoxels())
            {
                UINT strides[2] = { sizeof(SimpleVertex), sizeof(InstanceData) };
                UINT offset = 0;
                //Set vertex buffer, instance buffer
                m_immediateContext->IASetVertexBuffers(0, 1, voxel->GetVertexBuffer().GetAddressOf(), &strides[0], &offset);
                m_immediateContext->IASetVertexBuffers(1, 1, voxel->GetInstanceBuffer().GetAddressOf(), &strides[1], &offset);
                
                //Set Index buffer
                m_immediateContext->IASetIndexBuffer(voxel->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

                //Set input layer
                m_immediateContext->IASetInputLayout(voxel->GetVertexLayout().Get());

                //Create and update constant buffer
                CBChangesEveryFrame cb = {
                    .World = XMMatrixTranspose(voxel->GetWorldMatrix()),
                    .OutputColor = voxel->GetOutputColor()
                };
                m_immediateContext->UpdateSubresource(voxel->GetConstantBuffer().Get(), 0, NULL, &cb, 0, 0);

                //Rendering triangles
                m_immediateContext->VSSetShader(voxel->GetVertexShader().Get(), nullptr, 0);


                m_immediateContext->VSSetConstantBuffers(1, 1, m_cbChangeOnResize.GetAddressOf());
                m_immediateContext->VSSetConstantBuffers(2, 1, voxel->GetConstantBuffer().GetAddressOf());

                m_immediateContext->PSSetShader(voxel->GetPixelShader().Get(), nullptr, 0);

                m_immediateContext->PSSetConstantBuffers(2, 1, voxel->GetConstantBuffer().GetAddressOf());

                m_immediateContext->DrawIndexedInstanced(voxel->GetNumIndices(), voxel->GetNumInstances(), 0, 0, 0);
            }
        }

        for (auto& each : m_models)
        {
            std::shared_ptr<Model> model = each.second;
            //Set vertex buffer
            UINT aStrides[2] = { static_cast<UINT>(sizeof(SimpleVertex)), static_cast<UINT>(sizeof(AnimationData)) };
            UINT aOffsets[2] = { 0u, 0u };

            ID3D11Buffer* aBuffers[2]
            {
               model->GetVertexBuffer().Get(),
               model->GetAnimationBuffer().Get()
            };

            m_immediateContext->IASetVertexBuffers(0, 1, &aBuffers[0], &aStrides[0], &aOffsets[0]);
            m_immediateContext->IASetVertexBuffers(0, 1, &aBuffers[1], &aStrides[1], &aOffsets[1]);

            //Set Index buffer
            m_immediateContext->IASetIndexBuffer(model->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

            //Set input layer
            m_immediateContext->IASetInputLayout(model->GetVertexLayout().Get());

            //Create and update constant buffer
            CBChangesEveryFrame cb = {
                .World = XMMatrixTranspose(model->GetWorldMatrix()),
                .OutputColor = model->GetOutputColor()
            };

            m_immediateContext->UpdateSubresource(model->GetConstantBuffer().Get(), 0, NULL, &cb, 0, 0);

            CBSkinning cbs;
            for (int i = 0; i < model->GetBoneTransforms().size(); i++)
            {
                cbs.BoneTransforms[i] = model->GetBoneTransforms()[i];
            }

            m_immediateContext->UpdateSubresource(model->GetConstantBuffer().Get(), 0, NULL, &cbs, 0, 0);

            //Rendering triangles
            m_immediateContext->VSSetShader(model->GetVertexShader().Get(), nullptr, 0);


            m_immediateContext->VSSetConstantBuffers(1, 1, m_cbChangeOnResize.GetAddressOf());
            m_immediateContext->VSSetConstantBuffers(2, 1, model->GetConstantBuffer().GetAddressOf());

            m_immediateContext->PSSetShader(model->GetPixelShader().Get(), nullptr, 0);

            m_immediateContext->PSSetConstantBuffers(2, 1, model->GetConstantBuffer().GetAddressOf());


            if (model->HasTexture())
            {
                for (UINT i = 0; i < model->GetNumMeshes(); i++)
                {
                    m_immediateContext->PSSetShaderResources(0, 1, model->GetMaterial(model->GetMesh(i).uMaterialIndex).pDiffuse->GetTextureResourceView().GetAddressOf());
                    m_immediateContext->PSSetSamplers(0, 1, model->GetMaterial(model->GetMesh(i).uMaterialIndex).pDiffuse->GetSamplerState().GetAddressOf());

                    m_immediateContext->DrawIndexed(model->GetMesh(i).uNumIndices, model->GetMesh(i).uBaseIndex, model->GetMesh(i).uBaseVertex);
                }
            }
            else {
                m_immediateContext->DrawIndexed(model->GetNumIndices(), 0, 0);
            }

        }
        m_swapChain->Present(0, 0);
    }



    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetVertexShaderOfRenderable

      Summary:  Sets the vertex shader for a renderable

      Args:     PCWSTR pszRenderableName
                  Key of the renderable
                PCWSTR pszVertexShaderName
                  Key of the vertex shader

      Modifies: [m_renderables].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::SetVertexShaderOfRenderable definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::SetVertexShaderOfRenderable(_In_ PCWSTR pszRenderableName, _In_ PCWSTR pszVertexShaderName) 
    {
        std::unordered_map<std::wstring, std::shared_ptr<Renderable>>::const_iterator iRenderable = m_renderables.find(pszRenderableName);
        std::unordered_map<std::wstring, std::shared_ptr<VertexShader>>::const_iterator iVertexShader = m_vertexShaders.find(pszVertexShaderName);

        if (iRenderable == m_renderables.end() || iVertexShader == m_vertexShaders.end())
        {
            return E_FAIL;
        }

        iRenderable->second->SetVertexShader(iVertexShader->second);

        return S_OK;
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetPixelShaderOfRenderable

      Summary:  Sets the pixel shader for a renderable

      Args:     PCWSTR pszRenderableName
                  Key of the renderable
                PCWSTR pszPixelShaderName
                  Key of the pixel shader

      Modifies: [m_renderables].

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::SetPixelShaderOfRenderable definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::SetPixelShaderOfRenderable(_In_ PCWSTR pszRenderableName, _In_ PCWSTR pszPixelShaderName)
    {

        if (!m_renderables.count(pszRenderableName))
        {
            return E_FAIL;
        }
        else
        {
            if (m_vertexShaders.count(pszPixelShaderName))
            {
                m_renderables.find(pszRenderableName)->second->SetPixelShader(m_pixelShaders.find(pszPixelShaderName)->second);
                return S_OK;
            }
        }
        return E_FAIL;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetVertexShaderOfScene
      Summary:  Sets the vertex shader for the voxels in a scene
      Args:     PCWSTR pszSceneName
                  Key of the scene
                PCWSTR pszVertexShaderName
                  Key of the vertex shader
      Modifies: [m_scenes].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::SetVertexShaderOfScene definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::SetVertexShaderOfScene(_In_ PCWSTR pszSceneName, _In_ PCWSTR pszVertexShaderName)
    {
        if (!m_scenes.count(pszSceneName))
            return E_FAIL;
        int i;
        for (i = 0; i < m_scenes[pszSceneName]->GetVoxels().size(); i++)
        {
            m_scenes[pszSceneName]->GetVoxels()[i]->SetVertexShader(m_vertexShaders[pszVertexShaderName]);
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetPixelShaderOfScene
      Summary:  Sets the pixel shader for the voxels in a scene
      Args:     PCWSTR pszRenderableName
                  Key of the renderable
                PCWSTR pszPixelShaderName
                  Key of the pixel shader
      Modifies: [m_renderables].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::SetPixelShaderOfScene definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::SetPixelShaderOfScene(_In_ PCWSTR pszSceneName, _In_ PCWSTR pszVertexShaderName)
    {
        if (!m_scenes.count(pszSceneName))
            return E_FAIL;
        int i;
        for (i = 0; i < m_scenes[pszSceneName]->GetVoxels().size(); i++)
        {
            m_scenes[pszSceneName]->GetVoxels()[i]->SetPixelShader(m_pixelShaders[pszVertexShaderName]);
        }

        return S_OK;
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::GetDriverType

      Summary:  Returns the Direct3D driver type

      Returns:  D3D_DRIVER_TYPE
                  The Direct3D driver type used
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::GetDriverType definition (remove the comment)
    --------------------------------------------------------------------*/
    D3D_DRIVER_TYPE Renderer::GetDriverType() const {
        return m_driverType;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::AddModel
      Summary:  Add a model object
      Args:     PCWSTR pszModelName
                  Key of the model object
                const std::shared_ptr<Model>& pModel
                  Shared pointer to the model object
      Modifies: [m_models].
      Returns:  HRESULT
                  Status code.
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::AddModel definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::AddModel(_In_ PCWSTR pszModelName, _In_ const std::shared_ptr<Model>& pModel)
    {
        if (m_models.count(pszModelName))
        {
            return E_FAIL;
        }
        else
        {
            m_models.insert(std::make_pair(pszModelName, pModel));
            return S_OK;
        }
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetVertexShaderOfModel
      Summary:  Sets the pixel shader for a model
      Args:     PCWSTR pszModelName
                  Key of the model
                PCWSTR pszVertexShaderName
                  Key of the vertex shader
      Modifies: [m_renderables].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::SetVertexShaderOfModel definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::SetVertexShaderOfModel(_In_ PCWSTR pszModelName, _In_ PCWSTR pszVertexShaderName)
    {
        if (!m_models.count(pszModelName))
            return E_FAIL;
        m_models[pszModelName]->SetVertexShader(m_vertexShaders[pszVertexShaderName]);
    }


    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   Renderer::SetPixelShaderOfModel
      Summary:  Sets the pixel shader for a model
      Args:     PCWSTR pszModelName
                  Key of the model
                PCWSTR pszPixelShaderName
                  Key of the pixel shader
      Modifies: [m_renderables].
      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: Renderer::SetPixelShaderOfModel definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT Renderer::SetPixelShaderOfModel(_In_ PCWSTR pszModelName, _In_ PCWSTR pszVertexShaderName)
    {
        if (!m_models.count(pszModelName))
            return E_FAIL;
        m_models[pszModelName]->SetPixelShader(m_pixelShaders[pszVertexShaderName]);
    }
}