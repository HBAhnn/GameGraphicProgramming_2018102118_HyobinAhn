#pragma once

#include "Cube/BaseCube.h"

class CubeThree : public BaseCube
{
public:
	CubeThree() = default;
	~CubeThree() = default;

	void Update(_In_ FLOAT deltaTime) override;
};