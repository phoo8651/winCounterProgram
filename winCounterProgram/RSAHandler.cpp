#pragma warning(disable:4996)
#include "RSAHandler.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <fstream>
#include <stdexcept>
#include <vector>

// ���� ������ ���ڿ��� �д� �Լ�
std::string readFile(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::in | std::ios::binary);
    if (!file) {
        throw std::runtime_error("Unable to open file: " + fileName);
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// ���� Ű�� �ε��ϴ� �Լ�
void RSAHandler::loadPublicKey(const std::string& publicKeyFile) {
    publicKey = readFile(publicKeyFile);
}

// RSA ���� Ű�� �����͸� ��ȣȭ�ϴ� �Լ�
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

    int result = RSA_public_encrypt(static_cast<int>(data.size()), reinterpret_cast<const unsigned char*>(data.c_str()), encryptedText.data(), rsa, RSA_PKCS1_PADDING);
    RSA_free(rsa);
    BIO_free(keybio);

    if (result == -1) {
        throw std::runtime_error("Failed to encrypt message");
    }

    return std::string(encryptedText.begin(), encryptedText.end());
}
