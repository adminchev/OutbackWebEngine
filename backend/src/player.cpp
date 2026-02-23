#include <ctime>
#include <vector>

#include "player.hpp"

namespace Backend {

	constexpr float GRAVITY = 40.0f;
	constexpr float JUMP_VELO = 15.0f;

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
		if (isOnGround) {
			verticalVelocity = JUMP_VELO;
			isOnGround = false;
		}
	}

	float Player::getX(void) const { return this->x; }
	float Player::getY(void) const { return this->y; }
	float Player::getZ(void) const { return this->z; }
	
	void Player::updatePhysics(double timeDelta) {
		if (!isOnGround) {
			verticalVelocity -= GRAVITY * timeDelta;
			y += verticalVelocity * timeDelta;
		};
		if (y <= 1.0f) {
			isOnGround = true;
			y = 1.0f;
		}
	}
}
