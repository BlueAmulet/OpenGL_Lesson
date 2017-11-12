//
//  ZlibManager.cpp
//  OpenGL_Practice
//
//  Created by Tomoya Fujii on 2017/10/20.
//  Copyright c 2017�N TomoyaFujii. All rights reserved.
//

#include "ZlibManager.h"
#include <iostream>

ZlibManager::~ZlibManager() {
	if (inflateInitialized) {
		inflateEnd(&strm);
	}
}
/**
* �𓀊֐�
*
* @param compData  �𓀌��f�[�^�z��(uint8_t�z��)
* @param decoData  �𓀐�f�[�^�z��(uint8_t�z��)
*/
bool ZlibManager::Inflate(std::vector<uint8_t>& compData, std::vector<uint8_t>& decoData) {
	if (inflateInitialized == true) {
		inflateEnd(&strm);
	}
	else {
		inflateInitialized = true;
	}
	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	strm.next_in = Z_NULL;
	strm.avail_in = 0;
	int ret = inflateInit(&strm);
	if (ret != Z_OK) {
		std::cerr << "inflate �̏������Ɏ��s" << std::endl;
		return false;
	}
	ret = Z_OK;

	int outCount = 0;
	strm.next_in = (Bytef*)compData.data();
	strm.avail_in = (unsigned int)compData.size();
	strm.next_out = (Bytef*)decoData.data();
	strm.avail_out = (unsigned int)decoData.size();

	while (ret != Z_STREAM_END) {
		if (strm.avail_in == 0) {
		}
		ret = inflate(&strm, Z_NO_FLUSH);
		if (ret == Z_STREAM_END) break;
		if (ret != Z_OK) {
			std::cout << "ERROR: inflate" << std::endl;
			return false;
		}
		if (strm.avail_out == 0) {
			std::cout << "�o�͐�o�b�t�@�̃T�C�Y������܂���B�g�����܂�" << std::endl;
			decoData.resize(decoData.size() + 32768 * (1 + ++outCount));
			strm.avail_out = 32768;
		}
	}

	decoData.shrink_to_fit();

	return true;
}
