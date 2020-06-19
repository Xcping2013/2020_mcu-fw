/*******************************************************************************
  Project: stepRocker Mini-TMCL

  Module:  TMC4361A.h
           Defintions for TMC4361A register addresses and TMC4361A support functions.

   Copyright (C) 2011 TRINAMIC Motion Control GmbH & Co KG
                      Waterloohain 5
                      D - 22769 Hamburg, Germany
                      http://www.trinamic.com/

   This program is free software; you can redistribute it and/or modify it
   freely.

   This program is distributed "as is" in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.
*******************************************************************************/

/**
  \file TMC4361A.h
  \author Trinamic Motion Control GmbH & Co KG
  \version 1.00

  \brief TMC4361A library definitions

  This file contains all macro and function definitions of the TMC4361A
  library.
*/
#ifndef __TMC4361A_H
#define __TMC4361A_H




#include "inc_mbkbcn1.h"
//#include "inc_fbtmc4361A.h"	
//#include "tmc4361A_parameter.h"

#if 1
//registers for TMC4361 (from https://github.com/trinamic/T-Bone/blob/master/Software/ArduinoClient/ants.h)
#define TMC4361_GENERAL_CONFIG_REGISTER 				0x00
#define TMC4361_REFERENCE_CONFIG_REGISTER 			0x01
#define TMC4361_START_CONFIG_REGISTER 					0x2
#define TMC4361_INPUT_FILTER_REGISTER 					0x3
#define TMC4361_SPIOUT_CONF_REGISTER 						0x04
#define TMC4361_CURRENT_CONF_REGISTER 					0x05
#define TMC4361_SCALE_VALUES_REGISTER 					0x06
#define TMC4361_ENCODER_INPUT_CONFIG_REGISTER 	0x07
#define TMC4361_ENC_IN_DATA 										0x08
#define TMC4361_ENC_OUT_DATA 										0x09
#define TMC4361_STEP_CONF_REGISTER 							0x0A
#define TMC4361_SPI_STATUS_SELECTION 						0x0B
#define TMC4361_EVENT_CLEAR_CONF_REGISTER 			0x0C
#define TMC4361_INTERRUPT_CONFIG_REGISTER 			0x0D
#define TMC4361_EVENTS_REGISTER 								0x0E
#define TMC4361_STATUS_REGISTER 								0x0F
#define TMC4361_STP_LENGTH_ADD 									0x10
#define TMC4361_START_OUT_ADD_REGISTER 					0x11
#define TMC4361_GEAR_RATIO_REGISTER 						0x12
#define TMC4361_START_DELAY_REGISTER				 		0x13
#define TMC4361_STDBY_DELAY_REGISTER 						0x15
#define TMC4361_FREEWHEEL_DELAY_REGISTER 				0x16
#define TMC4361_VRDV_SCALE_LIMIT_REGISTER 			0x17
#define TMC4361_UP_SCALE_DELAY_REGISTER 				0x18
#define TMC4361_HOLD_SCALE_DELAY_REGISTER 			0x19
#define TMC4361_DRV_SCALE_DELAY_REGISTER 				0x1A
#define TMC4361_BOOST_TIME_REGISTER 						0x1B
#define TMC4361_CLOSE_LOOP_REGISTER 						0x1C
#define TMC4361_DAC_ADDR_REGISTER 							0x1D
#define TMC4361_HOME_SAFETY_MARGIN_REGISTER 		0x1E
#define TMC4361_PWM_FREQ_CHOPSYNC_REGISTER 			0x1F
#define TMC4361_RAMP_MODE_REGISTER 							0x20
#define TMC4361_X_ACTUAL_REGISTER 							0x21
#define TMC4361_V_ACTUAL_REGISTER 							0x22
#define TMC4361_A_ACTUAL_REGISTER 							0x23
#define TMC4361_V_MAX_REGISTER 									0x24
#define TMC4361_V_START_REGISTER 								0x25
#define TMC4361_V_STOP_REGISTER 								0x26
#define TMC4361_V_BREAK_REGISTER							  0x27
#define TMC4361_A_MAX_REGISTER 									0x28
#define TMC4361_D_MAX_REGISTER 									0x29
#define TMC4361_A_START_REGISTER 								0x2A
#define TMC4361_D_FINAL_REGISTER 								0x2B
#define TMC4361_D_STOP_REGISTER 								0x2C
#define TMC4361_BOW_1_REGISTER 									0x2D
#define TMC4361_BOW_2_REGISTER 									0x2E
#define TMC4361_BOW_3_REGISTER 									0x2F
#define TMC4361_BOW_4_REGISTER 									0x30
#define TMC4361_CLK_FREQ_REGISTER 							0x31
#define TMC4361_POSITION_COMPARE_REGISTER 			0x32
#define TMC4361_VIRTUAL_STOP_LEFT_REGISTER 			0x33
#define TMC4361_VIRTUAL_STOP_RIGHT_REGISTER 		0x34
#define TMC4361_X_HOME_REGISTER 								0x35
#define TMC4361_X_LATCH_REGISTER 								0x36
#define TMC4361_X_TARGET_REGISTER 							0x37
#define TMC4361_X_TARGET_PIPE_0_REGSISTER 			0x38
#define TMC4361_SH_V_MAX_REGISTER 							0x40
#define TMC4361_SH_A_MAX_REGISTER 							0x41
#define TMC4361_SH_D_MAX_REGISTER							  0x42
#define TMC4361_SH_VBREAK_REGISTER 							0x45
#define TMC4361_SH_V_START_REGISTER 						0x46
#define TMC4361_SH_V_STOP_REGISTER 							0x47
#define TMC4361_SH_BOW_1_REGISTER 							0x48
#define TMC4361_SH_BOW_2_REGISTER 							0x49
#define TMC4361_SH_BOW_3_REGISTER 							0x4A
#define TMC4361_SH_BOW_4_REGISTER 							0x4B
#define TMC4361_SH_RAMP_MODE_REGISTER 					0x4C
#define TMC4361_D_FREEZE_REGISTER 							0x4E
#define TMC4361_RESET_CLK_GATING_REGISTER 			0x4F
#define TMC4361_ENCODER_POSITION_REGISTER 			0x50
#define TMC4361_ENCODER_INPUT_RESOLUTION_REGISTER 0x54
#define TMC4361_COVER_LOW_REGISTER 								0x6c
#define TMC4361_COVER_HIGH_REGISTER 							0x6d
#define TMC4361_VERSION_REGISTER 									0x7f



	enum RampMode {
    VELOCITY_MODE = 0x00,
    POSITIONING_MODE = (0x01 << 2)
		};

  enum RampType {
    HOLD_RAMP = 0x00, //Follow max speed (rectangle shape)
    TRAPEZOIDAL_RAMP = 0x01,
    S_SHAPED_RAMP = 0x02
  };

  //See Status Events Register description for details
  enum EventType {
    TARGET_REACHED = 0,
    POS_COMP_REACHED,
    VEL_REACHED,
    VEL_STATE_ZERO,
    VEL_STATE_POS,
    VEL_STATE_NEG,
    RAMP_STATE_ACCEL_ZERO,
    RAMP_STATE_ACCEL_POS,
    RAMP_STATE_ACCEL_NEG,
    MAX_PHASE_TRAP,
    FROZEN,
    STOPL,
    STOPR,
    VSTOPL_ACTIVE,
    VSTOPR_ACTIVE,
    HOME_ERROR,
    XLATCH_DONE,
    FS_ACTIVE,
    ENC_FAIL,
    N_ACTIVE,
    ENC_DONE,
    SER_ENC_DATA_FAIL,
    SER_DATA_DONE = 23,
    SERIAL_ENC_FLAG,
    COVER_DONE,
    ENC_VEL_ZERO,
    CL_MAX,
    CL_FIT,
    STOP_ON_STALL_EV,
    MOTOR_EV,
    RST_EV
  };

  //See Status Flags Register description for details
  enum FlagType {
    TARGET_REACHED_F = 0,
    POS_COMP_REACHED_F,
    VEL_REACHED_F,
    VEL_STATE_F0,
    VEL_STATE_F1,
    RAMP_STATE_F0,
    RAMP_STATE_F1,
    STOPL_ACTIVE_F,
    STOPR_ACTIVE_F,
    VSTOPL_ACTIVE_F,
    VSTOPR_ACTIVE_F,
    ACTIVE_STALL_F,
    HOME_ERROR_F,
    FS_ACTIVE_F,
    ENC_FAIL_F,
    N_ACTIVE_F,
    ENC_LATCH_F,
  };
  
  //Reference switch configuration register
  enum ReferenceConfRegisterFields {
    STOP_LEFT_EN = 0,
    STOP_RIGHT_EN,
    POL_STOP_LEFT,
    POL_STOP_RIGHT,
    INVERT_STOP_DIRECTION,
    SOFT_STOP_EN,
    VIRTUAL_LEFT_LIMIT_EN,
    VIRTUAL_RIGHT_LIMIT_EN,
    VIRT_STOP_MODE,
    LATCH_X_ON_INACTIVE_L = 10,
    LATCH_X_ON_ACTIVE_L,
    LATCH_X_ON_INACTIVE_R,
    LATCH_X_ON_ACTIVE_R,
    STOP_LEFT_IS_HOME,
    STOP_RIGHT_IS_HOME,
    HOME_EVENT,
    START_HOME_TRACKING = 20,
    CLR_POS_AT_TARGET,
    CIRCULAR_MOVEMENT_EN,
    POS_COMP_OUTPUT,
    POS_COMP_SOURCE = 25,
    STOP_ON_STALL,
    DRV_AFTER_STALL,
    MODIFIED_POS_COMPARE,
    AUTOMATIC_COVER = 30,
    CIRCULAR_ENC_EN
  };
  /* HW or SW reset */
  void TMC4361_reset(void);

  /* Check runtime flags (as-is condition, as opposed to events that indicate a change since the last read) */
  uint8_t TMC4361_checkFlag(enum FlagType flag);
  uint8_t TMC4361_isTargetReached(void);

  //TODO interrupt configuration
  
  /* Clear all events */
  void TMC4361_clearEvents(void);
  
  /* Check event and clear it if necessary */
  uint8_t TMC4361_checkEvent(enum EventType event);

  /* Set step/dir outputs polarity
   * if stepInverted is true, LOW indicates an active step
   * if dirInverted is true, HIGH indicates negative direction
   */
  void TMC4361_setOutputsPolarity(uint8_t stepInverted, uint8_t dirInverted);

  /* Set output interface timings :
   * step duration in microseconds
   * dir setup time in microseconds (no step will be issued for this duration after a direction change)
   */
  void TMC4361_setOutputTimings(int stepWidth, int dirSetupTime);

  /* Ramp generator commands */
  void TMC4361_setRampMode(enum RampMode mode, enum RampType type);

  /* Return the current internal position (in steps) */
  long TMC4361_getCurrentPosition(void);

  /* Set the current internal position (in steps) */
  void TMC4361_setCurrentPosition(long position);

  /* Return the current speed (in steps / second) */
  float TMC4361_getCurrentSpeed(void);

  /* Return the current acceleration (in steps / second^2) */
  float TMC4361_getCurrentAcceleration(void);

  /* Set the max speed VMAX (steps/second)
   * /!\ Don't exceed clockFreq / 2 in velocity mode and clockFreq / 4 in positioning mode
   */
  void TMC4361_setMaxSpeed(float speed);

  /* Set the ramp start, stop and break speeds (in steps / second). See datasheet §6.4 for details */
  void TMC4361_setRampSpeeds(float startSpeed, float stopSpeed, float breakSpeed);

  /* Set the ramp accelerations (in steps / second^2). See datasheet §6.3.6 */
  void TMC4361_setAccelerations(float maxAccel, float maxDecel, float startAccel, float finalDecel);

  /* Set the bow values for S-shaped ramps (in steps / second^3). */
  void TMC4361_setBowValues(long bow1, long bow2, long bow3, long bow4);

  /* Get the target position in steps */
  long TMC4361_getTargetPosition(void);

  /* Set the target position
   * /!\ Set all other motion profile parameters before
   */
  void TMC4361_setTargetPosition(long position);

  /* Stop the current motion according to the set ramp mode and motion parameters. The max speed is set to 0 but the target position stays unchanged. */
  void TMC4361_stop(void);

  void TMC4361_writeRegister( uint8_t address,  long data);
  long TMC4361_readRegister( uint8_t address);
  
  void TMC4361_setRegisterBit( uint8_t address,  uint8_t bit);
  void TMC4361_clearRegisterBit( uint8_t address,  uint8_t bit);
  uint8_t TMC4361_readRegisterBit( uint8_t address,  uint8_t bit);

  long TMC4361_floatToFixedPoint(float value, int decimalPlaces);
  float TMC4361_ixedPointToFloat(long value, int decimalPlaces);

#endif

#if 0    //copy from TMC429

#define MOTOR0 0
#define MOTOR1 0x20
#define MOTOR2 0x40

#define IDX_XTARGET 0x00
#define IDX_XACTUAL 0x02
#define IDX_VMIN 0x04
#define IDX_VMAX 0x06
#define IDX_VTARGET 0x08
#define IDX_VACTUAL 0x0A
#define IDX_AMAX 0x0C
#define IDX_AACTUAL 0x0E
#define IDX_AGTAT_ALEAT 0x10
#define IDX_PMUL_PDIV 0x12
#define IDX_REFCONF_RM 0x14
#define IDX_IMASK_IFLAGS 0x16
#define IDX_PULSEDIV_RAMPDIV 0x18
#define IDX_DX_REFTOLERANCE 0x1A
#define IDX_XLATCHED 0x1C
#define IDX_USTEP_COUNT_4361A 0x1E

#define IDX_LOW_WORD 0x60
#define IDX_HIGH_WORD 0x62
#define IDX_COVER_POS_LEN 0x64
#define IDX_COVER_DATA 0x66

#define IDX_IF_CONFIG_4361A 0x68
#define IDX_POS_COMP_4361A 0x6A
#define IDX_POS_COMP_INT_4361A 0x6C
#define IDX_TYPE_VERSION_4361A 0x72

#define IDX_REF_SWITCHES 0x7c
#define IDX_SMGP 0x7e

#define TMC4361A_READ 0x01

#define RM_RAMP 0
#define RM_SOFT 1
#define RM_VELOCITY 2
#define RM_HOLD 3

#define NO_REF 	 									 0x03
#define HARD_REF 									 0x00 //Ó²¼þÏÞÎ»
#define SOFT_REF 								   0x04	//Èí¼þÏÞÎ»

#define NO_LIMIT 0
#define HARD_LIMIT 1
#define SOFT_LIMIT 2

#define REFSW_LEFT 0x02
#define REFSW_RIGHT 0x01

#define M0_POS_REACHED 0x01
#define M1_POS_REACHED 0x04
#define M2_POS_REACHED 0x10
#define TMC4361A_STATUS_CDGW 0x40
#define TMC4361A_STATUS_INT  0x80

#define IFLAG_POS_REACHED     0x01
#define IFLAG_REF_WRONG       0x02
#define IFLAG_REF_MISS        0x04
#define IFLAG_STOP            0x08
#define IFLAG_STOP_LEFT_LOW   0x10
#define IFLAG_STOP_RIGHT_LOW  0x20
#define IFLAG_STOP_LEFT_HIGH  0x40
#define IFLAG_STOP_RIGHT_HIGH 0x80

#define IFCONF_INV_REF        0x0001
#define IFCONF_SDO_INT        0x0002
#define IFCONF_STEP_HALF      0x0004
#define IFCONF_INV_STEP       0x0008
#define IFCONF_INV_DIR        0x0010
#define IFCONF_EN_SD          0x0020
#define IFCONF_POS_COMP_0     0x0000
#define IFCONF_POS_COMP_1     0x0040
#define IFCONF_POS_COMP_2     0x0080
#define IFCONF_POS_COMP_OFF   0x00C0
#define IFCONF_EN_REFR        0x0100


void ReadWrite4361A(UCHAR *Read, UCHAR *Write);
void Write4361AZero(UCHAR Address);
void Write4361ABytes(UCHAR Address, UCHAR *Bytes);
void Write4361ADatagram(UCHAR Address, UCHAR HighByte, UCHAR MidByte, UCHAR LowByte);
void Write4361AShort(UCHAR Address, int Value);
void Write4361AInt(UCHAR Address, int Value);
UCHAR Read4361AStatus(UCHAR Which4361A );
UCHAR Read4361ABytes(UCHAR Address, UCHAR *Bytes);
UCHAR Read4361ASingleByte(UCHAR Address, UCHAR Index);
int Read4361AShort(UCHAR Address);
int Read4361AInt(UCHAR Address);
void Set4361ARampMode(UCHAR Axis, UCHAR RampMode);
void Set4361ASwitchMode(UCHAR Axis, UCHAR SwitchMode);
UCHAR SetAMax(UCHAR Motor, UINT AMax);
void HardStop(UINT Motor);

void Init4361A(UCHAR Which4361A);
#endif
//
#endif
//


