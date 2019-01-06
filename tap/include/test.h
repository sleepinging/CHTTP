/*
 * @Description: 单元测试
 * @Author: taowentao
 * @Date: 2019-01-06 11:42:43
 * @LastEditors: taowentao
 * @LastEditTime: 2019-01-06 13:43:14
 */

#if !defined(__H__TEST__H__)
#define __H__TEST__H__

class MyMAC;

class MyIPNet;

/**
 * @description: 测试
 * @param {type} 
 * @return: 
 */
int test(int argc, char const *argv[]);

/**
 * @description: 测试命令行
 * @param {type} 
 * @return: 
 */
int test_cmd(int argc, char const *argv[]);

/**
 * @description: 测试重连
 * @param {type} 
 * @return: 
 */
int test_reconnect();

/**
 * @description: 测试连接
 * @param {type} 
 * @return: 
 */
int test_connect();

/**
 * @description: 测试tap
 * @param {type} 
 * @return: 
 */
int test_tap(const MyMAC *mac, const MyIPNet *ipnet);

#ifdef _WIN32
/**
 * @description: 测试win tap
 * @param {type} 
 * @return: 
 */
int test_tap_win(const MyMAC* mac,const MyIPNet* ipnet);

#else
/**
 * @description: 测试linux tap
 * @param {type} 
 * @return: 
 */
int test_tap_linux(const MyMAC *mac, const MyIPNet *ipnet);

#endif

#endif // __H__TEST__H__

