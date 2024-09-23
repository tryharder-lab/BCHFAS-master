#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>
#include <windows.h>
#include <openssl/crypto.h>
#include<openssl/err.h>
#include <cfloat>
#include <openssl/sha.h>
#define AES_KEY_SIZE 32  // AES-256 密钥长度为 32 字节
#define AES_BLOCK_SIZE 16  // AES 块大小为 16 字节

void handleErrors() {
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
}

// 加密函数
void encrypt(const unsigned char* plaintext, int plaintext_len, const unsigned char* key, unsigned char* ciphertext) {
    EVP_CIPHER_CTX* ctx;

    // 创建并初始化加密上下文
    ctx = EVP_CIPHER_CTX_new();
    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, NULL)) {
        handleErrors();
    }

    // 执行加密操作
    int ciphertext_len = 0;
    if (!EVP_EncryptUpdate(ctx, ciphertext, &ciphertext_len, plaintext, plaintext_len)) {
        handleErrors();
    }

    // 完成加密
    int final_len = 0;
    if (!EVP_EncryptFinal_ex(ctx, ciphertext + ciphertext_len, &final_len)) {
        handleErrors();
    }
    ciphertext_len += final_len;

    // 释放加密上下文
    EVP_CIPHER_CTX_free(ctx);
}

// 解密函数
void decrypt(const unsigned char* ciphertext, int ciphertext_len, const unsigned char* key, unsigned char* plaintext) {
    EVP_CIPHER_CTX* ctx;

    // 创建并初始化解密上下文
    ctx = EVP_CIPHER_CTX_new();
    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, NULL)) {
        handleErrors();
    }

    // 执行解密操作
    int plaintext_len = 0;
    if (!EVP_DecryptUpdate(ctx, plaintext, &plaintext_len, ciphertext, ciphertext_len)) {
        handleErrors();
    }

    // 完成解密
    int final_len = 0;
    if (!EVP_DecryptFinal_ex(ctx, plaintext + plaintext_len, &final_len)) {
        handleErrors();
    }
    plaintext_len += final_len;

    // 释放解密上下文
    EVP_CIPHER_CTX_free(ctx);
}

// 测量时间函数
double measureTime(void (*func)(const unsigned char*, int, const unsigned char*, unsigned char*),
    const unsigned char* plaintext, int plaintext_len, const unsigned char* key, unsigned char* ciphertext) {
    LARGE_INTEGER start, end, frequency;
    double time;

    QueryPerformanceCounter(&start);
    func(plaintext, plaintext_len, key, ciphertext);
    QueryPerformanceCounter(&end);

    QueryPerformanceFrequency(&frequency);
    time = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart * 1000;  // 转换为毫秒

    return time;
}

// 计算 SHA-256 哈希值的函数
void calculateSHA256(const char* input, unsigned char* output) {
    SHA256((const unsigned char*)input, strlen(input), output);
}

// 测量时间的函数
double measureTime(void (*func)(const char*, unsigned char*), const char* input, unsigned char* output) {
    LARGE_INTEGER start, end, frequency;
    double time;

    QueryPerformanceCounter(&start);
    func(input, output);
    QueryPerformanceCounter(&end);

    QueryPerformanceFrequency(&frequency);
    time = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart * 1000;  // 转换为毫秒

    return time;
}
int main() {
    // 密钥
    unsigned char key[AES_KEY_SIZE] = "0123456789abcdef0123456789abcde";

    // 明文
    unsigned char plaintext[] = "Hello, AES-256!";
    int plaintext_len = strlen((char*)plaintext);

    // 密文
    unsigned char ciphertext[AES_BLOCK_SIZE];
    double min_encrypt_time = DBL_MAX;
    double min_decrypt_time = DBL_MAX;
    double min_SHA256_time = DBL_MAX;
    // 加密时间测量
    for (int i = 0; i <= 10000; i++) {
        printf("------------------第%d次测试-----------------------\n", i);
        double encryptionTime = measureTime(encrypt, plaintext, plaintext_len, key, ciphertext);
        printf("AES-256加密时间：%f ms\n", encryptionTime);
        if (encryptionTime < min_encrypt_time) {
            min_encrypt_time = encryptionTime;
        }

        // 解密时间测量
        double decryptionTime = measureTime(decrypt, ciphertext, AES_BLOCK_SIZE, key, ciphertext);
        printf("AES-256解密时间：%f ms\n", decryptionTime);
        if (decryptionTime < min_decrypt_time) {
            min_decrypt_time = decryptionTime;
        }      
    }
    // 输出结果
    printf("明文：%s\n", plaintext);
    printf("密文：");
    for (int i = 0; i < AES_BLOCK_SIZE; i++) {
        printf("%02x", ciphertext[i]);
    }
    printf("******************************************************\n");
    const char* input = "This is a piece of code to test the time consuming sha-256 operation!";
    unsigned char output[SHA256_DIGEST_LENGTH];
    for (int i = 0; i <= 10000; i++) {
        printf("------------------第%d次测试-----------------------\n", i);
        double hashingTime = measureTime(calculateSHA256, input, output);
        if (hashingTime < min_SHA256_time) {
            min_SHA256_time = hashingTime;
        }
        printf("SHA-256 Hashing time: %f ms\n", hashingTime);

    }
    printf("SHA-256 Hash: ");
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        printf("%02x", output[i]);
    }
    printf("\n");
    printf("*************************************************************************\n");
    printf("Minimum AES encryption time: %f ms\n", min_encrypt_time);
    printf("Minimum AES decryption time: %f ms\n", min_decrypt_time);
    printf("Minimum SHA-256 Hashing time: %f ms\n", min_SHA256_time);
    return 0;
}


//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <openssl/sha.h>
//#include <windows.h>

//// 计算 SHA-256 哈希值的函数
//void calculateSHA256(const char* input, unsigned char* output) {
//    SHA256((const unsigned char*)input, strlen(input), output);
//}
//
//// 测量时间的函数
//double measureTime(void (*func)(const char*, unsigned char*), const char* input, unsigned char* output) {
//    LARGE_INTEGER start, end, frequency;
//    double time;
//
//    QueryPerformanceCounter(&start);
//    func(input, output);
//    QueryPerformanceCounter(&end);
//
//    QueryPerformanceFrequency(&frequency);
//    time = (double)(end.QuadPart - start.QuadPart) / frequency.QuadPart * 1000;  // 转换为毫秒
//
//    return time;
//}

//int main() {
//    const char* input = "This is a piece of code to test the time consuming sha-256 operation!";
//    unsigned char output[SHA256_DIGEST_LENGTH];
//
//    double hashingTime = measureTime(calculateSHA256, input, output);
//    printf("SHA-256 Hashing time: %f ms\n", hashingTime);
//
//    printf("SHA-256 Hash: ");
//    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
//        printf("%02x", output[i]);
//    }
//    printf("\n");
//
//    return 0;
//}
