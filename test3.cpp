//#include <openssl/rsa.h>
//#include <openssl/pem.h>
//#include <openssl/err.h>
//#include <stdio.h>
//#include <string.h>
//#include <Windows.h>
//#pragma warning(disable:4996)
//
//// 用于测量时间的函数
//double measure_time_in_ms(void) {
//    return GetTickCount64() * 1.0; // GetTickCount64()返回的单位是毫秒
//}
//
//// 用于初始化RSA上下文的函数
//RSA* init_rsa(int key_size) {
//    RSA* rsa = RSA_new();
//    BIGNUM* bn = BN_new();
//    BN_set_word(bn, RSA_F4); // 设置公钥指数为65537
//    RSA_generate_key_ex(rsa, key_size, bn, NULL);
//    BN_free(bn);
//    return rsa;
//}
//
//// 用于释放RSA上下文的函数
//void free_rsa(RSA* rsa) {
//    RSA_free(rsa);
//}
//
//int main() {
//    // 初始化OpenSSL库
//    ERR_load_crypto_strings();
//    OpenSSL_add_all_algorithms();
//
//    // 初始化RSA密钥
//    RSA* rsa = init_rsa(2048);
//
//    // 要加密的明文
//    unsigned char* plaintext = (unsigned char*)"This is a test code that uses RSA algorithm to encrypt and decrypt strings and consumes time！";
//    int plaintext_len = strlen((char*)plaintext);
//    unsigned char ciphertext[256];
//    unsigned char decryptedtext[256];
//
//    // 为存储加解密结果分配内存
//    memset(ciphertext, 0, sizeof(ciphertext));
//    memset(decryptedtext, 0, sizeof(decryptedtext));
//
//    // RSA加密
//    LARGE_INTEGER frequency1, start1, end1;
//    QueryPerformanceFrequency(&frequency1); // 获取计时器的频率  
//    QueryPerformanceCounter(&start1); // 获取开始时间  
//    // 执行需要测量时间的操作  
//    int ciphertext_len = RSA_public_encrypt(plaintext_len, plaintext, ciphertext, rsa, RSA_PKCS1_OAEP_PADDING);
//    QueryPerformanceCounter(&end1); // 获取结束时间  
//    double elapsed_time1 = (end1.QuadPart - start1.QuadPart) / (double)frequency1.QuadPart * 1000.0; // 计算时间差（毫秒）
//    printf("RSA encryption takes time：%f ms\n", elapsed_time1);
//    
//  
//    if (ciphertext_len < 0) {
//        fprintf(stderr, "Encryption failed.\n");
//        ERR_print_errors_fp(stderr);
//        return 1;
//    }
//
//    // RSA解密
//    LARGE_INTEGER frequency2, start2, end2;
//    QueryPerformanceFrequency(&frequency2); // 获取计时器的频率  
//    QueryPerformanceCounter(&start2); // 获取开始时间  
//    // 执行需要测量时间的操作  
//    int decryptedtext_len = RSA_private_decrypt(ciphertext_len, ciphertext, decryptedtext, rsa, RSA_PKCS1_OAEP_PADDING);
//    QueryPerformanceCounter(&end2); // 获取结束时间  
//    double elapsed_time2 = (end2.QuadPart - start2.QuadPart) / (double)frequency2.QuadPart * 1000.0; // 计算时间差（毫秒）
//    printf("RSA decryption takes time：%f ms\n", elapsed_time2);
//   
//
//    if (decryptedtext_len < 0) {
//        fprintf(stderr, "Decryption failed.\n");
//        ERR_print_errors_fp(stderr);
//        return 1;
//    }
//    // 清理
//    free_rsa(rsa);
//    ERR_free_strings();
//    return 0;
//}
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <cfloat>
#pragma warning(disable:4996)

// 用于初始化RSA上下文的函数
RSA* init_rsa(int key_size) {
    RSA* rsa = RSA_new();
    BIGNUM* bn = BN_new();
    BN_set_word(bn, RSA_F4); // 设置公钥指数为65537
    RSA_generate_key_ex(rsa, key_size, bn, NULL);
    BN_free(bn);
    return rsa;
}

// 用于释放RSA上下文的函数
void free_rsa(RSA* rsa) {
    RSA_free(rsa);
}

// 测量单个操作的时间并更新最小时间
double measure_single_operation(RSA* rsa, unsigned char* data, int len, int operation) {
    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency); // 获取计时器的频率
    QueryPerformanceCounter(&start); // 获取开始时间
    unsigned char buffer[256] = { 0 };
    int buffer_len;

    if (operation == 1) { // 加密
        buffer_len = RSA_public_encrypt(len, data, buffer, rsa, RSA_PKCS1_OAEP_PADDING);
    }
    else { // 解密
        buffer_len = RSA_private_decrypt(len, data, buffer, rsa, RSA_PKCS1_OAEP_PADDING);
    }

    QueryPerformanceCounter(&end); // 获取结束时间
    double elapsed_time = (end.QuadPart - start.QuadPart) / (double)frequency.QuadPart * 1000.0; // 计算时间差（毫秒）
    return elapsed_time;
}

int main() {
    // 初始化OpenSSL库
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    // 初始化RSA密钥
    RSA* rsa = init_rsa(2048);

    // 要加密的明文
    unsigned char* plaintext = (unsigned char*)"This is a test code that uses RSA algorithm to encrypt and decrypt strings and consumes time！";
    int plaintext_len = strlen((char*)plaintext);
    unsigned char ciphertext[256];
    unsigned char decryptedtext[256];

    // 分别初始化最小加密和解密时间
    double min_encrypt_time = DBL_MAX;
    double min_decrypt_time = DBL_MAX;

    for (int i = 0; i <= 10000; i++) {
        // 重置内存
        memset(ciphertext, 0, sizeof(ciphertext));
        memset(decryptedtext, 0, sizeof(decryptedtext));
        printf("------------------第%d次测试-----------------------\n", i);
        // 测量加密时间
        double current_encrypt_time = measure_single_operation(rsa, plaintext, plaintext_len, 1);
        if (current_encrypt_time < min_encrypt_time) {
            min_encrypt_time = current_encrypt_time;
        }
        printf("RSA加密时间：%f ms\n", current_encrypt_time);
        // 测量解密时间
        double current_decrypt_time = measure_single_operation(rsa, ciphertext, sizeof(ciphertext), 2);
        if (current_decrypt_time < min_decrypt_time) {
            min_decrypt_time = current_decrypt_time;
        }
        printf("RSA解密时间：%f ms\n", current_decrypt_time);
        
    }
    printf("*************************************************************************\n");
    printf("Minimum RSA encryption time: %f ms\n", min_encrypt_time);
    printf("Minimum RSA decryption time: %f ms\n", min_decrypt_time);

    // 清理
    free_rsa(rsa);
    ERR_free_strings();

    return 0;
}