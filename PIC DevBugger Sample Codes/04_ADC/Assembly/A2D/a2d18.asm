
#include <p18f4620.inc>
	list P=18F4620, F=INHX32, C=160, N=80, ST=OFF, MM=OFF, R=DEC

;;;;;;Configuration Bits;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

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

;**************************************************************
; Initialize Part
;  
; ADCON1 - AN0-AN12 are analog (pg. 222)
; ADCON2 - Acquisition time = 8 TAD
;	 - Conversion clock = Fosc/64
;	 - Right justify A2D result
; TRISB - PORTB as output
;**************************************************************
	org	0
	movlw	B'01110000'	;set internal oscillator frequency
	movwf	OSCCON		;to 8 MHz
	bsf	OSCTUNE, 6	;turn on PLL to enable 32 MHz clock frequency
	bra	INIT

INIT	bcf	INTCON,GIE	;disable global interrupt
	movlw	B'00000000'	;configure ADCON1
	movwf	ADCON1		
	movlw	B'10100110'	;configure ADCON2
	movwf	ADCON2
	clrf   	TRISC		;configure PORTC as output
	clrf	TRISD		;configure PORTD as output
	bra	ADSTART


;***************************************************************
; MAIN PROGRAM
;***************************************************************
ADSTART	movlw	B'00001101'	;acquire analog signal from channel 3 (RA3)
	rcall	AD_CONV		;call the A2D subroutine
     	movwf	PORTC		;display the high 8-bit result to the LEDs
	
	movlw	B'00001001'	;acquire analog signal from channel 2 (RA2)
	rcall	AD_CONV		;call the A2D subroutine
     	movwf	PORTD		;display the high 8-bit result to the LEDs 

ENDLP	bra	ADSTART		;endless loop


;***************************************************************
; AD CONVERT ROUTINE
;***************************************************************
AD_CONV movwf	ADCON0		;configure ADCON0
     	bsf	ADCON0,1	;start the conversion

WAIT	btfsc	ADCON0,1	;wait until the conversion is completed
     	bra	WAIT		;poll the GO bit in ADCON0
     	movf	ADRESL,W	;move the high 8-bit to W
     	return
	end
