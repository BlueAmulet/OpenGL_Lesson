/**
* @file MainGameState.cpp
*/
#include "GameState.h"
#include "GameEngine.h"
#include "../Res/Audio/SampleCueSheet.h"
#include "Constants.h"
#include <algorithm>

namespace GameState {
	///衝突形状リスト
	static const Entity::CollisionData collisionDataList[] = {
		{},
		{ glm::vec3(-1.0f,-1.0f,-1.0f), glm::vec3(1.0f,1.0f,1.0f) },
		{ glm::vec3(-0.5f,-0.5f,-0.5f), glm::vec3(0.5f,0.5f,0.5f) },
		{ glm::vec3(-1.0f,-1.0f,-1.0f), glm::vec3(1.0f,1.0f,1.0f) },
		{ glm::vec3(-0.25f,-0.25f,-0.25f),glm::vec3(0.25,0.25,0.25) },
	};


	/**
	* 敵の弾の更新処理
	*/
	struct UpdateEnemyShot {
		void operator()(Entity::Entity& entity, double delta) {
			//範囲外に出たら削除する
			const glm::vec3 pos = entity.Position();
			if (std::abs(pos.x) > 40.0f || pos.z < -4 || pos.z > 40) {
				entity.Destroy();
				return;
			}
		}
	};

	struct UpdateToroid {
		void operator()(Entity::Entity& entity, double delta) {
			//範囲外に出たら削除する
			const glm::vec3 pos = entity.Position();
			if (std::abs(pos.x) > 40.0f || std::abs(pos.y) > 40.0f) {
				GameEngine::Instance().RemoveEntity(&entity);
				return;
			}

			//円盤を回転させる
			float rot = glm::angle(entity.Rotation());
			rot += glm::radians(15.0f) * static_cast<float>(delta);
			if (rot > glm::pi<float>() * 2.0f) {
				rot -= glm::pi<float>() * 2.0f;
			}
			entity.Rotation(glm::angleAxis(rot, glm::vec3(0, 1, 0)));

			counter -= delta;
			if (counter <= 0) {
				if (Entity::Entity* p = GameEngine::Instance().AddEntity(EntityGroupId_EnemyShot, entity.Position(), "Spario", IMAGE_FILE_TOROID, UpdateEnemyShot())) {
					p->Velocity(glm::vec3(0, 0, -20));
					p->Collision(collisionDataList[EntityGroupId_EnemyShot]);
					counter = 5;
				}
			}
		}

		double counter = 5;
	};

	/**
	* 自機の弾の更新処理
	*/
	struct UpdatePlayerShot {
		void operator()(Entity::Entity& entity, double delta) {
			//範囲外に出たら削除する
			const glm::vec3 pos = entity.Position();
			if (std::abs(pos.x) > 40.0f || pos.z < -4 || pos.z > 40) {
				entity.Destroy();
				return;
			}
		}
	};

	

	/**
	* 自機の更新
	*/
	struct UpdatePlayer {
		void operator()(Entity::Entity& entity, double delta) {
			GameEngine& game = GameEngine::Instance();
			const GamePad gamepad = game.GetGamePad();

			glm::vec3 vec;
			float rotZ = 0;
			if (gamepad.buttons & GamePad::DPAD_LEFT) {
				vec.x = 1;
				rotZ = -glm::radians(30.0f);
			}
			else if (gamepad.buttons & GamePad::DPAD_RIGHT) {
				vec.x = -1;
				rotZ = glm::radians(30.0f);
			}
			if (gamepad.buttons & GamePad::DPAD_UP) {
				vec.z = 1;
			}
			else if (gamepad.buttons & GamePad::DPAD_DOWN) {
				vec.z = -1;
			}
			if (vec.x || vec.z) {
				vec = glm::normalize(vec) * 2.0f;
			}
			entity.Velocity(vec);
			entity.Rotation(glm::vec3(0, 0, rotZ));
			glm::vec3 pos = entity.Position();
			pos = glm::min(glm::vec3(11, 100, 20), glm::max(pos, glm::vec3(-11, -100, 1)));
			entity.Position(pos);

			if (gamepad.buttons & GamePad::A) {
				shotInterval -= delta;
				if (shotInterval <= 0) {
					game.PlayAudio(0, CRI_SAMPLECUESHEET_PLAYERSHOT);
					glm::vec3 pos = entity.Position();
					pos.x -= 0.3f;
					for (int i = 0; i < 2; ++i) {
						if (Entity::Entity* p = game.AddEntity(EntityGroupId_PlayerShot, pos, "NormalShot", IMAGE_FILE_PLAYER, UpdatePlayerShot())) {
							p->Velocity(glm::vec3(0, 0, 80));
							p->Collision(collisionDataList[EntityGroupId_PlayerShot]);
						}
						pos.x += 0.6f;
					}
					shotInterval = 0.25;
				}
			}
			else {
				shotInterval = 0;
			}
		}

	private:
		double shotInterval = 0;
	};



	///メイン画面の更新処理
	void MainGame::operator()(double delta) {
		GameEngine& game = GameEngine::Instance();

		if (!pPlayer) {
			pPlayer = game.AddEntity(EntityGroupId_Player, glm::vec3(0, 0, 2), "Aircraft", "Res/Player/Player.bmp", UpdatePlayer());
			pPlayer->Collision(collisionDataList[EntityGroupId_Player]);
		}
		if (pPlayer) {
			if (pPlayer->GroupId() != EntityGroupId_Player) {
				rebornTimer -= delta;
				if (rebornTimer <= 0) {
					pPlayer = game.AddEntity(EntityGroupId_Player, glm::vec3(0, 0, 2), "Aircraft", "Res/Player/Player.bmp", UpdatePlayer());
					rebornTimer = 5;
				}
			}
		}
		game.Camera({ glm::vec3(0, 20, -8), glm::vec3(0,0,12), glm::vec3(0,0,1) });
		game.AmbientLight(glm::vec4(0.05f, 0.1f, 0.2f, 1));
		game.Light(0, { glm::vec4(40, 100, 10, 1), glm::vec4(12000, 12000, 12000, 1) });
		std::uniform_int_distribution<> distributerX(-12, 12);
		std::uniform_int_distribution<> distributerZ(40, 44);
		interval -= delta;
		if (interval <= 0) {
			const std::uniform_real_distribution<> rndInterval(8.0, 16.0);
			const std::uniform_int_distribution<> rndAddingCount(1, 5);
			for (int i = rndAddingCount(game.Rand()); i > 0; --i) {
				const glm::vec3 pos(distributerX(game.Rand()), 0, distributerZ(game.Rand()));
				if (Entity::Entity* p = game.AddEntity(EntityGroupId_Enemy, pos, "Toroid", IMAGE_FILE_TOROID, UpdateToroid())) {
					p->Velocity(glm::vec3(pos.x < 0 ? 1.0f : -1.0f, 0, -4));
					p->Collision(collisionDataList[EntityGroupId_Enemy]);
				}
			}
			interval = rndInterval(game.Rand());
		}
		char str[16];
		snprintf(str, 16, "%08.0f", game.UserVariable("score"));
		game.AddString(glm::vec2(-0.8f, 0.9f), str);
	}
} // namespace GameState