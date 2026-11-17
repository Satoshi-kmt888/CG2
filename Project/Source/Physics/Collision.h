#pragma once

struct Vector3;
struct AABB;

namespace Collision {
	bool IsCollision(const AABB& aabb, const Vector3& point);

	bool IsCollision(const AABB& aabb1, const AABB& aabb2);
}
