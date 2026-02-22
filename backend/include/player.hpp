#ifndef BACKEND_PLAYER
#define BACKEND_PLAYER

namespace Backend {
	class Player {
		private:
			float x;
			float y;
			float z;
			float verticalVelocity;
			bool isOnGround;
		
		public:
			Player(float posX = 0.0f, float posY = 1.0f, float posZ = 0.0f);
			void move(float dx, float dz);
			void jump(void);
			float getX(void) const;
			float getY(void) const;
			float getZ(void) const;
	};
}
#endif // !BACKEND_PLAYER
