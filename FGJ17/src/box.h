#ifndef BOX_H
#define BOX_H

#include "entity.h"

class Box : public Entity
{
public:
	Box(
		Game * const game,
		const vec2 & spawn,
		EntityProperties properties
	);
	virtual void update(Level & lvl, double t, double dt) override;
};

#endif // BOX_H
