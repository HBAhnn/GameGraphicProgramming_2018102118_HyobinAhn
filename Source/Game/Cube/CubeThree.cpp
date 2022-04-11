#include "Cube/CubeThree.h"


void CubeThree::Update(_In_ FLOAT deltaTime)
{
	XMMATRIX mSpin = XMMatrixRotationY(-deltaTime);
	XMMATRIX mOrbit = XMMatrixRotationX(deltaTime * 2.0f);
	XMMATRIX mTranslate = XMMatrixTranslation(2.0f, 3.0f, 0.0f);
	XMMATRIX mScale = XMMatrixScaling(0.3f, 0.3f, 0.3f);

	//m_world = mScale * mSpin * mTranslate * mOrbit;
	m_world = mScale * mSpin * mTranslate * mOrbit;
}