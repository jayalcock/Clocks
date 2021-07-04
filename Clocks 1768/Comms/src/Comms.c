#include "Comms.h"

#define NEW_WIFI_CONNECTION 0
#define HOME_WIFI 0
#define PHONE_WIFI 0
#define LYNDSEY_WIFI 0
#define OTHER_WIFI 1 

//WIFI Constants 
const char* INIT = "AT\r\n";
const char* RESET = "AT+RST\r\n";
const char* DISCONNECT = "AT+CWQAP\r\n";
const char* MODE = "AT+CWMODE=1\r\n";
#if HOME_WIFI
const char* SSIDPWD = "AT+CWJAP=\"NETGEAR47\",\"phobicjungle712\"\r\n";
#elif PHONE_WIFI
const char* SSIDPWD = "AT+CWJAP=\"Jay's iPhone\",\"shannon1\"\r\n";
#elif LYNDSEY_WIFI
const char* SSIDPWD = "AT+CWJAP=\"L-Gav Pad\",\"hgbbs123\"\r\n";
#elif OTHER_WIFI
const char* SSIDPWD = "AT+CWJAP=\"Gavins\",\"LakeHouse9307\"\r\n";
#endif


uint8_t rxBuffer[128];
   

CTL_MUTEX_t uart0_tx_mutex;
CTL_EVENT_SET_t comms_event;


//Pointers to UART Driver object
ARM_DRIVER_USART * USARTdrv0 = &Driver_USART0;
ARM_DRIVER_USART * USARTdrv1 = &Driver_USART1;

//CTL_EVENT_SET_t * comms_evt_ptr; 

//Ring Buffer
#define  SIZE_OF_BUFFER 50
static uint8_t writeIndex = 0;


void commsThread(void *p)
{
    uint32_t status;
    uint8_t tempChar; 

    ctl_events_init(&comms_event, 0);
    
    //Initialse UART's
    UARTinit(USARTdrv0, 115200, UART0_callback);
    UARTinit(USARTdrv1, 115200, UART1_callback);
    

    status = USARTdrv0->Send("\nStartup\n", 9);
    ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &comms_event, UART0_TX_DONE, CTL_TIMEOUT_NONE, 0);
    
    ////Ensure connection to ESP
    ESP_command(INIT, 200U, 0);
       
    //Skip if already connected to WIFI
    #if NEW_WIFI_CONNECTION
    
        //Reset ESP
        ESP_command(RESET, 5000U, 0);
    
        //Disconnect WIFI
        ESP_command(DISCONNECT, 1000U, 0);
    
        //Select WIFI mode
        ESP_command(MODE, 1000U, 0);
    
        //Connect to WIFI
        ESP_command(SSIDPWD, 6000U, 0);
   
    #endif
  
   
    status = USARTdrv0->Send("\n\nReady\n", 8);
    ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &comms_event, UART0_TX_DONE, CTL_TIMEOUT_NONE, 0);
     
   
    while(1) //Loop that receives data from computer and sends to UART1, then loops back UART1 data to computer. 
    {   
  
        USARTdrv0->Receive(&tempChar, 1);
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &comms_event, UART0_RX_DONE, CTL_TIMEOUT_NONE, 0);
        
        USARTdrv1->Send(&tempChar, 1); //Send to uart1
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &comms_event, UART1_TX_DONE, CTL_TIMEOUT_NONE, 0);

    }
    
}
    
void rx_thread(void *p)
{
    char rxChar; 
    ctl_mutex_init(&uart0_tx_mutex);
    
    ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &comms_event, UART0_EVENT, CTL_TIMEOUT_NONE, 0);
    
    while(1) //Keep receiving chars and loop through to UART0 until end of string
    {   
        USARTdrv1->Receive(&rxChar, 1);
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &comms_event, UART1_RX_DONE, CTL_TIMEOUT_NONE, 0);
        
        ctl_mutex_lock(&uart0_tx_mutex, CTL_TIMEOUT_NONE, 0);
        USARTdrv0->Send(&rxChar, 1); 
        ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &comms_event, UART0_TX_DONE, CTL_TIMEOUT_NONE, 0);
        ctl_mutex_unlock(&uart0_tx_mutex);
        
        writeBuffer(&rxChar); //Write to ring buffer
        
        if(rxChar == 0x0a) //End of string 
        {
            ctl_events_set_clear(&comms_event, UART1_RX_END_OF_STRING, 0); 
    
        }
    }
}


void UARTinit(ARM_DRIVER_USART* UART_driver_number, uint32_t baud, void(*callback)(uint32_t))
{
    /*Initialize the USART driver */
    UART_driver_number->Initialize(callback);
    /*Power up the USART peripheral */
    UART_driver_number->PowerControl(ARM_POWER_FULL);
    /*Configure the USART to 115200 Bits/sec */
    UART_driver_number->Control(ARM_USART_MODE_ASYNCHRONOUS |
                      ARM_USART_DATA_BITS_8 |
                      ARM_USART_PARITY_NONE |
                      ARM_USART_STOP_BITS_1 |
                      ARM_USART_FLOW_CONTROL_NONE, baud);
     
    /* Enable Receiver and Transmitter lines */
    UART_driver_number->Control (ARM_USART_CONTROL_TX, 1);
    UART_driver_number->Control (ARM_USART_CONTROL_RX, 1);
}

void UART0init(uint32_t baud)
{
        
    /*Initialize the USART driver */
    USARTdrv0->Initialize(UART0_callback);
    /*Power up the USART peripheral */
    USARTdrv0->PowerControl(ARM_POWER_FULL);
    /*Configure the USART to 115200 Bits/sec */
    USARTdrv0->Control(ARM_USART_MODE_ASYNCHRONOUS |
                      ARM_USART_DATA_BITS_8 |
                      ARM_USART_PARITY_NONE |
                      ARM_USART_STOP_BITS_1 |
                      ARM_USART_FLOW_CONTROL_NONE, baud);
     
    /* Enable Receiver and Transmitter lines */
    USARTdrv0->Control (ARM_USART_CONTROL_TX, 1);
    USARTdrv0->Control (ARM_USART_CONTROL_RX, 1);
    
}
    
void UART1init(uint32_t baud)
{    
    /*Initialize the USART driver */
    USARTdrv1->Initialize(UART1_callback);
    /*Power up the USART peripheral */
    USARTdrv1->PowerControl(ARM_POWER_FULL);
    /*Configure the USART to 115200 Bits/sec */
    USARTdrv1->Control(ARM_USART_MODE_ASYNCHRONOUS |
                      ARM_USART_DATA_BITS_8 |
                      ARM_USART_PARITY_NONE |
                      ARM_USART_STOP_BITS_1 |
                      ARM_USART_FLOW_CONTROL_NONE, baud);
    
    /* Enable Receiver and Transmitter lines */
    USARTdrv1->Control (ARM_USART_CONTROL_TX, 1);
    USARTdrv1->Control (ARM_USART_CONTROL_RX, 1);

}

void UART0_callback(uint32_t event)
{
    uint32_t mask; 
    mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |
         ARM_USART_EVENT_TRANSFER_COMPLETE |
         ARM_USART_EVENT_SEND_COMPLETE     |
         ARM_USART_EVENT_TX_COMPLETE       ;
    if (event & mask) 
    {
        /* Success: Wakeup Thread */
        ctl_events_set_clear(&comms_event, UART0_EVENT, 0);
    }
    if (event & ARM_USART_EVENT_RECEIVE_COMPLETE) 
    {
            /* Success: Wakeup Thread */
            ctl_events_set_clear(&comms_event, UART0_RX_DONE, 0);
            return;
    }
    if (event & ARM_USART_EVENT_SEND_COMPLETE) 
    {
            /* Success: Wakeup Thread */
            ctl_events_set_clear(&comms_event, UART0_TX_DONE, 0);
            return;
    }
    if (event & ARM_USART_EVENT_RX_TIMEOUT) 
    {
        __asm("BKPT"); /* Error: Call debugger or replace with custom error handling */
    }
    if (event & (ARM_USART_EVENT_RX_OVERFLOW | ARM_USART_EVENT_TX_UNDERFLOW)) 
    {
        __asm("BKPT"); /* Error: Call debugger or replace with custom error handling */
    }
}    


void UART1_callback(uint32_t event)
{
    uint32_t mask;
    mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |
         ARM_USART_EVENT_TRANSFER_COMPLETE |
         ARM_USART_EVENT_SEND_COMPLETE     |
         ARM_USART_EVENT_TX_COMPLETE       ;
         

    if (event & ARM_USART_EVENT_RECEIVE_COMPLETE) 
    {
            /* Success: Wakeup Thread */
            ctl_events_set_clear(&comms_event, UART1_RX_DONE, 0);
            return;
    }
    if (event & ARM_USART_EVENT_SEND_COMPLETE) 
    {
            /* Success: Wakeup Thread */
            ctl_events_set_clear(&comms_event, UART1_TX_DONE, 0);
            return;
    }
    if (event & mask) 
    {
        /* Success: Wakeup Thread */
        ctl_events_set_clear(&comms_event, UART1_EVENT, 0);
    }
     if (event & ARM_USART_EVENT_RX_TIMEOUT) 
    {
        __asm("BKPT"); /* Error: Call debugger or replace with custom error handling */
    }
    if (event & (ARM_USART_EVENT_RX_OVERFLOW | ARM_USART_EVENT_TX_UNDERFLOW)) 
    {
        __asm("BKPT"); /* Error: Call debugger or replace with custom error handling */
    }
} 

 


//Write data to ring buffer
void writeBuffer(const char *tempChar)
{
    
    rxBuffer[writeIndex] = *tempChar;
    writeIndex++;
    if(writeIndex == SIZE_OF_BUFFER)
    {
        writeIndex = 0;
    }
}

//Read data from ring buffer
void readBuffer(uint8_t *arr, uint8_t length)
{
    uint8_t readIndex;
    uint8_t i = 0;
     
    //if desired data to be extracted has wrapped around end of buffer
    if(length > writeIndex)
    {
        readIndex = SIZE_OF_BUFFER - length + writeIndex;
        for(readIndex; readIndex < SIZE_OF_BUFFER; readIndex++)
        {
            arr[i] = rxBuffer[readIndex];
            i++;
            length--;
        }
        readIndex = 0;
    }
    else
    {
        readIndex = writeIndex-length;
    }
    
    for(readIndex; readIndex < length; readIndex++)
    {
 
        arr[i] = rxBuffer[readIndex];
        i++;
        if(readIndex == SIZE_OF_BUFFER)
        {
            readIndex = 0;
        }
    }
    
    
}

//Send command to ESP8266
void ESP_command(const void* command, const uint16_t delay, uint8_t size)
{
    uint8_t status;
    status = sizeof(command)/sizeof(command[0]); 
    if(size > 0)
    {
        status = USARTdrv1->Send(command, size);
    }
    else
    {
        status = USARTdrv1->Send(command, strlen(command));
    }
    ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &comms_event, UART1_TX_DONE, CTL_TIMEOUT_NONE, 0);
    ctl_events_wait(CTL_EVENT_WAIT_ANY_EVENTS_WITH_AUTO_CLEAR, &comms_event, UART1_RX_END_OF_STRING, CTL_TIMEOUT_NONE, 0);
    ctl_timeout_wait(ctl_current_time + delay);
}