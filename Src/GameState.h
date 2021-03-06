/**
* GameState.h
*/
#ifndef GAMESTATE_H_INCLUDED
#define GAMESTATE_H_INCLUDED

#include "Entity.h"

namespace GameState {

	///エンティティの衝突グループID
	enum EntityGroupId {
		EntityGroupId_Background,
		EntityGroupId_Player,
		EntityGroupId_PlayerShot,
		EntityGroupId_Enemy,
		EntityGroupId_EnemyShot,
		EntityGroupId_Others,
	};

	//タイトル
	class Title {
	public:
		explicit Title(Entity::Entity* p = nullptr) : pSpaceSphere(p) {}
		void operator()(double delta);

	private:
		Entity::Entity* pSpaceSphere = nullptr;
		float timer = 0;
	};

	//メインゲーム画面
	class MainGame {
	public:
		explicit MainGame(Entity::Entity* p) : pSpaceSphere(p) {}
		void operator()(double delta);

	private:
		Entity::Entity* pSpaceSphere = nullptr;
		Entity::Entity* pPlayer = nullptr;
		double interval = 0;

		double rebornTimer = 5;
	};

	class GameOver {
	public:
		GameOver() {}
		void operator()(double delta);

	private:
		double timer = 3;

	};
} // namespace GameState

#endif // !GAMESTATE_H_INCLUDED
