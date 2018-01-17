//
//  OffscreenBuffer.cpp
//  OpenGL_Tutorial
//
//  Created by Tomoya Fujii on 2017/10/12.
//  Copyright c 2017�N TomoyaFujii. All rights reserved.
//

#include "OffscreenBuffer.h"

/**
* �I�t�X�N���[���o�b�t�@���쐬����
*
* @param   w	�I�t�X�N���[���o�b�t�@�̕��i�s�N�Z�����j
* @param   h	�I�t�X�N���[���o�b�t�@�̍����i�s�N�Z�����j
* @param   f	�e�N�X�`���`��
* 
* @return  �쐬�����I�t�X�N���[���o�b�t�@�ւ̃|�C���^
*/
OffscreenBufferPtr OffscreenBuffer::Create(int w, int h, GLenum f) {
	struct Impl : OffscreenBuffer {};
	OffscreenBufferPtr offscreen = std::make_shared<Impl>();
	if (!offscreen) {
		return offscreen;
	}

	offscreen->tex = Texture::Create(w, h, f, GL_RGBA, nullptr);
	if (!offscreen->tex) {
		return {};
	}

	//�f�v�X�o�b�t�@�쐬
	glGenRenderbuffers(1, &offscreen->depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, offscreen->depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	//�t���[���o�b�t�@�쐬
	glGenFramebuffers(1, &offscreen->frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, offscreen->frameBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, offscreen->depthBuffer);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, offscreen->tex->Id(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	offscreen->width = w;
	offscreen->height = h;

	return offscreen;
}

/**
* �f�X�g���N�^
*/
OffscreenBuffer::~OffscreenBuffer() {
	if (frameBuffer) {
		glDeleteFramebuffers(1, &frameBuffer);
	}
	if (depthBuffer) {
		glDeleteRenderbuffers(1, &depthBuffer);
	}
}

