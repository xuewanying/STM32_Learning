//
// Created by 23286 on 2026/1/26.
//
#include "mpu6050.h"         // MPU6050驱动头文件，包含函数声明
#include "i2c.h"             // I2C通信头文件，包含HAL_I2C函数

// 静态全局变量定义（仅在此文件内可见）
static float ax, ay, az;     // 三轴加速度值（单位：g）
static float gx, gy, gz;     // 三轴角速度值（单位：°/s）
static float temperature;    // 温度值（单位：℃）

// 陀螺仪零偏校准变量（静态全局，用于存储校准偏移量）
static float gyro_offset_x = 0, gyro_offset_y = 0, gyro_offset_z = 0;

// 陀螺仪比例因子：每个°/s对应的LSB数（根据±250°/s量程计算）
// 32768(16位有符号最大值) / 250(量程) = 131.072
float gyro_scale = 131.0f;

// ================= I2C寄存器读写函数 =================

/**
 * @brief  向MPU6050寄存器写入数据
 * @param  reg_addr: 寄存器地址
 * @param  data: 要写入的数据
 * @note   使用I2C1，MPU6050地址为0xD0（写入地址）
 */
static void reg_write(uint8_t reg_addr, uint8_t data) {
    uint8_t bytesTosend[] = { reg_addr, data };  // 第一个字节为寄存器地址，第二个为数据
    HAL_I2C_Master_Transmit(&hi2c1, 0xd0, bytesTosend, 2, HAL_MAX_DELAY);
}

/**
 * @brief  从MPU6050寄存器读取数据
 * @param  reg_addr: 要读取的寄存器地址
 * @retval 读取到的寄存器值
 * @note   先发送寄存器地址，然后接收数据
 */
static uint8_t reg_read(uint8_t reg_addr) {
    uint8_t bytesRead;  // 存储读取结果

    // 发送寄存器地址（1字节）
    HAL_I2C_Master_Transmit(&hi2c1, 0xd0, &reg_addr, 1, HAL_MAX_DELAY);
    // 从该寄存器读取1字节数据
    HAL_I2C_Master_Receive(&hi2c1, 0xd0, &bytesRead, 1, HAL_MAX_DELAY);

    return bytesRead;
}

// ================= MPU6050初始化函数 =================

/**
 * @brief  MPU6050初始化
 * @note   配置电源管理、陀螺仪和加速度计量程
 */
void MPU6050_Init(void) {
    // 复位设备（向PWR_MGMT_1寄存器写入0x80）
    reg_write(0x6b, 0x80);
    HAL_Delay(100);  // 等待复位完成

    // 唤醒设备，使用内部8MHz晶振
    reg_write(0x6b, 0x00);

    // 配置陀螺仪量程：±250°/s（GYRO_CONFIG寄存器）
    // 0x00: FS_SEL=00，对应±250°/s
    reg_write(0x1b, 0x00);

    // 配置加速度计量程：±2g（ACCEL_CONFIG寄存器）
    // 0x00: AFS_SEL=00，对应±2g
    reg_write(0x1c, 0x00);
}

// ================= 数据更新函数 =================

/**
 * @brief  更新MPU6050所有传感器数据
 * @note   从原始寄存器读取数据，转换为物理量
 *         加速度：±2g量程，16384 LSB/g
 *         温度：340 LSB/℃，零偏36.53
 *         陀螺仪：±250°/s量程，131 LSB/°/s
 */
void MPU6050_Updata(void) {
    // 读取三轴加速度原始数据（16位有符号）
    // 每个轴数据由高8位和低8位两个寄存器组成
    int16_t ax_raw = (int16_t)(reg_read(0x3b) << 8) + reg_read(0x3c);
    int16_t ay_raw = (int16_t)(reg_read(0x3d) << 8) + reg_read(0x3e);
    int16_t az_raw = (int16_t)(reg_read(0x3f) << 8) + reg_read(0x40);

    // 将原始数据转换为g值（±2g量程，16384 LSB/g）
    // 转换公式：物理值 = 原始值 / 16384 ≈ 原始值 × 6.1035e-5
    ax = ax_raw * 6.1035e-5f;
    ay = ay_raw * 6.1035e-5f;
    az = az_raw * 6.1035e-5f;

    // 读取温度原始数据
    int16_t temperature_raw = (int16_t)(reg_read(0x41) << 8) + reg_read(0x42);
    // 转换为摄氏度：T = T_raw / 340 + 36.53
    temperature = temperature_raw / 340 + 36.53;

    // 读取三轴角速度原始数据
    int16_t gx_raw = (int16_t)(reg_read(0x43) << 8) + reg_read(0x44);
    int16_t gy_raw = (int16_t)(reg_read(0x45) << 8) + reg_read(0x46);
    int16_t gz_raw = (int16_t)(reg_read(0x47) << 8) + reg_read(0x48);

    // 将原始数据转换为°/s（±250°/s量程，131 LSB/°/s）
    // 方法1：使用比例因子除法
    gx = gx_raw / gyro_scale;
    gy = gy_raw / gyro_scale;
    gz = gz_raw / gyro_scale;

    // 方法2：直接乘法（如果gyro_scale定义不同）
    // gx = gx_raw * 6.1035e-2f;  // 1/163.84 ≈ 0.061035
    // gy = gy_raw * 6.1035e-2f;
    // gz = gz_raw * 6.1035e-2f;
}

// ================= 数据获取函数 =================

/**
 * @brief  获取X轴加速度值
 * @retval X轴加速度（单位：g）
 */
float MPU6050_GetAx(void) {
    return ax;
}

/**
 * @brief  获取Y轴加速度值
 * @retval Y轴加速度（单位：g）
 */
float MPU6050_GetAy(void) {
    return ay;
}

/**
 * @brief  获取Z轴加速度值
 * @retval Z轴加速度（单位：g）
 */
float MPU6050_GetAz(void) {
    return az;
}

/**
 * @brief  获取温度值
 * @retval 温度（单位：℃）
 */
float MPU6050_Temperature(void) {
    return temperature;
}

/**
 * @brief  获取X轴角速度（未校准）
 * @retval X轴角速度（单位：°/s）
 */
float MPU6050_GetGx(void) {
    return gx;
}

/**
 * @brief  获取Y轴角速度（未校准）
 * @retval Y轴角速度（单位：°/s）
 */
float MPU6050_GetGy(void) {
    return gy;
}

/**
 * @brief  获取Z轴角速度（未校准）
 * @retval Z轴角速度（单位：°/s）
 */
float MPU6050_GetGz(void) {
    return gz;
}

