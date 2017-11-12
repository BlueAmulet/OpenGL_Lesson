//
//  GLFWEW.cpp
//  OpenGL_Tutorial
//
//  Created by Tomoya Fujii on 2017/10/08.
//  Copyright c 2017年 TomoyaFujii. All rights reserved.
//

#include "GLFWEW.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Controller.h"
#include <iostream>

///GLFW と GLEW をラップするための名前空間
namespace GLFWEW {
	/**
	* @desc    GLFW からのエラー報告を処理する
	*
	* @param   error   エラー番号
	* @param   desc    エラー内容
	*/
	void ErrorCallback(int error, const char* desc) {
		std::cerr << "ERROR: " << desc << std::endl;
	}

	/**
	* マウスクリックコールバック
	*/
	void click(GLFWwindow* window, int button, int action, int mods) {
		Mouse::getInstance().click(window, button, action, mods);
	}
	/**
	* マウス移動コールバック
	*/
	void move(GLFWwindow* window, double xpos, double ypos) {
		Mouse::getInstance().move(window, xpos, ypos);
	}
	/**
	* マウススクロールコールバック
	*/
	void scroll(GLFWwindow* window, double xoffset, double yoffset) {
		Mouse::getInstance().scroll(window, xoffset, yoffset);
	}

	/**
	* キーボードに変化があった時のコールバック
	*/
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Keyboard::getInstance().keyCallback(key, scancode, action, mods);
	}

	/**
	* コントローラー接続非接続時コールバック
	*/
	void controllerCallbakck(int joy, int event) {
		ControllerManager::getInstance().joystickCallback(joy, event);
	}
	/**
	* @desc    シングルトンインスタンスを取得する
	*
	* @return Window のシングルトンインスタンス
	*/
	Window& Window::Instance() {
		static Window instance;
		return instance;
	}

	/**
	* @desc    コンストラクタ
	*/
	Window::Window() : isGLFWInitialize(false), isInitialized(false), window(nullptr) {
	}

	/**
	* @desc    デストラクタ
	*/
	Window::~Window() {
		if (isInitialized) {
			glfwTerminate();
		}
	}

	/**
	* @desc    GLFW/GLEW の初期化
	*
	* @param   width   ウィンドウの描画範囲の幅(ピクセル)
	* @param   height  ウィンドウの描画範囲の高さ(ピクセル)
	* @param   title   ウィンドウのタイトル( UTF-8 の 0 終端文字列 )
	*
	* @retval  true    初期化成功
	* @retval  false   初期化失敗
	*/
	bool Window::Init(int width, int height, const char *title) {
		if (isInitialized) {
			std::cerr << "ERROR: GLFWEW は既に初期化されています。" << std::endl;
			return false;
		}

		if (!isGLFWInitialize) {
			glfwSetErrorCallback(ErrorCallback);
			if (glfwInit() != GL_TRUE) {
				return false;
			}
			isGLFWInitialize = true;
		}



		if (!window) {
			window = glfwCreateWindow(width, height, title, nullptr, nullptr);
			if (!window) {
				return false;
			}
			glfwMakeContextCurrent(window);
		}

		if (glewInit() != GLEW_OK) {
			std::cerr << "ERROR: GLEW の初期化に失敗しました。" << std::endl;
			return false;
		}

		glfwSwapInterval(1);

		
		//マウスコールバック設定
		glfwSetMouseButtonCallback(window, click);
		glfwSetCursorPosCallback(window, move);
		glfwSetScrollCallback(window, scroll);

		//キーボードコールバック設定
		glfwSetKeyCallback(window, keyCallback);
		
		//コントローラーコールバック
		//glfwSetJoystickCallback(controllerCallbakck);

		const GLubyte* renderer = glGetString(GL_RENDERER);
		std::cout << "Renderer:\t" << renderer << std::endl;
		const GLubyte* version = glGetString(GL_VERSION);
		std::cout << "Version:\t" << version << std::endl;
		const GLubyte* SLversion = glGetString(GL_SHADING_LANGUAGE_VERSION);
		std::cout << "SLVersion:\t" << SLversion << std::endl;
		isInitialized = true;

		return true;
	}

	/**
	* @desc    ウィンドウを閉じるべきか調べる
	*
	* @retval  true    閉じる
	* @retval  false   閉じない
	*/
	bool Window::ShouldClose() const {
		return glfwWindowShouldClose(window) != 0;
	}

	/**
	* @desc    フロントバッファとバックバッファを切り替える
	*/
	void Window::SwapBuffers() const {
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	/**
	* ゲームパッドの状態を取得する
	*
	* @return ゲームパッドの状態
	*/
	const GamePad & Window::GetGamePad() const{
		return gamepad;
	}

	/**	
	* ジョイスティックのアナログ入力装置ID
	*
	* @note XBOX360コントローラー基準
	*/
	enum GLFWAXESID {
		GLFWAXESID_LeftX,	///左スティックのX軸
		GLFWAXESID_LeftY,	///左スティックのX軸
		GLFWAXESID_BackX,	///アナログトリガー
		GLFWAXESID_RightX,	///右スティックのX軸
		GLFWAXESID_RightY,	///右スティックのY軸
	};

	/**
	* ジョイスティックのデジタル入力装置ID
	*
	* @note XBOX360コントローラー基準
	*/
	enum GLFWBUTTONID {
		GLFWBUTTONID_A,			///Aボタン
		GLFWBUTTONID_B,			///Bボタン
		GLFWBUTTONID_X,			///Xボタン
		GLFWBUTTONID_Y,			///Yボタン
		GLFWBUTTONID_L,			///Lボタン
		GLFWBUTTONID_R,			///Rボタン
		GLFWBUTTONID_Back,		///Backボタン
		GLFWBUTTONID_Start,		///Startボタン
		GLFWBUTTONID_LThumb,	///LTumbボタン
		GLFWBUTTONID_RThumb,	///RTumbボタン
		GLFWBUTTONID_Up,		///Upボタン
		GLFWBUTTONID_Right,		///Rightボタン
		GLFWBUTTONID_Down,		///Downボタン
		GLFWBUTTONID_Left,		///Leftボタン

	};

	void Window::UpdateGamePad(){
		const uint32_t prevButtons = gamepad.buttons;
		int axesCount, buttonCount;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		const uint8_t* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
		if (axes && buttons && axesCount >= 2 && buttonCount >= 8) {
			gamepad.buttons &= ~(GamePad::DPAD_UP | GamePad::DPAD_DOWN | GamePad::DPAD_LEFT | GamePad::DPAD_RIGHT);
			static const float threshould = 0.3f;
			if (axes[GLFWAXESID_LeftY] >= threshould) {
				gamepad.buttons |= GamePad::DPAD_UP;
			}
			else if (axes[GLFWAXESID_LeftY] <= threshould) {
				gamepad.buttons |= GamePad::DPAD_DOWN;
			}
			if (axes[GLFWAXESID_LeftX] >= threshould) {
				gamepad.buttons |= GamePad::DPAD_LEFT;
			}
			else if (axes[GLFWAXESID_LeftX] <= threshould) {
				gamepad.buttons |= GamePad::DPAD_RIGHT;
			}
			static const struct {
				int glfwCode;
				uint32_t gamepadCode;
			} keyMap[] = {
				{ GLFWBUTTONID_A, GamePad::A },
				{ GLFWBUTTONID_B, GamePad::B },
				{ GLFWBUTTONID_X, GamePad::X },
				{ GLFWBUTTONID_Y, GamePad::Y },
				{ GLFWBUTTONID_Start, GamePad::START },
			};
			for (const auto& e : keyMap) {
				if (buttons[e.glfwCode] == GLFW_PRESS) {
					gamepad.buttons |= e.gamepadCode;
				}
				else if(buttons[e.glfwCode] == GLFW_RELEASE){
					gamepad.buttons &= ~e.gamepadCode;
				}
			}
		}
		else {
			static const struct {
				int glfwCode;
				uint32_t gamepadCode;
			} keyMap[] = {
				{ GLFW_KEY_UP,		GamePad::DPAD_UP },
				{ GLFW_KEY_DOWN,	GamePad::DPAD_DOWN },
				{ GLFW_KEY_LEFT,	GamePad::DPAD_LEFT },
				{ GLFW_KEY_RIGHT,	GamePad::DPAD_RIGHT },
				{ GLFW_KEY_ENTER,	GamePad::START },
				{ GLFW_KEY_A, GamePad::A },
				{ GLFW_KEY_S, GamePad::B },
				{ GLFW_KEY_Z, GamePad::X },
				{ GLFW_KEY_X, GamePad::Y },
			};
			for (const auto& e : keyMap) {
				const int key = glfwGetKey(window, e.glfwCode);
				if (key == GLFW_PRESS) {
					gamepad.buttons |= e.gamepadCode;
				}
				else if (key == GLFW_RELEASE) {
					gamepad.buttons &= ~e.gamepadCode;
				}
			}
		}
		gamepad.buttonDown = gamepad.buttons & ~prevButtons;
	}
}// namespace GLFWEW
