#include <p18f4620.inc>
		list P=18F4620, F=INHX32, C=160, N=80, ST=OFF, MM=OFF, R=DEC

;;;;;;Configuration Bits;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

		CONFIG OSC=INTIO67, FCMEN=OFF, IESO=OFF
		CONFIG PWRT = OFF, BOREN = SBORDIS, BORV = 3
		CONFIG WDT = OFF, WDTPS = 32768
		CONFIG MCLRE = ON, LPT1OSC = OFF, PBADEN = OFF, CCP2MX = PORTC
		CONFIG STVREN = ON, LVP = OFF, XINST = OFF
		CONFIG DEBUG = OFF
		CONFIG CP0 = OFF, CP1 = OFF, CP2 = OFF, CP3 = OFF
		CONFIG CPB = OFF, CPD = OFF
		CONFIG WRT0 = OFF, WRT1 = OFF, WRT2 = OFF, WRT3 = OFF
		CONFIG WRTB = OFF, WRTC = OFF, WRTD = OFF
		CONFIG EBTR0 = OFF, EBTR1 = OFF, EBTR2 = OFF, EBTR3 = OFF
		CONFIG EBTRB = OFF

		org		0
		goto	INIT

INIT
		movlw	B'01110000'	;Set internal oscillator frequency to 8MHz
		movwf	OSCCON
		bsf	OSCTUNE, 6	;Enable PLL - oscillator speed = 32MHz
		bcf	INTCON,GIE	;disable global interrupt
		bcf	INTCON,PEIE
		movlw	B'01100011'	;configure PR2
		movwf	PR2
		movlw	B'00110010'	;configure CCPR2L
		movwf	CCPR2L
		movlw	B'00001100'	;configure CCP2CON
		movwf	CCP2CON
		clrf   	TRISC		;configure PORTC as output
		movlw	B'00000101'	;configure T2CON, set prescaler to 4
		movwf	T2CON
	
ENDLP	bra		ENDLP		;endless loop

		end
