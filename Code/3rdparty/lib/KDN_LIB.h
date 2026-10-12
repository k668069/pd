/***********************************************************************
* 파 일 명 : KDN_LIB.h
*
* 파일설명 : 암호모듈에서 사용되는 변수 선언 및 인터페이스 정의
*-----------------------------------------------------------------------
* 작 성 일 : 2019. 12. 12 (최초인증, ePower Crypto V1.1)
*
* 수 정 일 : 2021. 05. 31 (재인증, ePower Crypto V2.0)
*
*-----------------------------------------------------------------------
* 비	고 : 
*
***********************************************************************/

#ifndef _KDN_LIB_H
	#define _KDN_LIB_H

	#ifdef KDN_WIN
		#define KDN_MODULE_NAME "epowercrypto.dll"
	#elif defined(_LINUX_)
		#define KDN_MODULE_NAME "libepowercrypto.so"
	#endif

	#ifdef __cplusplus
	extern "C" {
	#endif


	/////////////////////////////// Definitions ////////////////////////////////
	// BLOCK
	#define KDN_BC_Algo_ARIA_ECB		  		    0x00
	#define KDN_BC_Algo_ARIA_CBC		  		    0x01
	#define KDN_BC_Algo_ARIA_CTR		  		    0x02
	#define KDN_BC_Algo_ARIA_CCM		  		    0x03
	#define KDN_BC_Algo_ARIA_GCM		  		    0x04

	#define KDN_BC_Algo_LEA_ECB						0x05
	#define KDN_BC_Algo_LEA_CBC						0x06
	#define KDN_BC_Algo_LEA_CTR						0x07
	#define KDN_BC_Algo_LEA_CCM						0x08
	#define KDN_BC_Algo_LEA_GCM						0x09

	#define KDN_BC_Algo_ARIA_MASK_ECB				0x10
	#define KDN_BC_Algo_ARIA_MASK_CBC				0x11
	#define KDN_BC_Algo_ARIA_MASK_CTR				0x12
	#define KDN_BC_Algo_ARIA_MASK_CCM				0x13
	#define KDN_BC_Algo_ARIA_MASK_GCM				0x14

	#define KDN_BC_Algo_LEA_MASK_ECB			    0x15
	#define KDN_BC_Algo_LEA_MASK_CBC			    0x16
	#define KDN_BC_Algo_LEA_MASK_CTR			    0x17
	#define KDN_BC_Algo_LEA_MASK_CCM			    0x18
	#define KDN_BC_Algo_LEA_MASK_GCM			    0x19

	#define KDN_BC_Algo_ARIA_ECB_NOPADDING			0x20
	#define KDN_BC_Algo_ARIA_CBC_NOPADDING			0x21	

	#define KDN_BC_Algo_LEA_ECB_NOPADDING			0x22
	#define KDN_BC_Algo_LEA_CBC_NOPADDING			0x23		

	#define KDN_BC_Algo_ARIA_MASK_ECB_NOPADDING		0x24
	#define KDN_BC_Algo_ARIA_MASK_CBC_NOPADDING		0x25

	#define KDN_BC_Algo_LEA_MASK_ECB_NOPADDING		0x26
	#define KDN_BC_Algo_LEA_MASK_CBC_NOPADDING		0x27

	// HASH
	#define KDN_SHA_256					     		0x30
	#define KDN_SHA_384					     		0x31
	#define KDN_SHA_512					     		0x32

	// HMAC
	#define KDN_HMAC_SHA_256				 		0x40
	#define KDN_HMAC_SHA_384				 		0x41
	#define KDN_HMAC_SHA_512				 		0x42

	// DRBG
	#define KDN_DRBG_CTR_MODE_ARIA128		 		0x50

	// KEY GENERATION
	#define KDN_SYM_KEY_GEN				     		0x60
	#define KDN_HMAC_KEY_GEN				 		0x61

	// KEY Length
	#define KDN_SYM_KEY_LEN_128				 128
	#define KDN_SYM_KEY_LEN_192				 192
	#define KDN_SYM_KEY_LEN_256				 256

	#define KDN_SYM_KEY_BYTE_LEN_16			 16
	#define KDN_SYM_KEY_BYTE_LEN_24			 24
	#define KDN_SYM_KEY_BYTE_LEN_32			 32
	
	// Return Value : 성공
	#define KDN_OK 0

	typedef unsigned char uint8;
	typedef uint8* uint8_ptr;
	typedef unsigned int uint32;
	typedef int	int32;

	// KDN library의 버전 정보
	typedef struct _KDN_VERSION
	{
	  uint8 major;						    // Integer Portion of
											// Version Number
	  uint8 minor;						    // 1/100ths Portion of
											// Version Number
	} KDN_VERSION, *_KL_VERSION_PTR;

	// KDN library의 세부 정보
	typedef struct _KDN_INFO
	{
	  KDN_VERSION KdnapiVersion;		  // Interface Version
	  uint8	libraryName[32];		      // Library Name
	  KDN_VERSION libraryVersion;		  // Library Version
	} KDN_INFO, *KDN_INFO_PTR;

	/////////////////////////////// CMVP States ////////////////////////////////
	enum KLS_CMVP_STATE
	{
		KDN_MODULE_LOADED = 1,			    // 호출성공상태
		KDN_MODULE_APPROVAL,				// 동작상태
		KDN_MODULE_SELFTEST,				// 자가시험 동작상태
		KDN_MODULE_FATAL_ERROR,			    // 오류상태
		KDN_MODULE_TERMINATE,			    // 종료상태
	};


	///////////////////////// 모듈 일반적인 에러 코드 //////////////////////////
	enum KDN_ERR_MODULE
	{
		KDN_ERR_ALLOC_FAILED = 1000,		// 메모리 할당 실패
		KDN_MODULE_ERROR_STATE,			    // 모듈이 에러상태에 있음
		KDN_MODULE_FINAL_STATE,			    // 모듈이 종료상태에 있음
		KDN_MODULE_STATE_UNDEFINED,		    // 정의되지 않은 상태임
		KDN_MODULE_ARGUMENT_BAD,			// 입력값 에러 : 포인터가 NULL인 경우
		KDN_MODULE_DRBG_SELFTEST_FAILED,	// 난수발생기 자가시험 실패
		KDN_MODULE_ENTROPY_SELFTEST_FAILED, // 엔트로피 자가시험 실패
		KDN_MODULE_BLOCK_SELFTEST_FAILED,   // 블록암호 자가시험 실패
		KDN_MODULE_HASH_SELFTEST_FAILED,	// HASH 자가시험 실패
		KDN_MODULE_HMAC_SELFTEST_FAILED,	// HMAC 자가시험 실패
		KDN_MODULE_ECDH_SELFTEST_FAILED,	// ECDH 자가시험 실패
		KDN_MODULE_ECDSA_SELFTEST_FAILED,   // ECDSA 자가시험 실패
		KDN_MODULE_ECKCDSA_SELFTEST_FAILED, // ECKCDSA 자가시험 실패
		KDN_MODULE_INTEGRITY_CHECK_FAILED,  // 모듈 무결성 검사 실패
		KDN_MODULE_DATE_EXPIRED,			// 모듈 사용 유효기간 초과
		KDN_MODULE_EXPIRED_DATE_NOT_FOUND,  // 모듈 사용 유효기간을 확인할 수 없는 경우
		KDN_MODULE_NOT_INITIALIZED,		    // 모듈이 초기화 되지 않음
		UNKNOWN_ERROR,					    // 정의되지 않은 에러임
		KDN_ERR_UNSUPPORTED_ALGO,		    // 지원하지 않는 알고리즘
	};


	//////////////////////////// 블록암호 에러 코드 ////////////////////////////
	enum KDN_ERR_BLOCK
	{
		KDN_ERR_BC_UNSUPPORTED_ALGO = 2000,
		KDN_ERR_BC_INPUT_NOT_INITIALIZED,
		KDN_ERR_BC_OUTPUT_NOT_INITIALIZED,
		KDN_ERR_BC_KEYLENGTH_INVALID,
		KDN_ERR_BC_ENCRYPT_FAILED,
		KDN_ERR_BC_DECRYPT_FAILED,
		KDN_ERR_BC_INLEN_NOT_MULBLOCK,
	};


	//////////////////////////// 해시함수 에러 코드 ////////////////////////////
	enum KDN_ERR_SHA
	{
		KDN_ERR_SHA_UNSUPPORTED_ALGO = 3000,
		KDN_ERR_SHA_INPUT_NOT_INITIALIZED,
		KDN_ERR_SHA_OUTPUT_NOT_INITIALIZED,
	};


	//////////////////////////// MAC 함수 에러 코드 ////////////////////////////
	enum KDN_ERR_MAC
	{
		KDN_ERR_MAC_INPUT_NOT_INITIALIZED = 4000,
		KDN_ERR_MAC_OUTPUT_NOT_INITIALIZED,
		KDN_ERR_MAC_KEY_NOT_INITIALIZED,
		KDN_ERR_MAC_INVALID_TAGLEN,
		KDN_ERR_MAC_INVALID_KEYLENGTH,
		KDN_ERR_MAC_VERIFICATION_FAILED,
		KDN_ERR_MAC_INVALID_MODE,
	};


	///////////////////////////// ECDSA 에러 코드 //////////////////////////////
	enum KDN_ERR_ECDSA
	{
		KDN_ERR_ECDSA_KEYGEN_PUBKEY_NOT_INITIALIZED = 5000,
		KDN_ERR_ECDSA_KEYGEN_PRIKEY_NOT_INITIALIZED,
		KDN_ERR_ECDSA_KEYGEN_FAILED,
		KDN_ERR_ECDSA_KEYPAIR_SELFTEST_FAILED,

		KDN_ERR_ECDSA_SIGN_RS_NOT_INITIALIZED,
		KDN_ERR_ECDSA_SIGN_RSLEN_INVALID,
		KDN_ERR_ECDSA_SIGN_PRIKEY_NOT_INITIALIZED,
		KDN_ERR_ECDSA_SIGN_MESSAGE_NOT_INITIALIZED,
		KDN_ERR_ECDSA_SIGN_MESSAGELEN_INVALID,
		KDN_ERR_ECDSA_SIGN_QGEN_INVALID,
		KDN_ERR_ECDSA_SIGN_NEED_NEW_K,

		KDN_ERR_ECDSA_VERIFY_RS_NOT_INITIALIZED,
		KDN_ERR_ECDSA_VERIFY_RSLEN_INVALID,
		KDN_ERR_ECDSA_VERIFY_PUBKEY_NOT_INITIALIZED,
		KDN_ERR_ECDSA_VERIFY_MESSAGE_NOT_INITIALIZED,
		KDN_ERR_ECDSA_VERIFY_MESSAGELEN_INVALID,
		KDN_ERR_ECDSA_VERIFY_PUBKEY_INVALID,
		KDN_ERR_ECDSA_VERIFY_RS_INVALID,
		KDN_ERR_ECDSA_VERIFY_QGEN_INVALID,
		KDN_ERR_ECDSA_VERIFY_FAILED,
	};


	//////////////////////////// EC-KSDSA 에러 코드 ////////////////////////////
	enum KDN_ERR_ECKCDSA
	{
		KDN_ERR_ECKCDSA_KEYGEN_PUBKEY_NOT_INITIALIZED = 6000,
		KDN_ERR_ECKCDSA_KEYGEN_PRIKEY_NOT_INITIALIZED,
		KDN_ERR_ECKCDSA_KEYGEN_FAILED,
		KDN_ERR_ECKCDSA_KEYPAIR_SELFTEST_FAILED,

		KDN_ERR_ECKCDSA_SIGN_RS_NOT_INITIALIZED,
		KDN_ERR_ECKCDSA_SIGN_RSLEN_INVALID,
		KDN_ERR_ECKCDSA_SIGN_PUBKEY_NOT_INITIALIZED,
		KDN_ERR_ECKCDSA_SIGN_PRIKEY_NOT_INITIALIZED,
		KDN_ERR_ECKCDSA_SIGN_MESSAGE_NOT_INITIALIZED,
		KDN_ERR_ECKCDSA_SIGN_MESSAGELEN_INVALID,
		KDN_ERR_ECKCDSA_SIGN_PUBKEY_INVALID,
		KDN_ERR_ECKCDSA_SIGN_QGEN_INVALID,
		KDN_ERR_ECKCDSA_SIGN_NEED_NEW_K,

		KDN_ERR_ECKCDSA_VERIFY_RS_NOT_INITIALIZED,
		KDN_ERR_ECKCDSA_VERIFY_RSLEN_INVALID,
		KDN_ERR_ECKCDSA_VERIFY_PUBKEY_NOT_INITIALIZED,
		KDN_ERR_ECKCDSA_VERIFY_MESSAGE_NOT_INITIALIZED,
		KDN_ERR_ECKCDSA_VERIFY_MESSAGELEN_INVALID,
		KDN_ERR_ECKCDSA_VERIFY_PUBKEY_INVALID,
		KDN_ERR_ECKCDSA_VERIFY_RS_INVALID,
		KDN_ERR_ECKCDSA_VERIFY_QGEN_INVALID,
		KDN_ERR_ECKCDSA_VERIFY_FAILED,
	};


	////////////////////////////// ECDH 에러 코드 //////////////////////////////
	enum KDN_ERR_ECDH
	{
		KDN_ERR_ECDH_KT_TOKEN_NOT_INITIALIZED = 7000,
		KDN_ERR_ECDH_KT_RAND_NOT_INITIALIZED,
		KDN_ERR_ECDH_KT_INVALID,
		KDN_ERR_ECDH_KEYPAIR_SELFTEST_FAILED,

		KDN_ERR_ECDH_SK_KEY_NOT_INITIALIZED,
		KDN_ERR_ECDH_SK_TOKEN_NOT_INITIALIZED,
		KDN_ERR_ECDH_SK_RAND_NOT_INITIALIZED,
		KDN_ERR_ECDH_SK_RANDLEN_INVALID,
		KDN_ERR_ECDH_SK_TOKEN_INVALID,
		KDN_ERR_ECDH_SK_INVALID,
	};


	////////////////////////////// DRBG 에러 코드 //////////////////////////////
	enum KDN_ERR_DRBG
	{
		KDN_ERR_DRBG_RANDOM_LENGTH_INVALID = 8000,
		KDN_ERR_DRBG_ARGUMENTS_BAD,
		KDN_ERR_DRBG_GEN_RANDOM_FAILED,
		KDN_ERR_DRBG_RESEED_ENTROPY_FAILED,
		KDN_ERR_DRBG_ENTROPY_HEALTH_TEST_FAILED,
	};


	//////////////////////////////// Functions /////////////////////////////////
	// CMVP Module Management
	uint32 KDN_Start();
	
	uint32 KDN_Terminate();

	uint32 KDN_CurrState();

	uint32 KDN_SelfTest();

	uint32 KDN_GetInfo(
		   KDN_INFO_PTR pInfo);

	uint8_ptr KDN_GetErrorMsg(
			  uint32 errorcode);

	// Key Generation
	uint32 KDN_Keygen(
		   uint8  *pKey,
		   uint32 Key_length,
		   uint32 Mode);

	// BLOCK
	uint32 KDN_BC_Encrypt(
		   uint8  *pOutput,
		   uint32 *pOutput_length,
		   uint8  *pInput,
		   uint32 Input_length,
		   uint8  *pKey,
		   uint32 Key_length,
		   uint32 Mode,
		   uint8  *pIV,
		   uint32 IV_length,
		   uint8  *pAuth,
		   uint32 Auth_length,
		   uint8  *pTag,
		   uint32 Tag_length);

	uint32 KDN_BC_Decrypt(
		   uint8  *pOutput,
		   uint32 *pOutput_length,
		   uint8  *pInput,
		   uint32 Input_length,
		   uint8  *pKey,
		   uint32 Key_length,
		   uint32 Mode,
		   uint8  *pIV,
		   uint32 IV_length,
		   uint8  *pAuth,
		   uint32 Auth_length,
		   uint8  *pTag,
		   uint32 Tag_length);

	// HASH
	uint32 KDN_Digest(
		   uint8  *pOutput,
		   uint8  *pInput,
		   uint32 Input_length,
		   uint32 Mode);

	// HMAC
	uint32 KDN_Gen_MAC(
		   uint8  *pMAC,
		   uint32 *MAC_length,
		   uint8  *pInput,
		   uint32 Input_length,
		   uint8  *pKey,
		   uint32 Key_length,
		   uint32 Mode);
	
	uint32 KDN_Verify_MAC(
		   uint8  *pMAC,
		   uint32 *MAC_length,
		   uint8  *pInput,
		   uint32 Input_length,
		   uint8  *pKey,
		   uint32 Key_length,
		   uint32 Mode);

	// ECDSA
	uint32 KDN_Keygen_ECDSA(
		   uint8  *pPubKey_x,
		   uint8  *pPubKey_y,
		   uint8  *pPriKey);

	uint32 KDN_Sign_ECDSA(
		   uint8  *pR,
		   int32  *pR_length,
		   uint8  *pS,
		   int32  *pS_length,
		   uint8  *pInput,
		   int32  Input_length,		   
		   uint8  *pPriKey);

	uint32 KDN_Verify_ECDSA(
		   uint8  *pR,
		   int32  pR_length,
		   uint8  *pS,
		   int32  pS_length,
		   uint8  *pInput,
		   int32  Input_length,
		   uint8  *pPubKey_x,
		   uint8  *pPubKey_y);

	// ECKCDSA
	uint32 KDN_Keygen_ECKCDSA(
		   uint8  *pPubKey_x,
		   uint8  *pPubKey_y,
		   uint8  *pPriKey);

	uint32 KDN_Sign_ECKCDSA(
		   uint8  *pR,
		   int32  *pR_length,
		   uint8  *pS,
		   int32  *pS_length,
		   uint8  *pInput,
		   int32  Input_length,
		   uint8  *pPriKey,
		   uint8  *pPubKey_x,
		   uint8  *pPubKey_y);

	uint32 KDN_Verify_ECKCDSA(
		   uint8  *pR,
		   int32  pR_length,
		   uint8  *pS,
		   int32  pS_length,
		   uint8  *pInput,
		   int32  Input_length,
		   uint8  *pPubKey_x,
		   uint8  *pPubKey_y);

	// ECDH
	uint32 KDN_KT_ECDH(
		   uint8  *pPubKey_x,
		   uint8  *pPubKey_y,
		   uint8  *pPriKey);

	uint32 KDN_SK_ECDH(
		   uint8 *pSKx,
		   uint8 *pSKy,
		   uint8 *pPubKey_x,
		   uint8 *pPubKey_y,
		   uint8 *pPriKey,
		   int32 PriKey_length);

	// DRBG
	uint32 KDN_Gen_RandNumber(
		   uint8 *pRand,
		   int32 Rand_length);

	uint32 KDN_Reseed_Entropy();

	uint32 KDN_Key_Encrypt(
		   uint8* Output,
		   uint32* Output_length,
		   uint8* Input,
		   uint32 Input_length);

	uint32 KDN_Key_Decrypt(
		   uint8* Output,
		   uint32* Ouput_length,
		   uint8* Input,
		   uint32 Input_length);


	//////////////////// Internal Functions for Self-test //////////////////////
	uint32 _int_KDN_BC_Encrypt(
		   uint8  *pOutput,
		   uint32 *pOutput_length,
		   uint8  *pInput,
		   uint32 Input_length,
		   uint8  *pKey,
		   uint32 Key_length,
		   uint32 Mode,
		   uint8  *pIV,
		   uint32 IV_length,
		   uint8  *pAuth,
		   uint32 Auth_length,
		   uint8  *pTag,
		   uint32 Tag_length);

	uint32 _int_KDN_BC_Decrypt(
		   uint8  *pOutput,
		   uint32 *pOutput_length,
		   uint8  *pInput,
		   uint32 Input_length,
		   uint8  *pKey,
		   uint32 Key_length,
		   uint32 Mode,
		   uint8  *pIV,
		   uint32 IV_length,
		   uint8  *pAuth,
		   uint32 Auth_length,
		   uint8  *pTag,
		   uint32 Tag_length);

	uint32 _int_KDN_Gen_RandNumber(
		   uint8 *pRand,
		   int32 Rand_length);

	uint32 _int_KDN_Digest(
		   uint8  *pOutput,
		   uint8  *pInput,
		   uint32 Input_length,
		   uint32 Mode);

	uint32 _int_KDN_Gen_MAC(
		   uint8  *pMAC,
		   uint32 *MAC_length,
		   uint8  *pInput,
		   uint32 Input_length,
		   uint8  *pKey,
		   uint32 Key_length,
			uint32 Mode);

	uint32 _int_KDN_Verify_MAC(
		   uint8  *pMAC,
		   uint32 *MAC_length,
		   uint8  *pInput,
		   uint32 Input_length,
		   uint8  *pKey,
		   uint32 Key_length,
		uint32 Mode);

	uint32 _int_KDN_KT_ECDH(
		   uint8 *pPubKey_x,
		   uint8 *pPubKey_y,
		   uint8 *pPriKey,
		   uint8 PriKey_length);

	uint32 _int_KDN_SK_ECDH(
		   uint8 *pSKx,
		   uint8 *pSKy,
		   uint8 *pPubKey_x,
		   uint8 *pPubKey_y,
		   uint8 *pPriKey,
		   int32 PriKey_length);

	uint32 _int_KDN_Sign_ECDSA(
		   uint8  *pR,
		   int32  *pR_length,
		   uint8  *pS,
		   int32  *pS_length,
		   uint8  *pInput,
		   int32  Input_length,
		   uint8  *pRand,
		   uint8  *pPriKey);

	uint32 _int_KDN_Verify_ECDSA(
		   uint8  *pR,
		   int32  pR_length,
		   uint8  *pS,
		   int32  pS_length,
		   uint8  *pInput,
		   int32  Input_length,
		   uint8  *pPubKey_x,
		   uint8  *pPubKey_y);

	uint32 _int_KDN_Sign_ECKCDSA(
		   uint8  *pR,
		   int32  *pR_length,
		   uint8  *pS,
		   int32  *pS_length,
		   uint8  *pInput,
		   int32  Input_length,
		   uint8  *pRand,
		   uint8  *pPriKey,
		   uint8  *pPubKey_x,
		   uint8  *pPubKey_y);

	uint32 _int_KDN_Verify_ECKCDSA(
		   uint8  *pR,
		   int32  pR_length,
		   uint8  *pS,
		   int32  pS_length,
		   uint8  *pInput,
		   int32  Input_length,
		   uint8  *pPubKey_x,
		   uint8  *pPubKey_y);

	uint32 _int_ReadTimeFile();

	#ifdef __cplusplus
			}
	#endif

#endif

// EXPIRATION DATE : 2023-08-18
// EX_DATE oTCdWDZQBJxIKf0oE+EhNPPSHkDYrDR1JuEM82h0NXaldv3gBSPE0V2PsrNurksV 
