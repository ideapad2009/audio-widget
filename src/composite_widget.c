/* -*- mode: c; tab-width: 4; c-basic-offset: 4 -*- */
/* This source file is part of the ATMEL AVR32-SoftwareFramework-AT32UC3-1.5.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Main file of the USB HID example.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a USB module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/*! \mainpage AVR32 USB software framework for HID
 *
 * \section intro License
 * Use of this program is subject to Atmel's End User License Agreement.
 *
 * Please read the \ref license at the bottom of this page.
 *
 * \section install Description
 * This embedded application source code illustrates how to implement a USB HID-mouse application
 * on the AVR32 microcontroller.
 *
 * As the AVR32 implements a device/host USB controller, the embedded application can operate
 * in one of the following USB operating modes:
 *   - USB device;
 *   - USB reduced-host controller;
 *   - USB dual-role device (depending on the ID pin).
 *
 * To optimize embedded code/RAM size and reduce the number of source modules, the application can be
 * configured to use one and only one of these operating modes.
 *
 * \section sample About the Sample Application
 * By default the sample code is delivered with a simple preconfigured dual-role USB application.
 * It means that the code generated allows to operate as a device or a host depending on the USB ID pin:
 *
 *   - Attached to a mini-B plug (ID pin unconnected) the application will be used in the device operating mode.
 * Thus, the application can be connected to a system host (e.g. a PC) to operate as a USB mouse device:
 *
 * \image html appli_evk1100_device.jpg "EVK1100 USB Device Mode"
 * \image html appli_evk1101_device.jpg "EVK1101 USB Device Mode"
 *
 * The mouse can be controlled thanks to the joystick and the buttons (shown in a red box in the above pictures).
 *
 *   - Attached to a mini-A plug (ID pin tied to ground) the application will operate in reduced-host mode.
 * This mode allows to connect a USB mouse device:
 *
 * \image html appli_evk1100_host.jpg "EVK1100 USB Host Mode"
 * \image html appli_evk1101_host.jpg "EVK1101 USB Host Mode"
 *
 * In this mode, a mouse ascii pointer is output on the USART1 connector. Open a serial terminal (e.g. HyperTerminal under
 * Windows systems or minicom under Linux systems; USART settings: 57600 bauds, 8-bit data, no parity bit,
 * 1 stop bit, no flow control). Make sure that the  serial terminal takes in charge the ANSI codes.
 *
 * \image html terminal.JPG "Debug output in terminal"
 *
 * \section device_use Using the USB Device Mode
 * Connect the application to a USB host (e.g. a PC) with a mini-B (embedded side) to A (PC host side) cable.
 * The application will behave as a mouse peripheral. The mouse can be controlled thanks to the joystick and
 * the buttons of the EVK110x.
 *
 * \section host_use Using the USB Host Mode
 * Connect the application to a USB mouse device. The application will behave as a USB mouse
 * reduced host. Leds will blink according to the mouse eactivity and a mouse ascii pointer is output on the USART1 connector.
 *
 * @section arch Architecture
 * As illustrated in the figure bellow, the application entry point is located is the hid_example.c file.
 * The application can be ran using the FreeRTOS operating system, or in standalone mode.
 * In the standalone mode, the main function runs the usb_task and device_mouse_hid_task tasks in an infinite loop.
 *
 * The application is based on three different tasks:
 * - The usb_task  (usb_task.c associated source file), is the task performing the USB low level
 * enumeration process in device mode.
 * Once this task has detected that the usb connection is fully operationnal, it updates different status flags
 * that can be check within the high level application tasks.
 * - The device_mouse_hid_task task performs the high level device application operation.
 * Once the device is fully enumerated (DEVICE SET_CONFIGURATION request received), the task
 * transmit data (reports) on its IN endpoint.
 * - The host_mouse_hid_task task performs the high level host application operation.
 * Once a device is connected and fully enumerated, the task
 * receive data (reports) on its IN endpoint.
 *
 * \image html arch_full.jpg "Architecture Overview"
 *
 * \section files Main Files
 * - device_mouse_hid_task.c : high level device mouse application operation
 * - device_mouse_hid_task.h : high level device mouse application operation header
 * - host_mouse_hid_task.c : high level host mouse application operation
 * - host_mouse_hid_task.h : high level host mouse application operation header
 * - hid_example.c : HID code example
 *
 * \section license Copyright notice
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 */

/* Modified by Alex Lee 20 Feb 2010
 * To enumerate as a USB composite device with multiple interfaces:
 * CDC
 * HID (generic HID interface, compatible with Jan Axelson's generichid.exe test programs
 * DG8SAQ (libusb API compatible interface for implementing DG8SAQ EP0 type of interface)
 * Audio (Start with Audio Class v1.  Will progress to Audio Class V2.  Tweaked for
 * 		compatibility when running at HIGH speed USB.)
 * For SDR-Widget and SDR-Widget-lite, custom boards based on the AT32UC3A3256
 *
 * See http://code.google.com/p/sdr-widget/
 *
 * Additions and Modifications to ATMEL AVR32-SoftwareFramework-AT32UC3 are:
 *
 * Copyright (C) Alex Lee
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


//_____  I N C L U D E S ___________________________________________________

#include <stdio.h>

#include "compiler.h"
#include "board.h"
#include "print_funcs.h"
#include "intc.h"
#include "pm.h"
#include "gpio.h"
#include "wdt.h"
#include "rtc.h"

#include "FreeRTOS.h"

#include "features.h"
#include "widget.h"
#include "image.h"
#include "composite_widget.h"
#include "Mobo_config.h"
#include "taskAK5394A.h"
#include "cycle_counter.h"
#include "ssc_i2s.h"

/*
 *  A few global variables.
 */

// To access global input source variable
#include "device_audio_task.h"

#if (defined HW_GEN_DIN10) || (defined HW_GEN_DIN20)
#include "wm8805.h"
#endif


xSemaphoreHandle mutexEP_IN;


//_____ M A C R O S ________________________________________________________


//_____ D E F I N I T I O N S ______________________________________________

 pm_freq_param_t   pm_freq_param=
 {
    .cpu_f  =       FCPU_HZ
 ,  .pba_f    =     FPBA_HZ
 ,  .osc0_f     =   FOSC0
 ,  .osc0_startup = OSC0_STARTUP
 };

 pm_freq_param_t   pm_freq_param_slow=
 {
    .cpu_f  =       FCPU_HZ_SLOW
 ,  .pba_f    =     FPBA_HZ_SLOW
 ,  .osc0_f     =   FOSC0
 ,  .osc0_startup = OSC0_STARTUP
 };



/*! \brief Main function. Execution starts here.
 *
 * \retval 42 Fatal error.
 */
int main(void)
{
int i;

	// Avoid burning power in LEDs next to MCU
	LED_Off(LED0);							// The LEDs on the PCB near the MCU
	LED_Off(LED1);


	// Make sure Watchdog timer is disabled initially (otherwise it interferes upon restart)
	wdt_disable();

	// The reason this is put as early as possible in the code
	// is that AK5394A has to be put in reset when the clocks are not
	// fully set up.  Otherwise the chip will overheat
	for (i=0; i< 1000; i++) gpio_clr_gpio_pin(AK5394_RSTN);	// put AK5394A in reset, and use this to delay the start up
															// time for various voltages (eg to the XO) to stablize
															// Not used in QNKTC / Henry Audio hardware

	// Set CPU and PBA clock at slow (12MHz) frequency
	if( PM_FREQ_STATUS_FAIL==pm_configure_clocks(&pm_freq_param_slow) )
		return 42;


/*
	// Low power sleep test start
	while (1) {
		mobo_sleep_rtc_ms(500); // Hm, it seems to be running for twice as long as desired...
		mobo_led(FLED_GREEN);
		mobo_sleep_rtc_ms(500);
		mobo_led(FLED_DARK);
	}
	// Low power sleep test end
*/



#if (defined HW_GEN_DIN10) || (defined HW_GEN_AB1X)
	gpio_set_gpio_pin(AVR32_PIN_PX51);						// Enables power to XO and DAC in USBI2C AB-1.X and USB DAC 128
#endif


#if (defined HW_GEN_DIN10) || (defined HW_GEN_DIN20)
//	mobo_led_select(FREQ_44, input_select);					// Front RGB LED
	wm8805_reset(WM8805_RESET_START);						// Early hardware reset of WM8805 because GPIO is interpreted for config

	// NB: No I2C activity until WM8805_RESET_END !
#endif


#ifdef HW_GEN_DIN20
	gpio_set_gpio_pin(AVR32_PIN_PX13);						// Reset pin override inactive. Should have external pull-up!

	// Disable all power supplies
	// Shouldn't be needed with pull-down
	mobo_km(MOBO_HP_KM_DISABLE);
	gpio_clr_gpio_pin(AVR32_PIN_PX31);
	gpio_clr_gpio_pin(AVR32_PIN_PA27);

	gpio_set_gpio_pin(AVR32_PIN_PA22); // TP18				// Disable pass transistor at DAC's charge pump input


//	gpio_clr_gpio_pin(USB_VBUS_A_PIN);						// NO USB A to MCU's VBUS pin
//	gpio_clr_gpio_pin(USB_DATA_ENABLE_PIN_INV);				// Enable USB MUX
//	gpio_set_gpio_pin(USB_DATA_A0_B1_PIN);					// Select USB B to MCU's USB data pins
//	gpio_set_gpio_pin(USB_VBUS_B_PIN);						// Select USB B to MCU's VBUS pin
	usb_ch = mobo_usb_detect();								// Detect which USB plug to use. A has priority if present
	mobo_usb_select(usb_ch);								// Connect USB plug
	usb_ch_swap = USB_CH_NOSWAP;							// No swapping detected yet

	// Try to remove this function. It is currently empty.
	mobo_i2s_enable(MOBO_I2S_DISABLE);	// Disable here and enable with audio on.

	// Just keep I2S on...
	gpio_set_gpio_pin(AVR32_PIN_PX11); 						// Enable I2S data


	// At default, one channel of current limiter is active. That charges digital side OS-CON and
	// OS-CON of step up's positive side (through the inductor).

	// TODO: Find a 100mA and up current limiter with controllable bias resistor

	// Q:How much does board burn during this kind of wait? A: Roughly 30mA
	cpu_delay_ms(2, FCPU_HZ_SLOW);


	// 3: Turn on analog part of current limiter and step-up converter.
	gpio_set_gpio_pin(AVR32_PIN_PA27);


	// Turn off clock controls to establish starting point
	gpio_clr_gpio_pin(AVR32_PIN_PX44); 		// SEL_USBN_RXP = 0. No pull-down or pull-up
	gpio_clr_gpio_pin(AVR32_PIN_PX58); 		// Disable XOs 44.1 control
	gpio_clr_gpio_pin(AVR32_PIN_PX45); 		// Disable XOs 48 control

	cpu_delay_ms(80, FCPU_HZ_SLOW); // Looks like 60 is actually needed with 4uF slow start


	// Turn on all KMs by enabling pass transistors. FIX: add to board design!
	// Analog KM charges LDOs through shared 22R FIX: add to board as 13R + 13R or something like that.
	mobo_km(MOBO_HP_KM_ENABLE);


	// Wait for analog KM output to settle.
	cpu_delay_ms(600, FCPU_HZ_SLOW); // 600 worked, but that was without considering the DAC charge pumps



	// Moved to I2S init code
	// Wait for some time

	// Short the shared 12R resistor at LDO inputs. FIX: add board design!
//	gpio_set_gpio_pin(AVR32_PIN_PX31);

	// Wait for some time


	// Short the shared 22R resistor at charge pump inputs. FIX: add board design!
//	gpio_clr_gpio_pin(AVR32_PIN_PA22); // TP18


	// Let things settle a bit
	cpu_delay_ms(200, FCPU_HZ_SLOW);

/* Not tied to MCLK and not yet a working solution
	// Generate a super-slow SCLK/LRCK pair to try to fool DAC into super slow startup sequence
	int count = 0;
	gpio_clr_gpio_pin(AVR32_PIN_PX23); // SCLK
	gpio_clr_gpio_pin(AVR32_PIN_PX27); // LRCK
	while (1) {
		gpio_tgl_gpio_pin(AVR32_PIN_PX23);
		count ++;
		if (count == 64) {
			count = 0;
			gpio_tgl_gpio_pin(AVR32_PIN_PX27);
		}
		cpu_delay_ms(1, FCPU_HZ_SLOW);
	}
*/


#endif														//      Later: Maybe make front USB constantly UAC2...


	// Set CPU and PBA clock
	if( PM_FREQ_STATUS_FAIL==pm_configure_clocks(&pm_freq_param) )
		return 42;

	// Initialize usart comm
	init_dbg_rs232(pm_freq_param.pba_f);


	gpio_clr_gpio_pin(AVR32_PIN_PX52);						// Not used in QNKTC / Henry Audio hardware


// Get going from known default state.
// It is very important to enable some sort of MCLK to the CPU, USB MCLK is the most reliable
// FIX: NVRAM should store preferred source and resort to it on boot-up!

	if (feature_get_nvram(feature_image_index) == feature_image_uac1_audio)
		input_select = MOBO_SRC_UAC1;
	else
		input_select = MOBO_SRC_UAC2;

//	mobo_xo_select(FREQ_44, input_select);					// Initial GPIO XO control and frequency indication
	mobo_xo_select(FREQ_INVALID, input_select);				// Initial GPIO XO control and frequency indication

#if (defined HW_GEN_DIN10) || (defined HW_GEN_DIN20)
	mobo_led_select(FREQ_44, input_select);					// Front RGB LED
//	wm8805_reset(WM8805_RESET_START);						// Early hardware reset of WM8805 because GPIO is interpreted for config

	input_select = MOBO_SRC_NONE;							// No input selected, allows state machines to grab it
	// mobodebug

#endif


//clear samplerate indication
#if defined(HW_GEN_AB1X)
	gpio_clr_gpio_pin(SAMPLEFREQ_VAL0);
	gpio_clr_gpio_pin(SAMPLEFREQ_VAL1);

	// Set initial status of LEDs on the front of AB-1.1. BSB 20110903, 20111016
	// Overriden by #if LED_STATUS == LED_STATUS_AB in SDRwdgt.h
	if (feature_get_nvram(feature_image_index) == feature_image_uac1_audio)
	{														// With UAC1:
		mobo_led(FLED_GREEN);	}
	else
	{														// With UAC != 1
		mobo_led(FLED_RED);
	}
#endif

	// Initialize Real Time Counter
	rtc_init(&AVR32_RTC, RTC_OSC_RC, 0);	// RC clock at 115kHz
	rtc_disable_interrupt(&AVR32_RTC);
	rtc_set_top_value(&AVR32_RTC, RTC_COUNTER_MAX);	// Counter reset once per 10 seconds
	rtc_enable(&AVR32_RTC);


	// Initialize features management
	features_init();

	// Initialize widget management
	widget_init();


//	if ( FEATURE_BOARD_WIDGET ) {
//		gpio_clr_gpio_pin(AK5394_RSTN);	// put AK5394A in reset
//	}

	if (FEATURE_ADC_AK5394A){
		int counter;
		// Set up AK5394A
		gpio_clr_gpio_pin(AK5394_RSTN);		// put AK5394A in reset
		gpio_clr_gpio_pin(AK5394_DFS0);		// L H -> 96khz   L L  -> 48khz
		gpio_clr_gpio_pin(AK5394_DFS1);
		gpio_set_gpio_pin(AK5394_HPFE);		// enable HP filter
		gpio_clr_gpio_pin(AK5394_ZCAL);		// use VCOML and VCOMR to cal
		gpio_set_gpio_pin(AK5394_SMODE1);	// SMODE1 = H for Master i2s
		gpio_set_gpio_pin(AK5394_SMODE2);	// SMODE2 = H for Master/Slave i2s

		gpio_set_gpio_pin(AK5394_RSTN);		// start AK5394A
		counter = 0;
		while (gpio_get_pin_value(AK5394_CAL) && (counter < COUNTER_TIME_OUT)) counter++;
		// wait till CAL goes low or time out
		// if time out then change feature adc to none
		if (counter >= COUNTER_TIME_OUT) features[feature_adc_index] = feature_adc_none;
	}

	gpio_enable_pin_pull_up(GPIO_PTT_INPUT);

#if (defined HW_GEN_DIN10) || (defined HW_GEN_DIN20)
	wm8805_reset(WM8805_RESET_END);			// Early hardware reset of WM8805 because GPIO is interpreted for config
#endif


	if (FEATURE_FILTER_FIR) gpio_clr_gpio_pin(GPIO_PCM5102_FILTER);
	else gpio_set_gpio_pin(GPIO_PCM5102_FILTER);

	// Initialize interrupt controller
	INTC_init_interrupts();

	// Initialize usart comm
// Moved up...	init_dbg_rs232(pm_freq_param.pba_f);

	// Initialize USB clock (on PLL1)
	pm_configure_usb_clock();

	// boot the image
	image_boot();

	// initialize the image
	image_init();

	// Start the image tasks
	image_task_init();

	// Start OS scheduler
	vTaskStartScheduler();
	portDBG_TRACE("FreeRTOS returned.");
	return 42;
}
