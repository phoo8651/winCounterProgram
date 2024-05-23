#ifndef RSAHANDLER_H
#define RSAHANDLER_H

#include <string>

class RSAHandler {
public:
    // ���� Ű ���Ͽ��� ���� Ű�� �д� �Լ�
    void loadPublicKey(const std::string& publicKeyFile);

    // �־��� ���� Ű�� �����͸� ��ȣȭ�ϴ� �Լ�
    std::string encrypt(const std::string& data);

private:
    std::string publicKey;
};

#endif // RSAHANDLER_H
