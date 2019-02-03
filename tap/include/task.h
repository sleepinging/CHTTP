/*
 * @Description: 定时任务
 * @Author: taowentao
 * @Date: 2019-02-03 11:58:13
 * @LastEditors: taowentao
 * @LastEditTime: 2019-02-03 14:30:38
 */
#ifndef __H__TASK__H__
#define __H__TASK__H__

//发送一次心跳,返回0成功
int SendHeartBeat();

//开始心跳线程,返回0成功
int DetachHeartBeat(int sec);

#endif