

#include "oled.h"
#include "font.h"


/**
  * @brief  OLED写字节函数
  * @param  dat: 要写入的数据
  * @param  cmd: 命令/数据标志 (0:命令, 1:数据)
  * @retval None
  */
void OledWrByte(unsigned dat, unsigned cmd)
{
    if(cmd) {
        WriteIicData(dat);   // 写数据
    } else {
        WriteIicCommand(dat); // 写命令
    }
}

/**
  * @brief  通过I2C写命令到OLED
  * @param  IIC_Command: 要写入的命令
  * @retval None
  */
void WriteIicCommand(unsigned IIC_Command)
{
    IicStart();                   // 启动I2C通信
    WriteIicByte(0x78);           // OLED设备地址
    IicWaitAck();                 // 等待应答
    WriteIicByte(0x00);           // 控制字节(表示后续是命令)
    IicWaitAck();                 // 等待应答
    WriteIicByte(IIC_Command);    // 发送命令
    IicWaitAck();                 // 等待应答
    IicStop();                    // 停止I2C通信
}

/**
  * @brief  通过I2C写数据到OLED
  * @param  IIC_Data: 要写入的数据
  * @retval None
  */
void WriteIicData(unsigned IIC_Data)
{
    IicStart();                   // 启动I2C通信
    WriteIicByte(0x78);           // OLED设备地址
    IicWaitAck();                 // 等待应答
    WriteIicByte(0x40);           // 控制字节(表示后续是数据)
    IicWaitAck();                 // 等待应答
    WriteIicByte(IIC_Data);       // 发送数据
    IicWaitAck();                 // 等待应答
    IicStop();                    // 停止I2C通信
}

/**
  * @brief  I2C通信启动信号
  * @param  None
  * @retval None
  */
void IicStart(void)
{
    OLED_SCLK_Set;   // SCL置高
    OLED_SDIN_Set;   // SDA置高
    OLED_SDIN_Clr;   // SDA拉低产生下降沿
    OLED_SCLK_Clr;   // SCL拉低，准备传输数据
}

/**
  * @brief  通过I2C写一个字节
  * @param  IIC_Byte: 要写入的字节
  * @retval None
  */
void WriteIicByte(unsigned char IIC_Byte)
{
    unsigned char i;
    unsigned char m, da;
    da = IIC_Byte;
    OLED_SCLK_Clr;   // 时钟线拉低准备发送
    for(i = 0; i < 8; i++) {
        m = da;
        m = m & 0x80; // 取最高位
        if(m == 0x80) {
            OLED_SDIN_Set; // 发送1
        } else {
            OLED_SDIN_Clr; // 发送0
        }
        da = da << 1; // 左移准备发送下一位
        OLED_SCLK_Set; // 时钟上升沿锁存数据
        OLED_SCLK_Clr; // 时钟下降沿准备下一位
    }
}

/**
  * @brief  I2C等待应答信号
  * @param  None
  * @retval None
  */
void IicWaitAck(void)
{
    OLED_SCLK_Set ; // 时钟线拉高
    OLED_SCLK_Clr;  // 时钟线拉低，完成一个时钟周期
}

/**
  * @brief  I2C通信停止信号
  * @param  None
  * @retval None
  */
void IicStop(void)
{
    OLED_SCLK_Set ; // SCL置高
    OLED_SDIN_Clr;  // SDA先拉低
    OLED_SDIN_Set;  // SDA拉高产生上升沿
}

/**
  * @brief  清除OLED屏幕
  * @param  None
  * @retval None
  */
void OledClear(void)
{
    uint8_t i, n;
    for(i = 0; i < 8; i++) {  // 遍历所有8页
        OledWrByte(0xb0 + i, OLED_CMD); // 设置页地址
        OledWrByte(0x00, OLED_CMD);     // 设置列地址低4位
        OledWrByte(0x10, OLED_CMD);     // 设置列地址高4位
        for(n = 0; n < 128; n++) {      // 遍历128列
            OledWrByte(0, OLED_DATA);   // 写入0清除像素
        }
    }
}

/**
  * @brief  在OLED上显示数字
  * @param  x: 显示位置的X坐标(0-127)
  * @param  y: 显示位置的Y坐标(0-7, 每页8行)
  * @param  num: 要显示的数字
  * @param  len: 数字的总位数
  * @param  size2: 字体大小(8或16)
  * @retval None
  */
void OledShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t len, uint8_t size2)
{
    uint8_t t, temp;
    uint8_t enshow = 0; // 前导零标志(0:不显示前导零)
    for(t = 0; t < len; t++) {
        temp = (num / OledPow(10, (len - t - 1))) % 10; // 提取当前位数字
        if((enshow == 0) && (t < (len-1))) {
            if(temp == 0) {
                OledShowChar((x + (size2 / 2) * t), y, ' ', size2); // 显示空格代替前导零
                continue;
            }else {
                enshow = 1;  // 遇到非零数字，开始显示后续所有数字
            }
        }
        OledShowChar((x + (size2 / 2) * t), y, temp + '0', size2);  // 显示当前数字
    }
}

/**
  * @brief  在OLED上显示字符串
  * @param  x: 显示位置的X坐标(0-127)
  * @param  y: 显示位置的Y坐标(0-7, 每页8行)
  * @param  chr: 要显示的字符串指针
  * @param  Char_Size: 字体大小(8或16)
  * @retval None
  */
void OledShowString(uint8_t x, uint8_t y, uint8_t * chr, uint8_t Char_Size)
{
    unsigned j = 0;
    while (chr[j] != '\0') { // 遍历字符串直到结束符
        OledShowChar(x, y, chr[j], Char_Size); // 显示单个字符
        x += 8; // 移动到下一个字符位置(每个字符占8列)
        if(x > 120) { // 超出屏幕宽度时换行
            x = 0;
            y += 2; // 换到下一页(每页8行)
        }
        j++;
    }
}

/**
  * @brief  在OLED上显示单个字符
  * @param  x: 显示位置的X坐标(0-127)
  * @param  y: 显示位置的Y坐标(0-7, 每页8行)
  * @param  chr: 要显示的字符
  * @param  Char_Size: 字体大小(8或16)
  * @retval None
  */
void OledShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t Char_Size)
{
    unsigned c = 0, i = 0;
    c = chr - ' '; // 计算字符在字库中的偏移量（ASCII从空格开始）
    
    // 边界检查：如果超出屏幕宽度，自动换行
    if(x > (Max_Column - 1)) { 
        x = 0;                // 列坐标归零
        y = y + 2;            // 页坐标增加2（每页8行）
    }
    
    // 处理16x8字体（8x16点阵）
    if(Char_Size == 16) { 
        OledSetPos(x, y);     // 设置字符上半部分显示位置
        for(i = 0; i < 8; i++) {
            // 写入上半部分字模数据（每个字符16字节，前8字节）
            OledWrByte(F8X16[c * 16 + i], OLED_DATA); 
        }
        
        OledSetPos(x, y + 1); // 设置字符下半部分显示位置（下一页）
        for(i = 0; i < 8; i++) {
            // 写入下半部分字模数据（每个字符16字节，后8字节）
            OledWrByte(F8X16[c * 16 + i + 8], OLED_DATA); 
        }
    } 
    // 处理8x8字体（6x8点阵）
    else { 
        OledSetPos(x, y);     // 设置字符显示位置
        for(i = 0; i < 6; i++) {
            // 写入6x8字模数据（每个字符6字节）
            OledWrByte(F6x8[c][i], OLED_DATA); 
        }
    }
}

/**
  * @brief  设置OLED显示位置
  * @param  x: 显示位置的X坐标(0-127)
  * @param  y: 显示位置的Y坐标(0-7, 每页8行)
  * @retval None
  */
void OledSetPos(unsigned x, unsigned y)
{
    OledWrByte(0xb0 + y, OLED_CMD);            // 设置页地址
    OledWrByte(((x & 0xf0) >> 4) | 0x10, OLED_CMD); // 设置列地址高4位
    OledWrByte((x & 0x0f), OLED_CMD);          // 设置列地址低4位
}

/**
  * @brief  计算幂函数
  * @param  m: 底数
  * @param  n: 指数
  * @retval m的n次幂
  */
uint32_t OledPow(uint8_t m, uint8_t n)
{
    uint32_t result = 1;
    while(n--) {
        result *= m; // 累乘计算幂
    }
    return result;
}

/**
  * @brief  点亮整个OLED屏幕
  * @param  None
  * @retval None
  */
void OledOn(void)
{
    uint8_t i, n;
    for(i = 0; i < 8; i++) { // 遍历所有8页
        OledWrByte(0xb0 + i, OLED_CMD); // 设置页地址
        OledWrByte(0x00, OLED_CMD);     // 设置列地址低4位
        OledWrByte(0x10, OLED_CMD);     // 设置列地址高4位
        for(n = 0; n < 128; n++) {      // 遍历128列
            OledWrByte(1, OLED_DATA);   // 写入1点亮所有像素
        }
    }
}

/**
  * @brief  开启OLED显示
  * @param  None
  * @retval None
  */
void OledDisplayOn(void)
{
    OledWrByte(0X8D, OLED_CMD); // 电荷泵设置命令
    OledWrByte(0X14, OLED_CMD); // 开启电荷泵
    OledWrByte(0XAF, OLED_CMD); // 开启显示
}
 
/**
  * @brief  关闭OLED显示
  * @param  None
  * @retval None
  */
void OledDisplayOff(void)
{
    OledWrByte(0X8D, OLED_CMD); // 电荷泵设置命令
    OledWrByte(0X10, OLED_CMD); // 关闭电荷泵
    OledWrByte(0XAE, OLED_CMD); // 关闭显示
}

/**
  * @brief  OLED初始化
  * @param  None
  * @retval None
  */
void OledInit(void)
{
    // OLED初始化命令序列
    OledWrByte(0xAE, OLED_CMD);  // 关闭显示
    OledWrByte(0x00, OLED_CMD);  // 设置低列地址
    OledWrByte(0x10, OLED_CMD);  // 设置高列地址
    OledWrByte(0x40, OLED_CMD);  // 设置显示起始行
    OledWrByte(0xB0, OLED_CMD);  // 设置页地址
    OledWrByte(0x81, OLED_CMD);  // 对比度设置命令
    OledWrByte(0xFF, OLED_CMD);  // 对比度最大值
    OledWrByte(0xA1, OLED_CMD);  // 设置段重定向(左右翻转)
    OledWrByte(0xA6, OLED_CMD);  // 正常显示(非反色)
    OledWrByte(0xA8, OLED_CMD);  // 设置多路复用率
    OledWrByte(0x3F, OLED_CMD);  // 1/64占空比
    OledWrByte(0xC8, OLED_CMD);  // 设置COM扫描方向(上下翻转)
    OledWrByte(0xD3, OLED_CMD);  // 设置显示偏移
    OledWrByte(0x00, OLED_CMD);  // 无偏移
    OledWrByte(0xD5, OLED_CMD);  // 设置显示时钟分频
    OledWrByte(0x80, OLED_CMD);  // 默认值
    OledWrByte(0xD8, OLED_CMD);  // 设置区域颜色模式
    OledWrByte(0x05, OLED_CMD);  // 默认值
    OledWrByte(0xD9, OLED_CMD);  // 设置预充电周期
    OledWrByte(0xF1, OLED_CMD);  // 默认值
    OledWrByte(0xDA, OLED_CMD);  // 设置COM硬件引脚配置
    OledWrByte(0x12, OLED_CMD);  // 默认值
    OledWrByte(0xDB, OLED_CMD);  // 设置VCOMH电压
    OledWrByte(0x30, OLED_CMD);  // 默认值
    OledWrByte(0x8D, OLED_CMD);  // 电荷泵设置命令
    OledWrByte(0x14, OLED_CMD);  // 开启电荷泵
    OledWrByte(0xAF, OLED_CMD);  // 开启显示

    OledClear(); // 初始化后清屏
}
