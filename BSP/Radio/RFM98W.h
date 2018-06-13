/* @file: radio.h
 * @author: Gerald Ebmer
 * @date: 13.06.2018
 * @brief: header file for RFM98W lora module support class
 */

#include "Radio.h"


class RFM98W : public Radio
{

public:
    RFM98W();
    int serviceRadio();

private:
    int sendBytes(unsigned char *data, int len);


};