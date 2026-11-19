#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <net/if_arp.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <stdbool.h>
#include <poll.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include "queue_client.h"
#ifdef CONFIG_LBH_CODEC
#include "lsbc.h"
#include "aacdec.h"
#include "coder.h"
#endif //CONFIG_LBH_CODEC
#include <sys/stat.h>
#include "lbh_wav.h"
#include "lbh_codec.h"


#define SOCKER_BUFFER_SIZE  1024

BT_ADDR connect_addr;

void client_queue_bt_msg_callback(uint8_t *data, uint16_t length)
{
    printf("client_queue_bt_msg_callback\n");
    int i = 0;
    BT_HST_MSG msg;
    if(length>0){
        msg.EventId = ((uint32_t)data[3]<<24) | ((uint32_t)data[2]<<16) | ((uint32_t)data[1]<<8) | ((uint32_t)data[0]);
        msg.len = length-6;
        msg.buff = data+6;
        if(msg.EventId != AIC_ADP_A2DP_STREAM_DATA_IND){
            for(i = 0; i < length; i++){
                printf("%02X ", data[i]);
            }
            printf("\r\n");
        }
        switch(msg.EventId ){
            case AIC_ADP_STACK_INIT:
                {
                    printf("APP: bt_stack_init success.\n");
                    //app_bt_setscanmode(BT_ALLSCAN);
#ifdef CONFIG_LBH_CODEC
                    app_codec_socket_init();
#endif//CONFIG_LBH_CODEC
                    app_bt_inquiry_dev(8,8);
                }
                break;
            case AIC_ADP_ACCESSIBLE_CHANGE:
                {
                    uint8_t mode = msg.buff[0];
                    printf("APP:accessible mode = %d\n",mode);
                }
                break;
            case AIC_ADP_ENCRYPYION_CHANGE:
                printf("APP: AIC_ADP_ENCRYPYION_CHANGE.\n");
                break;
/*********************************************************************/
            case AIC_ADP_ACL_CONNECT_IND:
                printf("APP: AIC_ADP_ACL_CONNECT_IND.\n");
            case AIC_ADP_ACL_CONNECT_CNF:
                printf("APP: AIC_ADP_ACL_CONNECT_CNF.\n");
                //app_bt_setscanmode(BT_NOSCAN);
                break;
            case AIC_ADP_ACL_DISCONNECT:
                printf("APP:acl_disconnect reason \n");
                break;
            case AIC_ADP_PAIRING_COMPLETE:
                printf("APP: AIC_ADP_PAIRING_COMPLETE.\n");

                break;
            case AIC_ADP_SCO_CONNECT_IND:
                printf("APP: AIC_ADP_SCO_CONNECT_IND.\n");
            case AIC_ADP_SCO_CONNECT_CNF:
                printf("APP: AIC_ADP_SCO_CONNECT_CNF.\n");

                break;
            case AIC_ADP_SCO_DISCONNECTED:
                printf("APP: AIC_ADP_SCO_DISCONNECTED.\n");

                break;
            case AIC_ADP_INQUIRY_RESULT:
                {
                    uint8_t *p = msg.buff;
                    uint8_t tem_addr[6]={0x0d,0xb5,0x7e,0x1c,0x99,0x70};
                    uint8_t addr[6];
                    uint32_t cod = (uint32_t)p[0]|(((uint32_t)p[1])<<8)|(((uint32_t)p[2])<<16)|(((uint32_t)p[3])<<24);
                    memcpy(addr,p+4,6);
                    printf("APP: aic_adp_inquiry_result. cod = 0x%x,addr 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x\n",cod,\
                        addr[5],addr[4],addr[3],addr[2],addr[1],addr[0]);
                    if(!memcmp(addr,tem_addr,6)){
                        memcpy(connect_addr.addr,addr,6);
                        app_bt_inquiry_cancel();
                        app_bt_source_connect_a2dp(&connect_addr);
                    }
                }
                break;
            case AIC_ADP_INQUIRY_COMPLETE:
                printf("APP: aic_adp_inquiry_complete.\n");
                break;
                /** The Inquiry process is canceled. */
            case AIC_ADP_INQUIRY_CANCELED:
                printf("APP: aic_adp_inquiry_canceled.\n");
                break;
/*********************************************************************/
            case AIC_ADP_A2DP_STREAM_OPEN:
                {
                    uint8_t *p = msg.buff;
                    uint8_t a2dp_role = p[0];
                    uint8_t codec_type = p[1];
                    uint8_t sample_rate_bit = p[2];
                    uint8_t sbc_frame_number = p[3];
                    uint8_t channel = p[4];
                    uint8_t numBlocks = p[5];
                    uint8_t numSubBands = p[6];
                    uint8_t maxbitpool = p[7];
                    printf("APP: aic_adp_a2dp_stream_open.\n");
                    printf("APP: a2dp_role %d,codec_type %d,sample_rate_bit %d,sbc_frame_number %d,channel %d,numBlocks %d,numSubBands %d,maxbitpool %d\n",\
                                a2dp_role,codec_type,sample_rate_bit,sbc_frame_number,channel,numBlocks,numSubBands,maxbitpool);
#ifdef CONFIG_LBH_CODEC
		    app_codec_client_msg_send(&msg,RX_DATA_MODE);
#endif //CONFIG_LBH_CODEC
                    app_bt_a2dp_start(&connect_addr,1);
                }
                break;
            case AIC_ADP_A2DP_STREAM_STARTED:
                {
                    uint8_t *p = msg.buff;
                    uint8_t a2dp_role = p[0];
                    printf("APP: aic_adp_a2dp_stream_started.a2dp_role %d\n",a2dp_role);
                    app_bt_trace_onoff(1,0xffffffff);
                    if(a2dp_role == 0)//a2dp source
                    {
#ifdef CONFIG_LBH_CODEC
                        pthread_t lbh_a2dp_pcm_tx_test_th;
                        pthread_create( &lbh_a2dp_pcm_tx_test_th, NULL, lbh_a2dp_pcm_data_tx_test_thread, NULL);
#endif //CONFIG_LBH_CODEC
                    }
                }
                break;
            case AIC_ADP_A2DP_STREAM_SUSPENDED:
                {
                    printf("APP: aic_adp_a2dp_stream_suspended.\n");
                }
                break;
            case AIC_ADP_A2DP_STREAM_CLOSED:
                {
                    printf("APP: aic_adp_a2dp_stream_closed.\n");
#ifdef CONFIG_LBH_CODEC
                    app_codec_client_msg_send(&msg,RX_DATA_MODE);
#endif //CONFIG_LBH_CODEC
                }
                break;
            case AIC_ADP_A2DP_STREAM_PACKET_SENT:
                {
                    printf("APP:a2dp source data send succ!\n");
                }
                break;
            case AIC_ADP_A2DP_STREAM_DATA_IND:
                {
                    uint8_t *p = msg.buff;
                    uint16_t a2dp_headerlen = (uint16_t)p[0]|(((uint16_t)p[1])<<8);
                    uint8_t *p_data = &p[2+a2dp_headerlen];
                    uint16_t p_len = msg.len-2-a2dp_headerlen;
                    printf("APP: aic_adp_a2dp_stream_data_ind. len = %d\n",p_len);
#ifdef CONFIG_LBH_CODEC
                    app_codec_client_msg_send(&msg,RX_DATA_MODE);
#endif //CONFIG_LBH_CODEC
                }
                break;
/*********************************************************************/
            case AIC_ADP_AVRCP_CONNECT:
                {
                    printf("APP: aic_adp_avrcp_connect.\n");
                }
                break;
            case AIC_ADP_AVRCP_DISCONNECT:
                {
                    printf("APP:app_avrcp_disconnect\n");
                }
                break;
            case AIC_ADP_AVRCP_PANEL_PRESS:
                {
                    printf("APP: aic_adp_avrcp_panel_press.\n");
                }
                break;
            case AIC_ADP_AVRCP_ADV_VOLUME_CHANGE:
                {
                    uint8_t *p = msg.buff;
                    uint8_t volume = p[0];
                    printf("APP: aic_adp_avrcp_adv_volume_change %d.\n",volume);
                }
                break;
            case AIC_ADP_AVRCP_ADV_PLAYSTAUTS_CHANGE:
                {
                    uint8_t *p = msg.buff;
                    uint8_t playstauts = p[0];
                    printf("APP: aic_adp_avrcp_adv_playstauts_change %d.\n",playstauts);
                }
                break;
/*********************************************************************/
            case AIC_ADP_HFP_CONNECTED:
                {
                    uint8_t *p = msg.buff;
                    uint8_t role = p[0];
                    uint8_t result = p[1];
                    printf("APP:app_hfp_connect %d,%d\n",role,result);
                }
                break;
            case AIC_ADP_HFP_DISCONNECTED:
                {
                    uint8_t *p = msg.buff;
                    uint8_t role = p[0];
                    uint8_t discReason = p[1];
                    printf("APP:app_hfp_disconnect %d,%d\n",role,discReason);
                }
                break;
            case AIC_ADP_HFP_AUDIO_CONNECTED:
                {
                    uint8_t *p = msg.buff;
                    uint8_t role = p[0];
                    uint16_t negotiated_codec = (uint16_t)p[1]|(((uint16_t)p[2])<<8);
                    printf("APP:hfp_sco_connect %d,%d\n",role,negotiated_codec);
#ifdef CONFIG_LBH_CODEC
                    app_codec_client_msg_send(&msg,RX_DATA_MODE);
#endif //CONFIG_LBH_CODEC
                }
                break;
            case AIC_ADP_HFP_AUDIO_DISCONNECTED:
                {
                    uint8_t *p = msg.buff;
                    uint8_t role = p[0];
                    printf("APP: aic_adp_hfp_audio_disconnected.%d\n",role);
#ifdef CONFIG_LBH_CODEC
                    app_codec_client_msg_send(&msg,RX_DATA_MODE);
#endif //CONFIG_LBH_CODEC
                }
                break;
            case AIC_ADP_HFP_AUDIO_DATA_SENT:
                {
                    uint8_t *p = msg.buff;
                    uint8_t role = p[0];
                    printf("APP: aic_adp_hfp_audio_data_sent %d.\n",role);
                }
                break;
            case AIC_ADP_HFP_AUDIO_DATA_IND:
                {
                    uint8_t *p = msg.buff;
                    uint8_t p_len = msg.len;
                    printf("APP: aic_adp_hfp_audio_data_ind len %d.\n",p_len);
#ifdef CONFIG_LBH_CODEC
                    app_codec_client_msg_send(&msg,RX_DATA_MODE);
#endif //CONFIG_LBH_CODEC
                }
                break;
            case AIC_ADP_HFP_CALL_STATUS_IND:
                {
                    uint8_t *p = msg.buff;
                    uint32_t call_state = (uint32_t)p[0]|(((uint32_t)p[1])<<8)|(((uint32_t)p[2])<<16)|(((uint32_t)p[3])<<24);
                    printf("APP: aic_adp_hfp_call_status_ind call_state=0x%x.\n",call_state);
                }
                break;
            case AIC_ADP_HFP_CALLSETUP_STATUS_IND:
                {
                    uint8_t *p = msg.buff;
                    uint32_t call_state = (uint32_t)p[0]|(((uint32_t)p[1])<<8)|(((uint32_t)p[2])<<16)|(((uint32_t)p[3])<<24);
                    printf("APP: aic_adp_hfp_callsetup_status_ind call_state=0x%x.\n",call_state);
                }
                break;
            case AIC_ADP_HFP_CALLHOLD_STATUS_IND:
                {
                    uint8_t *p = msg.buff;
                    uint8_t hfp_call_hold = p[0];
                    printf("APP: aic_adp_hfp_callhold_status_ind hfp_call_hold=0x%x.\n",hfp_call_hold);
                }
                break;
            case AIC_ADP_HFP_RING_IND:
                {
                    printf("APP: aic_adp_hfp_ring_ind.\n");
                }
                break;
            case AIC_ADP_HFP_SIRI_STATUS:
                {
                    uint8_t *p = msg.buff;
                    uint8_t siriStatus = p[0];
                    printf("APP: aic_adp_hfp_siri_status siriStatus=0x%x.\n",siriStatus);
                }
                break;
            case AIC_ADP_HFP_CURRENT_CALL_NUM:
                {
                    uint8_t *p = msg.buff;
                    char call_num[32];
                    memcpy((uint8_t *)call_num,p,32);
                    printf("APP: aic_adp_hfp_current_call_num %s.\n",call_num);
                }
                break;
            case AIC_ADP_HFP_SPEAKER_VOLUME:
                {
                    uint8_t *p = msg.buff;
                    uint8_t volume = p[0];
                    printf("APP: aic_adp_hfp_speaker_volume %d.\n",volume);
                }
                break;
/*********************************************************************/
            case AIC_ADP_SPP_CONNECTED_IND:
                {
                    uint8_t *p = msg.buff;
                    uint8_t result = p[0];
                    if(result == 0){
                        printf("APP:spp connect ind\n");
                    }
                }
                break;
            case AIC_ADP_SPP_CONNECTED_CNF:
                {
                    uint8_t *p = msg.buff;
                    uint8_t result = p[0];
                    if(result == 0){
                        printf("APP:spp connect cnf\n");
                    }
                }
            break;
            case AIC_ADP_SPP_DISCONNECTED:
                {
                    uint8_t *p = msg.buff;
                    uint8_t result = p[0];
                    printf("APP:spp disconnected %d",result);
                }
            break;
            case AIC_ADP_SPP_DATA_SENT:
                {
                    uint8_t *p = msg.buff;
                    uint16_t txdonelen = msg.len;
                    printf("APP: aic_adp_spp_data_sent txdonelen=%d.\n",txdonelen);
                }
            break;
            case AIC_ADP_SPP_DATA_IND:
                {
                    uint8_t *p = msg.buff;
                    uint16_t rxlen = msg.len;
                    printf("APP: aic_adp_spp_data_ind rxlen=%d.\n",rxlen);
                }
            break;

            default:
                break;
        }
    }
}

