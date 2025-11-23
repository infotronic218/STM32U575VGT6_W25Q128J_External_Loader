/*
 * Loader_Src.c
 *
 *  Created on: Nov 18, 2025
 *      Author: levi
 */


#include "octospi.h"
#include "main.h"
#include "w25q128jvsq.h"
#include "gpio.h"
#include "usart.h"
//#include "Loader_Src.h"


#define LOADER_OK   0x1
#define LOADER_FAIL 0x0
extern void SystemClock_Config(void);
extern OSPI_HandleTypeDef hospi1;
/**
 * @brief  System initialization.
 * @param  None
 * @retval  LOADER_OK = 1   : Operation succeeded
 * @retval  LOADER_FAIL = 0 : Operation failed
 */
int
Init(void) {

    *(uint32_t*)0xE000EDF0 = 0xA05F0000; //enable interrupts in debug


    SystemInit();

    /* ADAPTATION TO THE DEVICE
     *
     * change VTOR setting for H7 device
     * SCB->VTOR = 0x24000000 | 0x200;
     *
     * change VTOR setting for other devices
     * SCB->VTOR = 0x20000000 | 0x200;
     *
     * */

    SCB->VTOR = 0x20000000 | 0x200;

    __set_PRIMASK(0); //enable interrupts

    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();

	 __HAL_RCC_OSPI1_FORCE_RESET();  //completely reset peripheral
	 __HAL_RCC_OSPI1_RELEASE_RESET();

    // if (CSP_QUADSPI_Init() != HAL_OK) {
    //     __set_PRIMASK(1); //disable interrupts
    //     return LOADER_FAIL;
    // }


    // if (CSP_QSPI_EnableMemoryMappedMode() != HAL_OK) {
    //     __set_PRIMASK(1); //disable interrupts
    //     return LOADER_FAIL;
    // }

    if (W25Q128_OCTO_SPI_Init(&hospi1) != HAL_OK)
    {
    	__set_PRIMASK(1); //disable interrupts
        return LOADER_FAIL;
    }


    if (W25Q128_OSPI_EnableMemoryMappedMode(&hospi1) != HAL_OK)
    {
    	__set_PRIMASK(1); //disable interrupts
        return LOADER_FAIL;
    }

    /*Trigger read access before HAL_QSPI_Abort() otherwise abort functionality gets stuck*/
    uint32_t a = *(uint32_t*) 0x90000000;
    a++;

    __set_PRIMASK(1); //disable interrupts

    USART_Println("Init OK");
    return LOADER_OK;
}


// /**
//  * @brief  System initialization.
//  * @param  None
//  * @retval  LOADER_OK = 1   : Operation succeeded
//  * @retval  LOADER_FAIL = 0 : Operation failed
//  */
// int Init(void)
// {

//     //*(uint32_t*)0xE000EDF0 = 0xA05F0000; //enable interrupts in debug

//     SystemInit();



//     /* ADAPTATION TO THE DEVICE
//      *
//      * change VTOR setting for H7 device
//      * SCB->VTOR = 0x24000000 | 0x200;
//      *
//      * change VTOR setting for other devices
//      * SCB->VTOR = 0x20000000 | 0x200;
//      *
//      * */
// //    return LOADER_OK;

//     SCB->VTOR = 0x24000000 | 0x200;

// //    return LOADER_OK;

//     __enable_irq(); //enable interrupts

// //    return LOADER_OK;

//     HAL_Init();

//     SystemClock_Config();

// //    return LOADER_OK;

//     MX_GPIO_Init();

//     MX_OCTOSPI2_Init();

// //    return LOADER_OK;

//     __HAL_RCC_OSPI2_FORCE_RESET();  //completely reset peripheral
//     __HAL_RCC_OSPI2_RELEASE_RESET();



//     if (W25Q128_OCTO_SPI_Init(&hospi2) != HAL_OK)
//     {
//     	__disable_irq();  //disable interrupts
//         return LOADER_FAIL;
//     }


//     if (W25Q128_OSPI_EnableMemoryMappedMode(&hospi2) != HAL_OK)
//     {
//     	__disable_irq(); //disable interrupts
//         return LOADER_FAIL;
//     }

//     /*Trigger read access before HAL_QSPI_Abort() otherwise abort functionality gets stuck*/
// //    volatile uint32_t a = *(uint32_t*) 0x70000000;
// //    a++;
// //
// //
// //    // 👇 Now perform sector erase + write "mi ze sham"
// //        const char msg[] = "mi ze sham";
// //        uint32_t address = 0x000000;         // Offset inside the flash
// //        uint32_t flash_addr = 0x70000000;    // Mapped XIP address
// //
// //        // Erase 4KB sector containing address 0
// //        if (SectorErase(address, address + 4095) != LOADER_OK)
// //            return LOADER_FAIL;
// //
// //        // Write the message to that address
// //        if (Write(address, sizeof(msg), (uint8_t*)msg) != LOADER_OK)
// //            return LOADER_FAIL;
// //
// //        char *xip_ptr = (char *)0x70000000;


//     __disable_irq(); //disable interrupts

//     return LOADER_OK;
// }






//int Read(uint32_t Address, uint32_t Size, uint8_t* buffer)
//{
//    __enable_irq(); // Required if HAL timing uses interrupts (like SysTick)
//
//    if (W25Q128_OCTO_SPI_Init(&hospi2) != HAL_OK)
//    {
//        __disable_irq();
//        return LOADER_FAIL;
//    }
//
//    // Perform read from flash using indirect read mode
//    if (W25Q128_OSPI_Read(&hospi2, buffer, Address & 0x00FFFFFF, Size) != HAL_OK)
//    {
//        __disable_irq();
//        return LOADER_FAIL;
//    }
//
//    __disable_irq();
//    return LOADER_OK;
//}






/**
 * @brief   Program memory.
 * @param   Address: page address
 * @param   Size   : size of data
 * @param   buffer : pointer to data buffer
 * @retval  LOADER_OK = 1       : Operation succeeded
 * @retval  LOADER_FAIL = 0 : Operation failed
 */
int Write(uint32_t Address, uint32_t Size, uint8_t* buffer)
{
    __enable_irq(); // enable interrupts if needed by HAL
    USART_Println("Write start");
    // Full peripheral reset to ensure clean state
    __HAL_RCC_OSPI1_FORCE_RESET();
    __HAL_RCC_OSPI1_RELEASE_RESET();

    // Re-init OSPI peripheral
    if (W25Q128_OCTO_SPI_Init(&hospi1) != HAL_OK)
    {
        __disable_irq();
        return LOADER_FAIL;
    }

    // Perform the actual write (page program via indirect mode)
    if (W25Q128_OSPI_Write(&hospi1, buffer, (Address & 0x00FFFFFF), Size) != HAL_OK)
    {
        __disable_irq();
        return LOADER_FAIL;
    }

    // Re-enable memory-mapped (XIP) mode so CubeProgrammer can read
    if (W25Q128_OSPI_EnableMemoryMappedMode(&hospi1) != HAL_OK)
    {
        __disable_irq();
        return LOADER_FAIL;
    }

    __disable_irq();
    return LOADER_OK;
}


/**
 * @brief   Sector erase.
 * @param   EraseStartAddress :  erase start address
 * @param   EraseEndAddress   :  erase end address
 * @retval  LOADER_OK = 1       : Operation succeeded
 * @retval  LOADER_FAIL = 0 : Operation failed
 */
int SectorErase(uint32_t EraseStartAddress, uint32_t EraseEndAddress)
{
    __enable_irq(); // enable if HAL timing relies on SysTick

    // Completely reset peripheral to safely exit memory-mapped mode
    __HAL_RCC_OSPI1_FORCE_RESET();
    __HAL_RCC_OSPI1_RELEASE_RESET();

    // Init OSPI again after reset
    if (W25Q128_OCTO_SPI_Init(&hospi1) != HAL_OK)
    {
        __disable_irq();
        return LOADER_FAIL;
    }

    // Perform erase (indirect mode)
    if (W25Q128_OSPI_EraseSector(&hospi1, EraseStartAddress, EraseEndAddress) != HAL_OK)
    {
        __disable_irq();
        return LOADER_FAIL;
    }

    // Re-enable memory-mapped mode so XIP works again
    if (W25Q128_OSPI_EnableMemoryMappedMode(&hospi1) != HAL_OK)
    {
        __disable_irq();
        return LOADER_FAIL;
    }

    __disable_irq();
    return LOADER_OK;
}


/**
 * Description :
 * Mass erase of external flash area
 * Optional command - delete in case usage of mass erase is not planed
 * Inputs    :
 *      none
 * outputs   :
 *     none
 * Note: Optional for all types of device
 */
int MassErase(void)
{
	__enable_irq(); //enable interrupts

    if (W25Q128_OCTO_SPI_Init(&hospi1) != HAL_OK)
    {
    	__disable_irq();  //disable interrupts
        return LOADER_FAIL;
    }

    if (W25Q128_OSPI_Erase_Chip(&hospi1) != HAL_OK)
    {
    	__disable_irq(); //disable interrupts
        return LOADER_FAIL;
    }

    __disable_irq(); //disable interrupts
    return LOADER_OK;
}

/**
 * Description :
 * Calculates checksum value of the memory zone
 * Inputs    :
 *      StartAddress  : Flash start address
 *      Size          : Size (in WORD)
 *      InitVal       : Initial CRC value
 * outputs   :
 *     R0             : Checksum value
 * Note: Optional for all types of device
 */
uint32_t CheckSum(uint32_t StartAddress, uint32_t Size, uint32_t InitVal)
{
    uint8_t missalignementAddress = StartAddress % 4;
    uint8_t missalignementSize = Size;
    int cnt;
    uint32_t Val;

    StartAddress -= StartAddress % 4;
    Size += (Size % 4 == 0) ? 0 : 4 - (Size % 4);

    for (cnt = 0; cnt < Size; cnt += 4)
    {
        Val = *(uint32_t*) StartAddress;
        if (missalignementAddress)
        {
            switch (missalignementAddress)
            {
                case 1:
                    InitVal += (uint8_t) (Val >> 8 & 0xff);
                    InitVal += (uint8_t) (Val >> 16 & 0xff);
                    InitVal += (uint8_t) (Val >> 24 & 0xff);
                    missalignementAddress -= 1;
                    break;
                case 2:
                    InitVal += (uint8_t) (Val >> 16 & 0xff);
                    InitVal += (uint8_t) (Val >> 24 & 0xff);
                    missalignementAddress -= 2;
                    break;
                case 3:
                    InitVal += (uint8_t) (Val >> 24 & 0xff);
                    missalignementAddress -= 3;
                    break;
            }
        }
        else if ((Size - missalignementSize) % 4 && (Size - cnt) <= 4)
        {
            switch (Size - missalignementSize)
            {
                case 1:
                    InitVal += (uint8_t) Val;
                    InitVal += (uint8_t) (Val >> 8 & 0xff);
                    InitVal += (uint8_t) (Val >> 16 & 0xff);
                    missalignementSize -= 1;
                    break;
                case 2:
                    InitVal += (uint8_t) Val;
                    InitVal += (uint8_t) (Val >> 8 & 0xff);
                    missalignementSize -= 2;
                    break;
                case 3:
                    InitVal += (uint8_t) Val;
                    missalignementSize -= 3;
                    break;
            }
        }
        else
        {
            InitVal += (uint8_t) Val;
            InitVal += (uint8_t) (Val >> 8 & 0xff);
            InitVal += (uint8_t) (Val >> 16 & 0xff);
            InitVal += (uint8_t) (Val >> 24 & 0xff);
        }
        StartAddress += 4;
    }

    return (InitVal);
}

/**
 * Description :
 * Verify flash memory with RAM buffer and calculates checksum value of
 * the programmed memory
 * Inputs    :
 *      FlashAddr     : Flash address
 *      RAMBufferAddr : RAM buffer address
 *      Size          : Size (in WORD)
 *      InitVal       : Initial CRC value
 * outputs   :
 *     R0             : Operation failed (address of failure)
 *     R1             : Checksum value
 * Note: Optional for all types of device
 */
uint64_t Verify(uint32_t MemoryAddr, uint32_t RAMBufferAddr, uint32_t Size, uint32_t missalignement)
{
	__enable_irq(); //enable interrupts
    uint32_t VerifiedData = 0, InitVal = 0;
    uint64_t checksum;
    Size *= 4;

    if (W25Q128_OCTO_SPI_Init(&hospi1) != HAL_OK)
    {
    	__disable_irq();  //disable interrupts
        return LOADER_FAIL;
    }

    if (W25Q128_OSPI_EnableMemoryMappedMode(&hospi1) != HAL_OK)
    {
    	__disable_irq(); //disable interrupts
        return LOADER_FAIL;
    }

    checksum = CheckSum((uint32_t) MemoryAddr + (missalignement & 0xf), Size - ((missalignement >> 16) & 0xF), InitVal);
    while (Size > VerifiedData)
    {
        if (*(uint8_t*) MemoryAddr++!= *((uint8_t*) RAMBufferAddr + VerifiedData))
        {
        	//__disable_irq(); //disable interrupts
            return ((checksum << 32) + (MemoryAddr + VerifiedData));
        }
        VerifiedData++;
    }

    __disable_irq(); //disable interrupts
    return (checksum << 32);
}
