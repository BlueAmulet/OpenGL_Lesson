//
//  main.cpp
//  OpenGL_Tutorial
//
//  Created by Tomoya Fujii on 2017/10/08.
//  Copyright c 2017îN TomoyaFujii. All rights reserved.
//

#include "Constants.h"
#include "GameEngine.h"
#include "../Res/Audio/SampleSound_acf.h"
#include "../Res/Audio/SampleCueSheet.h"
#include "Entity.h"
#include "GameState.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Controller.h"
#include <glm/gtc/matrix_transform.hpp>
#include <random>
#include <memory>



//==================================================================

float clamp(float val) {
	if (val > 1) {
		return 1;
	}
	if (val < 0) {
		return 0;
	}
	return val;
}

//ì]ínïœä∑
void transpose(glm::mat4& mat) {
	glm::vec4 vector;
	glm::mat4 result;
	for (int i = 0; i < 4; ++i) {
		vector = mat[i];
		for (int j = 0; j < 4; ++j) {
			result[j][i] = vector[j];
		}
	}
	mat = result;
}

//==================================================================

/**
* îöî≠ÇÃçXêV
*/
struct UpdateBlast {
	void operator()(Entity::Entity& entity, double delta) {
		timer += delta;
		if (timer >= 0.5) {
			entity.Destroy();
			return;
		}
		/**/
		const float variation = static_cast<float>(timer * 4);	//ïœâªó 
		entity.Scale(glm::vec3(static_cast<float>(1 + variation)));//èôÅXÇ…ägëÂÇ∑ÇÈ
																   //éûä‘åoâﬂÇ≈êFÇ∆ìßñæìxÇïœâªÇ≥ÇπÇÈ
		static const glm::vec4 color[] = {
			glm::vec4(1.0f,1.0f, 0.75f,1),
			glm::vec4(1.0f,0.5f,0.1f,1),
			glm::vec4(0.25f,0.1f,0.1f,0),
		};
		const glm::vec4 col0 = color[static_cast<int>(variation)];
		const glm::vec4 col1 = color[static_cast<int>(variation + 1)];
		const glm::vec4 newColor = glm::mix(col0, col1, std::fmod(variation, 1));
		entity.Color(newColor);
		//Yé≤âÒì]Ç≥ÇπÇÈ
		glm::vec3 euler = glm::eulerAngles(entity.Rotation());
		euler.y += glm::radians(60.0f) * static_cast<float>(delta);
		entity.Rotation(glm::quat(euler));
	}
	double timer = 0;
};

/**
* é©ã@ÇÃíeÇ∆ìGÇÃè’ìÀèàóù
*/
void PlayerShotAndEnemyCollisionHandler(Entity::Entity& lhs, Entity::Entity& rhs) {
	
	GameEngine& game = GameEngine::Instance();
	if (Entity::Entity* p = game.AddEntity(GameState::EntityGroupId_Others, rhs.Position(), "Blast", IMAGE_FILE_TOROID, UpdateBlast())) {
		const std::uniform_real_distribution<float> rotRange(0.0f, glm::pi<float>() * 2);
		p->Rotation(glm::quat(glm::vec3(0, rotRange(game.Rand()), 0)));
		game.UserVariable("score") += 100;
	}
	game.PlayAudio(1, CRI_SAMPLECUESHEET_BOMB);
	lhs.Destroy();
	rhs.Destroy();
}

/**
* é©ã@Ç∆ìGÇÃè’ìÀîªíË
*/
void PlayerAndEnemyCollisionHandler(Entity::Entity& lhs, Entity::Entity& rhs) {
	GameEngine& game = GameEngine::Instance();
	if (Entity::Entity* p = game.AddEntity(GameState::EntityGroupId_Others, lhs.Position(), "Blast", IMAGE_FILE_TOROID, UpdateBlast())) {
		const std::uniform_real_distribution<float> rotRange(0.0f, glm::pi<float>() * 2);
		p->Rotation(glm::quat(glm::vec3(0, rotRange(game.Rand()), 0)));
	}
	lhs.Destroy();
	rhs.Destroy();
	game.UpdateFunc(GameState::GameOver());
}

int main(int argc, const char * argv[]) {
	GameEngine& game = GameEngine::Instance();
	if (!game.Init(960, 600, "OpenGL_Practice")) {
		return 1;
	}
	if (!game.InitAudio(AUDIO_FILE_ACF_SAMPLE, AUDIO_FILE_ACB_SAMPLE, nullptr, CRI_SAMPLESOUND_ACF_DSPSETTING_DSPBUSSETTING_0)) {
		return 1;
	}

	game.LoadTextureFromFile(IMAGE_FILE_SPACESPHERE);
	game.LoadTextureFromFile(IMAGE_FILE_TOROID);
	game.LoadTextureFromFile(IMAGE_FILE_PLAYER);

	game.LoadMeshFromFile(MODEL_FILE_SPACESPHERE);
	game.LoadMeshFromFile(MODEL_FILE_TOROID);
	game.LoadMeshFromFile(MODEL_FILE_PLAYER);
	game.LoadMeshFromFile(MODEL_FILE_BLAST);
	if (!game.LoadFontFromFile(FONT_FILE_UNINEUE)) {
		return 1;
	}

	game.CollisionHandler(GameState::EntityGroupId_PlayerShot, GameState::EntityGroupId_Enemy, &PlayerShotAndEnemyCollisionHandler);
	game.CollisionHandler(GameState::EntityGroupId_Player, GameState::EntityGroupId_Enemy, &PlayerAndEnemyCollisionHandler);
	game.CollisionHandler(GameState::EntityGroupId_Player, GameState::EntityGroupId_EnemyShot, &PlayerAndEnemyCollisionHandler);
	game.UpdateFunc(GameState::Title());
	game.Run();

	return 0;
}
