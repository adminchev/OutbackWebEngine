#include <vector>

#include "player.hpp"

namespace Backend {
	void Player::Player(float posX, float posY, float posZ): x(posX), y(posY), z(posZ), verticalVelocity(0.0f), isOnGround(true);

	void Player::move(float dx, float dz) {
		;
	}

	void Player::jump(void) {
		;
	}
}
