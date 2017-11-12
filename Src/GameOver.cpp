#include "GameState.h"
#include "GameEngine.h"

namespace GameState {
	
	void GameOver::operator()(double delta) {
		GameEngine& game = GameEngine::Instance();
		game.FontScale(glm::vec2(2));
		game.AddString(glm::vec2(-0.25f, 0.125f), "GameOver...");

		timer -= delta;
		if (timer < 0) {
			game.UpdateFunc(Title());
		}
	}
} // namespace GameState