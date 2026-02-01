//
// Created by 23286 on 2026/1/29.
//

#include "mpu6050.h"
#include "usart.h"
#include "mpu6050_EAtest.h"

#include "main.h"
#include "main.h"
#include "mpu6050_test.h"  // 包含 pitch, roll, yaw 的声明

#pragma pack(1)  // 1字节对齐，确保结构体紧凑
typedef struct {
    float pitch;     // 俯仰角（X轴旋转）
    float roll;      // 横滚角（Y轴旋转）
    float yaw;       // 偏航角（Z轴旋转）
    uint8_t tail[4]; // JustFloat 帧尾
} VOFA_3Angle_t;
#pragma pack()

// 发送三个姿态角到 VOFA+
void VOFA_Send_3Angles()
{
    VOFA_3Angle_t data;

    // 从全局变量获取数据
    data.pitch = pitch;
    data.roll = roll;
    data.yaw = yaw;

    // 设置帧尾（JustFloat协议必须）
    data.tail[0] = 0x00;
    data.tail[1] = 0x00;
    data.tail[2] = 0x80;
    data.tail[3] = 0x7F;

    // 通过串口发送
    HAL_UART_Transmit(&huart1, (uint8_t*)&data, sizeof(data), 100);
    HAL_Delay(10);
}

/*如何书写一个函数？
 *
// 形式1：有参数无返回的函数
void 函数名(参数) {
    // 代码
}

// 形式2：无参数无返回的函数
void 函数名(void) {
    // 代码
}
// 形式3：返回值类型 函数名() {
    // 函数体
    // 具体执行的代码
    return 返回值;  // 如果有返回值
}

 *不需要输入参数函数自己知道那就可以写（void）
像这个函数
void VOFA_Send(void) {  // void 表示"不需要你告诉我任何东西"
    // 函数自己知道去用全局变量 pitch, roll, yaw
}

// 调用时：
VOFA_Send();  // 什么都不用传
还可以这样写：
void VOFA_Send(float pitch, float roll, float yaw) {
    // 这里使用传入的参数
}

// 调用时：
VOFA_Send(10.5, 20.3, 30.1);  // 必须传三个数
 *
 *为什么要用结构体体？写法简单，思路清楚，可读性高
 *否则要这么写
uint8_t buffer[16];
float pitch = 10.5f, roll = -5.3f, yaw = 45.0f;
memcpy(&buffer[0], &pitch, 4);   // 复制pitch
memcpy(&buffer[4], &roll, 4);    // 复制roll
memcpy(&buffer[8], &yaw, 4);     // 复制yaw
buffer[12] = 0x00;               // 帧尾
buffer[13] = 0x00;
buffer[14] = 0x80;
buffer[15] = 0x7F;
// 发送：传 buffer

*如何定义一个结构体？
// 方法1：先定义结构体类型，再声明变量
struct 结构体名 {
    类型 成员1;
    类型 成员2;
    // ...
};
// 使用：
struct Student stu1;  // 每次都要写 struct

// 方法2：定义类型的同时创建变量（不推荐，但要知道）
struct 结构体名 {
    成员定义;
} 变量名1, 变量名2;

// 方法3：使用typedef创建新类型（最常用！）
typedef struct {
    类型 成员1;
    类型 成员2;
    // ...
} 新类型名;
// 使用：
Student_t stu1;  // 直接使用，不用写 struct


 */