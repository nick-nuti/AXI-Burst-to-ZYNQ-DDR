/******************************************************************************
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "xbasic_types.h"
#include "xparameters.h"
#include "xil_io.h"

//timer

// Define the base address of the Global Timer
#define GLOBAL_TMR_BASEADDR 0xF8F00200
#define GLOBAL_TMR_COUNTER  (GLOBAL_TMR_BASEADDR + 0x02C)

// Function to initialize the Global Timer
void init_global_timer() {
    // Set the Global Timer Control Register to enable the timer and its auto-increment
    Xil_Out32(GLOBAL_TMR_BASEADDR, 0x00000001);
}

// Function to read the Global Timer counter
unsigned long long read_global_timer() {
    unsigned int lower = Xil_In32(GLOBAL_TMR_COUNTER);
    unsigned int upper = Xil_In32(GLOBAL_TMR_COUNTER + 4);
    return ((unsigned long long)upper << 32) | lower;
}

//timer

#define DDR_BASE_ADDR 0x40000000
#define WIDTH 1920
#define HEIGHT 1080
#define PIXEL_SIZE 4  // Assuming 32-bit (4 bytes) per pixel
#define MEM_SIZE (WIDTH * HEIGHT * PIXEL_SIZE)

int main()
{
    unsigned long long start_time, end_time, time_diff;

    xil_printf("init global timer\n\r");
    // Initialize the Global Timer
    init_global_timer();

    xil_printf("start global timer\n\r");
    // Capture the time at Event A
    start_time = read_global_timer();

    init_platform();

    volatile Xuint32 *data_p = (Xuint32 *) 0x10000000;

    volatile Xuint32 first_val, second_val;

    first_val = *(data_p);
    xil_printf("first_val\t\t0x%08x\n\r", first_val);
    second_val = first_val;

    while(first_val == second_val)
    {
        second_val = *(data_p);
    }

    // Capture the time at Event B
    end_time = read_global_timer();
    // Calculate the time difference
    time_diff = end_time - start_time;
    xil_printf("Time difference: %llu ticks\n", time_diff);

    //xil_printf("first_val\t\t0x%08x\n\r", first_val);
    xil_printf("second_val\t\t0x%08x\n\r", second_val);

    /*
    for(int i = 0; i < (WIDTH*HEIGHT)-1; i++) 
    {
        xil_printf("DATA+%d\t\t0x%08x\n\r", i, *(data_p+i));
    }
    */

    cleanup_platform();
    return 0;
}
