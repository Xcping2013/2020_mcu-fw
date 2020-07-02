
//Direct mode @TMCL-1110
//instruction			 description							type		 motor			value
/*	1.	ROR					rotate right							-				 X				 Y
*/
/*	2.	ROL					rotate left								-				 X				 Y
*/
/*	3.	MST					motor stop								-				 X				 -
*/
/*	4.  MVP					move to position	 ABS>absolute 	 X				 Y
																			 REL>relative
*/
/*	5.  SAP					set axis parameter next position 	 X				 Y
																	 actual position 	 	
																	 next speed[int]
																	 actual speed[int]
																	max.position speed[int]
																	max.acceleration[int]
																	max.current[int]
																	standy current
																	right limit switch disable
																	left limit switch disable
																	minimum speed[int]
																	actual acceleration[int]
																	ramp mode
																	microstep resolution
																	ref.switch tolerance																
																	soft stop flag
																	ramp divisor
																	pulse divisor
																	step interpolation enable
																	double step enable
																	chopper blank time
																	chopper mode
																	chopper hysteresis decrement
																	chopper hysteresis end
																	chopper hysteresis start
																	chopper off time
																	smartEnergy current minimum
																	smartEnergy current down step
																	smartEnergy hysteresis
																	smartEnergy current up step
																	smartEnergy hysteresis start
																	stallGuard filter enable
																	stallGuard threshold
																	slope control high side
																	slope control low side
																	short protection disable
																	short detection timer
																	Vsense
																	stop on stall
																	smartEnergy threshold speed
																	smartEnergy slow run current
																	random chopper off time
																	reference search mode
																	reference search speed
																	reference switch speed
																	freewheeling delay
																	encoder position
																	encoder prescaler
																	max.encoder deviation
																	power down delay
																	step/direction mode
*/
																									

/*	6.  GAP					get axis parameter next position 	 X				 -
																	 actual position 	 	
																	 next speed[int]
																	 actual speed[int]
																	max.position speed[int]
																	max.acceleration[int]
																	max.current[int]
																	standy current
																	
																	position reached
																	reference swicth status																															
																	right limit switch status
																	left limit switch status
																	right limit switch disable
																	left limit switch disable
																	minimum speed[int]
																	actual acceleration[int]
																	ramp mode
																	microstep resolution
																	ref.switch tolerance																
																	soft stop flag
																	ramp divisor
																	pulse divisor
																	step interpolation enable
																	double step enable
																	chopper blank time
																	chopper mode
																	chopper hysteresis decrement
																	chopper hysteresis end
																	chopper hysteresis start
																	chopper off time
																	smartEnergy current minimum
																	smartEnergy current down step
																	smartEnergy hysteresis
																	smartEnergy current up step
																	smartEnergy hysteresis start
																	stallGuard filter enable
																	stallGuard threshold
																	slope control high side
																	slope control low side
																	short protection disable
																	short detection timer
																	Vsense
																	smart ensrgy actual current
																	stop on stall
																	smartEnergy threshold speed
																	smartEnergy slow run current
																	random chopper off time
																	reference search mode
																	reference search speed
																	reference switch speed
																	reference switch distance
																	freewheeling delay
																	load value
																	driver erroe flags																	
																	encoder position
																	encoder prescaler
																	max.encoder deviation
																	power down delay
																	step/direction mode
*/
//
/*	7.  STAP				store axis parameter  	 	¡ý				 X				 -

																	max.position speed[int]
																	max.acceleration[int]
																	max.current[int]
																	standy current																	
																	right limit switch disable
																	left limit switch disable
																	minimum speed[int]
																	microstep resolution
																	ref.switch tolerance																											
																	soft stop flag
																	ramp divisor
																	pulse divisor
																	reference search mode
																	reference search speed
																	reference switch speed
																	freewheeling delay
																	encoder prescaler
																	max.encoder deviation
																	power down delay
																	step/direction mode
*/
/*	8.  RSAP				restore axis parameter  	¡ý				 X				 -

																	max.position speed[int]
																	max.acceleration[int]
																	max.current[int]
																	standy current																	
																	right limit switch disable
																	left limit switch disable
																	minimum speed[int]
																	microstep resolution
																	ref.switch tolerance																											
																	soft stop flag
																	ramp divisor
																	pulse divisor
																	reference search mode
																	reference search speed
																	reference switch speed
																	freewheeling delay
																	encoder prescaler
																	max.encoder deviation
																	power down delay
																	step/direction mode
*/
/*	9.  SGP					set global parameter     	¡ý				 0				 Y

																	65-serial baud rate
																	66-serial address
																	67-ASCII mode
																	68-serial heartbeat
																	69-CAN bit rate
																	70-CAN reply ID
																	71-CAN ID
																	73-EEPROM lock bit
																	75-telegram pause time
																	76-serial host address
																	77-auto start mode
																	78-I/O mask
																	79-limit switch polarity
																	81-program memory protection
																	82-CAN heartbeat
																	83-CAN secondary address
																	84-srore coordinates EEPROM
																	85-do not restore user variables
																	86-step pulse length
																	87-serial secondary address
																	131-app.error flags
																	132-tick timer
																	133-random number																	
*/
/*	10. GGP					get global parameter     	¡ý				 3				 -
																						0-timer 0
																						1-timer 1
																						2-timer 2
																					27-stop left 0
																					28-stop right 0
																					39-input 0
																					40-input 1
*/
/*	11. STGP				store global parameter    ¡ý				 2				 -
																					user variable	
*/
/*	12. RSGP				restore global parameter  ¡ý				 2				 -
																					user variable	
*/
/*	13. RFS			    reference search			    ¡ý				 X				 -

																					0-START start ref.seach
																					1-STOP stop ref.seach
																					1-STATUS get status																					
*/
/*	14. SIO			    set output		   				  ¡ý				 X				 Y																		
*/
/*	15. GIO			    get input/output		 		  ¡ý				 X				 -																		
*/
//Type
/*
0-4
5-9
10-14
15-19
20-24
25-29
30-34
35-39
40-44
45-49
50-54
55-59
60-64
65-69
70-74
75-79
80-84
85-89
*/

//


																																											
																	
																	
																	
																	

