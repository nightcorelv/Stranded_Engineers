#pragma once
#include <DirectXMath.h>

DirectX::XMFLOAT3 getSubtractionXMFLOAT3(DirectX::XMFLOAT3 argOne, const DirectX::XMFLOAT3& argTwo);

void subtractionXMFLOAT3(DirectX::XMFLOAT3& argOne, const DirectX::XMFLOAT3& argTwo);

void additionXMFLOAT3(DirectX::XMFLOAT3& argOne, const DirectX::XMFLOAT3& argTwo);

DirectX::XMFLOAT3 normalizeXMFLOAT3(DirectX::XMFLOAT3 argOne);

void scalarMultiplicationXMFLOAT3(const float& scalar, DirectX::XMFLOAT3& argOne);

void newNormalizeXMFLOAT3(DirectX::XMFLOAT3& argOne);

DirectX::XMFLOAT3 getScalarMultiplicationXMFLOAT3(const float& scalar, DirectX::XMFLOAT3 argOne);

float getLength(DirectX::XMFLOAT3 argOne);

bool withinVicinityTwoPoints(float length, DirectX::XMFLOAT3 posOne, const DirectX::XMFLOAT3& posTwo);