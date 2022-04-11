#include "Cube/CubeOne.h"

void CubeOne::Update(_In_ FLOAT deltaTime)
{
	m_world *= XMMatrixRotationY(deltaTime);
}