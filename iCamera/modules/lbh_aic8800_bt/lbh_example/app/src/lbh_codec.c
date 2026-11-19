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
#include "lsbc.h"
#include "aacdec.h"
#include "coder.h"
#include <sys/stat.h>
#include "lbh_wav.h"
#include "lbh_codec.h"


#define SOCKER_BUFFER_SIZE  1024
#define APP_CODEC_TH_PORT   5003


int app_codec_client_socket;
int app_codec_client_on=0;
int app_codec_server_socket;
struct sockaddr_in codec_clientAddr;


sbc_t sbc;
static HAACDecoder *hAACDecoder = NULL;
static AACFrameInfo aacFrameInfo;
enum CODEC_MODE a2dp_codec_mode = SBC_MODE;
sbc_t msbc;
uint16_t sco_len;
enum SCO_CODEC_MODE hfp_codec_mode = CVSD_MODE;

extern BT_ADDR connect_addr;
extern void lbh_encode(sbc_t *sbc, unsigned char *stream, int streamlen, unsigned char *output, int outputmaxlen, int *outputlen);

void* lbh_a2dp_pcm_data_tx_test_thread( void *arg ){
    const char *filename = APP_A2DP_TEST_PCM_FILE;
    unsigned char *stream;
    struct stat st;
    int fd, pos, streamlen;
    size_t len;
    size_t frame_len;
    size_t send_len;

    if (stat(filename, &st) < 0) {
        fprintf(stderr, "Can't get size of file %s: %s\n",
            filename, strerror(errno));
        exit(0);
    }

    stream = malloc(st.st_size);

    if (!stream) {
        fprintf(stderr, "Can't allocate memory for %s: %s\n",
            filename, strerror(errno));
        exit(0);
    }

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        fprintf(stderr, "Can't open file %s: %s\n",
            filename, strerror(errno));
        free(stream);
    }

    if (read(fd, stream, st.st_size) != st.st_size) {
        fprintf(stderr, "Can't read content of %s: %s\n",
            filename, strerror(errno));
        close(fd);
        free(stream);
        exit(0);
    }

    close(fd);

    pos = 0;
    streamlen = st.st_size;
    frame_len = sbc_get_codesize(&sbc);
    send_len = frame_len*4;
    //sleep(5);
    printf("lbh_a2dp_pcm_data_tx_test_thread ready %p,frame_len = %d\r\n", arg,frame_len);
    while(1){
        if(1)
        {
            uint8_t encodebuff[(4096 * 2)];//if codec is aac ,it will have 2 stereo frames, total 4096 samples
            int encodelen = 0;
            uint8_t frame_len = 0;
            uint8_t *p_data = stream+pos;
            uint16_t p_len = send_len;
            
            static uint32_t seq_num = 0;
            printf("a2dp pcm len %d\n",p_len);
            switch(a2dp_codec_mode){
                case SBC_MODE:
                    {
                        lbh_encode(&sbc,p_data,p_len,encodebuff,p_len,&encodelen);
                        if(encodelen>0){
                            struct timeval t_start;
                            TimeT res = 0;
                            
                            gettimeofday(&t_start, NULL);
                            //printf("Start time: %ld us,%ld ms\n", t_start.tv_usec,(t_start.tv_usec/1000));
                            res = (uint32_t)((t_start.tv_sec*1000000+t_start.tv_usec)/1000);
                            frame_len = (uint8_t)sbc_get_frame_length(&sbc);
                            printf("encode len %d,time %d(ms),%d,frame_len %d\n",encodelen,res,seq_num,frame_len);
                            seq_num++;
                            app_bt_a2dp_send_data(encodebuff,encodelen,frame_len);
                        }else{
                            printf("encode error \n");
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        else{
            app_codec_send_a2dp_pcm(stream+pos,send_len);
        }
        pos+=send_len;
        if(pos>=(streamlen-send_len)){
            break;
        }
        usleep(10000);
    }
    free(stream);
    app_bt_a2dp_start(&connect_addr,0);
}

void* lbh_a2dp_wav_data_tx_test_thread( void *arg ){
    const char *filename = APP_A2DP_TEST_WAV_FILE;
    tAPP_WAV_FILE_FORMAT file_format;
    unsigned char *stream;
    struct stat st;
    int fd, pos, streamlen ;
    size_t len;

    fd = lbh_wav_open_file(filename,&file_format);

    if(fd < 0){
        printf("Can't get file_format of file %s\n",filename);
        exit(0);
    }
    printf("codec %x,nb_channels %d,stereo_mode %d,sample_rate %d,bits_per_sample %d, is_be %d\n",\
    file_format.codec,file_format.nb_channels,file_format.stereo_mode,file_format.sample_rate,file_format.bits_per_sample,file_format.is_be);
    
    if (fstat(fd, &st) < 0) {
        fprintf(stderr, "Can't get size of file %s: %s\n",
            filename, strerror(errno));
        exit(0);
    }

    stream = malloc(st.st_size);

    if (!stream) {
        fprintf(stderr, "Can't allocate memory for %s: %s\n",
            filename, strerror(errno));
        exit(0);
    }

    streamlen = lbh_wav_read_data(fd,&file_format,stream,st.st_size);

    if (streamlen < 0) {
        fprintf(stderr, "Can't read file pcm data %s: %s\n",
            filename, strerror(errno));
        close(fd);
        free(stream);
        exit(0);
    }

    close(fd);
    if(file_format.is_be){
        sbc.endian = SBC_BE;
    }else{
        sbc.endian = SBC_LE;
    }
    pos = 0;
    //sleep(5);
    printf("lbh_a2dp_pcm_data_tx_test_thread ready %p\r\n", arg);
    while(1){
        app_codec_send_a2dp_pcm(stream+pos,2560);
        pos+=2560;
        if(pos>=(streamlen-2560)){
            break;
        }
        usleep(10000);
    }
    free(stream);
    app_bt_a2dp_start(&connect_addr,0);
}

void* app_codec_thread( void *arg ){
    struct sockaddr_in server_addr;
    struct sockaddr_in clientAddr;
    int addr_len = sizeof(clientAddr);
    char buffer[SOCKER_BUFFER_SIZE*16];
    int recv_len = 0;
    int processed_len = 0;
    int client_socket;
    int on = 1;
    uint8_t *ptr = NULL;
    static uint16_t recv_num_offset = 0;
    static uint16_t recv_num_len = 0;
    static uint32_t seq_num = 0;

    if((app_codec_server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        return ERR;
    }
    if(setsockopt(app_codec_server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on) ) < 0){
        printf("codec skt is time wait force return: %d\r\n",errno);
        exit(0);
    }

    //bzero(&server_addr, sizeof(server_addr));
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(APP_CODEC_TH_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(app_codec_server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){
        printf("q skt binding error %d\r\n",errno);
        return ERR;
    }

    if(listen(app_codec_server_socket, 2) < 0){
        printf("listen error");
        return ERR;
    }

    printf("app codec th wait connect accept\r\n");
    client_socket = accept(app_codec_server_socket, (struct sockaddr*)&clientAddr, (socklen_t*)&addr_len);
    if(client_socket == ERR){
        return ERR;
    }
    printf("app_codec_server connected  %d\r\n",client_socket);
    ptr = (uint8_t *)buffer;

    while(1){
        uint8_t *data = NULL;
        uint16_t length = 0;
        BT_HST_MSG msg;
        recv_len = recv(client_socket, buffer, SOCKER_BUFFER_SIZE*16, 0);
        printf("c_q_r_len %d\r\n",recv_len);
        while(recv_len>0){
            if(RX_DATA_MODE == ptr[recv_num_offset]){
                recv_num_len = (uint16_t)(ptr[recv_num_offset+5]&0xff) | (((uint16_t)(ptr[recv_num_offset+6]&0xff))<<8);
                printf("rx len %d,  offset %d\r\n",recv_num_len,recv_num_offset);
                data = (uint8_t *)(ptr+recv_num_offset+1);
                length = (uint16_t)(recv_num_len+6);
                msg.EventId = ((uint32_t)data[3]<<24) | ((uint32_t)data[2]<<16) | ((uint32_t)data[1]<<8) | ((uint32_t)data[0]);
                msg.len = length-6;
                msg.buff = data+6;
                switch(msg.EventId ){
                    case AIC_ADP_A2DP_STREAM_OPEN:
                        {
                            uint8_t role = msg.buff[0];
                            uint8_t codec_type = msg.buff[1];
                            uint8_t sample_rate_bit = msg.buff[2];
                            uint8_t sbc_frame_number = msg.buff[3];
                            uint8_t channel = msg.buff[4];
                            uint8_t numBlocks = msg.buff[5];
                            uint8_t numSubBands = msg.buff[6];
                            uint8_t maxbitpool = msg.buff[7];
                            printf("role %d,codec_type %d,sample_rate_bit %d,sbc_frame_number %d,channel %d,numBlocks %d,numSubBands %d,maxbitpool %d\n",\
                                role,codec_type,sample_rate_bit,sbc_frame_number,channel,numBlocks,numSubBands,maxbitpool);
                            seq_num = 0;
                            if(0x00 == codec_type){//sbc
                                a2dp_codec_mode = SBC_MODE;
                                lsbc_init_a2dp(&sbc,sample_rate_bit,channel,numSubBands,numBlocks,maxbitpool);
                            }else if(0x02 == codec_type){//aac
                                a2dp_codec_mode = AAC_MODE;
                                void *ps_info_ptr = NULL;
                                ps_info_ptr = malloc(AACGetPSInfoSize());
                                memset(ps_info_ptr, 0, AACGetPSInfoSize());
                                AACSetPSInfoBase(ps_info_ptr);
                                
                                hAACDecoder = (HAACDecoder *)AACInitDecoder();
                                AACSetLATMFormat(hAACDecoder);

                                printf("hAACDecoder:%p\n", hAACDecoder);
                            }
                        }
                        break;
                    case AIC_ADP_A2DP_STREAM_DATA_IND:
                        {
                            uint8_t decodebuff[(4096 * 2)];//if codec is aac ,it will have 2 stereo frames, total 4096 samples
                            int decodelen = 0;
                            uint8_t *p = msg.buff;
                            uint16_t a2dp_headerlen = (uint16_t)p[0]|(((uint16_t)p[1])<<8);
                            uint8_t *p_data = &p[2+a2dp_headerlen];
                            uint16_t p_len = msg.len-2-a2dp_headerlen;
                            switch(a2dp_codec_mode){
                                case SBC_MODE:
                                    {
                                        lbh_decode(&sbc,p_data,p_len,decodebuff,&decodelen);
                                        if(decodelen>0){
                                            printf("decode len %d\n",decodelen);
                                        }
                                    }
                                    break;
                                case AAC_MODE:
                                    {
                                        int bytesLeft, err;
                                        bytesLeft = p_len;
                                        while (bytesLeft > 0) {
                                            err = AACDecode(hAACDecoder, &p_data, &bytesLeft, (short *)decodebuff);
                                            if (err) {
                                                printf("Decode Fail err:%d\n", err);
                                            } else {
                                                printf("Decode Success\n");
                                            }
                                            printf("bytesLeft:%d\n", bytesLeft);
                                        
                                            memset(&aacFrameInfo, 0, sizeof(AACFrameInfo));
                                            AACGetLastFrameInfo(hAACDecoder, &aacFrameInfo);
                                            if (err) {
                                                break;
                                            }
                                        }

                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case AIC_ADP_A2DP_STREAM_CLOSED:
                        {
                            switch(a2dp_codec_mode){
                                case SBC_MODE:
                                    {
                                        sbc_finish(&sbc);
                                    }
                                    break;
                                case AAC_MODE:
                                    {
                                        if (AACGetPSInfoBase()) {
                                            free(AACGetPSInfoBase());
                                        }

                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case AIC_ADP_HFP_AUDIO_CONNECTED:
                        {
                            uint8_t *p = msg.buff;
                            uint8_t role = p[0];
                            uint16_t negotiated_codec = (uint16_t)p[1]|(((uint16_t)p[2])<<8);
                            switch(negotiated_codec){
                                case 0x0001:
                                    hfp_codec_mode = CVSD_MODE;
                                    break;
                                case 0x0002:
                                    hfp_codec_mode = MSBC_MODE;
                                    sbc_init_msbc(&msbc, 0L);
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case AIC_ADP_HFP_AUDIO_DATA_IND:
                        {
                            uint8_t decodebuff[1024];
                            int decodelen = 0;
                            uint8_t *p = msg.buff;
                            uint8_t p_len = msg.len;
                            sco_len = p_len;
                            switch(hfp_codec_mode){
                                case CVSD_MODE:
                                    {
                                        printf("sco pcm data len %d\n",p_len);
                                    }
                                    break;
                                case MSBC_MODE:
                                    {
                                        lbh_decode(&msbc,p,p_len,decodebuff,&decodelen);
                                        if(decodelen>0){
                                            printf("msbc decode len %d\n",decodelen);
                                        }
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case AIC_ADP_HFP_AUDIO_DISCONNECTED:
                        {
                            switch(hfp_codec_mode){
                                case CVSD_MODE:
                                    break;
                                case MSBC_MODE:
                                    {
                                        sbc_finish(&msbc);
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    default:
                        break;
                }

                recv_num_offset += (recv_num_len+7);
                processed_len = (recv_num_len+7);
            }else if(TX_DATA_MODE == ptr[recv_num_offset]){
                recv_num_len = (uint16_t)(ptr[recv_num_offset+5]&0xff) | (((uint16_t)(ptr[recv_num_offset+6]&0xff))<<8);
                printf("tx len %d,  offset %d\r\n",recv_num_len,recv_num_offset);
                data = (uint8_t *)(ptr+recv_num_offset+1);
                length = (uint16_t)(recv_num_len+6);
                msg.EventId = ((uint32_t)data[3]<<24) | ((uint32_t)data[2]<<16) | ((uint32_t)data[1]<<8) | ((uint32_t)data[0]);
                msg.len = length-6;
                msg.buff = data+6;
                switch(msg.EventId ){
                    case AIC_ADP_A2DP_STREAM_DATA_IND:
                        {
                            uint8_t encodebuff[(4096 * 2)];//if codec is aac ,it will have 2 stereo frames, total 4096 samples
                            int encodelen = 0;
                            uint8_t frame_len = 0;
                            uint8_t *p_data = msg.buff;
                            uint16_t p_len = msg.len;
                            printf("a2dp pcm len %d\n",p_len);
                            switch(a2dp_codec_mode){
                                case SBC_MODE:
                                    {
                                        lbh_encode(&sbc,p_data,p_len,encodebuff,p_len,&encodelen);
                                        if(encodelen>0){
                                            struct timeval t_start;
                                            TimeT res = 0;
                                            
                                            gettimeofday(&t_start, NULL);
                                            //printf("Start time: %ld us,%ld ms\n", t_start.tv_usec,(t_start.tv_usec/1000));
                                            res = (uint32_t)((t_start.tv_sec*1000000+t_start.tv_usec)/1000);
                                            frame_len = (uint8_t)sbc_get_frame_length(&sbc);
                                            printf("encode len %d,time %d(ms),%d,frame_len %d\n",encodelen,res,seq_num,frame_len);
                                            seq_num++;
                                            app_bt_a2dp_send_data(encodebuff,encodelen,frame_len);
                                        }else{
                                            printf("encode error \n");
                                        }
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                    case AIC_ADP_HFP_AUDIO_DATA_IND:
                        {
                            uint8_t encodebuff[1024];
                            int encodelen = 0;
                            uint8_t *p = msg.buff;
                            uint8_t p_len = msg.len;
                            switch(hfp_codec_mode){
                                case CVSD_MODE:
                                    {
                                        if(sco_len == p_len){
                                            app_bt_sco_send_data(p,p_len);
                                        }else{
                                            printf("sco send len error %d\n",p_len);
                                        }
                                    }
                                    break;
                                case MSBC_MODE:
                                    {
                                        lbh_encode(&sbc,p,p_len,encodebuff,p_len,&encodelen);
                                        if(encodelen>0){
                                            printf("decode len %d\n",encodelen);
                                            app_bt_sco_send_data(encodebuff,encodelen);
                                        }
                                    }
                                    break;
                                default:
                                    break;
                            }
                        }
                        break;
                }

                recv_num_offset += (recv_num_len+7);
                processed_len = (recv_num_len+7);
            }else{
                printf("app_q_server_socket ID error exit\r\n");
                exit(0);
            }
            recv_len = recv_len-processed_len;
        }
        recv_num_len = 0;
        recv_num_offset = 0;
        processed_len = 0;
    }
}

int app_codec_client_msg_send(BT_HST_MSG* msg,enum APP_CODEC_MSG_TYPE codec_msg_type)
{
    int i = 0, num = 10;
    ssize_t ret = 0;
    unsigned char tmp_data[3072];

    int offset = 0;

    tmp_data[offset++] = codec_msg_type;
    tmp_data[offset++] = (uint8_t)(msg->EventId & 0x000000ff);
    tmp_data[offset++] = (uint8_t)((msg->EventId & 0x0000ff00)>>8);
    tmp_data[offset++] = (uint8_t)((msg->EventId & 0x00ff0000)>>16);
    tmp_data[offset++] = (uint8_t)((msg->EventId & 0xff000000)>>24);
    tmp_data[offset++] = (uint8_t)(msg->len & 0x00ff);
    tmp_data[offset++] = (uint8_t)((msg->len & 0xff00)>>8);

    printf("bt codec client send(%d,0x%x,%d): \r\n", codec_msg_type,msg->EventId ,msg->len);
    if(msg->len){
        memcpy(&tmp_data[i+offset],msg->buff,msg->len);
    }

    if(app_codec_client_on){
        while(num){
            ret = send(app_codec_client_socket, tmp_data, msg->len+offset, 0);
            if(ret > 0){
                printf("codec send ok %d  ",ret);
                break;
            }
            printf("codec send err %d  ",ret);
            usleep(5000);
            num--;
        }
        printf("\r\n");
    }
    return 0;
}

void app_codec_socket_init(void)
{
    int on = 1;

    if((app_codec_client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        printf("client_socket error\r\n");
        return -1;
    }
    if(setsockopt(app_codec_client_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on) )<0){
        printf("codec skt is time wait force return: %d\r\n",errno);
        exit(0);
    }
    //setsockopt(app_q_server_socket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on) );

    codec_clientAddr.sin_family = AF_INET;
    codec_clientAddr.sin_port = htons(APP_CODEC_TH_PORT);
    codec_clientAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(connect(app_codec_client_socket, (struct sockaddr *)&codec_clientAddr, sizeof(codec_clientAddr)) < 0){
        printf("connect_socket error %d\r\n",errno);
        exit(0);
    }
    printf("app_codec_client_socket connect OK\r\n");
    app_codec_client_on = 1;

}
/***********************************************************************/
//func :app_codec_send_sco_pcm
//param:
//      data: 8K 16bit 1channel  OR 16K 16bit 1channel pcm data
//      length: must flow recieved sco data length.
//
/***********************************************************************/
void app_codec_send_sco_pcm(uint8_t *data, uint16_t length)
{
    BT_HST_MSG msg;

    msg.EventId = AIC_ADP_HFP_AUDIO_DATA_IND;
    msg.len = length;
    msg.buff = data;
    app_codec_client_msg_send(&msg,TX_DATA_MODE);
}
/***********************************************************************/
//func :app_codec_send_a2dp_pcm
//param:
//      data: 48K 16bit 2channel pcm data
//      length: 2048 (4 sbc frame).
/***********************************************************************/

void app_codec_send_a2dp_pcm(uint8_t *data, uint16_t length)
{
    BT_HST_MSG msg;

    msg.EventId = AIC_ADP_A2DP_STREAM_DATA_IND;
    msg.len = length;
    msg.buff = data;
    app_codec_client_msg_send(&msg,TX_DATA_MODE);
}

