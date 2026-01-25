#include <tgmath.h>
#include <stdint.h>

typedef struct {
    float kp;        // 比例系数
    float ki;        // 积分系数
    float kd;        // 微分系数

    float pre_error;     // 上一次的误差
    float integral;      // 积分累加值
    float derivative;    // 微分项（经过滤波）
    float output;        // PID输出值

    // 限幅参数
    float LimitIntegralMax;    // 积分最大值限制
    float LimitIntegralMin;    // 积分最小值限制
    float IntegralThreshold;   // 积分分离阈值
    float LimitOutputMax;      // 输出最大值限制
    float LimitOutputMin;      // 输出最小值限制

    float alpha;        // 低通滤波系数 (0~1之间)
} PIDControllerType_t;

/**
 * @brief PID控制器更新函数
 * @param pid PID控制器指针
 * @param target 目标值（设定值）
 * @param current 当前值（反馈值）
 * @param dt 时间间隔（秒）
 *
 * @note 本函数实现了位置式PID控制算法，包含：
 *       1. 积分分离
 *       2. 积分限幅
 *       3. 微分项低通滤波
 *       4. 输出限幅
 */
void PID_Update(PIDControllerType_t* pid, float target, float current, float dt) {
    // 1. 计算当前误差
    float error = target - current;

    // 2. 积分项：只在小误差时积分
    if (fabsf(error) < pid->IntegralThreshold) {
        pid->integral += error * dt;  // 积分需要乘以时间间隔
    }

    // 3. 微分项计算和滤波
    float raw_derivative = (error - pid->pre_error) / dt;  // 计算原始微分
    pid->derivative = pid->alpha * raw_derivative +
                     (1.0f - pid->alpha) * pid->derivative;  // 低通滤波

    // 4. 保存当前误差，供下次使用
    pid->pre_error = error;

    // 5. 积分限幅（防止积分饱和）
    if (pid->integral > pid->LimitIntegralMax) {
        pid->integral = pid->LimitIntegralMax;
    } else if (pid->integral < pid->LimitIntegralMin) {
        pid->integral = pid->LimitIntegralMin;
    }

    // 6. 计算PID输出
    pid->output = pid->kp * error +
                  pid->ki * pid->integral +
                  pid->kd * pid->derivative;  // 使用滤波后的微分

    // 7. 输出限幅
    if (pid->output > pid->LimitOutputMax) {
        pid->output = pid->LimitOutputMax;
    } else if (pid->output < pid->LimitOutputMin) {
        pid->output = pid->LimitOutputMin;
    }
}

//假装有很多定义
// 姿态数据结构（假设从传感器获取）
typedef struct {
    float roll;     // 横滚角（度）
    float pitch;    // 俯仰角（度）
    float yaw;      // 偏航角（度）
} Attitude_t;

// 角速度数据（假设从IMU获取）
typedef struct {
    float x;        // X轴角速度（度/秒）
    float y;        // Y轴角速度（度/秒）
    float z;        // Z轴角速度（度/秒）
} Gyro_t;

// 1. 先定义两个PID控制器//结构体变量 rollPID 是一个存储PID控制器相关数据的容器
PIDControllerType_t rollPID;        // 外环：角度控制
PIDControllerType_t rollRatePID;    // 内环：角速度控制

/**
 * @brief 串级PID控制（分步法）
 * @param outer_pid 外环PID（角度环）
 * @param inner_pid 内环PID（角速度环）
 * @param angle_target 角度目标值（度）
 * @param current_angle 当前角度（度）
 * @param current_rate 当前角速度（度/秒）
 * @param dt 时间间隔（秒）
 * @return 内环输出（最终控制量）
 *
 * @note 步骤：
 * 1. 外环计算期望角速度
 * 2. 内环计算控制输出
 */
float CascadePID_StepByStep(PIDControllerType_t* outer_pid,  // 参数1：外环工具箱的地址
                            PIDControllerType_t* inner_pid,  // 参数2：内环工具箱的地址
                            float angle_target,             // 参数3：想要的角度（比如0度=水平）
                            float current_angle,            // 参数4：当前角度（比如15度=向右倾斜）
                            float current_rate,             // 参数5：当前旋转速度（比如5度/秒=向右转）
                            float dt) {                     // 参数6：时间间隔（比如0.01秒=10毫秒）

    // 第一步：更新外环PID（角度环）
    // 输入：目标角度 - 当前角度
    // 输出：期望的角速度
    PID_Update(outer_pid, angle_target, current_angle, dt);

    // 第二步：更新内环PID（角速度环）
    // 输入：外环输出的期望角速度 - 当前角速度
    // 输出：最终控制量（如电机PWM）
    PID_Update(inner_pid, outer_pid->output, current_rate, dt);

    // 返回内环的输出（最终控制量）
    return inner_pid->output;
}
