#include <stdint.h>
#include "pid2.h"

/**
 * @brief PID控制器初始化函数
 * @param PID: 指向PID_TypeDef结构体的指针
 * @param Kp: 比例增益
 * @param Ki: 积分增益
 * @param Kd: 微分增益
 *
 * 这个函数初始化PID控制器的所有参数和状态变量
 */
void PID_Init(PID_TypeDef* PID, float Kp, float Ki, float Kd) {
    PID->Kp = Kp;      // 设置比例系数
    PID->Ki = Ki;      // 设置积分系数
    PID->Kd = Kd;      // 设置微分系数

    PID->SP = 0.0f;        // 初始设定值为0
    PID->t_t0 = 0;         // 初始时间戳为0
    PID->err_t0 = 0;       // 初始误差为0
    PID->err_in_t0 = 0;    // 初始误差积分为0
}

/**
 * @brief 改变PID设定值
 * @param PID: 指向PID_TypeDef结构体的指针
 * @param SP: 新的设定值
 *
 * 用于在运行过程中改变控制目标值
 */
void PID_Change_SP(PID_TypeDef* PID, float SP) {
    PID->SP = SP;  // 更新设定值
}
/*
// 创建结构体变量
PID_TypeDef myController;
// 获取它的地址
PID_TypeDef* ptr = &myController;  // ptr 存储 myController 的地址
// 访问成员有两种方式：
// 1. 通过结构体变量本身（用点号）
myController.Kp = 1.0;
// 2. 通过指针（用箭头 ->）
ptr->Kp = 1.0;
// 箭头运算符相当于：先解引用，再访问成员
// ptr->Kp 等价于 (*ptr).Kp

// 情况1：传值（不工作）
void bad_init(PID_TypeDef pid, float Kp) {
    pid.Kp = Kp;  // 修改的是副本
    // 函数结束后，这个副本被销毁
}
// 情况2：传指针（工作）
void good_init(PID_TypeDef* pid, float Kp) {
    pid->Kp = Kp;  // 修改的是原始数据
}

// C++ 中有引用，但 C 语言没有
// C++ 写法（不能用在此C代码中）：
void PID_Init(PID_TypeDef& pid, float Kp) {
    pid.Kp = Kp;  // 直接使用点号
}
// C语言只能用指针实现类似效果
void PID_Init(PID_TypeDef* pid, float Kp) {
    pid->Kp = Kp;  // 必须用箭头
}
*/

/**
 * @brief PID计算函数
 * @param PID: 指向PID_TypeDef结构体的指针
 * @param FB: 反馈值（实际测量值）
 * @return 计算得到的控制输出值
 *
 * 根据当前反馈值和设定值计算控制输出
 * 使用梯形积分法和后向差分微分
 */
float PID_Compute(PID_TypeDef* PID, float FB) {
    // 1. 计算当前误差
    float err = (PID->SP - FB);

    // 2. 获取当前时间（需要根据实际情况实现）
    float t_tk = 0;  // 注意：这里需要传入实际的定时器值
    // 例如：t_tk = __HAL_TIM_GET_COUNTER(&htimx);

    // 3. 计算时间间隔（转换为秒）
    // 假设时间单位为微秒，乘以1.0e-6转换为秒
    float deltaT = (t_tk - PID->t_t0) * 1.0e-6f;

    // 4. 计算微分项：误差变化率（后向差分）
    float err_dev = (err - PID->err_t0) / deltaT;

    // 5. 计算积分项：梯形积分法
    // 梯形面积 = (上底 + 下底) * 高 / 2
    float err_int = PID->err_in_t0 + (err + PID->err_t0) * deltaT * 0.5f;
    // 注意：这里代码有误，应该是 (err + PID->err_t0)，而不是 (err - PID->err_t0)
    // 梯形积分公式：新积分 = 旧积分 + (当前误差 + 上次误差) * Δt / 2

    // 6. 计算各项输出
    float COp = PID->Kp * err;        // 比例项
    float COi = PID->Ki * err_int;    // 积分项
    float COd = PID->Kd * err_dev;    // 微分项

    // 7. 计算总输出
    float CO = COp + COi + COd;

    // 8. 更新状态变量
    PID->t_t0 = t_tk;         // 更新时间戳
    PID->err_t0 = err;        // 更新误差
    PID->err_in_t0 = err_int; // 更新积分值

    return CO;  // 返回控制输出
}