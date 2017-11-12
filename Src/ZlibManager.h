//
//  ZlibManager.h
//  OpenGL_Practice
//
//  Created by Tomoya Fujii on 2017/10/20.
//  Copyright c 2017�N TomoyaFujii. All rights reserved.
//

#ifndef ZlibManager_h
#define ZlibManager_h

#include <vector>
#include <zlib.h>

class ZlibManager {
public:
	ZlibManager() = default;
	~ZlibManager();
	/**
	* �𓀊֐�
	*
	* @param compData  �𓀌��f�[�^�z��(uint8_t�z��)
	* @param decoData  �𓀐�f�[�^�z��(uint8_t�z��)
	*
	* @retval true     �𓀐���
	* @retval false    �𓀎��s
	*/
	bool Inflate(std::vector<uint8_t>& compData, std::vector<uint8_t>& decoData);
private:
	ZlibManager(const ZlibManager&) = delete;
	ZlibManager& operator= (const ZlibManager&) = delete;

	bool inflateInitialized = false;
	z_stream strm;
};

#endif /* ZlibManager_h */
