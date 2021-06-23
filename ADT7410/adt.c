
#include <bcm2835.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <math.h>


typedef struct temp_samples_t
{
    double tempC[10];  
    double temp_avg;
    double temp_std_dev;
    char timestamp_start[50];       // format: 16:25:13 4/22/2020
    char timestamp_end[50];       // format: 16:25:13 4/22/2020
}temp_samples;



/* I2C registers in ADT74x0 */
#define ADT7410_T_MSB       0x00
#define ADT7410_T_LSB       0x01
#define ADT7410_STATUS      0x02
#define ADT7410_CONFIG      0x03
#define ADT7410_IDREG       0x0b
#define ADT7410_RESET       0x2f

#define STATUS_GOOD         0
#define ADT7410_I2C_ADDR    0x48


static int init_ADT7410(const uint8_t addr);
static int read_ADT7410(const uint8_t addr, double *temp);
static double Get_ADT7410_Temp(const uint8_t addr);
static void Calc_Metrics(temp_samples samples);

int main(int argc, const char *argv[])
{
    bcm2835_init();
    bcm2835_i2c_begin();

    bcm2835_i2c_set_baudrate(10000); // slowing down the clock to 10kHz (default == 100kHz) works better

    int stat = init_ADT7410(ADT7410_I2C_ADDR);

    bcm2835_delay(500);    // give chip 500 ms to stabilize

    temp_samples Temp_Data;

    //stat = read_ADT7410(ADT7410_I2C_ADDR, &temp);
    //if (stat < 0) { printf("# 0x%02x error %d\n", ADT7410_I2C_ADDR, stat); }
    //else          { printf("0x%02x %.5fC\n", ADT7410_I2C_ADDR, temp);         }
    for(int i=0; i<10; ++i)
    {
        double temp = Get_ADT7410_Temp(ADT7410_I2C_ADDR);
        Temp_Data.tempC[i] = temp;
        printf("\t%.4f C\n", temp);
    }

    Calc_Metrics(Temp_Data);
    
    bcm2835_i2c_end();
    bcm2835_close();
    
    return 0;
}

static void Calc_Metrics(temp_samples samples)
{
    double temp_sum = 0;
    double cur_sample;
    int num_samples = 0;
    for(int i=0; i<10; ++i)
    {
        cur_sample = samples.tempC[i];
        if(cur_sample != -1)
        {
            temp_sum += cur_sample;
            ++num_samples;
        }

    }
    double avg = temp_sum/(double)num_samples;
    samples.temp_avg = avg;

    double SD_sum = 0;
    double SD;
    num_samples = 0;
    printf("   Avg: %.4f\n", avg);

    // calc standard deviation
    for(int i = 0; i < 10; ++i)
    {
        cur_sample = samples.tempC[i];
        if(cur_sample != -1)
        {
            SD_sum += pow(cur_sample - avg, 2);
            ++num_samples;
        }
    }
    SD = sqrt(SD_sum/(double)num_samples);
    printf("StdDev: %.4f\n", SD);
}

static double Get_ADT7410_Temp(const uint8_t addr)
{
    double cur_temp, avg_temp;
    double temperature_sum = 0;
    int num_samples = 0;

    for(int i =0; i<4; ++i)
    {
        int status = read_ADT7410(ADT7410_I2C_ADDR, &cur_temp);
        if( status == STATUS_GOOD )
        {
            temperature_sum += cur_temp;
            //printf(" %.4f, ", cur_temp);
            ++num_samples;
        }
        bcm2835_delay(250); // 250 ms per second in 16 bit mode
    }
    avg_temp = temperature_sum / (double)num_samples;
    return avg_temp;
}

// Return 0 if OK, -ve to show error
static int init_ADT7410(const uint8_t addr)
{
    int stat;
    uint8_t buff[4];

    bcm2835_i2c_setSlaveAddress(addr);
    
    buff[0] = ADT7410_RESET;
    if ((stat = bcm2835_i2c_write((char *)buff, 1)) != 0)   // send reset
        return -(0x10 + stat);

    bcm2835_delay(1); // Device needs 200us after reset, give it 1ms

    buff[0] = ADT7410_CONFIG;
    buff[1] = 0x80;    // 16bit cts conversions

    if ((stat = bcm2835_i2c_write((char *)buff, 2)) != 0)   // write config to ADT7410
        return -(0x20 + stat);

    char reg = ADT7410_IDREG;
    if ((stat = bcm2835_i2c_read_register_rs(&reg, (char *)buff, 1)) != 0)
        return -(0x30 + stat);

    if ((buff[0] & 0xf8) != 0xc8)
        return -0x3f;

    return 0;
}  

// Return 0 if OK, -ve to show error
// Set *temp to be the temperature in Celsius
static int read_ADT7410(const uint8_t addr, double *temp)
{
  int stat;
  uint8_t buff[4];

  bcm2835_i2c_setSlaveAddress(addr);
  
  char reg = ADT7410_T_MSB;
  if ((stat = bcm2835_i2c_read_register_rs(&reg, (char *)buff, 2)) != 0)
    return -(0x40 + stat);

  // ADT74x0 puts MSB first so flip order
  int16_t hi = buff[0];
  int16_t lo = buff[1];

  int16_t t128 = hi << 8 | lo;

  *temp = t128 / 128.0;

  return 0;
}