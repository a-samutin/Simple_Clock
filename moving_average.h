#ifndef MOVING_AVERAGE
#define MOVING_AVERAGE

typedef struct AvrgData
{
  uint8_t  head;
  uint8_t  tail;
  uint8_t  num;
  uint16_t sum;
  uint8_t  bufSz;
  uint8_t  *buf;   //caller must allocate data buffer before InitAvrg() invocation 
} avrgData_t;

// allowed size values - 4,8,16,32,64,128; 
void InitAvrg(avrgData_t *data,uint8_t size)
{
  data->head = 0;
  data->tail = 0;
  data->num = 0;
  data->sum = 0;
//  if (size>BUF_SZM) size = BUF_SZM;
  data->bufSz = size;
}

//Pushes new data to buffer and returns curren average
uint8_t GetNextAvrg(avrgData_t *data, uint8_t in)
{
    uint32_t ret;
    uint8_t BUF_MASK=data->bufSz - 1;
    data->sum += in;
    if (++(data->num) > data->bufSz)
    {
        data->num = data->bufSz;
        data->sum -= data->buf[data->tail];
        data->tail = ++(data->tail) & BUF_MASK;
    }
    data->buf[data->head] = in;
    data->head = ++(data->head) & BUF_MASK;
    //cчитаем среднее округляя результат с десятых до целого
    ret = data->sum;
    ret = ret * 10 / data->num;
    if (ret % 10 < 5)
        ret = ret / 10;
    else
        ret = ret / 10 + 1;
    return uint8_t(ret);
}
#endif //MOVING_AVERAGE
