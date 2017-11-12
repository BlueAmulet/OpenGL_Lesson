/**
* @file Audio.cpp
*/
#include "Audio.h"
#include <cri_adx2le.h>
#include <iostream>
#include <cstdint>

namespace Audio {
	CriAtomExVoicePoolHn voicePool;
	CriAtomDbasId dbas = CRIATOMDBAS_ILLEGAL_ID;
	CriAtomExAcbHn acb;
	CriAtomExPlayerHn player[playerMax];

	/**
	* オーディオシステム用エラーコールバック
	*/
	void ErrorCallback(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray) {
		const CriChar8* err = criErr_ConvertIdToMessage(errid, p1, p2);
		std::cerr << err << std::endl;
	}

	///オーディオシステム用アロケータ
	void* Allocate(void* obj, CriUint32 size) { return new uint8_t[size]; }
	///オーディオシステム用でアロケータ
	void Deallocate(void* obj, void* ptr) { delete[] static_cast<uint8_t*>(ptr); }

	/**
	* オーディオシステムを初期化する
	*
	* @param acfPath		ACFファイルのパス
	* @param acbPath		ACBファイルのパス
	* @param awbPath		AWBファイルのパス
	*
	* @retval true	初期化成功
	* @retval false 初期化失敗
	*/
	bool Initialize(const char* acfPath, const char* acbPath, const char* awbPath, const char* dspBusName) {
		//関数の設定
		criErr_SetCallback(ErrorCallback);
		criAtomEx_SetUserAllocator(Allocate, Deallocate, nullptr);

		CriFsConfig fsConfig;
		CriAtomExConfig_WASAPI libConfig;
		criFs_SetDefaultConfig(&fsConfig);
		criAtomEx_SetDefaultConfig_WASAPI(&libConfig);
		//並列に読み込めるオブジェクトの総数
		fsConfig.num_loaders = 64;
		//ファイルパスの最大長
		fsConfig.max_path = 1024;
		libConfig.atom_ex.fs_config = &fsConfig;
		//同時制御可能な音声の最大数
		libConfig.atom_ex.max_virtual_voices = 64;
		criAtomEx_Initialize_WASAPI(&libConfig, nullptr, 0);

		//ストリーミング再生を制御するD-BASオブジェクトを作成
		dbas = criAtomDbas_Create(nullptr, nullptr, 0);
		//全体設定を記したACFファイルをライブラリに登録
		if (criAtomEx_RegisterAcfFile(nullptr, acfPath, nullptr, 0) == CRI_FALSE) {
			return false;
		}
		//D-BASオブジェクトとACFファイルを関連付ける
		criAtomEx_AttachDspBusSetting(dspBusName, nullptr, 0);


		CriAtomExStandardVoicePoolConfig config;
		criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&config);
		//同時発音可能な音声の数
		config.num_voices = 16;
		//再生可能な音声の種類の設定
		config.player_config.streaming_flag = CRI_TRUE;
		//最大サンプリングレート
		config.player_config.max_sampling_rate = 48000 * 2;
		voicePool = criAtomExVoicePool_AllocateStandardVoicePool(&config, nullptr, 0);
		//波形データストリーミング再生用データ読み込み
		acb = criAtomExAcb_LoadAcbFile(nullptr, acbPath, nullptr, awbPath, nullptr, 0);
		for (auto& e : player) {
			e = criAtomExPlayer_Create(nullptr, nullptr, 0);
		}
		return true;
	}

	/**
	* オーディオシステムを破棄する
	*/
	void Destroy() {
		for (auto& e : player) {
			if (e) {
				criAtomExPlayer_Destroy(e);
				e = nullptr;
			}
		}
		if (acb) {
			criAtomExAcb_Release(acb);
			acb = nullptr;
		}
		if (voicePool) {
			criAtomExVoicePool_Free(voicePool);
			voicePool = nullptr;
		}
		criAtomEx_UnregisterAcf();
		if (dbas != CRIATOMDBAS_ILLEGAL_ID) {
			criAtomDbas_Destroy(dbas);
			dbas = CRIATOMDBAS_ILLEGAL_ID;
		}
		criAtomEx_Finalize_WASAPI();
	}

	/**
	* オーディオシステムの状態を更新する
	*/
	void Update() {
		criAtomEx_ExecuteMain();
	}

	/**
	* 音声を再生する
	*
	* @param playerId	再生に使用するプレイヤーのID
	* @param cueId		再生するキューのID
	*/
	void Play(int playerId, int cueId) {
		if (playerId < 0 || playerId >= playerMax) {
			std::cerr << "ERROR : 再生可能なプレイヤーIDではありません" << std::endl;
			return;
		}
		criAtomExPlayer_SetCueId(player[playerId], acb, cueId);
		criAtomExPlayer_Start(player[playerId]);
	}

	/**
	* 音声を停止する
	*
	* @param playerId	再生を停止するプレイヤーのID
	*/
	void Stop(int playerId) {
		criAtomExPlayer_Stop(player[playerId]);
	}
} // namespace Audio