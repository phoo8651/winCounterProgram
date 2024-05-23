#pragma warning(disable:4996)
#include "RSAHandler.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <fstream>
#include <stdexcept>
#include <vector>

// 파일 내용을 문자열로 읽는 함수
std::string readFile(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::in | std::ios::binary);
    if (!file) {
        throw std::runtime_error("Unable to open file: " + fileName);
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// 공개 키를 로드하는 함수
void RSAHandler::loadPublicKey(const std::string& publicKeyFile) {
    publicKey = readFile(publicKeyFile);
}

// RSA 공개 키로 데이터를 암호화하는 함수
std::string RSAHandler::encrypt(const std::string& data) {
    BIO* keybio = BIO_new_mem_buf(publicKey.data(), static_cast<int>(publicKey.size()));
    if (keybio == NULL) {
        throw std::runtime_error("Failed to create key BIO");
    }

    RSA* rsa = PEM_read_bio_RSA_PUBKEY(keybio, NULL, NULL, NULL);
    if (rsa == NULL) {
        BIO_free(keybio);
        throw std::runtime_error("Failed to create RSA");
    }

    int rsaLen = RSA_size(rsa);
    std::vector<unsigned char> encryptedText(rsaLen);

    // Use RSA_padding_add_PKCS1_OAEP_mgf1 instead of RSA_public_encrypt
    if (!RSA_padding_add_PKCS1_OAEP_mgf1(encryptedText.data(), rsaLen, reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), NULL, 0, EVP_sha256(), EVP_sha256())) {
        RSA_free(rsa);
        BIO_free(keybio);
        throw std::runtime_error("Failed to encrypt message");
    }

    int result = RSA_private_encrypt(rsaLen, encryptedText.data(), encryptedText.data(), rsa, RSA_NO_PADDING);
    RSA_free(rsa);
    BIO_free(keybio);

    if (result == -1) {
        throw std::runtime_error("Failed to encrypt message");
    }

    return std::string(encryptedText.begin(), encryptedText.end());
}
