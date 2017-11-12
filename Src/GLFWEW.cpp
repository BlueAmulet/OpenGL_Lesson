//
//  GLFWEW.cpp
//  OpenGL_Tutorial
//
//  Created by Tomoya Fujii on 2017/10/08.
//  Copyright c 2017�N TomoyaFujii. All rights reserved.
//

#include "GLFWEW.h"
#include "Mouse.h"
#include "Keyboard.h"
#include "Controller.h"
#include <iostream>

///GLFW �� GLEW �����b�v���邽�߂̖��O���
namespace GLFWEW {
	/**
	* @desc    GLFW ����̃G���[�񍐂���������
	*
	* @param   error   �G���[�ԍ�
	* @param   desc    �G���[���e
	*/
	void ErrorCallback(int error, const char* desc) {
		std::cerr << "ERROR: " << desc << std::endl;
	}

	/**
	* �}�E�X�N���b�N�R�[���o�b�N
	*/
	void click(GLFWwindow* window, int button, int action, int mods) {
		Mouse::getInstance().click(window, button, action, mods);
	}
	/**
	* �}�E�X�ړ��R�[���o�b�N
	*/
	void move(GLFWwindow* window, double xpos, double ypos) {
		Mouse::getInstance().move(window, xpos, ypos);
	}
	/**
	* �}�E�X�X�N���[���R�[���o�b�N
	*/
	void scroll(GLFWwindow* window, double xoffset, double yoffset) {
		Mouse::getInstance().scroll(window, xoffset, yoffset);
	}

	/**
	* �L�[�{�[�h�ɕω������������̃R�[���o�b�N
	*/
	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Keyboard::getInstance().keyCallback(key, scancode, action, mods);
	}

	/**
	* �R���g���[���[�ڑ���ڑ����R�[���o�b�N
	*/
	void controllerCallbakck(int joy, int event) {
		ControllerManager::getInstance().joystickCallback(joy, event);
	}
	/**
	* @desc    �V���O���g���C���X�^���X���擾����
	*
	* @return Window �̃V���O���g���C���X�^���X
	*/
	Window& Window::Instance() {
		static Window instance;
		return instance;
	}

	/**
	* @desc    �R���X�g���N�^
	*/
	Window::Window() : isGLFWInitialize(false), isInitialized(false), window(nullptr) {
	}

	/**
	* @desc    �f�X�g���N�^
	*/
	Window::~Window() {
		if (isInitialized) {
			glfwTerminate();
		}
	}

	/**
	* @desc    GLFW/GLEW �̏�����
	*
	* @param   width   �E�B���h�E�̕`��͈͂̕�(�s�N�Z��)
	* @param   height  �E�B���h�E�̕`��͈͂̍���(�s�N�Z��)
	* @param   title   �E�B���h�E�̃^�C�g��( UTF-8 �� 0 �I�[������ )
	*
	* @retval  true    ����������
	* @retval  false   ���������s
	*/
	bool Window::Init(int width, int height, const char *title) {
		if (isInitialized) {
			std::cerr << "ERROR: GLFWEW �͊��ɏ���������Ă��܂��B" << std::endl;
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
			std::cerr << "ERROR: GLEW �̏������Ɏ��s���܂����B" << std::endl;
			return false;
		}

		glfwSwapInterval(1);

		
		//�}�E�X�R�[���o�b�N�ݒ�
		glfwSetMouseButtonCallback(window, click);
		glfwSetCursorPosCallback(window, move);
		glfwSetScrollCallback(window, scroll);

		//�L�[�{�[�h�R�[���o�b�N�ݒ�
		glfwSetKeyCallback(window, keyCallback);
		
		//�R���g���[���[�R�[���o�b�N
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
	* @desc    �E�B���h�E�����ׂ������ׂ�
	*
	* @retval  true    ����
	* @retval  false   ���Ȃ�
	*/
	bool Window::ShouldClose() const {
		return glfwWindowShouldClose(window) != 0;
	}

	/**
	* @desc    �t�����g�o�b�t�@�ƃo�b�N�o�b�t�@��؂�ւ���
	*/
	void Window::SwapBuffers() const {
		glfwPollEvents();
		glfwSwapBuffers(window);
	}

	/**
	* �Q�[���p�b�h�̏�Ԃ��擾����
	*
	* @return �Q�[���p�b�h�̏��
	*/
	const GamePad & Window::GetGamePad() const{
		return gamepad;
	}

	/**	
	* �W���C�X�e�B�b�N�̃A�i���O���͑��uID
	*
	* @note XBOX360�R���g���[���[�
	*/
	enum GLFWAXESID {
		GLFWAXESID_LeftX,	///���X�e�B�b�N��X��
		GLFWAXESID_LeftY,	///���X�e�B�b�N��X��
		GLFWAXESID_BackX,	///�A�i���O�g���K�[
		GLFWAXESID_RightX,	///�E�X�e�B�b�N��X��
		GLFWAXESID_RightY,	///�E�X�e�B�b�N��Y��
	};

	/**
	* �W���C�X�e�B�b�N�̃f�W�^�����͑��uID
	*
	* @note XBOX360�R���g���[���[�
	*/
	enum GLFWBUTTONID {
		GLFWBUTTONID_A,			///A�{�^��
		GLFWBUTTONID_B,			///B�{�^��
		GLFWBUTTONID_X,			///X�{�^��
		GLFWBUTTONID_Y,			///Y�{�^��
		GLFWBUTTONID_L,			///L�{�^��
		GLFWBUTTONID_R,			///R�{�^��
		GLFWBUTTONID_Back,		///Back�{�^��
		GLFWBUTTONID_Start,		///Start�{�^��
		GLFWBUTTONID_LThumb,	///LTumb�{�^��
		GLFWBUTTONID_RThumb,	///RTumb�{�^��
		GLFWBUTTONID_Up,		///Up�{�^��
		GLFWBUTTONID_Right,		///Right�{�^��
		GLFWBUTTONID_Down,		///Down�{�^��
		GLFWBUTTONID_Left,		///Left�{�^��

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
