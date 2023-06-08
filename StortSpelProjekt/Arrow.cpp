#include "stdafx.h"
#include "Arrow.h"
#include "Input.h"
using namespace DirectX;

Arrow::Arrow(const std::string& objectPath, const DirectX::XMFLOAT3& pos)
{
	OBJ testObj(objectPath);
	this->mesh = new Mesh(testObj);
	int nrOfMat = (int)testObj.mtl.materials.size();
	for (int i = 0; i < nrOfMat; i++)
	{
		MaterialLibrary::LoadMaterial(testObj.mtl.materials[i]);
	}

	this->position = pos;
	this->mesh->position = pos;
	this->rotation = DirectX::XMMatrixIdentity();
	this->mesh->rotation = this->rotation;
	scale = DirectX::XMFLOAT3(1, 1, 1);
	this->mesh->scale = scale;
	this->scale = scale;
	this->downTime = 0.0f;
	this->currentTime = 0.0f;

	this->mesh->UpdateCB(position, rotation, scale);
}

Arrow::Arrow(Mesh* useMesh, const DirectX::XMFLOAT3& pos)
{
	this->mesh = useMesh;
	this->position = pos;
	this->mesh->position = pos;
	this->rotation = DirectX::XMMatrixIdentity();
	this->mesh->rotation = this->rotation;
	scale = DirectX::XMFLOAT3(1, 1, 1);
	this->mesh->scale = scale;
	this->scale = scale;
	this->downTime = 0.0f;
	this->currentTime = 0.0f;

	this->mesh->UpdateCB(position, rotation, scale);
}

Arrow::~Arrow()
{
}

DirectX::XMVECTOR Arrow::getUpVectorArrow() const
{
	return this->normalVector;
}

DirectX::XMVECTOR Arrow::getForwardVectorArrow() const
{
	return this->forwardVector;
}

DirectX::XMVECTOR Arrow::getRightVectorArrow() const
{
	return this->rightVector;
}

DirectX::XMVECTOR Arrow::getArrowVectorArrow() const
{
	return this->arrowVector;
}

DirectX::XMVECTOR Arrow::getPosition() const
{
	return this->position;
}

void Arrow::moveWithCamera(const DirectX::XMVECTOR& playerPosition, const DirectX::XMVECTOR& cameraForward, const DirectX::XMVECTOR& playerUp, const DirectX::XMMATRIX& playerRotation)
{
	this->position = playerPosition + playerUp * 10.f + cameraForward * 10.f;
	this->rotation = playerRotation;
}

void Arrow::showDirection(const DirectX::XMVECTOR& goalPosition, const DirectX::XMFLOAT3& gravity)
{
	//Calculations
	arrowVector = DirectX::XMVectorSubtract(goalPosition, this->position);
	XMVECTOR targetDistance = arrowVector;
	normalVector = DirectX::XMVectorSet(-gravity.x, -gravity.y, -gravity.z, 1.0f);
	rightVector = DirectX::XMVector3TransformCoord(DEFAULT_RIGHT, rotation);
	forwardVector = DirectX::XMVector3TransformCoord(DEFAULT_FORWARD, rotation);

	arrowVector = DirectX::XMVector3Normalize(arrowVector);
	normalVector = DirectX::XMVector3Normalize(normalVector);
	rightVector = DirectX::XMVector3Normalize(rightVector);
	forwardVector = DirectX::XMVector3Normalize(forwardVector);

	//Y-Rotation
	resultVector = DirectX::XMVector3Dot(arrowVector, rightVector);
	if (resultVector.x < 0.f)
	{
		resultVector = DirectX::XMVector3AngleBetweenNormals(arrowVector, forwardVector);
		rotation *= DirectX::XMMatrixRotationAxis(normalVector, -resultVector.x);
	}
	else if (resultVector.x > 0.f)
	{
		resultVector = DirectX::XMVector3AngleBetweenNormals(arrowVector, forwardVector);
		rotation *= DirectX::XMMatrixRotationAxis(normalVector, resultVector.x);
	}
	else
	{
		resultVector = DirectX::XMVector3AngleBetweenNormals(arrowVector, forwardVector);
		if (resultVector.x > XM_PIDIV2) rotation *= DirectX::XMMatrixRotationAxis(normalVector, resultVector.x);
	}

	rightVector = DirectX::XMVector3TransformCoord(DEFAULT_RIGHT, rotation);
	forwardVector = DirectX::XMVector3TransformCoord(DEFAULT_FORWARD, rotation);
	rightVector = DirectX::XMVector3Normalize(rightVector);
	forwardVector = DirectX::XMVector3Normalize(forwardVector);

	//X-Rotation
	resultVector = DirectX::XMVector3AngleBetweenNormals(arrowVector, forwardVector);
	rotation *= DirectX::XMMatrixRotationAxis(rightVector, resultVector.x);
	rightVector = DirectX::XMVector3TransformCoord(DEFAULT_RIGHT, rotation);
	forwardVector = DirectX::XMVector3TransformCoord(DEFAULT_FORWARD, rotation);
	rightVector = DirectX::XMVector3Normalize(rightVector);
	forwardVector = DirectX::XMVector3Normalize(forwardVector);

	
	//Animate when close
	DirectX::SimpleMath::Vector4 distance = DirectX::XMVector4LengthSq(targetDistance);
	static float dt = Time::DeltaTimeInSeconds();
	float constant = 1.5f;
	float range = 3500;
	
	if (distance.x <= range) //FIX grow vs shrink problem
	{
		float timeConst = 0.3; //time in sec for pulse
		currentTime += Time::DeltaTimeInSeconds();
		bool growing = false;
		bool shrinking = false;

		//Var effectiveness increases with proximity
		float prox = constant * (1 - (distance.x / range));


		if (currentTime < timeConst)	//grow
		{
			this->scale = { scale.x + dt * prox, scale.y + dt * prox, scale.z + dt * prox }; 
			growing = true;
			downTime = 0;
		}
		else if (downTime < timeConst)	//shrink
		{
			downTime += Time::DeltaTimeInSeconds();
			this->scale = { scale.x - dt * prox, scale.y - dt * prox, scale.z - dt * prox };
			shrinking = true;
		}

		if (!growing && !shrinking)
		{
			this->currentTime = this->downTime = 0.0f;
		}

		//Reset
		returnTime = 0;
	}
	else if (scale.x != 1)
	{
		float diff = 1 - scale.x;
		returnTime += Time::DeltaTimeInSeconds();


		if (this->scale.x != 1 && returnTime < 3) //decay over time //problem * exponentially smaller diff...
		{
			this->scale = { this->scale.x + diff * constant *dt,this->scale.y + diff * constant *dt,this->scale.z + diff * constant *dt};
		}
		else
		{
			this->scale = { 1,1,1 };
		}
		//Reset
		this->currentTime = this->downTime = 0.0f;
		//std::cout << "Scl: " << scale.x << std::endl;
		//std::cout << "Dif: " << diff << std::endl;
	}


	//Checking if rotation is wrong
	resultVector = DirectX::XMVector3AngleBetweenNormals(arrowVector, forwardVector);
	if (resultVector.x > 0.1f) rotation *= DirectX::XMMatrixRotationAxis(rightVector, -resultVector.x);
}

void Arrow::removeArrow()
{
	this->position = DirectX::XMFLOAT3(6969, 6969, 6969);
}

void Arrow::draw()
{
	this->mesh->UpdateCB(position, rotation, scale);
	this->mesh->DrawWithMat();
}

void Arrow::update()
{
	//Set mesh pos & rot to current member variable pos/rot
	this->mesh->position = this->position;
	this->mesh->rotation = this->rotation;
	this->mesh->scale = this->scale;

	//Update constantbuffer
	this->mesh->UpdateCB(position, rotation, scale);
}