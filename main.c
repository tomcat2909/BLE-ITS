
#include "project.h"

#define Dynamic (1u)

#if Dynamic
extern CYBLE_GAPP_DISC_MODE_INFO_T cyBle_discoveryModeInfo;

CYBLE_API_RESULT_T res;

#define Payload (cyBle_discoveryModeInfo.advData->advData)

#define TempIndex (13u) 
#define Min (10u)
#define Max (30u)
#define LoopDelay (10u)
    
#endif    
uint8 temp=Min;
uint8 count=0u;
uint8 state=1u;

char device_name[20];


void ChangeAdvPacket(void)
{
    if(CyBle_GetBleSsState()==CYBLE_BLESS_STATE_EVENT_CLOSE)
    {
        count++;
        //CyDelay(1000);
        
        if(count>=LoopDelay)
        {
            Payload[TempIndex]=temp++;
            
            if(temp>=Max)
            temp=Min;
            
            count=0u;
            
            res=CyBle_GapUpdateAdvData(cyBle_discoveryModeInfo.advData,cyBle_discoveryModeInfo.scanRspData);
            
            if(res==CYBLE_ERROR_OK)
            {
                led_Write(state);
                if(state==1u)
                state=0u;
                else if(state==0u)
                state=1u;
                CyDelay(1000);   
            }
           
        }
    }
    
   // else
    //led_Write(1);
}

    

void StackEventHandler(uint32 eventcode, void *eventparam)
{
    CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T wReqParam;
    uint8 i;
    CYBLE_API_RESULT_T code;
    
    char name[10]={};
    
    switch(eventcode)
    {
        case CYBLE_EVT_STACK_ON:                               
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:         CyBle_GapGetLocalName(device_name);  
                                                        CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
                                                        
                                                        
                                                        break;
        
        case CYBLE_EVT_GATTS_WRITE_REQ:                 led_Write(1);
                                                        wReqParam=*(CYBLE_GATTS_WRITE_CMD_REQ_PARAM_T*)eventparam;
                                                        for(i=0;i<wReqParam.handleValPair.value.len;i++)
                                                        {/*Client has to write the ASCI values of the characters that are in the name*/
                                                            name[i]=wReqParam.handleValPair.value.val[i];
                                                            device_name[i] = name[i];
                                                        }
                                                        
                                                        code = CyBle_GapSetLocalName((const char *)device_name);
                                                        
                                                        if(code==CYBLE_ERROR_OK)
                                                        led_Write(0);
            
            
            
                                                        CyBle_GattsWriteRsp(cyBle_connHandle);
            
                                                        break;                  
           
                        
                             
        
        default:                                        break;
    }
}



int main(void)
{
    CyGlobalIntEnable;
    
    
    CyBle_Start(StackEventHandler);
    
    while(1)
    {
        CyBle_ProcessEvents();
        
        #if Dynamic
        ChangeAdvPacket();
        #endif
    }

}


