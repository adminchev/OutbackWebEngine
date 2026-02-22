#include <vector>

#include "player.hpp"

namespace Backend {
	Player::Player(float posX, float posY, float posZ): x(posX), y(posY), z(posZ), verticalVelocity(0.0f), isOnGround(true){
		;
	};

	void Player::move(float dx, float dz) {
		this->x += dx;
		this->z += dz;

		if (this->x > 10.0f) this->x = 10.0f;
		if (this->x < -10.0f) this->x = -10.0f;
		if (this->z > 10.0f) this->z = 10.0f;
		if (this->z < -10.0f) this->z = -10.0f;
	}

	void Player::jump(void) {
		;
	}

	float Player::getX(void) const { return this->x; }
	float Player::getY(void) const { return this->y; }
	float Player::getZ(void) const { return this->z; }
	
}
