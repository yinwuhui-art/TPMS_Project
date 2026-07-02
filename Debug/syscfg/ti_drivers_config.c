/*
 *  ======== ti_drivers_config.c ========
 *  Configured TI-Drivers module definitions
 *
 *  DO NOT EDIT - This file is generated for the LP_EM_CC2745R10_Q1
 *  by the SysConfig tool.
 */

#include <stddef.h>
#include <stdint.h>

#ifndef DeviceFamily_CC27XXX10
#define DeviceFamily_CC27XXX10
#endif

#include <ti/devices/DeviceFamily.h>

#include "ti_drivers_config.h"

/*
 *  ============================= Display =============================
 */

#include <ti/display/Display.h>
#include <ti/display/DisplayUart2.h>

#define CONFIG_Display_COUNT 1


#define Display_UART2BUFFERSIZE 128
static char displayUART2Buffer[Display_UART2BUFFERSIZE];

DisplayUart2_Object displayUart2Object;

const DisplayUart2_HWAttrs displayUart2HWAttrs = {
    .uartIdx      = CONFIG_UART2_0,
    .baudRate     = 115200,
    .mutexTimeout = (unsigned int)(-1),
    .strBuf       = displayUART2Buffer,
    .strBufLen    = Display_UART2BUFFERSIZE
};

const Display_Config Display_config[CONFIG_Display_COUNT] = {
    /* CONFIG_Display_0 */
    /* XDS110 UART */
    {
        .fxnTablePtr = &DisplayUart2Min_fxnTable,
        .object      = &displayUart2Object,
        .hwAttrs     = &displayUart2HWAttrs
    },
};

const uint_least8_t Display_count = CONFIG_Display_COUNT;

/*
 *  =============================== AESCCM ===============================
 */

#include <ti/drivers/AESCCM.h>
#include <ti/drivers/aesccm/AESCCMXXF3.h>

#define CONFIG_AESCCM_COUNT 1
AESCCMXXF3_Object AESCCMXXF3_objects[CONFIG_AESCCM_COUNT];

/*
 *  ======== AESCCMXXF3_hwAttrs ========
 */
const AESCCMXXF3_HWAttrs AESCCMXXF3_hwAttrs[CONFIG_AESCCM_COUNT] = {
    {
        .intPriority = (~0),
    },
};

const AESCCM_Config AESCCM_config[CONFIG_AESCCM_COUNT] = {
    {   /* CONFIG_AESCCM_0 */
        .object  = &AESCCMXXF3_objects[CONFIG_AESCCM_0],
        .hwAttrs = &AESCCMXXF3_hwAttrs[CONFIG_AESCCM_0]
    },
};

const uint_least8_t CONFIG_AESCCM_0_CONST = CONFIG_AESCCM_0;
const uint_least8_t AESCCM_count = CONFIG_AESCCM_COUNT;

/*
 *  =============================== AESCTRDRBG ===============================
 */

#include <ti/drivers/AESCTRDRBG.h>
#include <ti/drivers/aesctrdrbg/AESCTRDRBGXX.h>

#define CONFIG_AESCTRDRBG_COUNT 1

/*
 *  ======== aesctrdrbgXXObjects ========
 */
AESCTRDRBGXX_Object aesctrdrbgXXObjects[CONFIG_AESCTRDRBG_COUNT];

/*
 *  ======== aesctrdrbgXXHWAttrs ========
 */
const AESCTRDRBGXX_HWAttrs aesctrdrbgXXHWAttrs[CONFIG_AESCTRDRBG_COUNT] = {
    /* CONFIG_AESCTRDRBG_0 */
    {
        .aesctrHWAttrs.intPriority = (~0)
    },
};

/*
 *  ======== AESCTRDRBG_config ========
 */
const AESCTRDRBG_Config AESCTRDRBG_config[CONFIG_AESCTRDRBG_COUNT] = {
    /* CONFIG_AESCTRDRBG_0 */
    {
        .object = &aesctrdrbgXXObjects[CONFIG_AESCTRDRBG_0],
        .hwAttrs = &aesctrdrbgXXHWAttrs[CONFIG_AESCTRDRBG_0]
    },
};

const uint_least8_t CONFIG_AESCTRDRBG_0_CONST = CONFIG_AESCTRDRBG_0;
const uint_least8_t AESCTRDRBG_count = CONFIG_AESCTRDRBG_COUNT;

/*
 *  =============================== AESECB ===============================
 */

#include <ti/drivers/AESECB.h>
#include <ti/drivers/aesecb/AESECBXXF3.h>

#define CONFIG_AESECB_COUNT 1
AESECBXXF3_Object AESECBXXF3_objects[CONFIG_AESECB_COUNT];

/*
 *  ======== AESECBXXF3_hwAttrs ========
 */
const AESECBXXF3_HWAttrs AESECBXXF3_hwAttrs[CONFIG_AESECB_COUNT] = {
    {
        .intPriority = (~0),
    },
};

const AESECB_Config AESECB_config[CONFIG_AESECB_COUNT] = {
    {   /* CONFIG_AESECB_0 */
        .object  = &AESECBXXF3_objects[CONFIG_AESECB_0],
        .hwAttrs = &AESECBXXF3_hwAttrs[CONFIG_AESECB_0]
    },
};


const uint_least8_t CONFIG_AESECB_0_CONST = CONFIG_AESECB_0;
const uint_least8_t AESECB_count = CONFIG_AESECB_COUNT;



/*
 *  =============================== Key Store ===============================
 */
#include <third_party/hsmddk/include/Integration/Adapter_PSA/incl/adapter_psa_key_management.h>
#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_device.h)

uint8_t volatileAllocBuffer[KEYSTORE_VOLATILE_MEMORY_POOL_SIZE];
const size_t volatileAllocBufferSizeBytes  = KEYSTORE_VOLATILE_MEMORY_POOL_SIZE;

const size_t MBEDTLS_KEY_VOLATILE_COUNT    = KEYSTORE_VOLATILE_SLOT_COUNT;
const size_t MBEDTLS_KEY_ASSET_STORE_COUNT = KEYSTORE_ASSET_STORE_SLOT_COUNT;
/* For cache slots */
const size_t MBEDTLS_KEY_PERSISTENT_COUNT  = KEYSTORE_PERSISTENT_SLOT_COUNT;
/* For Key Store flash space */
const size_t FLASH_KEY_PERSISTENT_COUNT    = KEYSTORE_PERSISTENT_NUM_KEYS;
/* ITS flash area size */
const size_t FLASH_ITS_SIZE                = KEYSTORE_FLASH_SIZE;


psa_key_context_t gl_PSA_Key[KEYSTORE_TOTAL_SLOT_COUNT];

#if ((KEYSTORE_FLASH_OFFSET + KEYSTORE_FLASH_SIZE) > FLASH_MAIN_SW_SIZE)
    #error "The configured KeyStore flash region goes beyond the accessible flash addresses on the device."
#elif (KEYSTORE_FLASH_OFFSET < FLASH_MAIN_BASE)
    #error "The configured KeyStore flash region begins before the start of accessible flash on the device."
#endif


/* ITS flash area address, with 'Pointer' region type */
const void *FLASH_ITS_ADDRESS = (void *) 0xe6000;




/*
 *  =============================== DMA ===============================
 */

#include <ti/drivers/dma/UDMALPF3.h>
#include <ti/devices/cc27xxx10/inc/hw_memmap.h>

const UDMALPF3_Config UDMALPF3_config = {
        .CtrlBaseAddr = UDMALPF3_CONFIG_BASE,
};

/*
 *  =============================== ECDH ===============================
 */

#include <ti/drivers/ECDH.h>
#include <ti/drivers/ecdh/ECDHXXF3HSM.h>

#define CONFIG_ECDH_COUNT 1

ECDHXXF3HSM_Object ECDHXXF3HSM_objects[CONFIG_ECDH_COUNT];

/*
 *  ======== ECDHXXF3HSMHWAttrs ========
 */
const ECDHXXF3HSM_HWAttrs ECDHXXF3HSM_hwAttrs[CONFIG_ECDH_COUNT] = {
    {0},
};

const ECDH_Config ECDH_config[CONFIG_ECDH_COUNT] = {
    {   /* CONFIG_ECDH_0 */
        .object  = &ECDHXXF3HSM_objects[CONFIG_ECDH_0],
        .hwAttrs = &ECDHXXF3HSM_hwAttrs[CONFIG_ECDH_0]
    },
};

const uint_least8_t CONFIG_ECDH_0_CONST = CONFIG_ECDH_0;
const uint_least8_t ECDH_count = CONFIG_ECDH_COUNT;
/*
 *  =============================== GPIO ===============================
 */

#include <ti/drivers/GPIO.h>

/* The range of pins available on this device */
const uint_least8_t GPIO_pinLowerBound = 0;
const uint_least8_t GPIO_pinUpperBound = 28;

/*
 *  ======== gpioPinConfigs ========
 *  Array of Pin configurations
 */
GPIO_PinConfig gpioPinConfigs[GPIO_NUMBER_OF_CONFIGS] = {
    /* Owned by CONFIG_BUTTON_1 as Button GPIO */
    GPIO_CFG_INPUT_INTERNAL | GPIO_CFG_IN_INT_NONE | GPIO_CFG_PULL_UP_INTERNAL, /* CONFIG_GPIO_BUTTON_1_INPUT */
    /* Owned by CONFIG_UART2_0 as TX */
    GPIO_CFG_OUTPUT_INTERNAL | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_HIGH, /* CONFIG_PIN_UART_TX */
    GPIO_CFG_NO_DIR, /* DIO_2 */
    GPIO_CFG_NO_DIR, /* DIO_3 */
    GPIO_CFG_NO_DIR, /* DIO_4 */
    GPIO_CFG_NO_DIR, /* DIO_5 */
    GPIO_CFG_NO_DIR, /* DIO_6 */
    GPIO_CFG_NO_DIR, /* DIO_7 */
    GPIO_CFG_NO_DIR, /* DIO_8 */
    GPIO_CFG_DO_NOT_CONFIG, /* DIO_9 */
    GPIO_CFG_DO_NOT_CONFIG, /* DIO_10 */
    GPIO_CFG_NO_DIR, /* DIO_11 */
    GPIO_CFG_OUTPUT_INTERNAL | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_LOW, /* CONFIG_GPIO_LED_GREEN */
    GPIO_CFG_NO_DIR, /* DIO_13 */
    GPIO_CFG_NO_DIR, /* DIO_14 */
    GPIO_CFG_NO_DIR, /* DIO_15 */
    GPIO_CFG_OUTPUT_INTERNAL | GPIO_CFG_OUT_STR_MED | GPIO_CFG_OUT_LOW, /* CONFIG_GPIO_LED_RED */
    GPIO_CFG_NO_DIR, /* DIO_17 */
    GPIO_CFG_NO_DIR, /* DIO_18 */
    GPIO_CFG_NO_DIR, /* DIO_19 */
    /* Owned by CONFIG_BUTTON_0 as Button GPIO */
    GPIO_CFG_INPUT_INTERNAL | GPIO_CFG_IN_INT_NONE | GPIO_CFG_PULL_UP_INTERNAL, /* CONFIG_GPIO_BUTTON_0_INPUT */
    GPIO_CFG_NO_DIR, /* DIO_21 */
    GPIO_CFG_NO_DIR, /* DIO_22 */
    GPIO_CFG_NO_DIR, /* DIO_23 */
    GPIO_CFG_NO_DIR, /* DIO_24 */
    GPIO_CFG_NO_DIR, /* DIO_25 */
    GPIO_CFG_NO_DIR, /* DIO_26 */
    GPIO_CFG_NO_DIR, /* DIO_27 */
    GPIO_CFG_NO_DIR, /* DIO_28 */
};

/*
 *  ======== gpioCallbackFunctions ========
 *  Array of callback function pointers
 *  Change at runtime with GPIO_setCallback()
 */
GPIO_CallbackFxn gpioCallbackFunctions[GPIO_NUMBER_OF_CONFIGS];

/*
 *  ======== gpioUserArgs ========
 *  Array of user argument pointers
 *  Change at runtime with GPIO_setUserArg()
 *  Get values with GPIO_getUserArg()
 */
void* gpioUserArgs[GPIO_NUMBER_OF_CONFIGS];

const uint_least8_t CONFIG_GPIO_LED_GREEN_CONST = CONFIG_GPIO_LED_GREEN;
const uint_least8_t CONFIG_GPIO_LED_RED_CONST = CONFIG_GPIO_LED_RED;
const uint_least8_t CONFIG_GPIO_BUTTON_0_INPUT_CONST = CONFIG_GPIO_BUTTON_0_INPUT;
const uint_least8_t CONFIG_GPIO_BUTTON_1_INPUT_CONST = CONFIG_GPIO_BUTTON_1_INPUT;
const uint_least8_t CONFIG_PIN_UART_TX_CONST = CONFIG_PIN_UART_TX;

/*
 *  ======== GPIO_config ========
 */
const GPIO_Config GPIO_config = {
    .configs = (GPIO_PinConfig *)gpioPinConfigs,
    .callbacks = (GPIO_CallbackFxn *)gpioCallbackFunctions,
    .userArgs = gpioUserArgs,
    .intPriority = (~0)
};

/*
 *  =============================== NVS ===============================
 */

#include <ti/drivers/NVS.h>
#include <ti/drivers/nvs/NVSLPF3.h>

/*
 *  NVSLPF3 Internal NVS flash region definitions
 *
 * Place uninitialized char arrays at addresses
 * corresponding to the 'regionBase' addresses defined in
 * the configured NVS regions. These arrays are used as
 * place holders so that the linker will not place other
 * content there.
 *
 * For GCC targets, the char arrays are each placed into
 * the shared ".nvs" section. The user must add content to
 * their GCC linker command file to place the .nvs section
 * at the lowest 'regionBase' address specified in their NVS
 * regions.
 */

#if defined(__TI_COMPILER_VERSION__) || defined(__clang__)

static char flashBuf0[0x3000] __attribute__ ((retain, noinit, location(0xe5000)));

#elif defined(__IAR_SYSTEMS_ICC__)

__no_init static char flashBuf0[0x3000] @ 0xe5000;

#elif defined(__GNUC__)

__attribute__ ((section (".nvs")))
static char flashBuf0[0x3000];

#endif

NVSLPF3_Object NVSLPF3_objects[1];

static const NVSLPF3_HWAttrs NVSLPF3_hwAttrs[1] = {
    /* CONFIG_NVSINTERNAL */
    {
        .regionBase = (void *) flashBuf0,
        .regionSize = 0x3000
    },
};

#define CONFIG_NVS_COUNT 1

const NVS_Config NVS_config[CONFIG_NVS_COUNT] = {
    /* CONFIG_NVSINTERNAL */
    {
        .fxnTablePtr = &NVSLPF3_fxnTable,
        .object = &NVSLPF3_objects[0],
        .hwAttrs = &NVSLPF3_hwAttrs[0],
    },
};

const uint_least8_t CONFIG_NVSINTERNAL_CONST = CONFIG_NVSINTERNAL;
const uint_least8_t NVS_count = CONFIG_NVS_COUNT;

/*
 *  =============================== Power ===============================
 */
#include <ti/drivers/Power.h>
#include "ti_drivers_config.h"
#include DeviceFamily_constructPath(driverlib/ckmd.h)
#include DeviceFamily_constructPath(driverlib/pmctl.h)

extern void PowerCC27XX_standbyPolicy(void);

const uint32_t PowerLPF3_capArrayP0 = 553648128; /* floor(8.25 * 2^26) */
const uint32_t PowerLPF3_capArrayP1 = 4697620; /* floor(0.07 * 2^26) */
const uint8_t  PowerLPF3_capArrayShift = 26;  /* shift-value to bring floating-point coefficients to fixed-point */


const PowerCC27XX_Config PowerCC27XX_config = {
    .policyInitFxn              = NULL,
    .policyFxn                  = PowerCC27XX_standbyPolicy,
    .startInitialHfxtAmpCompFxn = NULL,
};


/*
 * =========================== RCL Regulatory Domain Configuration ===========================
 */

uint8_t rclRegulatoryMask = RCL_REGULATORY_DOMAIN_ETSI;



/*
 *  =============================== RNG ===============================
 */

#include <ti/drivers/RNG.h>
#include <ti/drivers/rng/RNGXXF3HSM.h>

#define CONFIG_RNG_COUNT 1

#if defined(__IAR_SYSTEMS_ICC__)
#pragma data_alignment=32
#else
__attribute__ ((aligned (32)))
#endif

const RNG_ReturnBehavior RNGXXF3HSM_returnBehavior = RNG_RETURN_BEHAVIOR_BLOCKING;

RNGXXF3HSM_Object RNGXXF3HSM_objects[CONFIG_RNG_COUNT];

/*
 *  ======== RNGXXF3HSMHWAttrs ========
 */
const RNGXXF3HSM_HWAttrs RNGXXF3HSM_hwAttrs[CONFIG_RNG_COUNT] = {
    {0},
};

const RNG_Config RNG_config[CONFIG_RNG_COUNT] = {
    {   /* CONFIG_RNG_0 */
        .object         = &RNGXXF3HSM_objects[CONFIG_RNG_0],
        .hwAttrs        = NULL
    },
};

const uint_least8_t CONFIG_RNG_0_CONST = CONFIG_RNG_0;
const uint_least8_t RNG_count = CONFIG_RNG_COUNT;

/*
 *  =============================== UART2 ===============================
 */

#include <ti/drivers/UART2.h>
#include <ti/drivers/uart2/UART2LPF3.h>
#include <ti/drivers/GPIO.h>
#include <ti/drivers/Power.h>
#include <ti/drivers/dma/UDMALPF3.h>
#include <ti/drivers/Power.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_memmap.h)
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(driverlib/evtsvt.h)

#define CONFIG_UART2_COUNT 1

UART2LPF3_Object UART2LPF3_objects[CONFIG_UART2_COUNT];

static unsigned char uart2RxRingBuffer0[32];



ALLOCATE_CONTROL_TABLE_ENTRY(dmaChannel3ControlTableEntry, 3);
ALLOCATE_CONTROL_TABLE_ENTRY(dmaChannel2ControlTableEntry, 2);

static const UART2LPF3_HWAttrs UART2LPF3_hwAttrs[CONFIG_UART2_COUNT] = {
  {
    .baseAddr              = UART0_BASE,
    .intNum                = INT_UART0_COMB,
    .intPriority           = (~0),
    .rxPin                 = GPIO_INVALID_INDEX,
    .txPin                 = CONFIG_PIN_UART_TX,
    .ctsPin                = GPIO_INVALID_INDEX,
    .rtsPin                = GPIO_INVALID_INDEX,
    .flowControl           = UART2_FLOWCTRL_NONE,
    .rxBufPtr              = uart2RxRingBuffer0,
    .rxBufSize             = sizeof(uart2RxRingBuffer0),
    .txBufPtr              = NULL,
    .txBufSize             = 0,
    .txPinMux              = 4,
    .rxPinMux              = GPIO_MUX_GPIO_INTERNAL,
    .ctsPinMux             = GPIO_MUX_GPIO_INTERNAL,
    .rtsPinMux             = GPIO_MUX_GPIO_INTERNAL,
    .dmaRxTableEntryPri    = &dmaChannel3ControlTableEntry,
    .dmaTxTableEntryPri    = &dmaChannel2ControlTableEntry,
    .rxChannelMask         = UDMA_CHANNEL_3_M,
    .txChannelMask         = UDMA_CHANNEL_2_M,
    .rxChannelEvtMux       = EVTSVT_DMA_TRIG_UART0RXTRG,
    .txChannelEvtMux       = EVTSVT_DMA_TRIG_UART0TXTRG,
    .txChannelSubscriberId = EVTSVT_DMA_CH2,
    .rxChannelSubscriberId = EVTSVT_DMA_CH3,
    .codingScheme          = UART2LPF3_CODING_UART,
    .concatenateFIFO       = false,
    .irLPClkDivider        = 8,
    .powerID               = PowerLPF3_PERIPH_UART0
  },
};

const UART2_Config UART2_config[CONFIG_UART2_COUNT] = {
    {   /* CONFIG_UART2_0 */
        .object      = &UART2LPF3_objects[CONFIG_UART2_0],
        .hwAttrs     = &UART2LPF3_hwAttrs[CONFIG_UART2_0]
    },
};

const uint_least8_t CONFIG_UART2_0_CONST = CONFIG_UART2_0;
const uint_least8_t UART2_count = CONFIG_UART2_COUNT;

/* Forward-declare write function used for blocking and callback mode */
extern int_fast16_t UART2_writeTimeoutBlocking(UART2_Handle handle, const void *buffer, size_t size, size_t *bytesWritten, uint32_t timeout);

/* Override weak writeTimeout function in UART2 library to only use blocking/callback mode */
int_fast16_t UART2_writeTimeout(UART2_Handle handle, const void *buffer, size_t size, size_t *bytesWritten, uint32_t timeout)
{
    return UART2_writeTimeoutBlocking(handle, buffer, size, bytesWritten, timeout);
}

/*
 *  =============================== Button ===============================
 */
#include <ti/drivers/apps/Button.h>

#define CONFIG_BUTTON_COUNT 2
Button_Object ButtonObjects[CONFIG_BUTTON_COUNT];

static const Button_HWAttrs ButtonHWAttrs[CONFIG_BUTTON_COUNT] = {
    /* CONFIG_BUTTON_0 */
    /* LaunchPad Button BTN-1 (Left) */
    {
        .gpioIndex = CONFIG_GPIO_BUTTON_0_INPUT,
        .pullMode = Button_PULL_UP,
        .internalPullEnabled = 1,
    },
    /* CONFIG_BUTTON_1 */
    /* LaunchPad Button BTN-2 (Right) */
    {
        .gpioIndex = CONFIG_GPIO_BUTTON_1_INPUT,
        .pullMode = Button_PULL_UP,
        .internalPullEnabled = 1,
    },
};

const Button_Config Button_config[CONFIG_BUTTON_COUNT] = {
    /* CONFIG_BUTTON_0 */
    /* LaunchPad Button BTN-1 (Left) */
    {
        .object = &ButtonObjects[CONFIG_BUTTON_0],
        .hwAttrs = &ButtonHWAttrs[CONFIG_BUTTON_0]
    },
    /* CONFIG_BUTTON_1 */
    /* LaunchPad Button BTN-2 (Right) */
    {
        .object = &ButtonObjects[CONFIG_BUTTON_1],
        .hwAttrs = &ButtonHWAttrs[CONFIG_BUTTON_1]
    },
};

const uint_least8_t CONFIG_BUTTON_0_CONST = CONFIG_BUTTON_0;
const uint_least8_t CONFIG_BUTTON_1_CONST = CONFIG_BUTTON_1;
const uint_least8_t Button_count = CONFIG_BUTTON_COUNT;

/*
 *  =============================== BatMon Support ===============================
 */
#include <ti/drivers/batterymonitor/BatMonSupportLPF3.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(inc/hw_ints.h)
#include DeviceFamily_constructPath(driverlib/evtsvt.h)

const BatMonSupportLPF3_Config BatMonSupportLPF3_config = {
    .intNum = INT_CPUIRQ2,
    .intPriority = (~0),
    .intSubscriberId = EVTSVT_SUB_CPUIRQ2,
};

/*
 *  =============================== AESCommonXXF3 ===============================
 */

#include <ti/drivers/dma/UDMALPF3.h>

/*
 *  ======== AES DMA Channel configuration ========
 */

ALLOCATE_CONTROL_TABLE_ENTRY(dmaChannel4ControlTableEntry, 4);
ALLOCATE_CONTROL_TABLE_ENTRY(dmaChannel5ControlTableEntry, 5);

volatile uDMAControlTableEntry *AESCommonXXF3_dmaPriControlTableEntryChA = &dmaChannel4ControlTableEntry;
volatile uDMAControlTableEntry *AESCommonXXF3_dmaPriControlTableEntryChB = &dmaChannel5ControlTableEntry;
uint32_t AESCommonXXF3_dmaChannelMaskChA = (uint32_t)0x01UL << 4;
uint32_t AESCommonXXF3_dmaChannelMaskChB = (uint32_t)0x01UL << 5;


#include <stdbool.h>

#include <ti/devices/DeviceFamily.h>
#include DeviceFamily_constructPath(driverlib/cpu.h)
#include DeviceFamily_constructPath(driverlib/hapi.h)

#include <ti/drivers/GPIO.h>

/* Board GPIO defines */
#define BOARD_EXT_FLASH_SPI_CS      19
#define BOARD_EXT_FLASH_SPI_CLK     3
#define BOARD_EXT_FLASH_SPI_PICO    5
#define BOARD_EXT_FLASH_SPI_POCI    4
/*
 *  ======== Board_sendExtFlashByte ========
 */
void Board_sendExtFlashByte(uint8_t byte)
{
    uint8_t i;

    /* SPI Flash CS */
    GPIO_write(BOARD_EXT_FLASH_SPI_CS, 0);

    for (i = 0; i < 8; i++) {
        GPIO_write(BOARD_EXT_FLASH_SPI_CLK, 0); /* SPI Flash CLK */

        /* SPI Flash PICO */
        GPIO_write(BOARD_EXT_FLASH_SPI_PICO, (byte >> (7 - i)) & 0x01);
        GPIO_write(BOARD_EXT_FLASH_SPI_CLK, 1);  /* SPI Flash CLK */

        /* Waste a few cycles to keep the CLK high for at
         * least 45% of the period.
         * 3 cycles per loop: 16 loops @ 96 MHz = 0.5 us.
         */
        CPUDelay(16);
    }

    GPIO_write(BOARD_EXT_FLASH_SPI_CLK, 0);  /* CLK */
    GPIO_write(BOARD_EXT_FLASH_SPI_CS, 1);  /* CS */

    /* Keep CS high at least 40 us */
    HapiWaitUs(40);
}

/*
 *  ======== Board_wakeUpExtFlash ========
 */
void Board_wakeUpExtFlash(void)
{
    /* SPI Flash CS*/
    GPIO_setConfig(BOARD_EXT_FLASH_SPI_CS, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_HIGH | GPIO_CFG_OUT_STR_MED);

    /* To wake up we need to toggle the chip select at
     * least 20 ns and then wait at least 35 us.
     */

    /* Toggle chip select for ~20ns to wake ext. flash */
    GPIO_write(BOARD_EXT_FLASH_SPI_CS, 0);
    /* 3 cycles per loop: 2 loop @ 96 MHz ~= 62 ns */
    CPUDelay(2);
    GPIO_write(BOARD_EXT_FLASH_SPI_CS, 1);
    /* Wait for 35 us */
    HapiWaitUs(35);
}

/*
 *  ======== Board_shutDownExtFlash ========
 */
void Board_shutDownExtFlash(void)
{
    /* To be sure we are putting the flash into sleep and not waking it,
     * we first have to make a wake up call
     */
    Board_wakeUpExtFlash();

    /* SPI Flash CS*/
    GPIO_setConfig(BOARD_EXT_FLASH_SPI_CS, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_HIGH | GPIO_CFG_OUT_STR_MED);
    /* SPI Flash CLK */
    GPIO_setConfig(BOARD_EXT_FLASH_SPI_CLK, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW | GPIO_CFG_OUT_STR_MED);
    /* SPI Flash PICO */
    GPIO_setConfig(BOARD_EXT_FLASH_SPI_PICO, GPIO_CFG_OUTPUT | GPIO_CFG_OUT_LOW | GPIO_CFG_OUT_STR_MED);
    /* SPI Flash POCI */
    GPIO_setConfig(BOARD_EXT_FLASH_SPI_POCI, GPIO_CFG_IN_PD);

    uint8_t extFlashShutdown = 0xB9;

    Board_sendExtFlashByte(extFlashShutdown);

    GPIO_resetConfig(BOARD_EXT_FLASH_SPI_CS);
    GPIO_resetConfig(BOARD_EXT_FLASH_SPI_CLK);
    GPIO_resetConfig(BOARD_EXT_FLASH_SPI_PICO);
    GPIO_resetConfig(BOARD_EXT_FLASH_SPI_POCI);
}


#include <ti/drivers/Board.h>

/*
 *  ======== Board_initHook ========
 *  Perform any board-specific initialization needed at startup.  This
 *  function is declared weak to allow applications to override it if needed.
 */
void __attribute__((weak)) Board_initHook(void)
{
}

/*
 *  ======== Board_init ========
 *  Perform any initialization needed before using any board APIs
 */
void Board_init(void)
{
    /* ==== /ti/drivers/Power initialization ==== */

    /* Set non-default cap array trims */
    CKMDSetInitialCapTrim(33, 33);
    CKMDSetTargetCapTrim(33, 33);

    Power_init();

    PowerLPF3_selectLFXT();

    PMCTLSetVoltageRegulator(PMCTL_VOLTAGE_REGULATOR_DCDC);


    /* Set constraint to keep flash LDO on in IDLE */
    Power_setConstraint(PowerLPF3_NEED_FLASH_IN_IDLE);

    PowerLPF3_initHFXTCompensation(61075777, /* floor(14.5616 * 2^22) */
                                   -1831946, /* floor(-0.43677 * 2^22) */
                                   -35777, /* floor(-0.00853 * 2^22) */
                                   419, /* floor(0.0001 * 2^22) */
                                   22,
                                   false);               /* FCFG trim-values available */

    /* HFXT temperature compensation will be performed as long as the measured temperature exceeds -50
     * degrees Celsius, and drifts 2 degrees or more.
     */
    PowerLPF3_enableHFXTCompensation(-50, 2);


    /* ==== /ti/drivers/GPIO initialization ==== */
    /* Setup GPIO module and default-initialise pins */
    GPIO_init();

    /* ==== /ti/drivers/DMA initialization ==== */
    UDMALPF3_init();

    /* ==== /ti/drivers/RCL initialization ==== */

    RNG_init();

    Board_shutDownExtFlash();

    Board_initHook();
}

