//#include "stdafx.h"
#include <pbc.h>
#include <pbc_test.h>
#include <iostream>
#include<Windows.h>
#include<gmp.h>
#include<time.h>
#include<pbc_utils.h>
#include<stdio.h>
#include<string.h>
#include <cfloat>
void hash_and_map_to_curve(element_t p, const char* data, pbc_param_t param) {
	// 直接使用element_from_hash函数，该函数内部会处理数据的哈希
	element_from_hash(p, (void*)data, strlen(data));
}

int main(int argc, char** argv)
{

	pairing_t pairing;
	pbc_demo_pairing_init(pairing, argc, argv);
	if (!pairing_is_symmetric(pairing)) pbc_die("pairing must be symmetric");
	element_t P, Ppub, x, S, H, t1, t2, t3, t4,t5;
	element_init_Zr(x, pairing);
	element_init_Zr(H, pairing);
	element_init_Zr(t1, pairing);

	element_init_G1(S, pairing);
	element_init_G1(P, pairing);
	element_init_G1(Ppub, pairing);
	element_init_G1(t2, pairing);

	element_init_GT(t3, pairing);
	element_init_GT(t4, pairing);
	element_init_GT(t5, pairing);
	printf("ZSS short signature schema\n");
	printf("KEYGEN\n");
	element_random(x);
	element_random(P);
	element_mul_zn(Ppub, P, x);
	element_printf("P = %B\n", P);
	element_printf("x = %B\n", x);
	element_printf("Ppub = %B\n", Ppub);

	printf("SIGN\n");
	//element_from_hash(H, "Message", 7);
	element_add(t1, H, x);
	element_invert(t1, t1);
	element_mul_zn(S, P, t1);
	printf("Signature of message \"Message\" is:\n");
	element_printf("S = %B\n", S);
	double min_time1 = DBL_MAX;
	double min_time2 = DBL_MAX;
	double min_time3 = DBL_MAX;
	double min_time4 = DBL_MAX;
	double min_time5 = DBL_MAX;
	printf("VERIFY\n");
	//element_from_hash(H, "Message", 7);
	for(int i=1;i<=10000;i++){
	printf("------------------第%d次测试-----------------------\n", i);
	LARGE_INTEGER frequency1, start1, end1;
	QueryPerformanceFrequency(&frequency1); // 获取计时器的频率  
	QueryPerformanceCounter(&start1); // 获取开始时间  
	element_mul_zn(t2, P, H);// 执行需要测量时间的操作  
	QueryPerformanceCounter(&end1); // 获取结束时间  
	double elapsed_time1 = (end1.QuadPart - start1.QuadPart) / (double)frequency1.QuadPart * 1000.0; // 计算时间差（毫秒）
	if (elapsed_time1 < min_time1) {
		min_time1 = elapsed_time1;
	}
	printf("Scalar multiplication operations on bilinear pairings in group G1 consume time：%f ms\n", elapsed_time1);
	}
	element_add(t2, t2, Ppub);
	for (int i = 1; i <= 10000; i++) {
		printf("------------------第%d次测试-----------------------\n", i);
		LARGE_INTEGER frequency2, start2, end2;
		QueryPerformanceFrequency(&frequency2); // 获取计时器的频率  
		QueryPerformanceCounter(&start2); // 获取开始时间  
		element_pairing(t3, t2, S);// 执行需要测量时间的操作  
		QueryPerformanceCounter(&end2); // 获取结束时间  
		double elapsed_time2 = (end2.QuadPart - start2.QuadPart) / (double)frequency2.QuadPart * 1000.0; // 计算时间差（毫秒）
		if (elapsed_time2 < min_time2) {
			min_time2 = elapsed_time2;
		}
		printf("Bilinear pairing consumes time：%f ms\n", elapsed_time2);
	}
	for (int i = 1; i <= 10000; i++) {
		printf("------------------第%d次测试-----------------------\n", i);
		LARGE_INTEGER frequency3, start3, end3;
		QueryPerformanceFrequency(&frequency3); // 获取计时器的频率  
		QueryPerformanceCounter(&start3); // 获取开始时间  
		element_mul_zn(t5, t3, H);// 执行需要测量时间的操作  
		QueryPerformanceCounter(&end3); // 获取结束时间  
		double elapsed_time3 = (end3.QuadPart - start3.QuadPart) / (double)frequency3.QuadPart * 1000.0; // 计算时间差（毫秒）
		printf("Scalar multiplication operations on bilinear pairings in group GT consume time：%f ms\n", elapsed_time3);
		if (elapsed_time3 < min_time3) {
			min_time3 = elapsed_time3;
		}
	}
	element_pairing(t4, P, P);
	element_printf("e(H(m)P + Ppub, S) = %B\n", t3);
	element_printf("e(P, P) = %B\n", t4);
	if (!element_cmp(t3, t4)) printf("Signature is valid\n");
	else printf("Signature is invalid\n");
	//测试与双线性配对相关的指数化操作
	element_t a, b;
	mpz_t c;
	element_init_G1(a, pairing);
	element_init_G1(b, pairing);
	mpz_init(c);
	element_random(b);
	mpz_set_ui(c,925323658568962683);
	element_printf("b = %B\n", b);
	for (int i = 1; i <= 10000; i++) {
		printf("------------------第%d次测试-----------------------\n", i);
		LARGE_INTEGER frequency4, start4, end4;
		QueryPerformanceFrequency(&frequency4); // 获取计时器的频率  
		QueryPerformanceCounter(&start4); // 获取开始时间  
		element_pow_mpz(a, b, c);// 执行需要测量时间的操作,a=b^c  
		QueryPerformanceCounter(&end4); // 获取结束时间  
		double elapsed_time4 = (end4.QuadPart - start4.QuadPart) / (double)frequency4.QuadPart * 1000.0; // 计算时间差（毫秒）
		if (elapsed_time4 < min_time4) {
			min_time4 = elapsed_time4;
		}
		printf("The indexation operation trumpet time associated with bilinear pairing is:%f ms\n", elapsed_time4);
	}
	
	//测试maptopoint操作消耗时间
	// 从文件中读取配对参数
	FILE* file = fopen("a.param", "r");
	if (file == NULL) {
		perror("Error opening file");
		return 1;
	}
	char param[1024];
	size_t count = fread(param, 1, 1024, file);
	fclose(file);
	if (!count) {
		pbc_die("Input error");
	}
	pairing_init_set_buf(pairing, param, count);

	// 定义一些元素
	element_t g1, g2, hash_element;
	element_init_G1(g1, pairing);
	element_init_G2(g2, pairing);
	element_init_G1(hash_element, pairing);

	// 假设要进行散列的消息（这里使用一个简单的字符串示例）
	 const char* message = "Hello, maptopoint!";
	 for (int i = 1; i <= 10000; i++) {
		 printf("------------------第%d次测试-----------------------\n", i);
		 LARGE_INTEGER frequency5, start5, end5;
		 QueryPerformanceFrequency(&frequency5); // 获取计时器的频率  
		 QueryPerformanceCounter(&start5); // 获取开始时间  
		 element_from_hash(hash_element, (void*)message, strlen(message));// 执行需要测量时间的操作,
		 QueryPerformanceCounter(&end5); // 获取结束时间  
		 double elapsed_time5 = (end5.QuadPart - start5.QuadPart) / (double)frequency5.QuadPart * 1000.0; // 计算时间差（毫秒）
		 if (elapsed_time5 < min_time5) {
			 min_time5 = elapsed_time5;
		 }
		 printf("The maptopoint operation takes time is:%f ms\n", elapsed_time5);
	 }
	// 使用 element_from_hash 函数进行映射点散列运算
	 printf("*************************************************************************\n");
	 element_printf("a =b^c= %B\n", a);
	 printf("Minimum Scalar multiplication operations on bilinear pairings in group G1 consume time: %f ms\n", min_time1);
	 printf("Minimum Bilinear pairing consumes time: %f ms\n", min_time2);
	 printf("Minimum Scalar multiplication operations on bilinear pairings in group GT consume time: %f ms\n", min_time3);
	 printf("Minimum  time of the indexation operation trumpet time associated with bilinear pairing : %f ms\n", min_time4);
	 printf("Minimum maptopoint operation takes time: %f ms\n", min_time5);
	
	// 释放元素内存
	element_clear(g1);
	element_clear(g2);
	element_clear(hash_element);

	// 释放配对内存
	pairing_clear(pairing);
	element_clear(a);
	element_clear(b);
	mpz_clear(c);
	element_clear(P);
	element_clear(Ppub);
	element_clear(x);
	element_clear(S);
	element_clear(H);
	element_clear(t1);
	element_clear(t2);
	element_clear(t3);
	element_clear(t4);
	//std::cin.get();
	
	return 0;
}

