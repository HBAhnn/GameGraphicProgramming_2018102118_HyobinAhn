#pragma once

#include "Cube/BaseCube.h"

class CubeOne : public BaseCube
{
public:
	CubeOne() = default;
	~CubeOne() = default;

	void Update(_In_ FLOAT deltaTime) override;
};