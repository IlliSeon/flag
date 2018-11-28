#include "include.h"
#include <stdbool.h>

typedef enum {
    fsm_rt_err=-1,
    fsm_rt_cpl=0,
    fsm_rt_on_going=1
} fsm_rt_t;

static fsm_rt_t task_print(void);
static fsm_rt_t task_delay(void);
static fsm_rt_t print(void);
static fsm_rt_t delay(void);

static bool s_bFlag=false;


#define MAX_DELAY_TICK         0x5ffff

int main(void)
{
    system_init();

    while(1) {
        breath_led();
        task_print();
        task_delay();
    }

}

#define TASK_PRINT_RESET_FSM()       \
         do {\
                 s_tState = START;\
         } while(0)

static fsm_rt_t task_print(void)
{
    static enum {
        START=0,
        TASK_PEND,
        TASK_RUN
    } s_tState=START;
    switch(s_tState) {
    case START:
        s_tState=TASK_PEND;
    //break;
    case TASK_PEND:
        if(s_bFlag) {
//			s_bFlag=false;
            s_tState=TASK_RUN;
        }
        break;
    case TASK_RUN:
        if(fsm_rt_cpl==print()) {
            TASK_PRINT_RESET_FSM();
			s_bFlag=false;
            return fsm_rt_cpl;
        }
        break;
    }
    return fsm_rt_on_going;
}


#define TASK_DELAY_RESET_FSM()       \
         do {\
                 s_tState = START;\
         } while(0)

static fsm_rt_t task_delay(void)
{
    static enum {
        START=0,
        TASK_RUN
    } s_tState=START;
    switch(s_tState) {
    case START:
        s_tState= TASK_RUN;
    //break;
    case TASK_RUN:
        if(fsm_rt_cpl==delay()) {
            TASK_DELAY_RESET_FSM();
            s_bFlag=true;
            return fsm_rt_cpl;
        }
        break;

    }
    return fsm_rt_on_going;
}


#define DELAY_RESET_FSM()       \
         do {\
                 s_tState = START;\
         } while(0)

static fsm_rt_t delay(void)
{
    static uint32_t s_wDelaytick;
    static enum {
        START=0,
        TIME_RUN
    } s_tState=START;

    switch(s_tState) {

    case START:
        s_wDelaytick=0;
        s_tState=TIME_RUN;
    //break;

    case TIME_RUN:
        s_wDelaytick++;
        if(s_wDelaytick>=MAX_DELAY_TICK) {
            DELAY_RESET_FSM();
            return fsm_rt_cpl;
        }
        break;

    default:
        break;
    }
    return fsm_rt_on_going;
}



#define PRINT_RESET_FSM()       \
        do {\
                s_tState = START;\
        } while(0)

static fsm_rt_t print(void)
{
    static uint8_t s_chCnt;
    static enum {
        START=0,
        TX_H,
        TX_E,
        TX_L,
        TX_O,
        TX_R,
        TX_N,
        DELAY
    } s_tState=START;
    switch(s_tState) {

    case  START :
        s_chCnt=0;
        s_tState=TX_H;
    //break;

    case  TX_H :
        if(serial_out('H')) {
            s_tState=TX_E;
        }
        break;

    case  TX_E :
        if(serial_out('E')) {
            s_tState=TX_L;
        }
        break;

    case  TX_L :
        if(serial_out('L')) {
            s_chCnt++;
            if(s_chCnt>=2) {
                s_chCnt=0;
                s_tState=TX_O;
            }
        }
        break;

    case  TX_O :
        if(serial_out('O')) {
            s_tState=TX_R;
        }
        break;

    case  TX_R :
        if(serial_out('\r')) {
            s_tState=TX_N;
        }
        break;

    case  TX_N :

        if(serial_out('\n')) {
            PRINT_RESET_FSM();
            return fsm_rt_cpl;
        }
        break;

    default  :

        break;
    }
    return fsm_rt_on_going;
}

