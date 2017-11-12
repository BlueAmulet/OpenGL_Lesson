//
//  Texture.cpp
//  OpenGL_Tutorial
//
//  Created by Tomoya Fujii on 2017/10/12.
//  Copyright c 2017�N TomoyaFujii. All rights reserved.
//

#include "Texture.h"
#include <iostream>
#include <vector>
#include <stdint.h>
#include <stdio.h>

/**
* �f�X�g���N�^
*/
Texture::~Texture() {
	if (texId) {
		glDeleteTextures(1, &texId);
	}
}

/**
* 2D�e�N�X�`�����쐬����
*
* @param   width   �e�N�X�`���̕��i�s�N�Z�����j
* @param   height  �e�N�X�`���̍����i�s�N�Z�����j
* @param   iformat �e�N�X�`���̃f�[�^�`��
* @param   format  �A�N�Z�X����v�f
* @param   data    �e�N�X�`���f�[�^�ւ̃|�C���^
*
* @return  �쐬�ɐ��������ꍇ�̓e�N�X�`���|�C���^��Ԃ�
*          ���s�����ꍇ�� nullptr ��Ԃ�
*/
TexturePtr Texture::Create(int width, int height, GLenum iformat, GLenum format, const void *data) {
	//�\���̂̓N���X�炵����  ��Texture�p�����Ă�
	struct Impl : Texture {};
	TexturePtr p = std::make_shared<Impl>();
	p->width = width;
	p->height = height;
	glGenTextures(1, &p->texId);
	glBindTexture(GL_TEXTURE_2D, p->texId);
	glTexImage2D(GL_TEXTURE_2D, 0, iformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	//�G���[�擾
	const GLenum result = glGetError();
	if (result != GL_NO_ERROR) {
		std::cerr << "ERROR �e�N�X�`���쐬�Ɏ��s: 0x" << std::hex << result << std::endl;
		return {};
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_2D, 0);
	return p;
}

/**
* �t�@�C������2D�e�N�X�`����ǂݍ���
*
* @param   filename    �t�@�C����
*
* @return  �쐬�ɐ��������ꍇ�̓e�N�X�`���|�C���^��Ԃ�
*          ���s�����ꍇ�� nullptr ��Ԃ�
*/
TexturePtr Texture::LoadFromFile(const char* filename) {
	TextureData data;
	TextureLoader::getInstance().loadFile(filename, data);

	if (data.type == TEXTURE_TYPE::NONE) {
		return {};
	}
	return Create(data.width, data.height, data.iformat, data.format, data.data.data());
}

/**
* �g��k�����̕ۊǕ��@�؂�ւ�
* �f�t�H���g
*/
void Texture::defaultFilter() {
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
}
/**
* �g��k�����̕ۊǕ��@�؂�ւ�
* �������茩����
*/
void Texture::dotFilter() {
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}
