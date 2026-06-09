#include"hwhandler.h"                                                                                                                                                                      #include "hwhandler.h"
#include<QDebug>
// Use mapped_base here, but do not define it

void *mapped_base, *mapped_dev_base;
off_t dev_base = XPAR_AXI_COMBINED_BASEADDR;

void *mapped_base2, *mapped_dev_base2;
off_t dev_base2 = SPEAKER_BASEADDR;

void *mapped_base3, *mapped_dev_base3;
off_t dev_base3 = STEPPER_MOTOR;
#define SPI_DEVICE "/dev/spidev2.1"
#define SPI_BITS_PER_WORD 8
#define SPI_SPEED 1000000

static const char *ltc2604_device = "/dev/spidev2.1";


hwhandler::hwhandler(QObject *parent):
    QThread(parent)
{
    l = new ltc2614;
    v = new Vaccum;

    memfd = open("/dev/mem", O_RDWR | O_SYNC);
    if (memfd == -1) {
        //        printf("Can't open /dev/mem.\n");
        exit(0);
    }
    mapped_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_base & ~MAP_MASK);
    if (mapped_base == (void *) -1) {
        //        printf("Can't map the memory to user space.\n");
        exit(0);
    }

        mapped_dev_base = mapped_base + (dev_base & MAP_MASK);
        
    mapped_base2 = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_base2 & ~MAP_MASK);
    if (mapped_base2 == (void *) -1) {
        //        printf("Can't map the memory to user space.\n");
        exit(0);
    }

        mapped_dev_base2 = mapped_base2 + (dev_base2 & MAP_MASK);

        mapped_base3 = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memfd, dev_base3 & ~MAP_MASK);
        if (mapped_base3 == (void *) -1) {
            //        printf("Can't map the memory to user space.\n");
            exit(0);
        }

            mapped_dev_base3 = mapped_base3 + (dev_base3 & MAP_MASK);

            phaco_off();
            phaco_power(0);
            qDebug() << "[HW INIT] DAC initialized to 0 at startup";
}

void hwhandler::vit_on(int periodCount)
{
 // qDebug()<<"the values is sended to board"<<periodCount;
    int nONtime = static_cast<int>(periodCount / 60.0); // Convert to integer
    //int nONTIME[43] = {40,40,40,40,40,40,40,40,40,40,40,40,38,36,36,30,28,23,22,19,17,15,14,12,12,11,11,10,10,10,10,10,10,10,9,9,8,8,7,7,7,7,7};PERISTALTIC
    int nONTIME[43] = {40,40,40,40,40,40,40,40,40,40,40,40,38,36,36,30,28,23,22,19,17,17,16,14,14,13,13,12,12,12,10,10,10,10,9,9,8,8,7,7,7,7,7};//VENTURI
    if (periodCount < 60 || periodCount > 2520)
        return;

    if (nONtime < 0) nONtime = 0; // Clamp to minimum
    if (nONtime >= 43) nONtime = 42; // Clamp to maximum
    *((uint8_t *) (mapped_dev_base + VIT_ONOFF_REG)) = ON_MASK;
    *((uint8_t *) (mapped_dev_base + VIT_ONTIME_REG)) = nONTIME[nONtime];
 *((uint16_t *) (mapped_dev_base  + VIT_COUNT_REG ))    = (uint16_t)((1000.0/periodCount)*60);
   // qDebug() << "The count is" << nONTIME[nONtime]<<"........"<<periodCount<<nONtime<<(uint16_t)((1000.0/periodCount)*60);
}

void hwhandler::vit_off()
{
    *((uint8_t *) (mapped_dev_base  + VIT_ONOFF_REG ))   = OFF_MASK;
        //qDebug()<<"Vit Off";
}

void hwhandler::vit_ontime(int ontime)
{
//qDebug()<<"the ontime is reterived from  the mainwindow is"<<ontime;
    *((uint8_t *) (mapped_dev_base  + VIT_ONTIME_REG ))   = ontime;
}


void hwhandler::vso_ontime(float ontime)
{

    *((uint16_t *) (mapped_dev_base  + VSO_PWM_ON_REG ))   = (uint16_t)(ontime);
}

void hwhandler::vso_period(float count)
{
*((uint16_t *) (mapped_dev_base  + VSO_PWM_PERIOD_REG ))    = (uint16_t)(count);
}

void hwhandler::airinjon()
{
    *((uint8_t *) (mapped_dev_base  + AI_ONOFF_REG ))    = ON_MASK;

}

void hwhandler::airinjoff()
{
    *((uint8_t *) (mapped_dev_base  + AI_ONOFF_REG ))    = OFF_MASK;
    airpreset(0);
    aircount(0);

}

void hwhandler::airpreset(int count)
{

    *((uint16_t *) (mapped_dev_base  + AI_PRESET_REG ))    = (uint16_t)(count);

}

void hwhandler::aircount(int count)
{
    *((uint16_t *) (mapped_dev_base  + AI_COUNT_REG ))    = (uint16_t)(count);

}

void hwhandler::pinchon()
{
    *((uint16_t *) (mapped_dev_base + PINCH_VALVE)) |= 0x08;
}

void hwhandler::pinchoff()
{
    *((uint16_t *) (mapped_dev_base + PINCH_VALVE)) &= 0x07;
}


void hwhandler::vso_off()
{
    double timeon=0;
    double freq=65536;
    double resolution=0.000000005;

    vso_ontime(((timeon / (100*freq)) / resolution));
    vso_period((1/freq) / resolution);
}

int hwhandler::phaco_power(int val)
{   
//    int dac_value = val*16384/100;
//    l->writeDAC(dac_value);
    //qDebug()<<"The dac count will be"<<val;
    convert_dac(0x30,val);
return v->convert(0xA7);

}


void hwhandler::dia_on()
{
    *((uint16_t *) (mapped_dev_base  + DIA_ONOFF_REG ))   = ON_MASK;
    //diathermy(diapow);

}

void hwhandler::diathermy(int diapow)
{
  
    // Convert the percentage to a DAC value
    int dac_value = diapow * 16384 / 100;
    l->writeDAC(0x33,dac_value);

}

void hwhandler::dia_off()
{
    *((uint16_t *) (mapped_dev_base  + DIA_ONOFF_REG ))   = OFF_MASK;
}

void hwhandler::dia_count(int count)
{
    *((uint8_t *) (mapped_dev_base  + DIA_COUNT_REG ))    = (uint8_t)(count);
}

void hwhandler::write_motor(uint16_t status, uint16_t direction, uint16_t value)
{
 // qDebug()<<status<< direction<<value;
    *((uint16_t *) (mapped_dev_base  + REG1 )) = status | direction;
    *((uint16_t *) (mapped_dev_base  + REG2 )) = Flow_LUT[value];
}
void hwhandler::fs_count_limit(int count)
{
   //qDebug()<<"the fs count limit in hwhandler"<<count;
   // count=1900;
  // fscountlimit=count;

    *((uint16_t *) (mapped_dev_base  + FS_COUNT_REG ))   = (uint16_t)(count);

}
void hwhandler::freq_count(int count)
{
   // convert_dac(count);
    //qDebug()<<"the hwhandler freqcount is"<<count;
    //count=2500;
   //freqcount=count;
    *((uint16_t *) (mapped_dev_base  + FREQ_COUNT_REG ))   = (uint16_t)(count);

}

void hwhandler::phaco_on(int nFSCount)
{
   // nFSCount=1800;
   // qDebug()<<"resonantfreq _ fscountlimit _ 3rd position count"<<freqcount<<fscountlimit<<nFSCount;
              *((uint16_t *) (mapped_dev_base  + PHACO_ONOFF_REG ))   = (nFSCount <<1) | 0x01;
    //*((uint16_t *) (mapped_dev_base  + FS_COUNT_REG)) = nFSCount;
   // qDebug()<<(nFSCount <<1 | 0x01);

}

void hwhandler::phaco_off()

{
    *((uint16_t *) (mapped_dev_base  + PHACO_ONOFF_REG ))   = 0x00;
    //*((uint16_t *) (mapped_dev_base  + FS_COUNT_REG)) = 0;
    *((uint16_t *) (mapped_dev_base  + FREQ_COUNT_REG)) = 0;

}

void hwhandler::pdm_mode(int mode)
{
    *((uint16_t *) (mapped_dev_base  + PDM_MODE_REG ))   = (uint16_t)(mode);

}

void hwhandler::pulse_count(int count)
{
    *((uint16_t *) (mapped_dev_base  + PULSE_COUNT_REG ))   = (uint16_t)(500.0/count);

}

void hwhandler::burst_length(int time)
{
    *((uint16_t *) (mapped_dev_base  + BURST_LENGTH_REG ))   = (uint16_t)(time*10);
 
}

void hwhandler::cold_pulse(int time, int pulse)
{
    *((uint16_t *) (mapped_dev_base  + PULSE_COUNT_REG ))   = (uint16_t)(500.0/pulse);
    *((uint16_t *) (mapped_dev_base  + COLD_PULSE_REG ))   = (uint16_t) (time*500)/(pulse*100);

}


void hwhandler::burst_off_length(int length)
{
    *((uint16_t *) (mapped_dev_base  + BURST_OFF_LENGTH_REG ))   = (uint16_t)(length*10);

}



void hwhandler::digitalgain(int value)
{
    l->writeDAC7811(value);
    //qDebug()<<"the ltc called"<<value;
}

void hwhandler::emitTuneStartPhaco()
{
   *((uint16_t *) (mapped_dev_base  + TUNE_REQ_REG ))   = (uint16_t)(TUNE_REQUEST_MASK);
 //   *((uint16_t *) (mapped_dev_base  + PHACO_ONOFF_REG)) = 3001;
 //   *((uint16_t *) (mapped_dev_base  + FS_COUNT_REG    )) = 3000;
 //   *((uint16_t *) (mapped_dev_base  + PULSE_COUNT_REG )) = (uint16_t)(500/1.0);
 //   *((uint16_t *) (mapped_dev_base  + PDM_MODE_REG ))    = CONTINOUS;

}

void hwhandler::emitTuneStopPhaco()
{
    *((uint16_t *) (mapped_dev_base  + TUNE_REQ_REG ))   = (uint16_t)(0x00);
  //   *((uint16_t *) (mapped_dev_base  + PHACO_ONOFF_REG)) = 3001;
  //   *((uint16_t *) (mapped_dev_base  + FS_COUNT_REG    )) = 3000;
  //   *((uint16_t *) (mapped_dev_base  + PULSE_COUNT_REG )) = (uint16_t)(500/1.0);
  //   *((uint16_t *) (mapped_dev_base  + PDM_MODE_REG ))    = CONTINOUS;

}

void hwhandler::safetyvent_on()
{
    *((uint16_t *) (mapped_dev_base  + SIL_OIL_REG ))   |= 0x0001;
}

void hwhandler::safetyvent_off()
{
    *((uint16_t *) (mapped_dev_base  + SIL_OIL_REG ))   &= 0xFFFE;
}

void hwhandler::pinchvalve_on()
{
//    // Turn off pinchvalve2 (clear bit 2)
  //*((uint16_t *) (mapped_dev_base + SIL_OIL_REG)) &= ~(1 << 2);  // Clear bit 2

//    // Turn on pinchvalve (set bit 1)
 // *((uint16_t *) (mapped_dev_base + SIL_OIL_REG)) |= (1 << 1);   // Set bit 1

     //*((uint16_t *) (mapped_dev_base + SIL_OIL_REG)) |= 0x02;

//qDebug()<<"the pinchvalveon";
    *((uint16_t *) (mapped_dev_base  + SIL_OIL_REG ))   |= 0x0002;

}
void hwhandler::pinchvalve_off()
{
    // Clear bit 1
  //  qDebug()<<"the pinchvalveoff";

// *((uint16_t *) (mapped_dev_base + SIL_OIL_REG)) &= ~(1 << 1);  // Clear bit 1
  *((uint16_t *) (mapped_dev_base + SIL_OIL_REG)) &= 0xFFFC;
}


void hwhandler::pinchvalve2_on()
{
//    // Turn off pinchvalve (clear bit 1)
//    *((uint16_t *) (mapped_dev_base + SIL_OIL_REG)) &= ~(1 << 1);  // Clear bit 1

//    // Turn on pinchvalve2 (set bit 2)
//    *((uint16_t *) (mapped_dev_base + SIL_OIL_REG)) |= (1 << 2);   // Set bit 2
    *((uint16_t *) (mapped_dev_base + SIL_OIL_REG)) |= 0x0004;

}

void hwhandler::pinchvalve2_off()
{
    // Clear bit 2
    *((uint16_t *) (mapped_dev_base + SIL_OIL_REG)) &= ~(1 << 2);  // Clear bit 2
}


//void hwhandler::convert_dac(int channel,int count)
//{
//    uint16_t dac_value;
  //  if(channel == 0x30){
 //     dac_value = (int) (count);
 //    dac_value = 39321 + (int) (count*249.03);
//     if(dac_value >=64224)
 //        dac_value==64224;
//      float voltage = (float)dac_value / 64224 * 5.0f;
 //   l->writeDAC(channel,dac_value);
//
//    }
//
//
//}

void hwhandler::convert_dac(int channel,int count)
{
    uint16_t dac_value;
    if(channel == 0x30){
        dac_value = 39321 + (int)(count * 249.03);
        if(dac_value >= 64224)
            dac_value = 64224;    // = not ==
        if(dac_value < 39321)
            dac_value = 39321;
        float voltage = (float)dac_value / 64224 * 5.0f;
        qDebug() << "[DAC] count=" << count
                 << "dac_value=" << dac_value
                 << "voltage=" << voltage;
        l->writeDAC(channel, dac_value);
    }
}
void hwhandler::type550X_count(int channel, int count)
{
   // qDebug()<<"the channel and count"<<channel<<count;
    if(channel == 0x33){
     int nVacCount=static_cast<int>(count / 5.0);
               int nVoltage[101] = {
                   0,160,260,365,480,615,740,840,965,1075,1195,1305,1425,1525,1640,
                   1755,1870,1975,2025,2115,2210,2285,2230,2420,2485,2580,2700,2780,
                   2855,2935,3110,3165,3245,3355,3470,3605,3720,3850,3960,4075,
                   4200,4320,4450,4575,4695,4830,4955,5090,5210,5340,5455,5585,5705,
                   5835,5955,6080,6210,6330,6475,6600,6690,6825,6975,7085,7210,7345,
                   7465,7600,7735,7865,8005,8120,8245,8360,8500,8620,8735,8860,8995,
                   9140,9255,9395,9500,9620,9745,9850,9980,10105,10220,10345,10470,10595,
                   10725,10860,10995,11105,11230,11385,11515,11665,11965

         };

       if (nVacCount >= 101) {
           nVacCount = 100;
           if(nVacCount == 0){
               nVacCount = 0;
           }
       } else if (nVacCount < 0) {
           nVacCount = 0;
       }
       int nDACVolt = nVoltage[nVacCount];
       l->writeDAC550x(channel,nDACVolt);
       }
}
void hwhandler::speaker_on(uint8_t value, uint8_t asp, uint8_t irr, uint8_t occ)
{
    if(asp==1)
    {
        *((uint8_t *) (mapped_dev_base2  + 0x04 ))   = SPEAKER_ASPIRATION;
        uint8_t freq = 128-(value*0.14);
        *((uint8_t *) (mapped_dev_base2  + 0x00 ))   = freq;
    }
    if(irr==1)
    {
        *((uint8_t *) (mapped_dev_base2  + 0x04 ))   = SPEAKER_IRRIGATION;
    }
    if(occ==1)
    {
        *((uint8_t *) (mapped_dev_base2  + 0x04 ))   = SPEAKER_OCCLUSION;
    }
}

void hwhandler::speaker_off()
{
    *((uint8_t *) (mapped_dev_base2  + 0x04 ))   = 0x00;
}

void hwhandler::vibrator_on(uint8_t onoff, uint8_t position,uint16_t value)
{
    *((uint8_t *) (mapped_dev_base2  + 0x08 ))   = onoff;
    *((uint8_t *) (mapped_dev_base2  + 0x09 ))   = position;
    *((uint16_t *) (mapped_dev_base2  + 0x0A )) =value;

}

void hwhandler::vibrator_off()
{
    *((uint8_t *) (mapped_dev_base2  + 0x08 ))   = 0x0;
    *((uint8_t *) (mapped_dev_base2  + 0x09 ))   = 0x0;
    *((uint16_t *) (mapped_dev_base2  + 0x0A )) =0x0;


}


void hwhandler::buzz()
{
    *((uint8_t *) (mapped_dev_base2  + 0x04 ))   = 0x81;
    *((uint8_t *) (mapped_dev_base2  + 0x00 ))   = 200;
    usleep(50000);
    *((uint8_t *) (mapped_dev_base2  + 0x04 ))   = 0x00;
}

void hwhandler::stepper_motorOn()
{
*((uint8_t *) (mapped_dev_base3 + 0x10 )) = 0x01;
}

void hwhandler::stepper_motorOFF()
{
    *((uint8_t *) (mapped_dev_base3 + 0x10 )) = 0x00;
}

void hwhandler::stepper_motorCount(int count)
{

    int step_count = static_cast<int>(count);
    int stpper_motor[41]={
        0,0,4000,0,3000,0,2500,0,1800,0,1000,0,900,0,800,0,700,0,600,0,500,
        0,450,0,400,0,350,0,300,0,250,0,230,0,215,0,200,0,180,0,170
    };
//    qDebug()<<"the input from the phaco is"<<count<<"the step count "<<step_count;
//    qDebug()<<"the stpper motor count is"<<stpper_motor[step_count];


    if (step_count < 0) step_count = 0;
    if (step_count >= 41)step_count = 41;

    if(step_count <=41){
    *((uint16_t *) (mapped_dev_base3  + 0x10)) =stpper_motor[step_count];
    }
    if(step_count > 41){
        step_count=41;
         *((uint16_t *) (mapped_dev_base3  + 0x10 )) =stpper_motor[step_count];
    }

}

void hwhandler::stepper_motorclk(int clock)
{
  //  *((uint16_t *) (mapped_dev_base3  + 0x10)) =clock;

}
