#ifndef __RADIO_EVENT_H__
#define __RADIO_EVENT_H__

#include "dot_util.h"
#include "mDotEvent.h"
#include "Fota.h"

class RadioEvent : public mDotEvent
{

public:
    RadioEvent();

    virtual ~RadioEvent() {}
    
    void setReceiveHandler(void (*onRx)(uint8_t *RxBuffer, uint8_t RxBufferSize));

    virtual void RadioEvent::PacketRx(uint8_t port, 
                          uint8_t *payload, 
                          uint16_t size, 
                          int16_t rssi, 
                          int8_t snr, 
                          lora::DownlinkControl ctrl, 
                          uint8_t slot, 
                          uint8_t retries, 
                          uint32_t address, 
                          bool dupRx);

    /*!
     * MAC layer event callback prototype.
     *
     * \param [IN] flags Bit field indicating the MAC events occurred
     * \param [IN] info  Details about MAC events occurred
     */
    virtual void MacEvent(LoRaMacEventFlags* flags, LoRaMacEventInfo* info);
    
private:
    void (*mOnRx)(uint8_t *RxBuffer, uint8_t RxBufferSize);
};

#endif

