#include "Shader/VertexShader.h"

namespace library
{
    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   VertexShader::VertexShader

      Summary:  Constructor

      Args:     PCWSTR pszFileName
                  Name of the file that contains the shader code
                PCSTR pszEntryPoint
                  Name of the shader entry point functino where shader
                  execution begins
                PCSTR pszShaderModel
                  Specifies the shader target or set of shader features
                  to compile against

      Modifies: [m_vertexShader].
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: VertexShader::VertexShader definition (remove the comment)
    --------------------------------------------------------------------*/
    VertexShader::VertexShader(_In_ PCWSTR pszFileName, _In_ PCSTR pszEntryPoint, _In_ PCSTR pszShaderModel) : 
        Shader(pszFileName, pszEntryPoint, pszShaderModel)
    {
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   VertexShader::Initialize

      Summary:  Initializes the vertex shader and the input layout

      Args:     ID3D11Device* pDevice
                  The Direct3D device to create the vertex shader

      Returns:  HRESULT
                  Status code
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: VertexShader::Initialize definition (remove the comment)
    --------------------------------------------------------------------*/
    HRESULT VertexShader::Initialize(_In_ ID3D11Device* pDevice) {
        //Compile a vertex shader
        ComPtr<ID3DBlob> pVSBlob = nullptr;
        HRESULT hr = compile(pVSBlob.GetAddressOf());
        if (FAILED(hr))
            return hr;

        //Create the Direct 3D Vertex Shader object
        hr = pDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, m_vertexShader.GetAddressOf());
        if (FAILED(hr))
            return hr;

        //Define and create the input layout
        //Define
        D3D11_INPUT_ELEMENT_DESC layout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "INSTANCE_TRANSFORM", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            { "INSTANCE_TRANSFORM", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            { "INSTANCE_TRANSFORM", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 32, D3D11_INPUT_PER_INSTANCE_DATA, 1},
            { "INSTANCE_TRANSFORM", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 2, 48, D3D11_INPUT_PER_INSTANCE_DATA, 1},
        };
        UINT numElements = ARRAYSIZE(layout);

        //Create the input layout
        hr = pDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
            pVSBlob->GetBufferSize(), &m_vertexLayout);
        if (FAILED(hr))
            return hr;

        return S_OK;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   VertexShader::GetVertexShader

      Summary:  Returns the vertex shader

      Returns:  ComPtr<ID3D11VertexShader>&
                  Vertex shader. Could be a nullptr
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: VertexShader::GetVertexShader definition (remove the comment)
    --------------------------------------------------------------------*/
    ComPtr<ID3D11VertexShader>& VertexShader::GetVertexShader() {
        return m_vertexShader;
    }

    /*M+M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M+++M
      Method:   VertexShader::GetVertexLayout

      Summary:  Returns the vertex input layout

      Returns:  ComPtr<ID3D11InputLayout>&
                  Vertex input layout
    M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M---M-M*/
    /*--------------------------------------------------------------------
      TODO: VertexShader::GetVertexLayout definition (remove the comment)
    --------------------------------------------------------------------*/
    ComPtr<ID3D11InputLayout>& VertexShader::GetVertexLayout() {
        return m_vertexLayout;
    }
}
