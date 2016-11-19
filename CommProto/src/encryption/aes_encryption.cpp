#include <CommProto/encryption/aes_encryption.h>//header file
#include <CommProto/architecture/macros.h>//str_length
#include <CommProto/debug/comms_debug.h>//COMMS_DEBUG
#include <fstream>//file input

using namespace comnet;

AesEncryption::AesEncryption() :randomGen(0, 255){}

AesEncryption::~AesEncryption(){}

/** input c string as the form of encrytion key*/
uint8_t AesEncryption::LoadKey(char* key){	
	byte temp_key[KEY_LENGTH] = { 0 };
	uint16_t length = 0;
	str_length(key,length);
	if (length >= KEY_LENGTH){		
		memcpy(&temp_key, key, KEY_LENGTH);//copy key method want byte or constant *byte not char*
		sec_key = CryptoPP::SecByteBlock(temp_key, KEY_LENGTH);
	}else{//key lenth too small
		COMMS_DEBUG("Encryption Key length is too small. AesEncryption::LoadKey(char* key)\n");
		COMMS_DEBUG("Wanted key size %d read key size of %d\n", KEY_LENGTH, length);
		return 0;
	}
	return 1;//success
}

/** load file which contatins the encryption key by the file name*/
uint8_t AesEncryption::LoadKeyFromFile(char*keyFileName){
	std::ifstream keyFileInput(keyFileName);//open file
	std::string inputString;//string to read line from file
	char temp_key[KEY_LENGTH];//temp key to be inserted from file line

	if (keyFileInput.is_open()){
		std::getline(keyFileInput, inputString);//get line

		if (inputString.length() >= KEY_LENGTH){//translate line of file to key length as char*
			for (int x = 0; x < KEY_LENGTH; x++){
				temp_key[x] = inputString[x];
			}
		}
		else{//key length is too small
			COMMS_DEBUG("key.txt characters mismatch.\nCharacters found: %d\nCharacters needed: %d\n", inputString.length(), KEY_LENGTH);
			return 0;
		}
	}
	else{//file not open	
		COMMS_DEBUG("%s not found. \n", temp_key);
		return 0;
	}
	keyFileInput.close();
	return LoadKey(temp_key);//load key from temp key
}

/** Encrypt buffer for desired length of data stream and return any agumented legnth by reference
A return value of - value is an error */
int32_t AesEncryption::Encrypt(uint8_t* buffer, uint32_t& length, uint8_t iv[BLOCK_SIZE]){
	CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfbEncryption(sec_key, sec_key.size(), iv);
	cfbEncryption.ProcessData((byte*)buffer, (byte*)buffer, length);
	return 1;
}

/** Decrypt buffer for desired length of data stream and return any agumented legnth by reference
A return value of - value is an error */
int32_t AesEncryption::Decrypt(uint8_t* buffer, uint32_t& length, uint8_t iv[BLOCK_SIZE]){
	CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cfbDecryption(sec_key, sec_key.size(), iv);
	cfbDecryption.ProcessData((byte*)buffer, (byte*)buffer, length);
	return 1; 
}

/** Randome numbder generator which fills an array of size legnth*/
uint8_t AesEncryption::GenerateRandomIV(uint8_t * buffer, uint32_t length){	
	for (int x = 0; x < length; x++){
		buffer[x] = randomGen.randomUint8();
	}
	return 1;
}