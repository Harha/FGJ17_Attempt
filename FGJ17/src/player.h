#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"

class Player : public Entity
{
public:
	Player(
		Game * const game,
		const vec2 & position
	);
	virtual void update(Level & lvl, double t, double dt) override;
};

#endif // PLAYER_H
