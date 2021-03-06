/****************************************************************************
 *
 * CRI Middleware SDK
 *
 * Copyright (c) 2009-2017 CRI Middleware Co., Ltd.
 *
 * Library  : CRI Atom
 * Module   : Library User's Header
 * File     : cri_le_atom.h
 *
 ****************************************************************************/
/*!
 *	\file		cri_le_atom.h
 */

/* 多重定義防止					*/
/* Prevention of redefinition	*/
#ifndef CRI_INCL_CRI_ATOM_H
#define CRI_INCL_CRI_ATOM_H

/***************************************************************************
 *      インクルードファイル
 *      Include files
 ***************************************************************************/
#include <cri_le_xpt.h>
#include <cri_le_file_system.h>

/***************************************************************************
 *      定数マクロ
 *      Macro Constants
 ***************************************************************************/
/* バージョン情報 */
/* Version Number */
#define CRI_ATOM_VERSION				(0x02143503)
#define CRI_ATOM_VER_NUM				"2.14.35.03"
#define CRI_ATOM_VER_NAME				"CRI Atom(LE)"

/*==========================================================================
 *      CRI Atom API
 *=========================================================================*/
/* Mac OSX用のデフォルト設定 */
#define CRIATOM_DEFAULT_OUTPUT_CHANNELS				(6)
#define CRIATOM_DEFAULT_OUTPUT_SAMPLING_RATE		(48000)
#define CRIATOM_DEFAULT_INPUT_MAX_CHANNELS			(2)
#define CRIATOM_DEFAULT_INPUT_MAX_SAMPLING_RATE		(48000)
#define CRIATOM_DEFAULT_DECODE_LATENCY				(0)
#define CRIATOM_DEFAULT_DBAS_MAX_STREAMS			(8)
#define CRIATOM_SOUND_RENDERER_DEFAULT				(CRIATOM_SOUND_RENDERER_ASR)

/*==========================================================================
 *      CRI Atom Player API
 *=========================================================================*/
/*JP
 * \brief ループ回数制御用
 * \ingroup ATOMLIB_PLAYER
 * \par 説明:
 */
#define CRIATOMPLAYER_FORCE_LOOP			(-3)		/*JP<強制ループ再生		*/

/*==========================================================================
 *      CRI Atom AWB API
 *=========================================================================*/

/*==========================================================================
 *      CRI Atom D-BAS API
 *=========================================================================*/
/*JP
 * \brief Atom D-BAS ID
 * \ingroup ATOMLIB_DBAS
 * \par 説明:
 * ::criAtomDbas_Create 関数に失敗した際に返る値です。 <br>
 * \sa criAtomDbas_Create, criAtomDbas_Destroy
 */
#define CRIATOMDBAS_ILLEGAL_ID	(-1)

/*JP
 * \brief D-BAS作成用のデフォルト初期化最大ビットレート値
 * \ingroup ATOMLIB_AWB
 * \par 説明:
 * D-BAS作成用のデフォルト初期化パラメータ値定義です。<br>
 * 各プラットフォームで定義しているデフォルトの最大ストリーム本数分、
 * ステレオADXデータをストリーミング再生する前提で計算しています。 <br>
 * \sa criAtomDbas_Create, criAtomDbas_CalculateWorkSize, criAtom_CalculateAdxBitrate
 */
#define CRIATOM_DEFAULT_DBAS_MAX_BPS \
	(criAtom_CalculateAdxBitrate(CRIATOM_DEFAULT_INPUT_MAX_CHANNELS,\
								 CRIATOM_DEFAULT_INPUT_MAX_SAMPLING_RATE)\
								 * CRIATOM_DEFAULT_DBAS_MAX_STREAMS)

/*JP
 * \brief D-BAS作成用のデフォルトのバッファ保証数
 * \ingroup ATOMLIB_DBAS
 * \par 説明:
 * D-BAS作成用のデフォルトのバッファ保証数です。
 * ループ先頭、およびループ終端で極端にデータが少なくなる音声データであっても
 * 途切れないようにストリーミング再生可能な安全な値を設定しています。
 * \sa criAtomDbas_Create, criAtomDbas_CalculateWorkSize
 */
#define CRIATOMDBAS_DEFAULT_NUM_SECUREMENT_BUFFERS	(4)

/*JP
 * \brief D-BAS作成時に指定可能なバッファ保証数の下限値
 * \ingroup ATOMLIB_DBAS
 * \par 説明:
 * D-BAS作成時に指定可能なバッファ保証数の下限値です。
 * 途切れずにストリーミング再生するためには、
 * 最低でも2バッファ（ダブルバッファリング）必要なため、
 * 2未満の値に設定する事はできません。
 * \sa criAtomDbas_Create, criAtomDbas_CalculateWorkSize
 */
#define CRIATOMDBAS_MINIMUM_NUM_SECUREMENT_BUFFERS	(2)

/***************************************************************************
 *      処理マクロ
 *      Macro Functions
 ***************************************************************************/
/*==========================================================================
 *      CRI Atom API
 *=========================================================================*/
/*JP
 * \brief ユーザアロケータの登録
 * \ingroup ATOMLIB_GLOBAL
 * \param[in]	p_malloc_func	メモリ確保関数
 * \param[in]	p_free_func		メモリ解放関数
 * \param[in]	p_obj			ユーザ指定オブジェクト
 * \par 説明:
 * CRI Atom ライブラリにメモリアロケータ（メモリの確保／解放関数）を登録します。<br>
 * 本マクロでアロケータを登録すると、Atomライブラリがワーク領域を必要とするタイミングで、
 * ユーザが登録したメモリ確保／解放処理が呼び出されることになります。<br>
 * その結果、ワーク領域を必要とする関数（ ::criAtomPlayer_CreateAdxPlayer 関数等）に対し、
 * 個別にワーク領域をセットする処理を省略することが可能になります。<br>
 * （ワーク領域に NULL ポインタ、ワーク領域サイズに 0 バイトを指定した場合でも、
 * アロケータからの動的メモリ確保によりライブラリが問題なく動作するようになります。）<br>
 * \par 注意:
 * メモリ確保／解放関数のポインタに NULL を指定することで、
 * アロケータの登録を解除することも可能です。<br>
 * ただし、未解放のメモリ領域が残っている状態で登録を解除すると、
 * エラーコールバックが返され、登録の解除に失敗します。<br>
 * （引き続き登録済みのアロケータが呼び出されることになります。）<br>
 * <br>
 * 本マクロは内部的に ::criAtom_SetUserMallocFunction 関数と
 * ::criAtom_SetUserFreeFunction 関数を呼び出します。<br>
 * 本関数とこれらの API を併用しないようご注意ください。<br>
 * （本関数の呼び出しにより、上記 API にセットした内容が上書きされます。）<br>
 * <br>
 * また、登録されたメモリアロケータ関数はマルスレッドモード時に複数のスレッドからコール
 * されることがあります。従って、メモリアロケート処理がスレッドセーフでない場合は独自に
 * メモリアロケート処理を排他制御する必要があります。
 */
#define criAtom_SetUserAllocator(p_malloc_func, p_free_func, p_obj)	\
{\
	criAtom_SetUserMallocFunction(p_malloc_func, p_obj);\
	criAtom_SetUserFreeFunction(p_free_func, p_obj);\
}

/*==========================================================================
 *      CRI Atom D-BAS API
 *=========================================================================*/
/*JP
 * \brief CriAtomDbasConfig へのデフォルトパラメータのセット
 * \ingroup ATOMLIB_DBAS
 * \param[out]	p_config	D-BAS作成用コンフィグ構造体へのポインタ
 * \par 説明:
 * ::criAtomDbas_Create 関数に設定するコンフィグ構造体
 * （ ::CriAtomDbasConfig ）に、デフォルトの値をセットします。<br>
 * \sa CriAtomDbasConfig, criAtomDbas_Create, criAtomDbas_CalculateWorkSize
 */
#define criAtomDbas_SetDefaultConfig(p_config)	\
{\
	(p_config)->identifier	= 0;\
	(p_config)->max_streams	= CRIATOM_DEFAULT_DBAS_MAX_STREAMS;\
	(p_config)->max_bps		= CRIATOM_DEFAULT_DBAS_MAX_BPS;\
	(p_config)->max_mana_streams	= 0;\
	(p_config)->max_mana_bps		= 0;\
	(p_config)->num_securement_buffers = CRIATOMDBAS_DEFAULT_NUM_SECUREMENT_BUFFERS;\
}

/* ========================================================================*/
/*       CRI Atom Meter API                                                */
/* ========================================================================*/

/*JP
 * \brief レベルメータ機能コンフィグ構造体にデフォルト値をセット
 * \ingroup ATOMLIB_METER
 * \par 説明:
 * ::criAtomExAsr_AttachLevelMeter 関数に設定するコンフィグ構造体
 * （ ::CriAtomLevelMeterConfig ）に、デフォルト値をセットします。<br>
 * \sa criAtomExAsr_AttachLevelMeter, CriAtomLevelMeterConfig
 */
#define criAtomMeter_SetDefaultConfigForLevelMeter(p_config) \
{\
	(p_config)->interval = 50;\
	(p_config)->hold_time = 1000;\
}

/*JP
 * \brief ラウドネスメータ機能コンフィグ構造体にデフォルト値をセット
 * \ingroup ATOMLIB_ASR
 * \par 説明:
 * ::criAtomMeter_AttachLoudnessMeter 関数に設定するコンフィグ構造体
 * （ ::CriAtomLoudnessMeterConfig ）に、デフォルト値をセットします。<br>
 * \sa criAtomMeter_AttachLoudnessMeter, CriAtomLoudnessMeterConfig
 */
#define criAtomMeter_SetDefaultConfigForLoudnessMeter(p_config) \
{\
	(p_config)->short_term_time = 3;\
	(p_config)->integrated_time = 600;\
}

/***************************************************************************
 *      データ型宣言
 *      Data Type Declarations
 ***************************************************************************/
/*==========================================================================
 *      CRI Atom API
 *=========================================================================*/


/*JP
 * \brief メモリ確保関数
 * \ingroup ATOMLIB_GLOBAL
 * \param[in]	obj		ユーザ指定オブジェクト
 * \param[in]	size	要求メモリサイズ（バイト単位）
 * \return		void*	確保したメモリのアドレス（失敗時はNULL）
 * \par 説明:
 * メモリ確保関数登録用のインターフェースです。<br>
 * CRI Atomライブラリがライブラリ内で行うメモリ確保処理を、
 * ユーザ独自のメモリ確保処理に置き換えたい場合に使用します。<br>
 * \par 備考:
 * コールバック関数が実行される際には、sizeに必要とされるメモリのサイズがセット
 * されています。<br>
 * コールバック関数内でsize分のメモリを確保し、確保したメモリのアドレスを
 * 戻り値として返してください。<br>
 * 尚、引数の obj には、::criAtom_SetUserMallocFunction 関数で登録したユーザ指定
 * オブジェクトが渡されます。<br>
 * メモリ確保時にメモリマネージャ等を参照する必要がある場合には、
 * 当該オブジェクトを ::criAtom_SetUserMallocFunction 関数の引数にセットしておき、
 * 本コールバック関数の引数を経由して参照してください。<br>
 * \attention
 * メモリの確保に失敗した場合、エラーコールバックが返されたり、呼び出し元の関数が
 * 失敗する可能性がありますのでご注意ください。
 * \sa CriAtomFreeFunc, criAtom_SetUserMallocFunction
 */
typedef void *(CRIAPI *CriAtomMallocFunc)(void *obj, CriUint32 size);

/*JP
 * \brief メモリ解放関数
 * \ingroup ATOMLIB_GLOBAL
 * \param[in]	obj		ユーザ指定オブジェクト
 * \param[in]	mem		解放するメモリアドレス
 * \return				なし
 * \par 説明:
 * メモリ解放関数登録用のインターフェースです。<br>
 * CRI Atomライブラリ内がライブラリ内で行うメモリ解放処理を、
 * ユーザ独自のメモリ解放処理に置き換えたい場合に使用します。<br>
 * \par 備考:
 * コールバック関数が実行される際には、memに解放すべきメモリのアドレスがセット
 * されています。<br>
 * コールバック関数内でmemの領域のメモリを解放してください。
 * 尚、引数の obj には、::criAtom_SetUserFreeFunction 関数で登録したユーザ指定
 * オブジェクトが渡されます。<br>
 * メモリ確保時にメモリマネージャ等を参照する必要がある場合には、
 * 当該オブジェクトを ::criAtom_SetUserFreeFunction 関数の引数にセットしておき、
 * 本コールバック関数の引数を経由して参照してください。<br>
 * \sa CriAtomMallocFunc, criAtom_SetUserFreeFunction
 */
typedef void (CRIAPI *CriAtomFreeFunc)(void *obj, void *mem);



/*JP
 * \brief サウンドレンダラタイプ
 * \ingroup ATOMLIB_GLOBAL
 * \par 説明:
 * Atomプレーヤが内部で作成するサウンドレンダラの種別を指定するためのデータ型です。<br>
 * Atomプレーヤ作成時にコンフィグ構造体のパラメータとして指定します。
 * \attention
 * ::CRIATOM_SOUND_RENDERER_ANY は ::criAtomExPlayer_SetSoundRendererType 関数に対してのみ指定可能です。<br>
 * ボイスプール作成時には使用できません。<br>
 * \sa CriAtomAdxPlayerConfig, criAtomPlayer_CreateAdxPlayer
 */
typedef enum CriAtomSoundRendererTypeTag {
	CRIATOM_SOUND_RENDERER_NATIVE = 1,		/*JP<プラットフォームライブラリへ出力	*/
	CRIATOM_SOUND_RENDERER_ASR = 2,			/*JP<Atom Sound Rendererへ出力			*/
	CRIATOM_SOUND_RENDERER_HAPTIC = 3,



	/* enum size is 4bytes */
	CRIATOM_SOUND_RENDERER_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriAtomSoundRendererType;

/*JP
 * \brief スピーカーID
 * \ingroup ATOMLIB_GLOBAL
 * \par 説明:
 * 音声を出力するスピーカーを指定するためのIDです。<br>
 * ::criAtomPlayer_SetSendLevel 関数で利用します。
 * \sa criAtomPlayer_SetSendLevel
 */
typedef enum CriAtomSpeakerIdTag {
	CRIATOM_SPEAKER_FRONT_LEFT = 0,				/*JP<フロントレフトスピーカー			*/
	CRIATOM_SPEAKER_FRONT_RIGHT = 1,			/*JP<フロントライトスピーカー			*/
	CRIATOM_SPEAKER_FRONT_CENTER = 2,			/*JP<フロントセンタースピーカー			*/
	CRIATOM_SPEAKER_LOW_FREQUENCY = 3,			/*JP<LFE（≒サブウーハー）				*/
	CRIATOM_SPEAKER_SURROUND_LEFT = 4,			/*JP<サラウンドレフトスピーカー			*/
	CRIATOM_SPEAKER_SURROUND_RIGHT = 5,			/*JP<サラウンドライトスピーカー			*/
	CRIATOM_SPEAKER_SURROUND_BACK_LEFT = 6,		/*JP<サラウンドバックレフトスピーカー	*/
	CRIATOM_SPEAKER_SURROUND_BACK_RIGHT = 7,	/*JP<サラウンドバックライトスピーカー	*/

	/* enum size is 4bytes */
	CRIATOM_SPEAKER_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriAtomSpeakerId;

/*JP
 * \brief パフォーマンス情報
 * \ingroup ATOMLIB_GLOBAL
 * \par 説明:
 * パフォーマンス情報を取得するための構造体です。<br>
 * ::criAtom_GetPerformanceInfo 関数で利用します。
 * \sa criAtom_GetPerformanceInfo
 */
typedef struct CriAtomPerformanceInfoTag {
	CriUint32 server_process_count;		/*JP<サーバ処理実行回数									*/
	CriUint32 last_server_time;			/*JP<サーバ処理時間の最終計測値（マイクロ秒単位）		*/
	CriUint32 max_server_time;			/*JP<サーバ処理時間の最大値（マイクロ秒単位）			*/
	CriUint32 average_server_time;		/*JP<サーバ処理時間の平均値（マイクロ秒単位）			*/
	CriUint32 last_server_interval;		/*JP<サーバ処理実行間隔の最終計測値（マイクロ秒単位）	*/
	CriUint32 max_server_interval;		/*JP<サーバ処理実行間隔の最大値（マイクロ秒単位）		*/
	CriUint32 average_server_interval;	/*JP<サーバ処理実行間隔の平均値（マイクロ秒単位）		*/
} CriAtomPerformanceInfo;

/*JP
 * \brief エンコード品質
 * \ingroup ATOMLIB_GLOBAL
 * \par 説明:
 * CRI Atom Encoder, CRI Atom Craftで設定されるエンコード品質のデータ型です。<br>
 * 音声データのビットレートを計算するときに使用します。
 * \sa criAtom_CalculateHcaBitrate, criAtom_CalculateHcaMxBitrate
 */
typedef enum CriAtomEncodeQualityTag {
	CRIATOM_ENCODE_QUALITY_LOWEST = 0,	/*JP<最低品質設定	*/
	CRIATOM_ENCODE_QUALITY_LOW,			/*JP<低品質設定		*/
	CRIATOM_ENCODE_QUALITY_MIDDLE,		/*JP<中品質設定		*/
	CRIATOM_ENCODE_QUALITY_HIGH,		/*JP<高品質設定		*/
	CRIATOM_ENCODE_QUALITY_HIGHEST,		/*JP<最高品質設定	*/

	/* enum size is 4bytes */
	CRIATOM_ENCODE_QUALITY_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriAtomEncodeQuality;

/*JP
 * \brief フォーマット種別
 * \ingroup ATOMLIB_GLOBAL
 * \par 説明:
 * 音声データのフォーマットを示すデータ型です。<br>
 * 以下のいずれかの値が利用可能です。<br>
 * - ::CRIATOM_FORMAT_ADX
 * - ::CRIATOM_FORMAT_HCA
 * - ::CRIATOM_FORMAT_HCA_MX
 * - ::CRIATOM_FORMAT_WAVE
 * - ::CRIATOM_FORMAT_RAW_PCM
 * - ::CRIATOM_FORMAT_AIFF
 * - ::CRIATOM_FORMAT_VIBRATION
 * - ::CRIATOM_FORMAT_AUDIO_BUFFER
 * - ::CRIATOM_FORMAT_HW1
 * - ::CRIATOM_FORMAT_HW2
 * .
 */
typedef CriUint32 CriAtomFormat;
#define CRIATOM_FORMAT_NONE			(0x00000000)		/*JP< なし				*/
#define CRIATOM_FORMAT_ADX			(0x00000001)		/*JP< ADX				*/
#define CRIATOM_FORMAT_HCA			(0x00000003)		/*JP< HCA				*/
#define CRIATOM_FORMAT_HCA_MX		(0x00000004)		/*JP< HCA-MX			*/


/*JP
 * \brief PCMフォーマット
 * \ingroup ATOMLIB_GLOBAL
 * \par 説明:
 * PCMデータの型情報です。
 * \sa criAtomPlayer_SetFilterCallback
 */
typedef enum CriAtomPcmFormatTag {
	CRIATOM_PCM_FORMAT_SINT16 = 0,
	CRIATOM_PCM_FORMAT_FLOAT32,

	/* enum size is 4bytes */
	CRIATOM_PCM_FORMAT_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriAtomPcmFormat;


/*JP
 * \brief パラメータID
 * \ingroup ATOMLIB_PLAYER
 * \par 説明:
 * パラメータを指定するためのIDです。<br>
 */
typedef enum CriAtomParameterIdTag {
	CRIATOM_PARAMETER_ID_VOLUME					=  0,	/*JP< ボリューム */
	CRIATOM_PARAMETER_ID_FREQUENCY_RATIO		=  1,	/*JP< 周波数比 */

	/* enum size is 4bytes */
	CRIATOM_PARAMETER_ID_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriAtomParameterId;

/*JP
 * \brief ボイス停止理由
 * \ingroup ATOMLIB_GLOBAL
 * \par 説明:
 * ボイスの停止理由を表します。
 */
typedef enum CriAtomVoiceStopReasonTag {
	CRIATOM_VOICE_STOP_REASON_EXPLAYER_STOP = 0,					/*JP< EXプレーヤ停止		*/
	CRIATOM_VOICE_STOP_REASON_EXPLAYER_STOPWITHOUTRELEASE,			/*JP< EXプレーヤ停止		*/
	CRIATOM_VOICE_STOP_REASON_PLAYBACK_STOP,						/*JP< 再生ID指定停止	*/
	CRIATOM_VOICE_STOP_REASON_PLAYBACK_STOPWITHOUTRELEASE,			/*JP< 再生ID指定即時停止	*/
	CRIATOM_VOICE_STOP_REASON_SOUNDPLAYER_DESTROY,					/*JP< サウンドプレーヤ破棄		*/
	CRIATOM_VOICE_STOP_REASON_FADER_STOP,							/*JP< フェーダー停止		*/
	CRIATOM_VOICE_STOP_REASON_PLAYER_STOP,							/*JP< プレーヤ停止		*/
	CRIATOM_VOICE_STOP_REASON_AWB_STOP,								/*JP< AWB停止			*/
	CRIATOM_VOICE_STOP_REASON_STREAMING_CACHE_STOP,					/*JP< ストリーミングキャッシュ停止			*/
	CRIATOM_VOICE_STOP_REASON_PLAYER_FORCE_STOP,					/*JP< プレーヤ停止		*/
	CRIATOM_VOICE_STOP_REASON_PLAYER_DESTROY,						/*JP< プレーヤ破棄		*/
	CRIATOM_VOICE_STOP_REASON_MTPLAYER_STOP,						/*JP< MTプレーヤ破棄		*/
	CRIATOM_VOICE_STOP_REASON_VOICE_POOL_DESTROY,					/*JP< ボイスプール破棄		*/
	CRIATOM_VOICE_STOP_REASON_VOICE_STEAL_CASE1,					/*JP< ボイス奪い取り		*/
	CRIATOM_VOICE_STOP_REASON_VOICE_STEAL_CASE2,					/*JP< ボイス奪い取り		*/
	CRIATOM_VOICE_STOP_REASON_VOICE_STEAL_CASE3,					/*JP< ボイス奪い取り		*/
	CRIATOM_VOICE_STOP_REASON_VOICE_STEAL_CASE4,					/*JP< ボイス奪い取り		*/
	CRIATOM_VOICE_STOP_REASON_SOUND_SERVER_RESULT_VIRTUAL_VOICE_BEYOND_LIFE_TIME,	/*JP< サーバー処理			*/
	CRIATOM_VOICE_STOP_REASON_SOUND_SERVER_RESULT_CANT_CALC_3DPOS,					/*JP< サーバー処理			*/
	CRIATOM_VOICE_STOP_REASON_SOUND_SERVER_RESULT_INTERNAL_PLAYBACK_CANCEL,			/*JP< サーバー処理			*/
	CRIATOM_VOICE_STOP_REASON_SOUND_SERVER_RESULT_NO_PLAYER_NO_RETRY,				/*JP< サーバー処理			*/
	CRIATOM_VOICE_STOP_REASON_SOUND_SERVER_RESULT_FAILED_RETRY_BEYOND_LIFE_TIME,	/*JP< サーバー処理			*/
	CRIATOM_VOICE_STOP_REASON_SOUND_SERVER_RESULT_PLAYER_STATUS_PLAYEND,			/*JP< サーバー処理			*/
	CRIATOM_VOICE_STOP_REASON_SOUND_SERVER_RESULT_PLAYER_STATUS_ERROR,				/*JP< サーバー処理			*/
	CRIATOM_VOICE_STOP_REASON_SOUND_SERVER_RESULT_IMPOSSIBLE_RETRY,					/*JP< サーバー処理			*/
	CRIATOM_VOICE_STOP_REASON_SOUND_SERVER_RESULT_PLAYER_STATUS_STOP,				/*JP< サーバー処理			*/
	CRIATOM_VOICE_STOP_REASON_SOUND_SERVER_RESULT_INVALID_SERVER_REQUEST,			/*JP< サーバー処理			*/
	CRIATOM_VOICE_STOP_REASON_SOUND_SERVER_RESULT_SILENT_MODE_STOP,					/*JP< サーバー処理			*/
	CRIATOM_VOICE_STOP_REASON_SOUND_SERVER_RESULT_SOUNDCOMPLEX_STATUS_ERROR,		/*JP< サーバー処理			*/
	CRIATOM_VOICE_STOP_REASON_SOUND_SERVER_RESULT_NO_ELEMENT_CASE1,					/*JP< サーバー処理			*/
	CRIATOM_VOICE_STOP_REASON_SOUND_SERVER_RESULT_NO_ELEMENT_CASE2,					/*JP< サーバー処理			*/
	CRIATOM_VOICE_STOP_REASON_SOUND_SERVER_RESULT_STOP_NOT_PLAYING_ELEMENT,			/*JP< サーバー処理			*/
	CRIATOM_VOICE_STOP_REASON_SOUND_SERVER_RESULT_NOT_ACTIVE_VOICE,					/*JP< サーバー処理			*/
	CRIATOM_VOICE_STOP_REASON_ELEMENT_FREE,							/*JP< エレメント解放		*/
	CRIATOM_VOICE_STOP_REASON_EXPLAYER_STOP_ERROR_HN,				/*JP< エラープレーヤの停止		*/
	CRIATOM_VOICE_STOP_REASON_ACB_RELEASE,							/*JP< ACB解放		*/
	CRIATOM_VOICE_STOP_REASON_VOICE_ALLOCATE_FAIL,					/*JP< ボイス確保失敗		*/
	CRIATOM_VOICE_STOP_REASON_VOICE_RESET,							/*JP< ボイスリセット		*/
	CRIATOM_VOICE_STOP_REASON_VOICE_VIRTUALIZE,						/*JP< ボイスリバーチャル化		*/
	CRIATOM_VOICE_STOP_REASON_PLAYBACKSOUND_ALLOCATE_FAIL,			/*JP< プレイバックサウンド確保失敗		*/
	CRIATOM_VOICE_STOP_REASON_STOP_ACTION_WITH_BEATSYNC,			/*JP< ビート同期停止アクション		*/
	CRIATOM_VOICE_STOP_REASON_START_CANCEL_WITH_BEATSYNC,			/*JP< ビート同期再生キャンセル		*/
	CRIATOM_VOICE_STOP_REASON_STOP_ACTION,							/*JP< 停止アクション		*/
	CRIATOM_VOICE_STOP_REASON_BLOCK_TRANSITION_CASE0,				/*JP< ブロック遷移			*/
	CRIATOM_VOICE_STOP_REASON_BLOCK_TRANSITION_CASE1,				/*JP< ブロック遷移			*/
	CRIATOM_VOICE_STOP_REASON_BLOCK_TRANSITION_CASE2,				/*JP< ブロック遷移			*/
	CRIATOM_VOICE_STOP_REASON_BLOCK_TRANSITION_CASE3,				/*JP< ブロック遷移			*/
	CRIATOM_VOICE_STOP_REASON_CATEGORY_CUE_LIMIT,					/*JP< カテゴリキューリミット		*/
	CRIATOM_VOICE_STOP_REASON_UNSET_ACB,							/*JP< ACB内部解放		*/
	CRIATOM_VOICE_STOP_REASON_SEQUENCE_END,							/*JP< シーケンスエンドマーカー		*/
	CRIATOM_VOICE_STOP_REASON_BLODK_END,							/*JP< ブロックエンド		*/
	CRIATOM_VOICE_STOP_REASON_SEQUENCE_EXECUTE,						/*JP< シーケンスサーバー処理	*/
	CRIATOM_VOICE_STOP_REASON_TRACK_MONO,							/*JP< トラックモノモード処理	*/
	CRIATOM_VOICE_STOP_REASON_FADER_STOP_IMMEDIATE,					/*JP< フェーダー即時停止	*/
	CRIATOM_VOICE_STOP_REASON_CUE_LIMIT,							/*JP< キューリミット処理	*/
	CRIATOM_VOICE_STOP_REASON_STOP_ACB,								/*JP< 全ACB停止処理	*/
	CRIATOM_VOICE_STOP_REASON_SOUND_OBJECT_ADD_PLAYER,				/*JP< サウンドオブジェクトへのプレーヤ追加処理	*/
	CRIATOM_VOICE_STOP_REASON_SOUND_OBJECT_DELETE_PLAYER,			/*JP< サウンドオブジェクトからプレーヤの削除処理	*/
	CRIATOM_VOICE_STOP_REASON_SOUND_OBJECT_DELETE_ALL_PLAYER,		/*JP< サウンドオブジェクトからプレーヤの削除処理	*/
	CRIATOM_VOICE_STOP_REASON_UNREGISTER_ACF,						/*JP< ACFのアンレジスト処理	*/
	CRIATOM_VOICE_STOP_REASON_EXPLAYER_DESTROY,						/*JP< CriAtomExPlayerハンドルの破棄	*/
	CRIATOM_VOICE_STOP_REASON_EXPLAYER_ATTACH_FADER,				/*JP< CriAtomExPlayerへのフェーダー付加	*/
	CRIATOM_VOICE_STOP_REASON_EXPLAYER_DETACH_FADER,				/*JP< CriAtomExPlayerへのフェーダー取り外し	*/
	CRIATOM_VOICE_STOP_REASON_DETACH_AWB,							/*JP< AWB解放処理	*/
	CRIATOM_VOICE_STOP_MULTIPLE_PLAYBACK_PROHIBITION_TIME,			/*JP< 多重再生禁止時間内再生	*/
	CRIATOM_VOICE_STOP_REASON_CATEGORY_STOP,						/*JP< カテゴリ停止	*/
	CRIATOM_VOICE_STOP_REASON_CATEGORY_STOPWITHOUTRELEASE,			/*JP< カテゴリ即時停止	*/

	/* enum size is 4bytes */
	CRIATOM_VOICE_STOP_REASON_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriAtomVoiceStopReason;

 /*JP
  * \brief Ambisonics指定タイプ
  * \ingroup ATOMEXLIB_ASR
  * \par 説明:
  * Atomライブラリが再生に使用するAmbisonicsのオーダータイプを表します。<br>
  * ASR初期化時（ ::criAtomExAsr_Initialize 関数 ）に ::CriAtomExAsrConfig 
  * 構造体にて指定します。
  * \sa criAtomExAsr_Initialize, CriAtomExAsrConfig
  */
typedef enum CriAtomAmbisonicsOrderTypeTag {
	CRIATOMAMBISONICS_ORDER_TYPE_NONE = 0,	/*JP< 未設定 */
	CRIATOMAMBISONICS_ORDER_TYPE_FIRST,		/*JP< 1st Order */
	CRIATOMAMBISONICS_ORDER_TYPE_1P = CRIATOMAMBISONICS_ORDER_TYPE_FIRST,	/*JP< 1 Periphonic(1st Orderと同義) */
	CRIATOMAMBISONICS_ORDER_TYPE_ENUM_SIZE_IS_4BYTES = 0x7FFFFFFF
} CriAtomAmbisonicsOrderType;

/*==========================================================================
 *      CRI Atom HCA-MX API
 *=========================================================================*/

/*==========================================================================
 *      CRI Atom Decrypter API
 *=========================================================================*/


/*==========================================================================
 *      CRI Atom AWB API
 *=========================================================================*/
struct CriAtomAwbTag;
/*JP
 * \brief AWBハンドル
 * \ingroup ATOMLIB_AWB
 * \par 説明:
 * 再生データが含まれているAWBファイルのTOC情報を示すハンドルです。<br>
 * ::criAtomAwb_LoadToc 関数で取得します。<br>
 * \sa criAtomAwb_LoadToc
 */
typedef struct CriAtomAwbTag *CriAtomAwbHn;



/*==========================================================================
 *      CRI Atom Player API
 *=========================================================================*/
/*JP
 * 標準プレーヤ作成用コンフィグ構造体
 * \ingroup ATOMLIB_PLAYER
 * \par 説明:
 * 標準プレーヤ（ADXとHCAが再生可能なプレーヤ）を作成する際に、
 * 動作仕様を指定するための構造体です。<br>
 * ::criAtomPlayer_CreateStandardPlayer 関数の引数に指定します。<br>
 * <br>
 * 作成されるプレーヤは、ハンドル作成時に本構造体で指定された設定に応じて、
 * 内部リソースを必要なだけ確保します。<br>
 * プレーヤが必要とするワーク領域のサイズは、本構造体で指定されたパラメータに応じて変化します。
 * \attention
 * 将来的にメンバが増える可能性があるため、 ::criAtomPlayer_SetDefaultConfigForStandardPlayer
 * マクロを使用しない場合には、使用前に必ず構造体をゼロクリアしてください。<br>
 * （構造体のメンバに不定値が入らないようご注意ください。）
 * \sa criAtomPlayer_CreateStandardPlayer, criAtomPlayer_SetDefaultConfigForStandardPlayer
 */
typedef struct CriAtomStandardPlayerConfigTag {
	/*JP
		\brief 最大出力チャンネル数
		\par 説明:
		Atomプレーヤで再生する音声のチャンネル数を指定します。<br>
		::criAtomPlayer_CreateStandardPlayer 関数で作成されたAtomプレーヤは、max_channelsで指定した
		チャンネル数"以下の"音声データを再生可能です。<br>
		最大出力チャンネル数として指定する値と、作成されたAtomプレーヤで再生可能なデータの
		関係を以下に示します。<br>
		\table "最大出力チャンネル数と再生可能なデータの関係" align=center border=1 cellspacing=0 cellpadding=4
		[最大出力チャンネル数（指定する値）	| 作成されたAtomプレーヤで再生可能なデータ	]
		[1									| モノラル									]
		[2									| モノラル、ステレオ						]
		[6									| モノラル、ステレオ、5.1ch					]
		[8									| モノラル、ステレオ、5.1ch、7.1ch			]
		\endtable
		<br>
		\par 備考:
		サウンド出力時にハードウェアリソースを使用するプラットフォームにおいては、
		出力チャンネル数を小さくすることで、ハードウェアリソースの消費を抑えることが
		可能です。<br>
		\attention
		指定された最大出力チャンネル数を超えるデータは、再生することはできません。<br>
		例えば、最大出力チャンネル数を1に設定した場合、作成されたAtomプレーヤで
		ステレオ音声を再生することはできません。<br>
		（モノラルにダウンミックスされて出力されることはありません。）
	*/
	CriSint32 max_channels;

	/*JP
		\brief 最大サンプリングレート
		\par 説明:
		Atomプレーヤで再生する音声のサンプリングレートを指定します。<br>
		::criAtomPlayer_CreateStandardPlayer 関数で作成されたAtomプレーヤは、max_sampling_rateで指定した
		サンプリングレート"以下の"音声データを再生可能です。<br>
		<br>
		\par 備考:
		最大サンプリングレートを下げることで、Atomプレーヤ作成時に必要となるワークメモリ
		のサイズを抑えることが可能です。
		\attention
		指定された最大サンプリングレートを超えるデータは、再生することはできません。<br>
		例えば、最大サンプリングレートを24000に設定した場合、作成されたAtomプレーヤで
		48000Hzの音声を再生することはできません。<br>
		（ダウンサンプリングされて出力されることはありません。）
	*/
	CriSint32 max_sampling_rate;

	/*JP
		\brief ストリーミング再生を行うかどうか
		\par 説明:
		Atomプレーヤでストリーミング再生（ファイルからの再生）を行うかどうかを指定します。<br>
		streaming_flagにCRI_FALSEを指定した場合、作成されたAtomプレーヤはオンメモリのデータ
		再生（ ::criAtomPlayer_SetData 関数で指定したメモリアドレスの再生）のみをサポート
		します。（ファイルからの再生はできません。）<br>
		streaming_flagにCRI_TRUEを指定した場合、作成されたAtomプレーヤはオンメモリのデータ
		再生に加え、ファイルからの再生（ ::criAtomPlayer_SetFile 関数や
		::criAtomPlayer_SetContentId 関数で指定されたファイルの再生）をサポートします。<br>
		\par 補足:
		streaming_flagをCRI_TRUEにした場合、Atomプレーヤ作成時にファイル読み込み用のリソース
		が確保されます。<br>
		そのため、streaming_flagをCRI_FALSEの場合に比べ、Atomプレーヤの作成に必要なメモリの
		サイズが大きくなります。
	*/
	CriBool streaming_flag;

	/*JP
		\brief サウンドレンダラタイプ
		\par 説明:
		Atomプレーヤが使用するサウンドレンダラの種別を指定します。<br>
		sound_renderer_type に CRIATOM_SOUND_RENDERER_DEFAULT を指定した場合、
		音声データはデフォルト設定のサウンドレンダラに転送されます。<br>
		sound_renderer_type に CRIATOM_SOUND_RENDERER_NATIVE を指定した場合、
		音声データは各プラットフォームのサウンド出力に転送されます。<br>
		sound_renderer_type に CRIATOM_SOUND_RENDERER_ASR を指定した場合、
		音声データはASR（Atom Sound Renderer）に転送されます。<br>
		（ASRの出力先は、ASR初期化時に別途指定。）
	*/
	CriAtomSoundRendererType sound_renderer_type;

	/*JP
		\brief デコード処理のレイテンシ
		\par 説明:
		デコード処理のレイテンシを指定します。<br>
		decode_latency を 0 に設定した場合、プレーヤは音声再生開始時に
		音声データのデコードを可能な限り遅延なく行います。<br>
		（オンメモリ再生時は ::criAtomPlayer_Start 関数を実行後、
		最初のサーバ処理内で再生開始に必要な量のデータをデコードし、
		音声の出力を開始します。）<br>
		これに対し decode_latency を 1 以上に設定した場合、
		再生開始に必要なデータのデコードを、複数回のサーバ処理に分割して行います。<br>
		（オンメモリ再生時であっても ::criAtomPlayer_Start 関数を実行後、
		decode_latency に指定した回数サーバ処理が動作するまでは音声の出力が開始されません。）<br>
		\par 備考:
		Atomプレーヤはサウンドバッファ内のデータ残量を元に、
		サーバ処理当たりの音声データのデコード量を決定しています。<br>
		音声再生開始前はサウンドバッファが空の状態のため、
		音声再生中に比べて多くのデータ（再生中の 2 〜 4 倍程度）がデコードされます。<br>
		プレーヤ当たりの音声データのデコード処理負荷は小さいため、
		一音一音の発音開始時の処理負荷が問題になることはほとんどありません。<br>
		しかし、アプリケーション中で 1V に大量の発音リクエストを同時に発行した場合、
		全てのプレーヤの処理負荷のピークが同期し、負荷が目に見えて大きくなる場合があります。<br>
		こういった制御を行うケースでは、 decode_latency の値を増やすことで、
		局所的に処理負荷が高くなる症状を回避することが可能です。<br>
		<br>
		decode_latency のデフォルト値は、ほとんどの環境で 0 に設定されています。<br>
		しかし、携帯ゲーム機等、わずかな負荷変動でもアプリケーションに大きな影響を及ぼす環境では、
		デフォルト値が 1 以上に設定されている可能性があります。<br>
		（実際にセットされる値については ::criAtomPlayer_SetDefaultConfigForAdxPlayer
		マクロの適用結果を確認してください。）<br>
		<br>
		現状 decode_latency の値に 4 以上の値を指定することはできません。<br>
		（ decode_latency に 4 以上の値を指定した場合でも、ライブラリ内で 3 に変更されます。）<br>
	*/
	CriSint32 decode_latency;
} CriAtomStandardPlayerConfig;



/*JP
 * \ingroup ATOMLIB_PLAYER
 * HCA-MXプレーヤ作成用コンフィグ構造体
 * \par 説明:
 * HCA-MXが再生可能なプレーヤを作成する際に、動作仕様を指定するための構造体です。<br>
 * ::criAtomPlayer_CreateHcaMxPlayer 関数の引数に指定します。<br>
 * <br>
 * 作成されるプレーヤは、ハンドル作成時に本構造体で指定された設定に応じて、
 * 内部リソースを必要なだけ確保します。<br>
 * プレーヤが必要とするワーク領域のサイズは、本構造体で指定されたパラメータに応じて変化します。
 * \attention
 * 将来的にメンバが増える可能性があるため、 ::criAtomPlayer_SetDefaultConfigForHcaMxPlayer
 * マクロを使用しない場合には、使用前に必ず構造体をゼロクリアしてください。<br>
 * （構造体のメンバに不定値が入らないようご注意ください。）
 * \sa criAtomPlayer_CreateHcaMxPlayer, criAtomPlayer_SetDefaultConfigForHcaMxPlayer
 */
typedef struct CriAtomHcaMxPlayerConfigTag {
	/*JP
		\brief 最大出力チャンネル数
		\par 説明:
		Atomプレーヤで再生する音声のチャンネル数を指定します。<br>
		::criAtomPlayer_CreateHcaMxPlayer 関数で作成されたAtomプレーヤは、max_channelsで指定した
		チャンネル数"以下の"音声データを再生可能です。<br>
	*/
	CriSint32 max_channels;

	/*JP
		\brief 最大サンプリングレート
		\par 説明:
		Atomプレーヤで再生する音声のサンプリングレートを指定します。<br>
		::criAtomPlayer_CreateHcaMxPlayer 関数で作成されたAtomプレーヤは、max_sampling_rateで指定した
		サンプリングレートと一致する音声データのみを再生可能です。<br>
		<br>
		\par 備考:
		最大サンプリングレートを下げることで、Atomプレーヤ作成時に必要となるワークメモリ
		のサイズを抑えることが可能です。
		\attention
		指定された最大サンプリングレートに一致しないデータは、再生することはできません。<br>
		例えば、最大サンプリングレートを48000に設定した場合、作成されたAtomプレーヤで
		48000Hz以外の音声を再生することはできません。<br>
		（レート変換されて出力されることはありません。）
	*/
	CriSint32 max_sampling_rate;

	/*JP
		\brief ストリーミング再生を行うかどうか
		\par 説明:
		Atomプレーヤでストリーミング再生（ファイルからの再生）を行うかどうかを指定します。<br>
		streaming_flagにCRI_FALSEを指定した場合、作成されたAtomプレーヤはオンメモリのデータ
		再生（ ::criAtomPlayer_SetData 関数で指定したメモリアドレスの再生）のみをサポート
		します。（ファイルからの再生はできません。）<br>
		streaming_flagにCRI_TRUEを指定した場合、作成されたAtomプレーヤはオンメモリのデータ
		再生に加え、ファイルからの再生（ ::criAtomPlayer_SetFile 関数や
		::criAtomPlayer_SetContentId 関数で指定されたファイルの再生）をサポートします。<br>
		\par 補足:
		streaming_flagをCRI_TRUEにした場合、Atomプレーヤ作成時にファイル読み込み用のリソース
		が確保されます。<br>
		そのため、streaming_flagをCRI_FALSEの場合に比べ、Atomプレーヤの作成に必要なメモリの
		サイズが大きくなります。
	*/
	CriBool streaming_flag;
} CriAtomHcaMxPlayerConfig;





struct CriAtomPlayerTag;
/*JP
 * \brief Atomプレーヤハンドル
 * \ingroup ATOMLIB_PLAYER
 * \par 説明:
 * CriAtomPlayerHn は、音声再生用に作られたプレーヤを操作するためのハンドルです。<br>
 * ::criAtomPlayer_CreateAdxPlayer 関数等で音声再生用のプレーヤを作成すると、
 * 関数はプレーヤ操作用に、この"Atomプレーヤハンドル"を返します。
 * <br>
 * Atomプレーヤとは、コーデックに依存しない再生制御のためのインターフェースを提供する、
 * 抽象化されたプレーヤオブジェクトです。<br>
 * Atomプレーヤの作成方法は再生する音声コーデックにより異なりますが、
 * 作成されたプレーヤの制御については、Atomプレーヤ用のAPIが共通で利用可能です。
 * <br>
 * データのセットや再生の開始、ステータスの取得等、プレーヤに対して行う操作は、
 * 全てAtomプレーヤハンドルを介して実行されます。<br>
 * \sa criAtomPlayer_CreateAdxPlayer
 */
typedef struct CriAtomPlayerTag *CriAtomPlayerHn;



/*JP
 * \brief パラメータ変更コールバック関数
 * \ingroup ATOMLIB_PLAYER
 * \par 説明:
 * \param[in]	obj				ユーザ指定オブジェクト
 * \param[in]	player			Atomプレーヤハンドル
 * \param[in]	id				パラメータID
 * \param[in]	value			パラメータ値
 * \return				なし
 * \par 説明:
 * Atomプレーヤのパラメータが変更されるタイミングで実行されるコールバック関数です。<br>
 * <br>
 * コールバック関数の登録には ::criAtomPlayer_SetParameterChangeCallback 関数を使用します。<br>
 * 登録したコールバック関数は、Atomプレーヤのパラメータが更新されるタイミングで実行されます。<br>
 * \attention
 * 本コールバック関数内で長時間処理をブロックすると、音切れ等の問題が発生しますので、
 * ご注意ください。<br>
 * \sa criAtomPlayer_SetParameterChangeCallback
 */
typedef void (CRIAPI *CriAtomPlayerParameterChangeCbFunc)(void *obj,
	CriAtomPlayerHn player, CriAtomParameterId id, CriFloat32 value);

/*JP
 * \brief 波形フィルタコールバック関数
 * \ingroup ATOMLIB_PLAYER
 * \par 説明:
 * \param[in]		obj				ユーザ指定オブジェクト
 * \param[in]		format			PCMの形式
 * \param[in]		num_channels	チャンネル数
 * \param[in]		num_samples		サンプル数
 * \param[in,out]	data			PCMデータのチャンネル配列
 * \return						なし
 * \par 説明:
 * デコード結果の PCM データを受け取るコールバック関数です。<br>
 * <br>
 * コールバック関数の登録には ::criAtomPlayer_SetFilterCallback 関数を使用します。<br>
 * コールバック関数を登録すると、 Atom プレーヤが音声データをデコードする度に、
 * コールバック関数が実行されるようになります。<br>
 * <br>
 * フィルタコールバック関数には、 PCM データのフォーマットやチャンネル数、
 * 参照可能なサンプル数、 PCM データを格納した領域のアドレスが返されます。<br>
 * コールバック内では PCM データの値を直接参照可能になるので、
 * 再生中の音声の振幅をチェックするといった用途に利用可能です。<br>
 * <br>
 * また、コールバック関数内で PCM データを加工すると、再生音に反映されるため、
 * PCM データに対してユーザ独自のエフェクトをかけることも可能です。<br>
 * （ただし、タイムストレッチ処理のようなデータ量が増減する加工を行うことはできません。）<br>
 * \par 備考:
 * PCM データはチャンネル単位で分離されています。<br>
 * （インターリーブされていません。）<br>
 * 第 6 引数（ data 配列）には、各チャンネルの PCM データ配列の先頭アドレスが格納されています。<br>
 * （二次元配列の先頭アドレスではなく、チャンネルごとの PCM データ配列の先頭アドレスを格納した
 * 一次元のポインタ配列です。）<br>
 * <br>
 * プラットフォームによって、 PCM データのフォーマットは異なります。<br>
 * 実行環境のデータフォーマットについては、第 3 引数（ format ）で判別可能です。<br>
 * PCM データのフォーマットが 16 bit 整数型の場合、 format は CRIATOM_PCM_FORMAT_SINT16 となり、
 * PCM データのフォーマットが 32 bit 浮動小数点数型の場合、 format は CRIATOM_PCM_FORMAT_FLOAT32 となります。<br>
 * それぞれのケースで PCM データの値域は異なりますのでご注意ください。<br>
 * - CRIATOM_PCM_FORMAT_SINT16 時は -32768 〜 +32767
 * - CRIATOM_PCM_FORMAT_FLOAT32 時は -1.0f 〜 +1.0f
 * .
 * （デコード時点ではクリッピングが行われていないため、 CRIATOM_PCM_FORMAT_FLOAT32
 * 時は上記範囲をわずかに超えた値が出る可能性があります。）<br>
 * \attention
 * コールバック関数内で長時間処理をブロックすると、音切れ等の問題が発生する可能性があります。<br>
 * \sa criAtomPlayer_SetFilterCallback
 */
typedef void (CRIAPI *CriAtomPlayerFilterCbFunc)(void *obj, CriAtomPcmFormat format,
	CriSint32 num_channels, CriSint32 num_samples, void *data[]);

/*JP
 * \brief ロード要求コールバック関数
 * \ingroup ATOMLIB_PLAYER
 * \par 説明:
 * \param[in]	obj		ユーザ指定オブジェクト
 * \param[in]	binder	ファイルロード先バインダ
 * \param[in]	path	ファイルパス
 * \param[in]	offset	ロード開始位置
 * \param[in]	length	ロード要求サイズ
 * \return				なし
 * \par 説明:
 * Atomプレーヤのファイルロード状態を監視するための、デバッグ用のコールバック関数です。<br>
 * コールバック関数の登録には ::criAtomPlayer_SetLoadRequestCallback 関数を使用します。<br>
 * <br>
 * 登録したコールバック関数は、Atomプレーヤが音声データのロード要求を発行するタイミングで実行されます。<br>
 * \par 備考:
 * \attention
 * 本コールバック関数内で長時間処理をブロックすると、音切れ等の問題が発生しますので、
 * ご注意ください。<br>
 * \sa criAtomPlayer_SetLoadRequestCallback
 */
typedef void (CRIAPI *CriAtomPlayerLoadRequestCbFunc)(void *obj,
	CriFsBinderHn binder, const CriChar8 *path, CriSint64 offset, CriSint64 length);

/*==========================================================================
 *      CRI Atom D-BAS API
 *=========================================================================*/
/*JP
 * \brief Atom D-BAS ID
 * \ingroup ATOMLIB_DBAS
 * \par 説明:
 * CriAtomDbasId は、D-BAS管理用のIDです。<br>
 * ::criAtomDbas_Create 関数でD-BASを作成すると取得できます。
 * <br>
 * アプリケーションがこのD-BAS IDを利用するのは、D-BASの破棄時のみです。
 * \sa criAtomDbas_Create, criAtomDbas_Destroy
 */
typedef CriSint32 CriAtomDbasId;

/*JP
 * \brief D-BAS作成パラメータ構造体
 * \ingroup ATOMLIB_DBAS
 * \par 説明:
 * ::criAtomDbas_Create 関数の引数に指定する、D-BASの作成パラメータ構造体です。<br>
 * <br>
 * \attention
 * 将来的にメンバが増える可能性があるため、 ::criAtomDbas_SetDefaultConfig
 * マクロを使用しない場合には、使用前に必ず構造体をゼロクリアしてください。<br>
 * （構造体のメンバに不定値が入らないようご注意ください。）
 * \sa criAtomDbas_Create, criAtomDbas_CalculateWorkSize, criAtomDbas_SetDefaultConfig
 */
typedef struct CriAtomDbasConfigTag {
	/*JP
		\brief D-BAS 識別子
		\par 説明:
		アプリケーション側がD-BASを作成する際に指定する識別子です。<br>
		アプリケーション側がどのD-BASを使うか明示的に指定する際に使用します。<br>
	*/
	CriUint32 identifier;				/*JP< D-BAS 識別子	*/
	/*JP
		\brief 最大ストリーミング数
		\par 説明:
		D-BASで管理する瞬間最大ストリーミング本数を指定します。<br>
		オーディオだけでなく、Sofdec2で再生するムービーデータのストリーミング本数も加味する必要があります。<br>
		例えば、シーンAではオーディオデータを２本、
		シーンBではオーディオデータを１本とムービーデータを２本、ストリーミング再生するとします。<br>
		この場合、瞬間最大ストリーミング本数はシーンBの３本を設定してください。<br>
		つまり、アプリケーション全体を通して、最悪状態のストリーミング本数を想定した値を設定してください。
	*/
	CriSint32 max_streams;
	/*JP
		\brief 最大ビットレート
		\par 説明:
		ストリーミング全体における瞬間最大ビットレートを指定します。<br>
		この値には、アプリケーション全体を通して、
		ストリーミング再生するデータの消費ビットレートのピーク値を設定してください。<br>
		オーディオだけでなく、Sofdec2で再生するムービーデータの消費ビットレートも加味する必要があります。<br>
		例えば、シーンAではオーディオデータを４本、シーンBではムービーデータを１本、ストリーミング再生するとします。<br>
		この時、オーディオデータ４本分の消費ビットレートよりもムービーデータ１本の消費ビットレートが大きい場合、<br>
		ムービーデータの消費ビットレートを設定してください。<br>
		つまり、アプリケーション全体を通して、最悪状態の消費ビットレートを想定した値を設定してください。
	*/
	CriSint32 max_bps;
	/*JP
		\brief CRI Mana側で再生する最大ストリーミング数
		\par 説明:
		CRI Mana側で再生する瞬間最大ストリーミング本数を指定します。<br>
		D-BASのメモリ使用量の計算では、max_streams からこの値を減じたストリーミング本数を、<br>
		CRI Atomの最大ストリーミング本数として扱います。<br>
		CRI Mana側でストリーミング再生を行わない場合は 0 に設定してください。<br>
	*/
	CriSint32 max_mana_streams;
	/*JP
		\brief CRI Mana側で再生する最大ビットレート
		\par 説明:
		CRI Mana側で再生する瞬間最大ビットレートを指定します。<br>
		D-BASのメモリ使用量の計算では、max_bps からこの値を減じたビットレートを、<br>
		CRI Atomの最大ビットレートとして扱います。<br>
		CRI Mana側でストリーミング再生を行わない場合は 0 に設定してください。<br>
	*/
	CriSint32 max_mana_bps;
	/*JP
		\brief １ストリームに割り当てる最低保証バッファ数
		\par 説明:
		１ストリームに割り当てる最低保証バッファ数を指定します。単位は[個]です。<br>
		ADX2内部ではバッファをブロック単位で管理しているため、
		ファイル終端やループ先頭等の半端なデータサイズに対しても、
		バッファブロックを１つ分割り当てます。<br>
		ワンショットのストリーミング再生では、
		ダブルバッファリングでデータを読むだけの単純な処理であっても、
		ストリームが途切れる事はありません。<br>
		一方、ループ付きデータの場合では、ループ終端のわずかなデータに１ブロック、
		ループ先頭のわずかなデータにも１ブロックを使ってしまうと、<br>
		バッファリング済みのデータが極端に少ないにもかかわらず、
		次のデータを読み込むバッファが空かないためストリームが途切れてしまいます。<br>
		本パラメータは、D-BASが確保するストリーミングバッファサイズに影響を与えます。<br>
		音が途切れない事を十分に確認できていれば、
		本パラメータの下限値は ::CRIATOMDBAS_MINIMUM_NUM_SECUREMENT_BUFFERS です。<br>
	*/
	CriSint32 num_securement_buffers;
} CriAtomDbasConfig;

/* ========================================================================*/
/*       CRI Atom Streaming Cache API                                      */
/* ========================================================================*/


/* ========================================================================*/
/*       CRI Atom DSP API                                                  */
/* ========================================================================*/






/* スペクトラムアナライザハンドル */

/* ========================================================================*/
/*       CRI Atom Meter API                                                */
/* ========================================================================*/

/*JP
 * \brief レベルメータ機能追加用コンフィグ構造体
 * \ingroup ATOMLIB_METER
 * \par 説明:
 * レベルメータ機能を追加するための構造体です。<br>
 * ::criAtomMeter_AttachLevelMeter 関数の引数に指定します。<br>
 * \par 備考:
 * デフォルト設定を使用する場合、 ::criAtomMeter_SetDefaultConfigForLevelMeter マクロで
 * 構造体にデフォルトパラメータをセットした後、 ::criAtomMeter_AttachLevelMeter 関数
 * に構造体を指定してください。<br>
 * \attention
 * 将来的にメンバが増える可能性があるため、 ::criAtomMeter_SetDefaultConfigForLevelMeter
 * マクロを使用しない場合には、使用前に必ず構造体をゼロクリアしてください。<br>
 * （構造体のメンバに不定値が入らないようご注意ください。）
 * \sa criAtomMeter_AttachLevelMeter
 */
typedef struct {
	/*JP
		\brief 測定間隔（ミリ秒単位）
		\par 説明:
		測定結果を更新する間隔です。<br>
	*/
	CriSint32 interval;

	/*JP
		\brief ピークホールド時間（ミリ秒単位）
		\par 説明:
		ピーク値がより大きい値で更新されたとき、下がらないようにホールドする時間です。<br>
	*/
	CriSint32 hold_time;
} CriAtomLevelMeterConfig;

/*JP
 * \brief レベル情報
 * \ingroup ATOMLIB_METER
 * \par 説明:
 * レベル情報を取得するための構造体です。<br>
 * ::criAtomMeter_GetLevelInfo 関数で利用します。
 * \par 備考:
 * 各レベル値の単位はdBです。<br>
 * \sa criAtomMeter_GetLevelInfo
 */
typedef struct CriAtomLevelInfoTag {
	/*JP
		\brief 有効チャンネル数
		\par 説明:
		測定結果が有効なチャンネル数です。<br>
	*/
	CriSint32 num_channels;
	
	/*JP
		\brief RMSレベル
		\par 説明:
		測定間隔間の音声振幅のRMS（二乗平均平方根）を計算した値です。<br>
		音圧レベルとして扱われます。
	*/
	CriFloat32 rms_levels[8];
	
	/*JP
		\brief ピークレベル
		\par 説明:
		測定間隔間の音声振幅の最大値です。<br>
	*/
	CriFloat32 peak_levels[8];
	
	/*JP
		\brief ピークホールドレベル
		\par 説明:
		ホールドしているピークレベル値です。<br>
	*/
	CriFloat32 peak_hold_levels[8];
} CriAtomLevelInfo;

/*JP
 * \brief ラウドネスメータ機能追加用コンフィグ構造体
 * \ingroup ATOMLIB_METER
 * ラウドネスメータ機能を追加するための構造体です。<br>
 * ::criAtomMeter_AttachLoudnessMeter 関数の引数に指定します。<br>
 * \par 備考:
 * デフォルト設定を使用する場合、 ::criAtomMeter_SetDefaultConfigForLoudnessMeter マクロで
 * 構造体にデフォルトパラメータをセットした後、 ::criAtomMeter_AttachLoudnessMeter 関数
 * に構造体を指定してください。<br>
 * \attention
 * 将来的にメンバが増える可能性があるため、 ::criAtomMeter_SetDefaultConfigForLoudnessMeter
 * マクロを使用しない場合には、使用前に必ず構造体をゼロクリアしてください。<br>
 * （構造体のメンバに不定値が入らないようご注意ください。）
 * \sa criAtomMeter_AttachLoudnessMeter
 */
typedef struct {
	/**
		\brief ショートターム測定時間
		\par 説明:
		短期の平均ラウドネスの測定時間（秒単位）です。<br>
		標準は3秒です。<br>
	*/
	CriSint32 short_term_time;

	/**
		\brief インテグレーテッド測定時間
		\par 説明:
		長期の平均ラウドネスの測定時間（秒単位）です。<br>
		インテグレーテッド値はコンテンツ単位の平均ラウドネスです。<br>
	*/
	CriSint32 integrated_time;
} CriAtomLoudnessMeterConfig;

/***************************************************************************
 *      変数宣言
 *      Prototype Variables
 ***************************************************************************/

/***************************************************************************
 *      関数宣言
 *      Prototype Functions
 ***************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

/*==========================================================================
 *      CRI Atom API
 *=========================================================================*/
/*JP
 * \brief ライブラリバージョン番号の取得
 * \ingroup ATOMLIB_GLOBAL
 * \return		CriUint32	バージョン番号
 * \par 説明:
 * ライブラリのバージョン番号を取得します。<br>
 * 本関数で取得した値とヘッダファイル（ cri_atom.h ）に記載された
 * CRI_ATOM_VERSION の値が異なる場合、インクルードしているヘッダと、
 * リンクしているライブラリのバージョンが一致していない可能性があります。<br>
 * （ビルド環境に設定されたインクルードパスとライブラリパスを確認してください。）<br>
 * \par 備考:
 * 以下のコードで、デバッグ時にライブラリとヘッダバージョンの不一致を検出可能です。<br>
 * \code
 * 		：
 * 	// ライブラリとヘッダの整合性をチェック
 * 	assert(CRI_ATOM_VERSION == criAtom_GetVersionNumber());
 * 		：
 * \endcode
 */
CriUint32 CRIAPI criAtom_GetVersionNumber(void);





/*JP
 * \brief オーディオ出力が有効かどうかのチェック
 * \ingroup ATOMLIB_GLOBAL
 * \return	CriBool		オーディオ出力が有効かどうか
 * \retval	CRI_TRUE	有効
 * \retval	CRI_FALSE	無効
 * \par 説明:
 * オーディオ出力が有効化どうかをチェックします。<br>
 * CRI Atomライブラリ初期化時、サウンドデバイスが利用可能であった場合、
 * 本関数はCRI_TRUEを返します。<br>
 * CRI Atomライブラリ初期化時に、サウンドデバイスが利用不可能であった場合、
 * 本関数はCRI_FALSEを返します。<br>
 * \par 補足:
 * サウンドデバイスが無効な場合でも、Atomライブラリは音声を出力せずに動作します。<br>
 * （音声データ消費量をタイマを元に計算し、可能な限り音声出力が有効な場合と同等の動作をエミュレートします。）<br>
 * そのため、音声出力デバイスが使用できないケースであっても、
 * アプリケーション側でAtomライブラリのAPI呼び出しを回避する必要はありません。<br>
 * （PC環境等、ユーザがサウンドデバイスを無効化しているケースに対し通知を行いたい場合に、
 * 本関数を使用してください。）<br>
 * \attention
 * 本関数は、「Atomライブラリ初期化時点でオーディオデバイスが利用可能だったかどうか」を返します。<br>
 * ライブラリ初期化後にユーザがサウンドデバイスを抜き差しするケースについては、本関数では検知できません。<br>
 * （各機種固有のAPIを使用する必要があります。）<br>
 * <br>
 * オーディオ出力が無効な状態でAtomライブラリを初期化後、
 * ユーザがオーディオデバイスを有効化したとしても、Atomライブラリは音声出力を行いません。<br>
 * （オーディオデバイスが刺されたことをAtomライブラリが自動で検出することはありません。）<br>
 * アプリケーション実行中にオーディオデバイスを有効化したい場合には、
 * Atomライブラリの初期化処理をやり直す必要があります。<br>
 * \sa criAtom_Initialize
 */
CriBool CRIAPI criAtom_IsAudioOutputActive(void);



/*JP
 * \brief メモリ確保関数の登録
 * \ingroup ATOMLIB_GLOBAL
 * \param[in]	func		メモリ確保関数
 * \param[in]	obj			ユーザ指定オブジェクト
 * \par 説明:
 * CRI Atomライブラリにメモリ確保関数を登録します。<br>
 * CRI Atomライブラリ内がライブラリ内で行うメモリ確保処理を、
 * ユーザ独自のメモリ確保処理に置き換えたい場合に使用します。<br>
 * <br>
 * 本関数の使用手順は以下のとおりです。<br>
 * (1) ::CriAtomMallocFunc インターフェースに副ったメモリ確保関数を用意する。<br>
 * (2) ::criAtom_SetUserMallocFunction 関数を使用し、CRI Atomライブラリに対して
 * メモリ確保関数を登録する。<br>
 * <br>
 * 具体的なコードの例は以下のとおりです。
 * \par 例:
 * \code
 * // 独自のメモリ確保関数を用意
 * void *user_malloc(void *obj, CriUint32 size)
 * {
 * 	void *mem;
 *
 * 	// メモリの確保
 * 	mem = malloc(size);
 *
 * 	return (mem);
 * }
 *
 * main()
 * {
 * 		:
 * 	// メモリ確保関数の登録
 * 	criAtom_SetUserMallocFunction(user_malloc, NULL);
 * 		:
 * }
 * \endcode
 * \par 備考:
 * 引数の obj に指定した値は、 ::CriAtomMallocFunc に引数として渡されます。<br>
 * メモリ確保時にメモリマネージャ等を参照する必要がある場合には、
 * 当該オブジェクトを本関数の引数にセットしておき、コールバック関数で引数を経由
 * して参照してください。<br>
 * \attention
 * メモリ確保関数を登録する際には、合わせてメモリ解放関数（ ::CriAtomFreeFunc ）を
 * 登録する必要があります。
 * \sa CriAtomMallocFunc, criAtom_SetUserFreeFunction
 */
void CRIAPI criAtom_SetUserMallocFunction(CriAtomMallocFunc func, void *obj);

/*JP
 * \brief メモリ解放関数の登録
 * \ingroup ATOMLIB_GLOBAL
 * \param[in]	func		メモリ解放関数
 * \param[in]	obj			ユーザ指定オブジェクト
 * \par 説明:
 * CRI Atomライブラリにメモリ解放関数を登録します。<br>
 * CRI Atomライブラリ内がライブラリ内で行うメモリ解放処理を、
 * ユーザ独自のメモリ解放処理に置き換えたい場合に使用します。<br>
 * <br>
 * 本関数の使用手順は以下のとおりです。<br>
 * (1) ::CriAtomFreeFunc インターフェースに副ったメモリ解放関数を用意する。<br>
 * (2) ::criAtom_SetUserFreeFunction 関数を使用し、CRI Atomライブラリに対して
 * メモリ解放関数を登録する。<br>
 * <br>
 * 具体的なコードの例は以下のとおりです。
 * \par 例:
 * \code
 * // 独自のメモリ解放関数を用意
 * void user_free(void *obj, void *mem)
 * {
 * 	// メモリの解放
 * 	free(mem);
 *
 * 	return;
 * }
 *
 * main()
 * {
 * 		:
 * 	// メモリ解放関数の登録
 * 	criAtom_SetUserFreeFunction(user_free, NULL);
 * 		:
 * }
 * \endcode
 * \par 備考:
 * 引数の obj に指定した値は、 ::CriAtomFreeFunc に引数として渡されます。<br>
 * メモリ確保時にメモリマネージャ等を参照する必要がある場合には、
 * 当該オブジェクトを本関数の引数にセットしておき、コールバック関数で引数を経由
 * して参照してください。<br>
 * \attention
 * メモリ解放関数を登録する際には、合わせてメモリ確保関数（ ::CriAtomMallocFunc ）を
 * 登録する必要があります。
 * \sa CriAtomFreeFunc, criAtom_SetUserMallocFunction
 */
void CRIAPI criAtom_SetUserFreeFunction(CriAtomFreeFunc func, void *obj);



/*JP
 * \brief サーバ処理の割り込みを防止
 * \ingroup ATOMLIB_GLOBAL
 * \par 説明:
 * サーバ処理の割り込み抑止します。<br>
 * 本関数実行後、::criAtom_Unlock 関数実行までの間、サーバ処理の動作が抑止されます。<br>
 * 複数のAPIを同一オーディオフレーム内で確実に実行したい場合には、本関数でサーバ処理の
 * 割り込みを防止し、それらの関数を実行してください。
 * \par 例:
 * \code
 * 	：
 * // サーバ処理の割り込みを防止
 * criAtom_Lock();
 *
 * // 複数の音声を同時に再生開始
 * criAtomPlayer_Start(player1);
 * criAtomPlayer_Start(player2);
 * criAtomPlayer_Start(player3);
 * criAtomPlayer_Start(player4);
 *
 * // サーバ処理の割り込み防止を解除
 * criAtom_Unlock();
 * 	：
 * \endcode
 * \attention
 * 上記の例のように、複数のプレーヤで同時に再生をスタートする場合でも、
 * ストリーム再生時は同時に発音が開始されるとは限りません。<br>
 * （バッファリングに伴う再生遅延があるため。）<br>
 * <br>
 * 本関数実行後、長時間::criAtom_Unlock 関数を呼ばない場合、音声再生が途切れる恐れがあります。<br>
 * サーバ処理の割り込みを防止する区間は、最小限に抑える必要があります。
 * \sa criAtom_Unlock
 */
void CRIAPI criAtom_Lock(void);

/*JP
 * \brief サーバ処理の割り込み防止を解除
 * \ingroup ATOMLIB_GLOBAL
 * \par 説明:
 * ::criAtom_Lock 関数による、サーバ処理の割り込み防止を解除します。
 * \sa criAtom_Lock
 */
void CRIAPI criAtom_Unlock(void);


/*JP
 * \brief パフォーマンスモニタ機能の追加
 * \ingroup ATOMLIB_GLOBAL
 * \par 説明:
 * パフォーマンス計測機能を追加し、パフォーマンス計測処理を開始します。<br>
 * 本関数を実行後、 ::criAtom_GetPerformanceInfo 関数を実行することで、
 * サーバ処理の負荷や、サーバ処理の実行間隔等、ライブラリのパフォーマンス情報を
 * 取得することが可能です。
 * \sa criAtom_GetPerformanceInfo, criAtom_DetachPerformanceMonitor
 * \par 例:
 * \code
 * 	// パフォーマンス情報取得用構造体
 * 	CriAtomPerformanceInfo info;
 *
 * 	// ライブラリの初期化
 * 	criAtom_Initialize(…);
 *
 * 	// パフォーマンスモニタ機能の追加
 * 	criAtom_AttachPerformanceMonitor();
 *
 * 	// パフォーマンスモニタのリセット
 * 	criAtom_ResetPerformanceMonitor();
 *
 * 	// パフォーマンス計測区間
 * 	…
 *
 * 	// パフォーマンス情報の取得
 * 	criAtom_GetPerformanceInfo(&info);
 *
 * 	// 計測結果の表示
 * 	printf(
 * 		"Max CPU Load            : %d (us)\n"	\
 * 		"Average CPU Load        : %d (us)\n"	\
 * 		"Max Server Interval     : %d (us)\n"	\
 * 		"Average Server Interval : %d (us)\n",
 * 		info.max_server_time, info.average_server_time,
 * 		info.max_server_interval, info.average_server_interval
 * 	);
 * \endcode
 */
void CRIAPI criAtom_AttachPerformanceMonitor(void);

/*JP
 * \brief パフォーマンスモニタ機能の削除
 * \ingroup ATOMLIB_GLOBAL
 * \par 説明:
 * パフォーマンス計測処理を終了し、パフォーマンス計測機能を削除します。
 * \attention
 * 本関数は完了復帰型の関数です。<br>
 * 本関数を実行すると、しばらくの間Atomライブラリのサーバ処理がブロックされます。<br>
 * 音声再生中に本関数を実行すると、音途切れ等の不具合が発生する可能性があるため、
 * 本関数の呼び出しはシーンの切り替わり等、負荷変動を許容できるタイミングで行ってください。
 */
void CRIAPI criAtom_DetachPerformanceMonitor(void);

/*JP
 * \brief パフォーマンスモニタのリセット
 * \ingroup ATOMLIB_GLOBAL
 * \par 説明:
 * 現在までの計測結果を破棄します。<br>
 * パフォーマンスモニタは、 ::criAtom_AttachPerformanceMonitor 関数実行直後
 * からパフォーマンス情報の取得を開始し、計測結果を累積します。<br>
 * これから計測する区間に対し、以前の計測結果を以降の計測結果に含めたくない場合には、
 * 本関数を実行し、累積された計測結果を一旦破棄する必要があります。
 */
void CRIAPI criAtom_ResetPerformanceMonitor(void);

/*JP
 * \brief パフォーマンス情報の取得
 * \ingroup ATOMLIB_GLOBAL
 * \par 説明:
 * パフォーマンス情報を取得します。<br>
 * 本関数は、 ::criAtom_AttachPerformanceMonitor 関数実行後から
 * ::criAtom_DetachPerformanceMonitor 関数を実行するまでの間、利用可能です。<br>
 * \sa criAtom_AttachPerformanceMonitor, criAtom_DetachPerformanceMonitor
 */
void CRIAPI criAtom_GetPerformanceInfo(CriAtomPerformanceInfo *info);

/*JP
 * \brief ADXデータのビットレート計算
 * \ingroup ATOMLIB_GLOBAL
 * \param[in]	num_channels	データのチャンネル数
 * \param[in]	sampling_rate	データのサンプリングレート
 * \return		CriSint32		ビットレート[bps]
 * \par 説明:
 * ADXデータのビットレートを計算します。<br>
 * 計算に失敗すると本関数は-1を返します。<br>
 * 計算に失敗した理由については、エラーコールバックのメッセージで確認可能です。<br>
 */
CriSint32 CRIAPI criAtom_CalculateAdxBitrate(
	CriSint32 num_channels, CriSint32 sampling_rate);

/*JP
 * \brief HCAデータのビットレート計算
 * \ingroup ATOMLIB_GLOBAL
 * \param[in]	num_channels	データのチャンネル数
 * \param[in]	sampling_rate	データのサンプリングレート
 * \param[in]	quality			データのエンコード品質
 * \return		CriSint32		ビットレート[bps]
 * \par 説明:
 * HCAデータのビットレートを計算します。<br>
 * 計算に失敗すると本関数は-1を返します。<br>
 * 計算に失敗した理由については、エラーコールバックのメッセージで確認可能です。<br>
 * \par 備考:
 * qualityにはCRI Atom CraftまたはCRI Atom Encoderで設定したエンコード品質を指定します。
 */
CriSint32 CRIAPI criAtom_CalculateHcaBitrate(
	CriSint32 num_channels, CriSint32 sampling_rate, CriAtomEncodeQuality quality);

/*JP
 * \brief HCA-MXデータのビットレート計算
 * \ingroup ATOMLIB_GLOBAL
 * \param[in]	num_channels	データのチャンネル数
 * \param[in]	sampling_rate	データのサンプリングレート
 * \param[in]	quality			データのエンコード品質
 * \return		CriSint32		ビットレート[bps]
 * \par 説明:
 * HCA-MXデータのビットレートを計算します。<br>
 * 計算に失敗すると本関数は-1を返します。<br>
 * 計算に失敗した理由については、エラーコールバックのメッセージで確認可能です。<br>
 * \par 備考:
 * qualityにはCRI Atom CraftまたはCRI Atom Encoderで設定したエンコード品質を指定します。
 */
CriSint32 CRIAPI criAtom_CalculateHcaMxBitrate(
	CriSint32 num_channels, CriSint32 sampling_rate, CriAtomEncodeQuality quality);




/*==========================================================================
 *      CRI Atom HCA-MX API
 *=========================================================================*/






/*==========================================================================
 *      CRI Atom Decrypter API
 *=========================================================================*/



/*==========================================================================
 *      CRI Atom AWB API
 *=========================================================================*/













/*==========================================================================
 *      CRI Atom Player API
 *=========================================================================*/


















/*JP
 * \brief 音声データのセット（音声データIDの指定）
 * \ingroup ATOMLIB_PLAYER
 * \param[in]	player		Atomプレーヤハンドル
 * \param[in]	awb			AWBハンドル
 * \param[in]	id			波形データID
 * \par 説明:
 * 再生する波形データをAtomプレーヤに関連付けます。<br>
 * 本関数にAWBハンドルと波形データIDを指定後、 ::criAtomPlayer_Start 関数で再生を
 * 開始すると、指定した波形データがストリーミング再生されます。<br>
 * 尚、本関数を実行した時点では、ファイルの読み込みは開始されません。<br>
 * ファイルの読み込みが開始されるのは、 ::criAtomPlayer_Start 関数実行後です。<br>
 * \par 例:
 * \code
 * main()
 * {
 * 		:
 * 	// AWBのTOC情報をロードしてAWBハンドルを作成
 * 	awb = criAtomAwb_LoadToc(NULL, "sample.awb", NULL, 0);
 * 		:
 * 	// 波形データをセット
 * 	// AWB内の1番の波形データをセット
 * 	criAtomPlayer_SetWaveId(player, awb, 1);
 *
 * 	// セットされた音声データを再生
 * 	criAtomPlayer_Start(player);
 * 		:
 * }
 * \endcode
 * 尚、一旦セットしたファイルの情報は、他のデータがセットされるまでAtomプレーヤ内に保持
 * されます。<br>
 * そのため、同じデータを何度も再生する場合には、再生毎にデータをセットしなおす必要
 * はありません。
 * \par 備考:
 * データ要求コールバック関数（ ::CriAtomPlayerDataRequestCbFunc ）内で本関数を実行すると、
 * 前回セットした音声の終端に連結して次のデータが再生されます。<br>
 * 例えば、以下のコードを実行すると、1番のデータと2番のデータとがシームレスに
 * 連結して再生されます。<br>
 * （その後は2番のデータが繰り返し再生されます。）<br>
 * \code
 * // データ要求コールバック関数
 * void on_data_request(void *obj, CriAtomPlayerHn player)
 * {
 * 	// AWBハンドルの取得
 * 	awb = (CriAtomAwbHn)obj;
 *
 * 	// 続けて再生するファイルをセット
 * 	// →AWB内の2番の波形データをセット
 * 	criAtomPlayer_SetWaveId(player, awb, 2);
 * }
 *
 * main()
 * {
 * 		:
 * 	// データ要求コールバック関数の登録
 * 	criAtomPlayer_SetDataRequestCallback(player, on_data_request, awb);
 *
 * 	// 波形データをセット
 * 	// →AWB内の1番の波形データをセット
 * 	criAtomPlayer_SetWaveId(player, awb, 1);
 *
 * 	// セットされた波形データを再生
 * 	criAtomPlayer_Start(player);
 * 		:
 * }
 * \endcode
 * データ要求コールバック関数内で ::criAtomPlayer_SetData 関数を実行することで、
 * ファイルとオンメモリデータを連結して再生することも可能です。
 * \attention
 * ファイルからの再生を行う場合には、ストリーミング再生に対応した
 * Atomプレーヤを使用する必要があります。<br>
 * （ ::CriAtomAdxPlayerConfig のstreaming_flagにCRI_TRUEを設定して
 * Atomプレーヤを作成する必要があります。）<br>
 * <br>
 * 本関数は停止中のプレーヤに対してのみ実行可能です。<br>
 * \sa CriAtomAdxPlayerConfig, criAtomPlayer_CreateAdxPlayer, criAtomPlayer_Start
 */
void CRIAPI criAtomPlayer_SetWaveId(
	CriAtomPlayerHn player, CriAtomAwbHn awb, CriSint32 id);

/*JP
 * \brief 同一音声データの再セット
 * \ingroup ATOMLIB_PLAYER
 * \param[in]	player		Atomプレーヤハンドル
 * \par 説明:
 * 前回再生したデータを、再度再生するようAtomプレーヤに指示します。<br>
 * \par 例:
 * 以下の処理により、同一音声データを無限にループ再生可能です。<br>
 * \code
 * // データ要求コールバック関数
 * void on_data_request(void *obj, CriAtomPlayerHn player)
 * {
 * 	// 前回再生したデータを再セット
 * 	criAtomPlayer_SetPreviousDataAgain(player);
 * }
 *
 * main()
 * {
 * 		:
 * 	// データ要求コールバック関数の登録
 * 	criAtomPlayer_SetDataRequestCallback(player, on_data_request, NULL);
 *
 * 	// 音声データをセット
 * 	criAtomPlayer_SetData(player, buffer, buffer_size);
 *
 * 	// セットされた音声データを再生
 * 	criAtomPlayer_Start(player);
 * 		:
 * }
 * \endcode
 * \attention
 * 本関数はデータ要求コールバック関数内でのみ使用します。<br>
 * （データ要求コールバック関数外でも実行可能ですが、その場合何の効果もありません。）<br>
 * \sa criAtomPlayer_SetDataRequestCallback
 */
void CRIAPI criAtomPlayer_SetPreviousDataAgain(CriAtomPlayerHn player);

/*JP
 * \brief コールバック関数の再実行要求
 * \ingroup ATOMLIB_PLAYER
 * \param[in]	player		Atomプレーヤハンドル
 * \par 説明:
 * データ要求コールバック関数の処理を先延ばしします。<br>
 * データ要求コールバック関数が実行された時点で、次に再生する音声データが
 * まだ決まっていない場合、本関数を実行することで、コールバック関数をリトライ
 * することが可能になります。<br>
 * （数ミリ秒後に再度データ要求コールバック関数が呼ばれます。）
 * \par 備考:
 * データ要求コールバック関数内で何もしなかった場合、Atomプレーヤのステータスは
 * CRIATOMPLAYER_STATUS_PLAYEND に遷移します。<br>
 * しかし、データ要求コールバック関数内で本関数を実行した場合、Atomプレーヤの
 * ステータスは CRIATOMPLAYER_STATUS_PLAYING を維持し続けます。
 * \attention
 * 本関数を実行した場合、Atomプレーヤのステータスは CRIATOMPLAYER_STATUS_PLAYING
 * を維持し続けますが、音声出力は途切れる可能性があります。<br>
 * （データの読み込みが間に合わない場合、前回再生した音声と、次にセットする音声
 * との間に、無音が入る可能性があります。）<br>
 * <br>
 * 本関数はデータ要求コールバック関数内でのみ使用可能です。<br>
 * （データ要求コールバック関数外で実行した場合、エラーが発生します。）<br>
 * \sa criAtomPlayer_SetDataRequestCallback
 */
void CRIAPI criAtomPlayer_DeferCallback(CriAtomPlayerHn player);
































/*==========================================================================
 *      Functions for HCA Streaming
 *=========================================================================*/


/*==========================================================================
 *      CRI Atom D-BAS API
 *=========================================================================*/
/*JP
 * \brief D-BAS作成用ワークサイズの計算
 * \ingroup ATOMLIB_DBAS
 * \param[in]	config		D-BAS作成用コンフィグ構造体へのポインタ
 * \return		CriSint32	D-BAS作成用ワークサイズ
 * \retval		0以上		正常に処理が完了
 * \retval		-1			エラーが発生
 * \par 説明:
 * D-BAS作成用パラメータに基づいて、D-BASの作成に必要ワークサイズを計算します。<br>
 * <br>
 * ワーク領域サイズの計算に失敗すると、本関数は -1 を返します。<br>
 * ワーク領域サイズの計算に失敗した理由については、エラーコールバックのメッセージで確認可能です。<br>
 * \attention
 * 本関数を実行する前に、ライブラリを初期化しておく必要があります。<br>
 * <br>
 * 設定によっては、D-BASが2GB以上のワークサイズを必要とする場合があり、<br>
 * その際はエラーになり、 -1 を返します。<br>
 * エラーが発生した場合は、max_streamsかmax_bpsの値を低く設定してください。<br>
 * \sa criAtomDbas_Create
 */
CriSint32 CRIAPI criAtomDbas_CalculateWorkSize(const CriAtomDbasConfig *config);

/*JP
 * \brief D-BASの作成
 * \ingroup ATOMLIB_DBAS
 * \param[in]	config			D-BAS作成用コンフィグ構造体へのポインタ
 * \param[in]	work			D-BAS作成用ワーク領域へのポインタ
 * \param[in]	work_size		D-BAS作成用ワークサイズ
 * \return		CriAtomDbasId	D-BAS管理用ID
 * \par 説明:
 * D-BAS作成用パラメータに基づいて、D-BASを作成します。<br>
 * 作成に成功すると、D-BASをライブラリに登録し、有効な管理用IDを返します。<br>
 * D-BASの作成に失敗した場合、本関数は ::CRIATOMDBAS_ILLEGAL_ID を返します。<br>
 * （エラーの原因はエラーコールバックに返されます。）<br>
 * <br>
 * 取得したIDは::criAtomDbas_Destroy 関数で使用します。<br>
 * \attention
 * 本関数を実行する前に、ライブラリを初期化しておく必要があります。<br>
 * \sa criAtomDbas_CalculateWorkSize, criAtomDbas_Destroy
 */
CriAtomDbasId CRIAPI criAtomDbas_Create(
	const CriAtomDbasConfig *config, void *work, CriSint32 work_size);

/*JP
 * \brief D-BASの破棄
 * \ingroup ATOMLIB_DBAS
 * \param[in]	atom_dbas_id	D-BAS管理用ID
 * \par 説明:
 * ::criAtomDbas_Create 関数で取得した管理用IDを指定して、D-BASを破棄します。<br>
 * \sa criAtomDbas_Create
 */
void CRIAPI criAtomDbas_Destroy(CriAtomDbasId atom_dbas_id);


/* ========================================================================*/
/*       CRI Atom Streaming Cache API                                      */
/* ========================================================================*/






/*==========================================================================
 *      CRI Atom DSP API
 *=========================================================================*/







/* ========================================================================*/
/*       CRI Atom Meter API                                                */
/* ========================================================================*/

/*JP
 * \brief レベルメータ機能用のワークサイズの計算
 * \ingroup ATOMLIB_METER
 * \param[in]	config			レベルメータ追加用のコンフィグ構造体
 * \return		CriSint32		必要なワーク領域サイズ
 * \par 説明:
 * レベルメータ追加に必要なワーク領域サイズを計算します。<br>
 * config にNULLを指定するとデフォルト設定で計算されます。<br>
 * \sa criAtomMeter_AttachLevelMeter
 */
CriSint32 CRIAPI criAtomMeter_CalculateWorkSizeForLevelMeter(
	const CriAtomLevelMeterConfig *config);

/*JP
 * \brief レベルメータ機能の追加
 * \ingroup ATOMLIB_METER
 * \param[in]	config			レベルメータ追加用のコンフィグ構造体
 * \param[in]	work			ワーク領域
 * \param[in]	work_size		ワーク領域サイズ
 * \par 説明:
 * ライブラリにレベルメータ機能を追加します。<br>
 * config にNULLを指定するとデフォルト設定でレベルメータが追加されます。<br>
 * work にNULL、work_size に0を指定すると、登録されたユーザアロケータによって
 * ワーク領域が確保されます。<br>
 * \sa criAtomMeter_GetLevelInfo
 */
void CRIAPI criAtomMeter_AttachLevelMeter(
	const CriAtomLevelMeterConfig *config, void *work, CriSint32 work_size);

/*JP
 * \brief レベルメータ機能の解除
 * \ingroup ATOMLIB_METER
 * \par 説明:
 * ライブラリのレベルメータ機能を解除します。<br>
 * \sa criAtomMeter_AttachLevelMeter
 */
void CRIAPI criAtomMeter_DetachLevelMeter(void);

/*JP
 * \brief レベル情報の取得
 * \ingroup ATOMLIB_METER
 * \param[out]	info			レベル情報の構造体
 * \par 説明:
 * レベルメータの結果を取得します。<br>
 * 指定するバスには ::criAtomMeter_AttachLevelMeter 関数であらかじめ
 * レベルメータ機能を追加しておく必要があります。<br>
 * \sa criAtomMeter_AttachLevelMeter
 */
void CRIAPI criAtomMeter_GetLevelInfo(CriAtomLevelInfo *info);


/*JP
 * \brief ラウドネスメータ機能の追加
 * \ingroup ATOMLIB_METER
 * \param[in]	config			ラウドネスメータ追加用のコンフィグ構造体
 * \param[in]	work			ワーク領域
 * \param[in]	work_size		ワーク領域サイズ
 * \par 説明:
 * ライブラリにITU-R BS.1770-3規格のラウドネスメータ機能を追加します。<br>
 * config にNULLを指定するとデフォルト設定でラウドネスメータが追加されます。<br>
 * work にNULL、work_size に0を指定すると、登録されたユーザアロケータによって
 * ワーク領域が確保されます。<br>
 * \sa criAtomMeter_GetLoudnessInfo
 */
void CRIAPI criAtomMeter_AttachLoudnessMeter(const CriAtomLoudnessMeterConfig *config, void *work, CriSint32 work_size);

/*JP
 * \brief ラウドネスメータ機能の解除
 * \ingroup ATOMLIB_METER
 * \par 説明:
 * ライブラリのラウドネスメータ機能を解除します。<br>
 * \sa criAtomMeter_AttachLoudnessMeter
 */
void CRIAPI criAtomMeter_DetachLoudnessMeter(void);







/***************************************************************************
 *      暫定API（使用はお控え下さい）
 *      Tentative API (Please don't use it)
 ***************************************************************************/
/* 以下に宣言されている関数はAtomの内部処理用、ゲームエンジンとの連携等にて
 * 暫定的に使用されているAPIです。
 * 今後のアップデートにて予告なく変更・削除が行われるため、使用はお控え下さい。
 */
typedef void (CRIAPI *CriAtomPlayerDataEndCbFunc)(void *obj, void *buffer, CriSint32 size);
typedef void (CRIAPI *CriAtomPlayerCbFunc)(void *obj, CriAtomPlayerHn player);

#ifdef __cplusplus
}
#endif


/* レンダラーの出力Ch数設定に応じて、自動でダウンミックスするため下記関数は無効にしています */
#define criAtomHcaMx_SetBusSendLevel(mixer_id, bus_no, level) \
	criAtomExHcaMx_SetBusSendLevel(mixer_id, bus_no, level)

#endif	/* CRI_INCL_CRI_ATOM_H */

/* --- end of file --- */
