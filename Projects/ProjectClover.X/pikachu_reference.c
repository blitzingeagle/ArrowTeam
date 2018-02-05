/*****************************************************************************
  AUTO-GENERATED CODE:  Graphics Resource Converter version: 4.02.00

  Company:
      Microchip Technology, Inc.
 
  File Name:
     pikachu_reference.c
 
  Summary:
      This file is generated by the Microchip's Graphics Resource Converter.
  Description:
      This file is generated by the Graphics Resource Converter containing
      resources such as images and fonts that can be used by Microchip's
      Graphics Library, which is part of the MLA.
 *****************************************************************************/

// DOM-IGNORE-BEGIN
/*****************************************************************************
  Software License Agreement

  Copyright(c) 2013 Microchip Technology Inc.  All rights reserved.
  Microchip licenses to you the right to use, modify, copy and distribute
  Software only when embedded on a Microchip microcontroller or digital
  signal controller that is integrated into your product or third party
  product (pursuant to the sublicense terms in the accompanying license
  agreement).
 
  You should refer to the license agreement accompanying this Software
  for additional information regarding your rights and obligations.
 
  SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY OF ANY
  KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY
  OF MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR
  PURPOSE. IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR
  OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION,
  BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT
  DAMAGES OR EXPENSES INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL,
  INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA,
  COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY
  CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF),
  OR OTHER SIMILAR COSTS.
 *****************************************************************************/
// DOM-IGNORE-END

/*****************************************************************************
 * SECTION:  Includes
 *****************************************************************************/
#include <gfx/gfx.h>
#include <stdint.h>

/*****************************************************************************
 * SECTION:  Graphics Library Format Check
 *****************************************************************************/
#ifndef GFX_RESOURCE_FORMAT_VERSION
    #if(GRAPHICS_LIBRARY_VERSION != 0x0401)
    #warning "It is suggested to use Graphics Library version 4.01 or later with this version of GRC."
    #endif
#elif(GFX_RESOURCE_FORMAT_VERSION != 0x0101)
#warning "It is suggested to use Graphics Library with the resource format version 1.01 with this version of GRC."
#endif




/*****************************************************************************
 * SECTION:  JPEGS
 *****************************************************************************/

/*********************************
 * JPEG Structure
 * Label: pikachu
 * Description:  80x80 pixels
 ***********************************/
#if defined (GFX_CONFIG_IMAGE_PACKED_ENABLE) && (__XC16_VERSION__ >= 1011)
extern uint8_gfx_image_prog __pikachu[] __attribute__((space(prog), section("grc_images")));
#else
extern uint8_gfx_image_prog __pikachu[] __attribute__((space(prog), section("grc_images"), aligned(2)));
#endif

const GFX_RESOURCE_HDR pikachu =
{
    .type = (GFX_RESOURCE_MEMORY_FLASH | GFX_RESOURCE_TYPE_JPEG | GFX_RESOURCE_COMP_NONE),
    .ID = 0,
    .resource.image.location.progByteAddress = (uint8_gfx_image_prog *) __pikachu,
    .resource.image.width = 80,
    .resource.image.height = 80,
    .resource.image.parameter1.reserved = 0,
    .resource.image.parameter2.reserved = 0,
    .resource.image.colorDepth = 16
};
