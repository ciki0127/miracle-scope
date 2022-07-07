/*******************************************************************************
 *   Copyright (c) 2022 - , Miracle Scope Korea. All Rights Reserved. 
 *******************************************************************************
 *																			  
 * File Name    : ms_chip_hw.h					                  
 * Description 	: 															  
 * Authors 	: Cheol Soo Park(sawool.park@gmail.com)							      
 * Histroy  	: 2022/05/20 - 1st version released			   
 *				  															   
 *******************************************************************************/

#ifndef MS_CHIP_HW_H
#define MS_CHIP_HW_H

enum {
  /* insert here !! */
  MS_DIR_UNDEFINED  = 0,
  MS_DIR_INPUT      = 1,
  MS_DIR_OUTPUT     = 2,
  MS_DIR_INOUT      = 3,
};

#define MS_CHIP_POWER_VDD               3.3 
#define MS_SAADC_CHANNEL_MAX            8     
#define MS_PWM_INSTANCE_MAX_ID          4     
#define MS_TWI_INSTANCE_MAX_ID          2    

// internal flash configuration
#define MS_FLASH_MAX_PAGE_NUMBER        128
#define MS_FLASH_BLOCK_NUMBER_PER_PAGE  8
#define MS_FLASH_BYTE_SIZE_PER_BLOCK    512
#define MS_FLASH_BYTE_SIZE_PER_PAGE     (MS_FLASH_BLOCK_NUMBER_PER_PAGE * MS_FLASH_BYTE_SIZE_PER_BLOCK)
#define MS_FLASH_RW_BYTE_SIZE           4
#define MS_FLASH_RW_BITS_SIZE           32

#endif //MS_CHIP_HW_H