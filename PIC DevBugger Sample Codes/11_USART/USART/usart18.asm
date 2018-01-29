
#include <p18f4620.inc>
		list P=18F4620, F=INHX32, C=160, N=80, ST=OFF, MM=OFF, R=DEC

;;;;;;Configuration Bits;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
		CONFIG OSC=HS, FCMEN=OFF, IESO=OFF
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

TIMCNT	EQU	020H
LPCNT	EQU	021H
TDATA	EQU	022H

	ORG	0
	GOTO	INIT



;-----------------------------------------------
; Initialization
;   - set baud rate to be 9600K
;   - port B as output
;-----------------------------------------------

INIT
		CLRF   	TRISB	;configure PORTB as output
		BCF		INTCON,GIE	;disable global interrupt
		BCF		INTCON,PEIE		

		movlw	B'10000000'
		movwf	TRISC	;set RC7=USART RC, RC6=USART TX

		MOVLW	15	;Baud rate 9600, assuming 10MHz oscillator
		MOVWF	SPBRG
		clrf	TXSTA	;8 bits data, no parity, 1 stop

		CLRF	RCSTA
		BSF		RCSTA,SPEN	;enable single receive
		BSF		RCSTA,CREN	;continuous

		BSF		TXSTA,TXEN	;enable tx

		MOVLW	H'01'
		MOVWF	TDATA	;Store the value 1 in TDATA




;-----------------------------------------------
; Main Program
;-----------------------------------------------
	
MAIN	CALL	S_WAIT
		CALL	S_WAIT
		CALL	S_WAIT
		CALL	S_WAIT
		CALL	S_WAIT
		MOVF	TDATA,W	;Load TDATA into W
		MOVWF	TXREG	;Send it over UART
		MOVWF	PORTB	;Display it on PORTB
		INCF	TDATA,F	;Increment TDATA

ENDLP	goto	MAIN	;Repeat

;-------------------------------
; Delay Subroutine
;-------------------------------

S_WAIT	MOVLW	081H
		MOVWF	LPCNT

TDEL	MOVLW	0FFH	
		MOVWF	TIMCNT	

TIMLP	DECFSZ	TIMCNT,F	
     	GOTO	TIMLP
		DECFSZ	LPCNT,F
		GOTO	TDEL
     	RETURN	

	END
