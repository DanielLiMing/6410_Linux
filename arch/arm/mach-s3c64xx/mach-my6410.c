/* linux/arch/arm/mach-s3c64xx/mach-my6410.c
 *
 * Copyright 2008 Openmoko, Inc.
 * Copyright 2008 Simtec Electronics
 *	Ben Dooks <ben@simtec.co.uk>
 *	http://armlinux.simtec.co.uk/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
*/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>
#include <linux/io.h>
#include <linux/i2c.h>
#include <linux/leds.h>
#include <linux/fb.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/smsc911x.h>
#include <linux/regulator/fixed.h>
#include <linux/regulator/machine.h>
#include <linux/pwm_backlight.h>

#ifdef CONFIG_SMDK6410_WM1190_EV1
#include <linux/mfd/wm8350/core.h>
#include <linux/mfd/wm8350/pmic.h>
#endif

#ifdef CONFIG_SMDK6410_WM1192_EV1
#include <linux/mfd/wm831x/core.h>
#include <linux/mfd/wm831x/pdata.h>
#endif

#include <video/platform_lcd.h>

#include <asm/hardware/vic.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/mach/irq.h>

#include <mach/hardware.h>
#include <mach/map.h>

#include <asm/irq.h>
#include <asm/mach-types.h>

#include <plat/regs-serial.h>
#include <mach/regs-modem.h>
#include <mach/regs-gpio.h>
#include <mach/regs-sys.h>
#include <mach/regs-srom.h>
#include <plat/ata.h>
#include <plat/iic.h>
#include <plat/fb.h>
#include <plat/gpio-cfg.h>

#include <plat/clock.h>
#include <plat/devs.h>
#include <plat/cpu.h>
#include <plat/adc.h>
// #include <plat/ts.h>
#include <plat/keypad.h>
#include <plat/backlight.h>
#include <plat/regs-fb-v4.h>

#include <mach/ts.h>
#include <mach/dma.h>
#include <linux/dma-mapping.h>
// #include <plat/audio.h>
// #include <linux/mtd/mtd.h>
// #include <linux/mtd/partitions.h>
#include <linux/gpio_keys.h>
#include <linux/dm9000.h>


#include "common.h"

#define UCON S3C2410_UCON_DEFAULT | S3C2410_UCON_UCLK
#define ULCON S3C2410_LCON_CS8 | S3C2410_LCON_PNONE | S3C2410_LCON_STOPB
#define UFCON S3C2410_UFCON_RXTRIG8 | S3C2410_UFCON_FIFOMODE

#define S3C64XX_PA_DM9000	(0x18000000)
#define S3C64XX_SZ_DN9000	SZ_1M

static struct s3c2410_uartcfg my6410_uartcfgs[] __initdata = {
	[0] = {
		.hwport	     = 0,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	},
	[1] = {
		.hwport	     = 1,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	},
	[2] = {
		.hwport	     = 2,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	},
	[3] = {
		.hwport	     = 3,
		.flags	     = 0,
		.ucon	     = UCON,
		.ulcon	     = ULCON,
		.ufcon	     = UFCON,
	},
};

static struct gpio_led my6410_leds[] = {
	[0] = {
		.name = "LED1",
		.gpio = S3C64XX_GPM(0),
	},
	[1] = {
		.name = "LED2",
		.gpio = S3C64XX_GPM(1),
	},
	[2] = {
		.name = "LED3",
		.gpio = S3C64XX_GPM(2),
	},
	[3] = {
		.name = "LED4",
		.gpio = S3C64XX_GPM(3),
	},
};

static struct gpio_led_platform_data my6410_gpio_led_pdata = {
	.num_leds = ARRAY_SIZE(my6410_leds),
	.leds = my6410_leds,
};

static struct platform_device my6410_device_led = {
	.name = "leds-gpio",
	.id = -1,
	.dev = {
		.platform_data = &my6410_gpio_led_pdata,
	},
};

static struct gpio_keys_button my6410_buttons[] = {
	{
		.gpio = S3C64XX_GPN(0),
		.code = KEY_UP,
		.desc = "Up",
		.active_low = 1,
		.wakeup = 0, 
	},
	{
		.gpio = S3C64XX_GPN(1),
		.code = KEY_DOWN,
		.desc = "Down",
		.active_low = 1,
		.wakeup = 0,
	},
	{
		.gpio = S3C64XX_GPN(2),
		.code = KEY_LEFT,
		.desc = "Left",
		.active_low = 1,
		.wakeup = 0,
	},
	{
		.gpio = S3C64XX_GPN(3),
		.code = KEY_RIGHT,
		.desc = "Right",
		.active_low = 1,
		.wakeup = 0,
	},
	{
		.gpio = S3C64XX_GPN(4),
		.code = KEY_ENTER,
		.desc = "Enter",
		.active_low = 1,
		.wakeup = 0.
	},
	{
		.gpio = S3C64XX_GPN(5),
		.code = KEY_ESC,
		.desc = "Esc",
		.active_low = 1,
		.wakeup = 0,
	}
};
static struct gpio_keys_platform_data my6410_button_data = {
	.buttons = my6410_buttons,
	.nbuttons = ARRAY_SIZE(my6410_buttons),
};

static struct platform_device my6410_device_button = {
	.name = "gpio-keys",
	.id = -1,
	.dev = {
		.platform_data = &my6410_button_data,
	},
};

static struct resource my6410_dm9000_resources[] = {
	[0] = {
		.start	= S3C64XX_PA_DM9000,
		.end	= S3C64XX_PA_DM9000 + 3,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= S3C64XX_PA_DM9000 + 4,
		.end	= S3C64XX_PA_DM9000 + S3C64XX_SZ_DN9000 -1,
		.flags	= IORESOURCE_MEM,
	},
	[2] = {
		.start	= IRQ_EINT(7),
		.end	= IRQ_EINT(7),
		.flags	= IORESOURCE_IRQ|IRQF_TRIGGER_HIGH,
	},
};

static struct dm9000_plat_data my6410_dm9000_platdata = {
	.flags = DM9000_PLATF_16BITONLY,
	.dev_addr = {0x08,0x90,0x00,0xa0,0x90,0x90},
};
static struct platform_device my6410_device_dm9000 = {
	.name	= "dm9000",
	.id		= 0,
	.num_resources = ARRAY_SIZE(my6410_dm9000_resources),
	.resource = my6410_dm9000_resources,
	.dev	= {
		.platform_data = &my6410_dm9000_platdata,
	},

};

// static struct s3c_ts_info s3c_ts_platform __initdata = {
// 	.delay		= 10000,
// 	.presc 		= 49,
// 	.oversampling_shift 	= 2,
// 	.resol_bit	= 12,
// 	.s3c_adc_con	= ADC_TYPE_2,
// };
static struct s3c_ts_mach_info s3c_ts_platform __initdata = {
	.delay	=10000,
	.presc		=49,
	.oversampling_shift=2,
	.resol_bit	=12,
	.s3c_adc_con	=ADC_TYPE_2,
};

/* framebuffer and LCD setup. */

/* GPF15 = LCD backlight control
 * GPF13 => Panel power
 * GPN5 = LCD nRESET signal
 * PWM_TOUT1 => backlight brightness
 */

static void my6410_lcd_power_set(struct plat_lcd_data *pd,
				   unsigned int power)
{
	if (power) {
		gpio_direction_output(S3C64XX_GPF(13), 1);

		/* fire nRESET on power up */
		gpio_direction_output(S3C64XX_GPN(5), 0);
		msleep(10);
		gpio_direction_output(S3C64XX_GPN(5), 1);
		msleep(1);
	} else {
		gpio_direction_output(S3C64XX_GPF(13), 0);
	}
}

static struct plat_lcd_data my6410_lcd_power_data = {
	.set_power	= my6410_lcd_power_set,
};

static struct platform_device my6410_lcd_powerdev = {
	.name			= "platform-lcd",
	.dev.parent		= &s3c_device_fb.dev,
	.dev.platform_data	= &my6410_lcd_power_data,
};

static struct s3c_fb_pd_win my6410_fb_win0 = {
	/* this is to ensure we use win0 */
	.win_mode	= {
		.left_margin	= 2,
		.right_margin	= 2,
		.upper_margin	= 2,
		.lower_margin	= 2,
		.hsync_len	= 41,
		.vsync_len	= 10,
		.xres		= 480,
		.yres		= 272,
	},
	.max_bpp	= 32,
	.default_bpp	= 16,
	// .virtual_y	= 272 * 2,
	// .virtual_x	= 880,
};

/* 405566 clocks per frame => 60Hz refresh requires 24333960Hz clock */
static struct s3c_fb_platdata my6410_lcd_pdata __initdata = {
	.setup_gpio	= s3c64xx_fb_gpio_setup_24bpp,
	.win[0]		= &my6410_fb_win0,
	.vidcon0	= VIDCON0_VIDOUT_RGB | VIDCON0_PNRMODE_RGB,
	.vidcon1	= VIDCON1_INV_HSYNC | VIDCON1_INV_VSYNC,
};

/*
 * Configuring Ethernet on SMDK6410
 *
 * Both CS8900A and LAN9115 chips share one chip select mediated by CFG6.
 * The constant address below corresponds to nCS1
 *
 *  1) Set CFGB2 p3 ON others off, no other CFGB selects "ethernet"
 *  2) CFG6 needs to be switched to "LAN9115" side
 */

static struct resource my6410_smsc911x_resources[] = {
	[0] = {
		.start = S3C64XX_PA_XM0CSN1,
		.end   = S3C64XX_PA_XM0CSN1 + SZ_64K - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = S3C_EINT(10),
		.end   = S3C_EINT(10),
		.flags = IORESOURCE_IRQ | IRQ_TYPE_LEVEL_LOW,
	},
};

static struct smsc911x_platform_config my6410_smsc911x_pdata = {
	.irq_polarity  = SMSC911X_IRQ_POLARITY_ACTIVE_LOW,
	.irq_type      = SMSC911X_IRQ_TYPE_OPEN_DRAIN,
	.flags         = SMSC911X_USE_32BIT | SMSC911X_FORCE_INTERNAL_PHY,
	.phy_interface = PHY_INTERFACE_MODE_MII,
};


static struct platform_device my6410_smsc911x = {
	.name          = "smsc911x",
	.id            = -1,
	.num_resources = ARRAY_SIZE(my6410_smsc911x_resources),
	.resource      = &my6410_smsc911x_resources[0],
	.dev = {
		.platform_data = &my6410_smsc911x_pdata,
	},
};

#ifdef CONFIG_REGULATOR
static struct regulator_consumer_supply my6410_b_pwr_5v_consumers[] __initdata = {
	REGULATOR_SUPPLY("PVDD", "0-001b"),
	REGULATOR_SUPPLY("AVDD", "0-001b"),
};

static struct regulator_init_data my6410_b_pwr_5v_data = {
	.constraints = {
		.always_on = 1,
	},
	.num_consumer_supplies = ARRAY_SIZE(my6410_b_pwr_5v_consumers),
	.consumer_supplies = my6410_b_pwr_5v_consumers,
};

static struct fixed_voltage_config my6410_b_pwr_5v_pdata = {
	.supply_name = "B_PWR_5V",
	.microvolts = 5000000,
	.init_data = &my6410_b_pwr_5v_data,
	.gpio = -EINVAL,
};

static struct platform_device my6410_b_pwr_5v = {
	.name          = "reg-fixed-voltage",
	.id            = -1,
	.dev = {
		.platform_data = &my6410_b_pwr_5v_pdata,
	},
};
#endif

static struct s3c_ide_platdata my6410_ide_pdata __initdata = {
	.setup_gpio	= s3c64xx_ide_setup_gpio,
};

static uint32_t my6410_keymap[] __initdata = {
	/* KEY(row, col, keycode) */
	KEY(0, 3, KEY_1), KEY(0, 4, KEY_2), KEY(0, 5, KEY_3),
	KEY(0, 6, KEY_4), KEY(0, 7, KEY_5),
	KEY(1, 3, KEY_A), KEY(1, 4, KEY_B), KEY(1, 5, KEY_C),
	KEY(1, 6, KEY_D), KEY(1, 7, KEY_E)
};

static struct matrix_keymap_data my6410_keymap_data __initdata = {
	.keymap		= my6410_keymap,
	.keymap_size	= ARRAY_SIZE(my6410_keymap),
};

static struct samsung_keypad_platdata my6410_keypad_data __initdata = {
	.keymap_data	= &my6410_keymap_data,
	.rows		= 2,
	.cols		= 8,
};

static struct map_desc my6410_iodesc[] = {
	{
		.virtual = (unsigned long)S3C_VA_LCD,
		.pfn 	 = __phys_to_pfn(S3C_PA_FB),
		.length  = SZ_16K,
		.type 	 = MT_DEVICE,
	},

};

static struct platform_device *my6410_devices[] __initdata = {
#ifdef CONFIG_SMDK6410_SD_CH0
	&s3c_device_hsmmc0,
#endif
#ifdef CONFIG_SMDK6410_SD_CH1
	&s3c_device_hsmmc1,
#endif
	&s3c_device_i2c0,
	// &s3c_device_i2c1,
	&s3c_device_fb,
	&s3c_device_ohci,
	&s3c_device_usb_hsotg,
	&samsung_asoc_dma,
	// &s3c64xx_device_iisv4,
	// &samsung_device_keypad,

#ifdef CONFIG_REGULATOR
	// &my6410_b_pwr_5v,
#endif
	&my6410_lcd_powerdev,

	// &my6410_smsc911x,
	// &s3c_device_adc,
	// &s3c_device_cfcon,
	&s3c_device_rtc,
	&s3c_device_ts,
	// &s3c_device_wdt,
	&my6410_device_led,
	&my6410_device_button,
	&my6410_device_dm9000,
};

#ifdef CONFIG_REGULATOR
/* ARM core */
static struct regulator_consumer_supply my6410_vddarm_consumers[] = {
	REGULATOR_SUPPLY("vddarm", NULL),
};

/* VDDARM, BUCK1 on J5 */
static struct regulator_init_data my6410_vddarm = {
	.constraints = {
		.name = "PVDD_ARM",
		.min_uV = 1000000,
		.max_uV = 1300000,
		.always_on = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
	},
	.num_consumer_supplies = ARRAY_SIZE(my6410_vddarm_consumers),
	.consumer_supplies = my6410_vddarm_consumers,
};

/* VDD_INT, BUCK2 on J5 */
static struct regulator_init_data my6410_vddint = {
	.constraints = {
		.name = "PVDD_INT",
		.min_uV = 1000000,
		.max_uV = 1200000,
		.always_on = 1,
		.valid_ops_mask = REGULATOR_CHANGE_VOLTAGE,
	},
};

/* VDD_HI, LDO3 on J5 */
static struct regulator_init_data my6410_vddhi = {
	.constraints = {
		.name = "PVDD_HI",
		.always_on = 1,
	},
};

/* VDD_PLL, LDO2 on J5 */
static struct regulator_init_data my6410_vddpll = {
	.constraints = {
		.name = "PVDD_PLL",
		.always_on = 1,
	},
};

/* VDD_UH_MMC, LDO5 on J5 */
static struct regulator_init_data my6410_vdduh_mmc = {
	.constraints = {
		.name = "PVDD_UH+PVDD_MMC",
		.always_on = 1,
	},
};

/* VCCM3BT, LDO8 on J5 */
static struct regulator_init_data my6410_vccmc3bt = {
	.constraints = {
		.name = "PVCCM3BT",
		.always_on = 1,
	},
};

/* VCCM2MTV, LDO11 on J5 */
static struct regulator_init_data my6410_vccm2mtv = {
	.constraints = {
		.name = "PVCCM2MTV",
		.always_on = 1,
	},
};

/* VDD_LCD, LDO12 on J5 */
static struct regulator_init_data my6410_vddlcd = {
	.constraints = {
		.name = "PVDD_LCD",
		.always_on = 1,
	},
};

/* VDD_OTGI, LDO9 on J5 */
static struct regulator_init_data my6410_vddotgi = {
	.constraints = {
		.name = "PVDD_OTGI",
		.always_on = 1,
	},
};

/* VDD_OTG, LDO14 on J5 */
static struct regulator_init_data my6410_vddotg = {
	.constraints = {
		.name = "PVDD_OTG",
		.always_on = 1,
	},
};

/* VDD_ALIVE, LDO15 on J5 */
static struct regulator_init_data my6410_vddalive = {
	.constraints = {
		.name = "PVDD_ALIVE",
		.always_on = 1,
	},
};

/* VDD_AUDIO, VLDO_AUDIO on J5 */
static struct regulator_init_data my6410_vddaudio = {
	.constraints = {
		.name = "PVDD_AUDIO",
		.always_on = 1,
	},
};
#endif

#ifdef CONFIG_SMDK6410_WM1190_EV1
/* S3C64xx internal logic & PLL */
static struct regulator_init_data wm8350_dcdc1_data = {
	.constraints = {
		.name = "PVDD_INT+PVDD_PLL",
		.min_uV = 1200000,
		.max_uV = 1200000,
		.always_on = 1,
		.apply_uV = 1,
	},
};

/* Memory */
static struct regulator_init_data wm8350_dcdc3_data = {
	.constraints = {
		.name = "PVDD_MEM",
		.min_uV = 1800000,
		.max_uV = 1800000,
		.always_on = 1,
		.state_mem = {
			 .uV = 1800000,
			 .mode = REGULATOR_MODE_NORMAL,
			 .enabled = 1,
		},
		.initial_state = PM_SUSPEND_MEM,
	},
};

/* USB, EXT, PCM, ADC/DAC, USB, MMC */
static struct regulator_consumer_supply wm8350_dcdc4_consumers[] = {
	REGULATOR_SUPPLY("DVDD", "0-001b"),
};

static struct regulator_init_data wm8350_dcdc4_data = {
	.constraints = {
		.name = "PVDD_HI+PVDD_EXT+PVDD_SYS+PVCCM2MTV",
		.min_uV = 3000000,
		.max_uV = 3000000,
		.always_on = 1,
	},
	.num_consumer_supplies = ARRAY_SIZE(wm8350_dcdc4_consumers),
	.consumer_supplies = wm8350_dcdc4_consumers,
};

/* OTGi/1190-EV1 HPVDD & AVDD */
static struct regulator_init_data wm8350_ldo4_data = {
	.constraints = {
		.name = "PVDD_OTGI+HPVDD+AVDD",
		.min_uV = 1200000,
		.max_uV = 1200000,
		.apply_uV = 1,
		.always_on = 1,
	},
};

static struct {
	int regulator;
	struct regulator_init_data *initdata;
} wm1190_regulators[] = {
	{ WM8350_DCDC_1, &wm8350_dcdc1_data },
	{ WM8350_DCDC_3, &wm8350_dcdc3_data },
	{ WM8350_DCDC_4, &wm8350_dcdc4_data },
	{ WM8350_DCDC_6, &my6410_vddarm },
	{ WM8350_LDO_1, &my6410_vddalive },
	{ WM8350_LDO_2, &my6410_vddotg },
	{ WM8350_LDO_3, &my6410_vddlcd },
	{ WM8350_LDO_4, &wm8350_ldo4_data },
};

static int __init my6410_wm8350_init(struct wm8350 *wm8350)
{
	int i;

	/* Configure the IRQ line */
	s3c_gpio_setpull(S3C64XX_GPN(12), S3C_GPIO_PULL_UP);

	/* Instantiate the regulators */
	for (i = 0; i < ARRAY_SIZE(wm1190_regulators); i++)
		wm8350_register_regulator(wm8350,
					  wm1190_regulators[i].regulator,
					  wm1190_regulators[i].initdata);

	return 0;
}

static struct wm8350_platform_data __initdata my6410_wm8350_pdata = {
	.init = my6410_wm8350_init,
	.irq_high = 1,
	.irq_base = IRQ_BOARD_START,
};
#endif

#ifdef CONFIG_SMDK6410_WM1192_EV1
static struct gpio_led wm1192_pmic_leds[] = {
	{
		.name = "PMIC:red:power",
		.gpio = GPIO_BOARD_START + 3,
		.default_state = LEDS_GPIO_DEFSTATE_ON,
	},
};

static struct gpio_led_platform_data wm1192_pmic_led = {
	.num_leds = ARRAY_SIZE(wm1192_pmic_leds),
	.leds = wm1192_pmic_leds,
};

static struct platform_device wm1192_pmic_led_dev = {
	.name          = "leds-gpio",
	.id            = -1,
	.dev = {
		.platform_data = &wm1192_pmic_led,
	},
};

static int wm1192_pre_init(struct wm831x *wm831x)
{
	int ret;

	/* Configure the IRQ line */
	s3c_gpio_setpull(S3C64XX_GPN(12), S3C_GPIO_PULL_UP);

	ret = platform_device_register(&wm1192_pmic_led_dev);
	if (ret != 0)
		dev_err(wm831x->dev, "Failed to add PMIC LED: %d\n", ret);

	return 0;
}

static struct wm831x_backlight_pdata wm1192_backlight_pdata = {
	.isink = 1,
	.max_uA = 27554,
};

static struct regulator_init_data wm1192_dcdc3 = {
	.constraints = {
		.name = "PVDD_MEM+PVDD_GPS",
		.always_on = 1,
	},
};

static struct regulator_consumer_supply wm1192_ldo1_consumers[] = {
	REGULATOR_SUPPLY("DVDD", "0-001b"),   /* WM8580 */
};

static struct regulator_init_data wm1192_ldo1 = {
	.constraints = {
		.name = "PVDD_LCD+PVDD_EXT",
		.always_on = 1,
	},
	.consumer_supplies = wm1192_ldo1_consumers,
	.num_consumer_supplies = ARRAY_SIZE(wm1192_ldo1_consumers),
};

static struct wm831x_status_pdata wm1192_led7_pdata = {
	.name = "LED7:green:",
};

static struct wm831x_status_pdata wm1192_led8_pdata = {
	.name = "LED8:green:",
};

static struct wm831x_pdata my6410_wm1192_pdata = {
	.pre_init = wm1192_pre_init,
	.irq_base = IRQ_BOARD_START,

	.backlight = &wm1192_backlight_pdata,
	.dcdc = {
		&my6410_vddarm,  /* DCDC1 */
		&my6410_vddint,  /* DCDC2 */
		&wm1192_dcdc3,
	},
	.gpio_base = GPIO_BOARD_START,
	.ldo = {
		 &wm1192_ldo1,        /* LDO1 */
		 &my6410_vdduh_mmc, /* LDO2 */
		 NULL,                /* LDO3 NC */
		 &my6410_vddotgi,   /* LDO4 */
		 &my6410_vddotg,    /* LDO5 */
		 &my6410_vddhi,     /* LDO6 */
		 &my6410_vddaudio,  /* LDO7 */
		 &my6410_vccm2mtv,  /* LDO8 */
		 &my6410_vddpll,    /* LDO9 */
		 &my6410_vccmc3bt,  /* LDO10 */
		 &my6410_vddalive,  /* LDO11 */
	},
	.status = {
		&wm1192_led7_pdata,
		&wm1192_led8_pdata,
	},
};
#endif

static struct i2c_board_info i2c_devs0[] __initdata = {
	{ I2C_BOARD_INFO("24c08", 0x50), },
	{ I2C_BOARD_INFO("wm8580", 0x1b), },

#ifdef CONFIG_SMDK6410_WM1192_EV1
	{ I2C_BOARD_INFO("wm8312", 0x34),
	  .platform_data = &my6410_wm1192_pdata,
	  .irq = S3C_EINT(12),
	},
#endif

#ifdef CONFIG_SMDK6410_WM1190_EV1
	{ I2C_BOARD_INFO("wm8350", 0x1a),
	  .platform_data = &my6410_wm8350_pdata,
	  .irq = S3C_EINT(12),
	},
#endif
};

static struct i2c_board_info i2c_devs1[] __initdata = {
	{ I2C_BOARD_INFO("24c128", 0x57), },	/* Samsung S524AD0XD1 */
};

/* LCD Backlight data */
static struct samsung_bl_gpio_info my6410_bl_gpio_info = {
	.no = S3C64XX_GPF(15),
	.func = S3C_GPIO_SFN(2),
};

static struct platform_pwm_backlight_data my6410_bl_data = {
	.pwm_id = 1,
};

static void __init my6410_map_io(void)
{
	u32 tmp;

	s3c64xx_init_io(my6410_iodesc, ARRAY_SIZE(my6410_iodesc));
	s3c24xx_init_clocks(12000000);
	s3c24xx_init_uarts(my6410_uartcfgs, ARRAY_SIZE(my6410_uartcfgs));

	/* set the LCD type */

	tmp = __raw_readl(S3C64XX_SPCON);
	tmp &= ~S3C64XX_SPCON_LCD_SEL_MASK;
	tmp |= S3C64XX_SPCON_LCD_SEL_RGB;
	__raw_writel(tmp, S3C64XX_SPCON);

	/* remove the lcd bypass */
	tmp = __raw_readl(S3C64XX_MODEM_MIFPCON);
	tmp &= ~MIFPCON_LCD_BYPASS;
	__raw_writel(tmp, S3C64XX_MODEM_MIFPCON);
}

static void __init my6410_machine_init(void)
{
	u32 cs1;

	s3c_i2c0_set_platdata(NULL);
	s3c_i2c1_set_platdata(NULL);
	s3c_fb_set_platdata(&my6410_lcd_pdata);

	samsung_keypad_set_platdata(&my6410_keypad_data);

	// s3c24xx_ts_set_platdata(NULL);
	// s3c_ts_set_platdata(&s3c_ts_platform);
	s3c_ts_set_platdata(&s3c_ts_platform);

	/* configure nCS1 width to 16 bits */

	cs1 = __raw_readl(S3C64XX_SROM_BW) &
		    ~(S3C64XX_SROM_BW__CS_MASK << S3C64XX_SROM_BW__NCS1__SHIFT);
	cs1 |= ((1 << S3C64XX_SROM_BW__DATAWIDTH__SHIFT) |
		(1 << S3C64XX_SROM_BW__WAITENABLE__SHIFT) |
		(1 << S3C64XX_SROM_BW__BYTEENABLE__SHIFT)) <<
						   S3C64XX_SROM_BW__NCS1__SHIFT;
	__raw_writel(cs1, S3C64XX_SROM_BW);

	/* set timing for nCS1 suitable for ethernet chip */

	__raw_writel((0 << S3C64XX_SROM_BCX__PMC__SHIFT) |
		     (6 << S3C64XX_SROM_BCX__TACP__SHIFT) |
		     (4 << S3C64XX_SROM_BCX__TCAH__SHIFT) |
		     (1 << S3C64XX_SROM_BCX__TCOH__SHIFT) |
		     (0xe << S3C64XX_SROM_BCX__TACC__SHIFT) |
		     (4 << S3C64XX_SROM_BCX__TCOS__SHIFT) |
		     (0 << S3C64XX_SROM_BCX__TACS__SHIFT), S3C64XX_SROM_BC1);

	gpio_request(S3C64XX_GPN(5), "LCD power");
	gpio_request(S3C64XX_GPF(13), "LCD power");

	i2c_register_board_info(0, i2c_devs0, ARRAY_SIZE(i2c_devs0));
	i2c_register_board_info(1, i2c_devs1, ARRAY_SIZE(i2c_devs1));

	s3c_ide_set_platdata(&my6410_ide_pdata);

	samsung_bl_set(&my6410_bl_gpio_info, &my6410_bl_data);

	platform_add_devices(my6410_devices, ARRAY_SIZE(my6410_devices));
}

MACHINE_START(MY6410, "MY6410")
	/* Maintainer: Ben Dooks <ben-linux@fluff.org> */
	.atag_offset	= 0x100,

	.init_irq	= s3c6410_init_irq,
	.handle_irq	= vic_handle_irq,
	.map_io		= my6410_map_io,
	.init_machine	= my6410_machine_init,
	.timer		= &s3c24xx_timer,
	.restart	= s3c64xx_restart,
MACHINE_END
