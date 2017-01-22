#ifndef AABB_H
#define AABB_H

#include "vec2.h"

class Display;

class AABB
{
public:
	AABB(vec2 minP = vec2(-1, -1), vec2 maxP = vec2(1, 1));
	void render(Display * const display);
	bool collidesXRight(const AABB & other) const;
	bool collidesXLeft(const AABB & other) const;
	bool collidesYUp(const AABB & other) const;
	bool collidesYDown(const AABB & other) const;
	bool collidesY(const AABB & other) const;
	bool collidesX(const AABB & other) const;
	bool collides(const AABB & other) const;
	void setMinP(const vec2 & minP);
	void setMaxP(const vec2 & maxP);
	AABB operator+(const vec2 & other) const;
	AABB operator-(const vec2 & other) const;
	AABB operator*(const float f) const;
	AABB operator/(const float f) const;
	vec2 getCenterP() const;
	vec2 getMinP() const;
	vec2 getMaxP() const;
private:
	vec2 m_minP;
	vec2 m_maxP;
};

#endif // AABB_H
