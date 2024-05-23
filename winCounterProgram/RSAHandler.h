#ifndef RSAHANDLER_H
#define RSAHANDLER_H

#include <string>

class RSAHandler {
public:
    // 공개 키 파일에서 공개 키를 읽는 함수
    void loadPublicKey(const std::string& publicKeyFile);

    // 주어진 공개 키로 데이터를 암호화하는 함수
    std::string encrypt(const std::string& data);

private:
    std::string publicKey;
};

#endif // RSAHANDLER_H
