//
//  OffscreenBuffer.h
//  OpenGL_Tutorial
//
//  Created by Tomoya Fujii on 2017/10/12.
//  Copyright c 2017�N TomoyaFujii. All rights reserved.
//

#ifndef OffscreenBuffer_h
#define OffscreenBuffer_h
#include "Texture.h"
#include <GL/glew.h>
#include <memory>

class OffscreenBuffer;
typedef std::shared_ptr<OffscreenBuffer> OffscreenBufferPtr;

/**
* �I�t�X�N���[���o�b�t�@
*/
class OffscreenBuffer {
public:
	static OffscreenBufferPtr Create(int w, int h);
	GLuint GetFrameBuffer() const { return frameBuffer; };         ///�t���[���o�b�t�@���擾����
	GLuint GetTexture() const { return tex->Id(); }               ///�t���[���o�b�t�@�pt�N�X�����擾����

private:
	OffscreenBuffer() = default;
	OffscreenBuffer(const OffscreenBuffer&) = delete;
	OffscreenBuffer& operator= (const OffscreenBuffer&) = delete;
	~OffscreenBuffer();

private:
	TexturePtr tex;            ///�t���[���o�b�t�@�p�e�N�X�`��
	GLuint depthBuffer = 0;     ///�k�x�o�b�t�@�I�u�W�F�N�g
	GLuint frameBuffer = 0;     ///�t���[���o�b�t�@�I�u�W�F�N�g
};

#endif /* OffscreenBuffer_h */
