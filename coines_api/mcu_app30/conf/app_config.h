// <e> NRFX_TWIM_ENABLED - nrfx_twim - TWIM peripheral driver
//==========================================================
#ifndef NRFX_TWIM_ENABLED
#define NRFX_TWIM_ENABLED 1
#endif
// <q> NRFX_TWIM0_ENABLED  - Enable TWIM0 instance
 

#ifndef NRFX_TWIM0_ENABLED
#define NRFX_TWIM0_ENABLED 1
#endif

// <q> NRFX_TWIM1_ENABLED  - Enable TWIM1 instance
 

#ifndef NRFX_TWIM1_ENABLED
#define NRFX_TWIM1_ENABLED 0
#endif

// <o> NRFX_TWIM_DEFAULT_CONFIG_FREQUENCY  - Frequency
 
// <26738688=> 100k 
// <67108864=> 250k 
// <104857600=> 400k 

#ifndef NRFX_TWIM_DEFAULT_CONFIG_FREQUENCY
#define NRFX_TWIM_DEFAULT_CONFIG_FREQUENCY 26738688
#endif

// <q> NRFX_TWIM_DEFAULT_CONFIG_HOLD_BUS_UNINIT  - Enables bus holding after uninit
 

#ifndef NRFX_TWIM_DEFAULT_CONFIG_HOLD_BUS_UNINIT
#define NRFX_TWIM_DEFAULT_CONFIG_HOLD_BUS_UNINIT 0
#endif

// <o> NRFX_TWIM_DEFAULT_CONFIG_IRQ_PRIORITY  - Interrupt priority
 
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef NRFX_TWIM_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_TWIM_DEFAULT_CONFIG_IRQ_PRIORITY 6
#endif

#ifndef NRFX_SPIM_ENABLED
#define NRFX_SPIM_ENABLED 1
#endif
// <q> NRFX_SPIM0_ENABLED  - Enable SPIM0 instance


#ifndef NRFX_SPIM0_ENABLED
#define NRFX_SPIM0_ENABLED 0
#endif

// <q> NRFX_SPIM1_ENABLED  - Enable SPIM1 instance


#ifndef NRFX_SPIM1_ENABLED
#define NRFX_SPIM1_ENABLED 1
#endif

// <q> NRFX_SPIM2_ENABLED  - Enable SPIM2 instance


#ifndef NRFX_SPIM2_ENABLED
#define NRFX_SPIM2_ENABLED 1
#endif

// <q> NRFX_SPIM3_ENABLED  - Enable SPIM3 instance


#ifndef NRFX_SPIM3_ENABLED
#define NRFX_SPIM3_ENABLED 0
#endif

// <q> NRFX_SPIM_EXTENDED_ENABLED  - Enable extended SPIM features


#ifndef NRFX_SPIM_EXTENDED_ENABLED
#define NRFX_SPIM_EXTENDED_ENABLED 0
#endif

// <o> NRFX_SPIM_MISO_PULL_CFG  - MISO pin pull configuration.

// <0=> NRF_GPIO_PIN_NOPULL
// <1=> NRF_GPIO_PIN_PULLDOWN
// <3=> NRF_GPIO_PIN_PULLUP

#ifndef NRFX_SPIM_MISO_PULL_CFG
#define NRFX_SPIM_MISO_PULL_CFG 1
#endif

// <o> NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY  - Interrupt priority

// <0=> 0 (highest)
// <1=> 1
// <2=> 2
// <3=> 3
// <4=> 4
// <5=> 5
// <6=> 6
// <7=> 7

#ifndef NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY
#define NRFX_SPIM_DEFAULT_CONFIG_IRQ_PRIORITY 6
#endif


// <e> SPI_ENABLED - nrf_drv_spi - SPI/SPIM peripheral driver - legacy layer
//==========================================================
#ifndef SPI_ENABLED
#define SPI_ENABLED 1
#endif
// <o> SPI_DEFAULT_CONFIG_IRQ_PRIORITY  - Interrupt priority
 

// <i> Priorities 0,2 (nRF51) and 0,1,4,5 (nRF52) are reserved for SoftDevice
// <0=> 0 (highest) 
// <1=> 1 
// <2=> 2 
// <3=> 3 
// <4=> 4 
// <5=> 5 
// <6=> 6 
// <7=> 7 

#ifndef SPI_DEFAULT_CONFIG_IRQ_PRIORITY
#define SPI_DEFAULT_CONFIG_IRQ_PRIORITY 6
#endif

// <o> NRF_SPI_DRV_MISO_PULLUP_CFG  - MISO PIN pull-up configuration.
 
// <0=> NRF_GPIO_PIN_NOPULL 
// <1=> NRF_GPIO_PIN_PULLDOWN 
// <3=> NRF_GPIO_PIN_PULLUP 

#ifndef NRF_SPI_DRV_MISO_PULLUP_CFG
#define NRF_SPI_DRV_MISO_PULLUP_CFG 1
#endif

// <e> SPI0_ENABLED - Enable SPI0 instance
//==========================================================
#ifndef SPI0_ENABLED
#define SPI0_ENABLED 0
#endif
// <q> SPI0_USE_EASY_DMA  - Use EasyDMA
 

#ifndef SPI0_USE_EASY_DMA
#define SPI0_USE_EASY_DMA 1
#endif

// </e>

// <e> SPI1_ENABLED - Enable SPI1 instance
//==========================================================
#ifndef SPI1_ENABLED
#define SPI1_ENABLED 1
#endif
// <q> SPI1_USE_EASY_DMA  - Use EasyDMA
 

#ifndef SPI1_USE_EASY_DMA
#define SPI1_USE_EASY_DMA 1
#endif

// </e>

// <e> SPI2_ENABLED - Enable SPI2 instance
//==========================================================
#ifndef SPI2_ENABLED
#define SPI2_ENABLED 1
#endif
// <q> SPI2_USE_EASY_DMA  - Use EasyDMA
 

#ifndef SPI2_USE_EASY_DMA
#define SPI2_USE_EASY_DMA 0
#endif