#pragma once
#include "Cube/BaseCube.h"

class CubeTwo : public BaseCube
{
public:
	CubeTwo() = default;
	~CubeTwo() = default;

	void Update(_In_ FLOAT deltaTime) override;
};