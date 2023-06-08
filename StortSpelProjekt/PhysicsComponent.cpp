#include "stdafx.h"
#include "PhysicsComponent.h"
#include "GameObject.h"

void PhysicsComponent::createRigidBody(const reactphysics3d::Transform& transform)
{
	this->rigidBody = this->worldPtr->createRigidBody(transform);
}

void PhysicsComponent::deallocate()
{
	this->worldPtr->destroyRigidBody(this->rigidBody);

	//Now we need to destroy the correct shape, and to get the shape we get the name and destroy accordingly
	reactphysics3d::CollisionShapeName shapeType = this->shape->getName();
	reactphysics3d::BoxShape* a = dynamic_cast<reactphysics3d::BoxShape*>(this->shape);
	reactphysics3d::SphereShape* b = dynamic_cast<reactphysics3d::SphereShape*>(this->shape);
	reactphysics3d::ConvexMeshShape* c = dynamic_cast<reactphysics3d::ConvexMeshShape*>(this->shape);
	reactphysics3d::ConvexMeshShape* d = dynamic_cast<reactphysics3d::ConvexMeshShape*>(this->shape);
	switch (shapeType)
	{
	case reactphysics3d::CollisionShapeName::BOX:
		this->comPtr->destroyBoxShape(a);
		break;
	case reactphysics3d::CollisionShapeName::SPHERE:
		this->comPtr->destroySphereShape(b);
		break;
	case reactphysics3d::CollisionShapeName::CONVEX_MESH:
		this->comPtr->destroyConvexMeshShape(c);
		break;
	case reactphysics3d::CollisionShapeName::CAPSULE:
		this->comPtr->destroyConvexMeshShape(d);
		break;
	default:
		std::cout << "Error: Non-Applicable shape was returned";
		break;
	}
}

PhysicsComponent::PhysicsComponent()
	:comPtr(nullptr), worldPtr(nullptr), parent(nullptr),
	rigidBody(nullptr), shape(nullptr), collider(nullptr)
{
}

PhysicsComponent::~PhysicsComponent()
{
	//We do not need to deallocate anything since destroying the world will destroy everything,
	//however, if you want to destory the object but not the world we can use this
	if (this->rigidBody != nullptr && this->shape != nullptr && this->collider != nullptr) this->deallocate();
}

void PhysicsComponent::initiateComponent(reactphysics3d::PhysicsCommon* com, reactphysics3d::PhysicsWorld* world, const reactphysics3d::CollisionShapeName& shape, const DirectX::XMFLOAT3& scale)
{
	this->comPtr = com;
	this->worldPtr = world;

	//Creates basic physics component
	this->createRigidBody();
	this->setShape(shape, scale);
	this->addCollider();

	//Gives some values	
	this->setType();
	this->setMass();
	this->rigidBody->enableGravity(false);

}

void PhysicsComponent::setType(const reactphysics3d::BodyType& physicsType)
{
	this->rigidBody->setType(physicsType);
}

void PhysicsComponent::setShape(const reactphysics3d::CollisionShapeName& shapeType, const DirectX::XMFLOAT3& scale)
{
	reactphysics3d::Transform defaultTransform = reactphysics3d::Transform::identity();
	switch (shapeType)
	{
	case reactphysics3d::CollisionShapeName::BOX:
		this->shape = this->comPtr->createBoxShape(reactphysics3d::Vector3(3.f*scale.x, 3.f * scale.y, 3.f * scale.z));
		break;
	case reactphysics3d::CollisionShapeName::SPHERE:
		this->shape = this->comPtr->createSphereShape(scale.x);
		break;
	/*case reactphysics3d::CollisionShapeName::CONVEX_MESH:
		this->comPtr->destroyConvexMeshShape(c);
		break;
	case reactphysics3d::CollisionShapeName::CAPSULE:
		this->comPtr->destroyConvexMeshShape(d);
		break;*/
	default:
		std::cout << "Error: Non-Applicable shape was created";
		break;
	}
}

void PhysicsComponent::addCollider(const reactphysics3d::Transform& transform)
{
	this->collider = this->rigidBody->addCollider(this->shape, transform);
}

void PhysicsComponent::setMass(const float& mass)
{
	this->rigidBody->setMass(mass);
}

void PhysicsComponent::setScale(const float& scale)
{
	return;
}

void PhysicsComponent::setScale(const DirectX::XMFLOAT3& scale)
{
	reactphysics3d::CollisionShapeName shapeType = this->shape->getName();
	this->rigidBody->removeCollider(this->collider);
	switch (shapeType)
	{
	case reactphysics3d::CollisionShapeName::BOX:
		this->comPtr->destroyBoxShape(dynamic_cast<reactphysics3d::BoxShape *>(this->shape));
		this->shape = this->comPtr->createBoxShape(reactphysics3d::Vector3(scale.x, scale.y, scale.z));
		break;
	case reactphysics3d::CollisionShapeName::SPHERE:
		this->comPtr->destroySphereShape(dynamic_cast<reactphysics3d::SphereShape*>(this->shape));
		this->shape = this->comPtr->createSphereShape(scale.x);
		break;
	default:
		break;
	}
	this->collider = this->rigidBody->addCollider(this->shape, reactphysics3d::Transform::identity());
}

void PhysicsComponent::setScale(const DirectX::SimpleMath::Vector3& scale)
{
	this->setScale(DirectX::XMFLOAT3(scale.x, scale.y, scale.z));
}

void PhysicsComponent::setBoxShape(const DirectX::XMFLOAT3& dimensions)
{
	this->rigidBody->removeCollider(this->collider);
	reactphysics3d::CollisionShapeName shapeType = this->shape->getName();
	switch (shapeType)
	{
	case reactphysics3d::CollisionShapeName::SPHERE:
		this->comPtr->destroySphereShape(dynamic_cast<reactphysics3d::SphereShape*>(this->shape));
		break;
	case reactphysics3d::CollisionShapeName::CONVEX_MESH:
		this->comPtr->destroyConvexMeshShape(dynamic_cast<reactphysics3d::ConvexMeshShape *>(this->shape));
		break;
	default:
		break;
	}
	this->shape = this->comPtr->createBoxShape(reactphysics3d::Vector3(dimensions.x, dimensions.y, dimensions.z));
	this->collider = this->rigidBody->addCollider(this->shape, reactphysics3d::Transform::identity());
}

void PhysicsComponent::setSphereShape(const float& radius)
{
	this->rigidBody->removeCollider(this->collider);
	reactphysics3d::CollisionShapeName shapeType = this->shape->getName();
	switch (shapeType)
	{
	case reactphysics3d::CollisionShapeName::BOX:
		this->comPtr->destroyBoxShape(dynamic_cast<reactphysics3d::BoxShape*>(this->shape));
		break;
	case reactphysics3d::CollisionShapeName::CONVEX_MESH:
		this->comPtr->destroyConvexMeshShape(dynamic_cast<reactphysics3d::ConvexMeshShape*>(this->shape));
		break;
	default:
		break;
	}
	this->shape = this->comPtr->createSphereShape(radius);
	this->collider = this->rigidBody->addCollider(this->shape, reactphysics3d::Transform::identity());
}

void PhysicsComponent::setConvexMeshShape(const std::vector<Vertex>& vertices)
{
	//This function is yet to be implemented
	std::cout << "FUNCTION NOT FOUND, PLEASE USE OTHER FUNCTION\n";
	return;

	this->rigidBody->removeCollider(this->collider);
	reactphysics3d::CollisionShapeName shapeType = this->shape->getName();
	switch (shapeType)
	{
	case reactphysics3d::CollisionShapeName::BOX:
		this->comPtr->destroyBoxShape(dynamic_cast<reactphysics3d::BoxShape*>(this->shape));
		break;
	case reactphysics3d::CollisionShapeName::SPHERE:
		this->comPtr->destroySphereShape(dynamic_cast<reactphysics3d::SphereShape*>(this->shape));
		break;
	default:
		break;
	}
	this->collider = this->rigidBody->addCollider(this->shape, reactphysics3d::Transform::identity());
}

void PhysicsComponent::setLinearDampning(const float& factor)
{
	this->rigidBody->setLinearDamping(factor);
}

void PhysicsComponent::setAngularDampning(const float& factor)
{
	this->rigidBody->setAngularDamping(factor);
}

void PhysicsComponent::setIsAllowedToSleep(const bool& allowedToSleep)
{
	this->rigidBody->setIsAllowedToSleep(allowedToSleep);
}

void PhysicsComponent::setIsSleeping(const bool& sleep)
{
	this->rigidBody->setIsSleeping(sleep);
}

void PhysicsComponent::applyForceToCenter(const reactphysics3d::Vector3& force)
{
	//this->rigidBody->applyWorldForceAtCenterOfMass(force);
	this->rigidBody->applyWorldForceAtCenterOfMass(force);
}

void PhysicsComponent::applyForceToPoint(const reactphysics3d::Vector3& force, const reactphysics3d::Vector3& point)
{
	this->rigidBody->applyWorldForceAtLocalPosition(force, point);
	//this->rigidBody->applyLocalForceAtWorldPosition(force, point);
}

void PhysicsComponent::applyWorldTorque(const reactphysics3d::Vector3& force)
{
	this->rigidBody->applyWorldTorque(force);
}

void PhysicsComponent::applyLocalTorque(const reactphysics3d::Vector3& force)
{
	this->rigidBody->applyWorldTorque(force);
}

void PhysicsComponent::setPosition(const reactphysics3d::Vector3& position)
{
	this->rigidBody->setTransform(reactphysics3d::Transform(position, this->rigidBody->getTransform().getOrientation()));
}

void PhysicsComponent::setRotation(const reactphysics3d::Quaternion& rotation)
{
	this->rigidBody->setTransform(reactphysics3d::Transform(this->getPosition(), rotation));
}

void PhysicsComponent::setRotation(const DirectX::SimpleMath::Quaternion& rotation)
{
	this->rigidBody->setTransform(reactphysics3d::Transform(this->getPosition(), reactphysics3d::Quaternion(rotation.x, rotation.y, rotation.z, rotation.w)));
}

void PhysicsComponent::setTransform(const reactphysics3d::Transform& transform)
{
	this->rigidBody->setTransform(transform);
}

void PhysicsComponent::resetForce() const
{
	this->rigidBody->resetForce();
}

void PhysicsComponent::resetTorque() const
{
	this->rigidBody->resetTorque();
}

reactphysics3d::BodyType PhysicsComponent::getType() const
{
	return this->rigidBody->getType();
}

reactphysics3d::Vector3 PhysicsComponent::getPosition() const
{
	return this->rigidBody->getTransform().getPosition();
}

reactphysics3d::Quaternion PhysicsComponent::getRotation() const
{
	return this->rigidBody->getTransform().getOrientation();
}

reactphysics3d::CollisionShapeName PhysicsComponent::getTypeName() const
{
	return this->shape->getName();
}

reactphysics3d::Collider* PhysicsComponent::getCollider() const
{
	return this->collider;
}

reactphysics3d::RigidBody* PhysicsComponent::getRigidBody() const
{
	return this->rigidBody;
}

DirectX::SimpleMath::Vector3 PhysicsComponent::getPosV3() const
{
	reactphysics3d::Vector3 temp = this->getPosition();
	return {temp.x, temp.y, temp.z};
}

bool PhysicsComponent::raycast(const reactphysics3d::Ray& ray, reactphysics3d::RaycastInfo& rayInfo) const
{
	return this->collider->raycast(ray, rayInfo);
}

bool PhysicsComponent::testPointInside(const reactphysics3d::Vector3& point) const
{
	return this->collider->testPointInside(point);
}

bool PhysicsComponent::testBodiesOverlap(PhysicsComponent* other) const
{
	return this->collider->testAABBOverlap(other->getCollider()->getWorldAABB());
}

float PhysicsComponent::getMass() const
{
	return this->rigidBody->getMass();
}

float PhysicsComponent::getLinearDampning() const
{
	return this->rigidBody->getLinearDamping();
}

float PhysicsComponent::getAngularDampning() const
{
	return this->rigidBody->getAngularDamping();
}

void PhysicsComponent::setParent(GameObject* parent)
{
	this->parent = parent;
}

GameObject* PhysicsComponent::getParent() const
{
	return this->parent;
}

void PhysicsComponent::resetPhysicsObject()
{
	this->deallocate();	//After this we can re-define a new physics component
}
