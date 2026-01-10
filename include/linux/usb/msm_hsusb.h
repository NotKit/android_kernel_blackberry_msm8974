/* include/linux/usb/msm_hsusb.h
 *
 * Copyright (C) 2008 Google, Inc.
 * Author: Brian Swetland <swetland@google.com>
 * Copyright (c) 2009-2014, The Linux Foundation. All rights reserved.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __ASM_ARCH_MSM_HSUSB_H
#define __ASM_ARCH_MSM_HSUSB_H

#include <linux/types.h>
#include <linux/usb/ch9.h>
#include <linux/usb/gadget.h>
#include <linux/usb/otg.h>
#include <linux/clk.h>

/**
 * OTG control
 *
 * OTG_NO_CONTROL	Id/VBUS notifications not required. Useful in host
 *                      only configuration.
 * OTG_PHY_CONTROL	Id/VBUS notifications comes form USB PHY.
 * OTG_PMIC_CONTROL	Id/VBUS notifications comes from PMIC hardware.
 * OTG_USER_CONTROL	Id/VBUS notifcations comes from User via sysfs.
 *
 */
enum otg_control_type {
	OTG_NO_CONTROL = 0,
	OTG_PHY_CONTROL,
	OTG_PMIC_CONTROL,
	OTG_USER_CONTROL,
};

/**
 * PHY used in
 *
 * INVALID_PHY			Unsupported PHY
 * CI_45NM_INTEGRATED_PHY	Chipidea 45nm integrated PHY
 * SNPS_28NM_INTEGRATED_PHY	Synopsis 28nm integrated PHY
 *
 */
enum msm_usb_phy_type {
	INVALID_PHY = 0,
	CI_45NM_INTEGRATED_PHY,
	SNPS_28NM_INTEGRATED_PHY,
};

#define IDEV_CHG_MAX	1500
#define IDEV_CHG_MIN	500
#define IUNIT		100

#define IDEV_ACA_CHG_MAX	1500
#define IDEV_ACA_CHG_LIMIT	500

/**
 * Different states involved in USB charger detection.
 *
 * USB_CHG_STATE_UNDEFINED	USB charger is not connected or detection
 *                              process is not yet started.
 * USB_CHG_STATE_WAIT_FOR_DCD	Waiting for Data pins contact.
 * USB_CHG_STATE_DCD_DONE	Data pin contact is detected.
 * USB_CHG_STATE_PRIMARY_DONE	Primary detection is completed (Detects
 *                              between SDP and DCP/CDP).
 * USB_CHG_STATE_SECONDARY_DONE	Secondary detection is completed (Detects
 *                              between DCP and CDP).
 * USB_CHG_STATE_DETECTED	USB charger type is determined.
 *
 */
enum usb_chg_state {
	USB_CHG_STATE_UNDEFINED = 0,
	USB_CHG_STATE_WAIT_FOR_DCD,
	USB_CHG_STATE_DCD_DONE,
	USB_CHG_STATE_PRIMARY_DONE,
	USB_CHG_STATE_SECONDARY_DONE,
	USB_CHG_STATE_DETECTED,
};

/**
 * USB charger types
 *
 * USB_INVALID_CHARGER	Invalid USB charger.
 * USB_SDP_CHARGER	Standard downstream port. Refers to a downstream port
 *                      on USB2.0 compliant host/hub.
 * USB_DCP_CHARGER	Dedicated charger port (AC charger/ Wall charger).
 * USB_CDP_CHARGER	Charging downstream port. Enumeration can happen and
 *                      IDEV_CHG_MAX can be drawn irrespective of USB state.
 * USB_ACA_A_CHARGER	B-device is connected on accessory port with charger
 *                      connected on charging port. This configuration allows
 *                      charging in host mode.
 * USB_ACA_B_CHARGER	No device (or A-device without VBUS) is connected on
 *                      accessory port with charger connected on charging port.
 * USB_ACA_C_CHARGER	A-device (with VBUS) is connected on
 *                      accessory port with charger connected on charging port.
 * USB_ACA_DOCK_CHARGER	A docking station that has one upstream port and one
 *			or more downstream ports. Capable of supplying
 *			IDEV_CHG_MAX irrespective of devices connected on
 *			accessory ports.
 * USB_PROPRIETARY_CHARGER A proprietary charger pull DP and DM to specific
 *			voltages between 2.0-3.3v for identification.
 *
 */
enum usb_chg_type {
	USB_INVALID_CHARGER = 0,
	USB_SDP_CHARGER,
	USB_DCP_CHARGER,
	USB_CDP_CHARGER,
	USB_ACA_A_CHARGER,
	USB_ACA_B_CHARGER,
	USB_ACA_C_CHARGER,
	USB_ACA_DOCK_CHARGER,
	USB_PROPRIETARY_CHARGER,
	USB_FLOATED_CHARGER,
};

/**
 * Used different VDDCX voltage voting mechnism
 * VDDCX_CORNER       Vote for VDDCX Corner voltage
 * VDDCX              Vote for VDDCX Absolute voltage
 */
enum usb_vdd_type {
	VDDCX_CORNER = 0,
	VDDCX,
	VDD_TYPE_MAX,
};

/**
 * Used different VDDCX voltage values
 */
enum usb_vdd_value {
	VDD_NONE = 0,
	VDD_MIN,
	VDD_MAX,
	VDD_VAL_MAX,
};

/**
 * Maintain state for hvdcp external charger status
 * DEFAULT	This is used when DCP is detected
 * ACTIVE	This is used when ioctl is called to block LPM
 * INACTIVE	This is used when ioctl is called to unblock LPM
 */

enum usb_ext_chg_status {
	DEFAULT = 1,
	ACTIVE,
	INACTIVE,
};


/**
 * struct msm_otg_platform_data - platform device data
 *              for msm_otg driver.
 * @phy_init_seq: PHY configuration sequence values. Value of -1 is reserved as
 *              "do not overwrite default vaule at this address".
 * @phy_init_sz: PHY configuration sequence size.
 * @vbus_power: VBUS power on/off routine.
 * @power_budget: VBUS power budget in mA (0 will be treated as 500mA).
 * @mode: Supported mode (OTG/peripheral/host).
 * @otg_control: OTG switch controlled by user/Id pin
 */
struct msm_otg_platform_data {
	int *phy_init_seq;
	int phy_init_sz;
	void (*vbus_power)(bool on);
	unsigned power_budget;
	enum usb_dr_mode mode;
	enum otg_control_type otg_control;
	enum msm_usb_phy_type phy_type;
	void (*setup_gpio)(enum usb_otg_state state);
	int (*link_clk_reset)(struct clk *link_clk, bool assert);
	int (*phy_clk_reset)(struct clk *phy_clk);
};

/* phy related flags */
#define ENABLE_DP_MANUAL_PULLUP		BIT(0)
#define ENABLE_SECONDARY_PHY		BIT(1)

/* Timeout (in msec) values (min - max) associated with OTG timers */

#define TA_WAIT_VRISE	100	/* ( - 100)  */
#define TA_WAIT_VFALL	500	/* ( - 1000) */

/*
 * This option is set for embedded hosts or OTG devices in which leakage
 * currents are very minimal.
 */
#ifdef CONFIG_USB_OTG
#define TA_WAIT_BCON	30000	/* (1100 - 30000) */
#else
#define TA_WAIT_BCON	-1
#endif

#define TA_AIDL_BDIS	500	/* (200 - ) */
#define TA_BIDL_ADIS	155	/* (155 - 200) */
#define TB_SRP_FAIL	6000	/* (5000 - 6000) */
#define TB_ASE0_BRST	200	/* (155 - ) */

/* TB_SSEND_SRP and TB_SE0_SRP are combined */
#define TB_SRP_INIT	2000	/* (1500 - ) */

#define TA_TST_MAINT	10100	/* (9900 - 10100) */
#define TB_TST_SRP	3000	/* ( - 5000) */
#define TB_TST_CONFIG	300

/* Timeout variables */

#define A_WAIT_VRISE	0
#define A_WAIT_VFALL	1
#define A_WAIT_BCON	2
#define A_AIDL_BDIS	3
#define A_BIDL_ADIS	4
#define B_SRP_FAIL	5
#define B_ASE0_BRST	6
#define A_TST_MAINT	7
#define B_TST_SRP	8
#define B_TST_CONFIG	9

/**
 * struct msm_otg: OTG driver data. Shared by HCD and DCD.
 * @otg: USB OTG Transceiver structure.
 * @pdata: otg device platform data.
 * @irq: IRQ number assigned for HSUSB controller.
 * @clk: clock struct of usb_hs_clk.
 * @pclk: clock struct of usb_hs_pclk.
 * @phy_reset_clk: clock struct of usb_phy_clk.
 * @core_clk: clock struct of usb_hs_core_clk.
 * @regs: ioremapped register base address.
 * @usb_phy_ctrl_reg: relevant PHY_CTRL_REG register base address.
 * @inputs: OTG state machine inputs(Id, SessValid etc).
 * @sm_work: OTG state machine work.
 * @pm_suspended: OTG device is system(PM) suspended.
 * @pm_notify: Notifier to receive system wide PM transition events.
		It is used to defer wakeup events processing until
		system is RESUMED.
 * @in_lpm: indicates low power mode (LPM) state.
 * @async_int: IRQ line on which ASYNC interrupt arrived in LPM.
 * @cur_power: The amount of mA available from downstream port.
 * @chg_work: Charger detection work.
 * @chg_state: The state of charger detection process.
 * @chg_type: The type of charger attached.
 * @dcd_retires: The retry count used to track Data contact
 *               detection process.
 * @wlock: Wake lock struct to prevent system suspend when
 *               USB is active.
 * @usbdev_nb: The notifier block used to know about the B-device
 *             connected. Useful only when ACA_A charger is
 *             connected.
 * @mA_port: The amount of current drawn by the attached B-device.
 * @id_timer: The timer used for polling ID line to detect ACA states.
 * @xo_handle: TCXO buffer handle
 * @bus_perf_client: Bus performance client handle to request BUS bandwidth
 * @mhl_enabled: MHL driver registration successful and MHL enabled.
 * @host_bus_suspend: indicates host bus suspend or not.
 * @chg_check_timer: The timer used to implement the workaround to detect
 *               very slow plug in of wall charger.
 * @ui_enabled: USB Intterupt is enabled or disabled.
 * @pm_done: It is used to increment the pm counter using pm_runtime_get_sync.
	     This handles the race case when PM resume thread returns before
	     the charger detection starts. When USB is disconnected pm_done
	     is set to true.
 */
struct msm_otg {
	struct usb_phy phy;
	struct msm_otg_platform_data *pdata;
	int irq;
	int async_irq;
	struct clk *xo_clk;
	struct clk *clk;
	struct clk *pclk;
	struct clk *phy_reset_clk;
	struct clk *core_clk;
	struct clk *sleep_clk;
	long core_clk_rate;
	struct resource *io_res;
	void __iomem *regs;
	void __iomem *usb_phy_ctrl_reg;
#define ID		0
#define B_SESS_VLD	1
#define ID_A		2
#define ID_B		3
#define ID_C		4
#define A_BUS_DROP	5
#define A_BUS_REQ	6
#define A_SRP_DET	7
#define A_VBUS_VLD	8
#define B_CONN		9
#define ADP_CHANGE	10
#define POWER_UP	11
#define A_CLR_ERR	12
#define A_BUS_RESUME	13
#define A_BUS_SUSPEND	14
#define A_CONN		15
#define B_BUS_REQ	16
#define MHL	        17
#define B_FALSE_SDP	18
	unsigned long inputs;
	struct work_struct sm_work;
	bool sm_work_pending;
	atomic_t pm_suspended;
	struct notifier_block pm_notify;
	atomic_t in_lpm;
	int async_int;
	unsigned cur_power;
	int phy_number;
	struct delayed_work chg_work;
	struct delayed_work pmic_id_status_work;
	struct delayed_work suspend_work;
	enum usb_chg_state chg_state;
	enum usb_chg_type chg_type;
	u8 dcd_retries;
	struct regulator *v3p3;
	struct regulator *v1p8;
	struct regulator *vddcx;

	struct reset_control *phy_rst;
	struct reset_control *link_rst;
	int vdd_levels[3];
};

/**
 * struct msm_hsic_host_platform_data - platform device data
 *              for msm_hsic_host driver.
 * @phy_sof_workaround: Enable ALL PHY SOF bug related workarounds for
		SUSPEND, RESET and RESUME.
 * @phy_susp_sof_workaround: Enable PHY SOF workaround only for SUSPEND.
 *
 */
struct msm_hsic_host_platform_data {
	unsigned strobe;
	unsigned data;
	bool ignore_cal_pad_config;
	bool phy_sof_workaround;
	bool phy_susp_sof_workaround;
	u32 reset_delay;
	int strobe_pad_offset;
	int data_pad_offset;

	struct msm_bus_scale_pdata *bus_scale_table;
	unsigned log2_irq_thresh;

	/* gpio used to resume peripheral */
	unsigned resume_gpio;

	/*swfi latency is required while driving resume on to the bus */
	u32 swfi_latency;

	/*standalone latency is required when HSCI is active*/
	u32 standalone_latency;
	bool pool_64_bit_align;
	bool enable_hbm;
	bool disable_park_mode;
	bool consider_ipa_handshake;
	bool ahb_async_bridge_bypass;
	bool disable_cerr;
};

struct msm_usb_host_platform_data {
	unsigned int power_budget;
	int pmic_gpio_dp_irq;
	unsigned int dock_connect_irq;
	bool use_sec_phy;
	bool no_selective_suspend;
	int resume_gpio;
	bool is_uicc;
};

/**
 * struct msm_hsic_peripheral_platform_data: HSIC peripheral
 * platform data.
 * @core_clk_always_on_workaround: Don't disable core_clk when
 *                                 HSIC enters LPM.
 */
struct msm_hsic_peripheral_platform_data {
	bool core_clk_always_on_workaround;
};

/**
 * struct usb_ext_notification: event notification structure
 * @notify: pointer to client function to call when ID event is detected.
 *          The function parameter is provided by driver to be called back when
 *          external client indicates it is done using the USB. This function
 *          should return 0 if handled successfully, otherise an error code.
 * @ctxt: client-specific context pointer
 *
 * This structure should be used by clients wishing to register (via
 * msm_register_usb_ext_notification) for event notification whenever a USB
 * cable is plugged in and ID pin status changes. Clients must provide a
 * callback function pointer. If this callback returns 0, the USB driver will
 * assume the client is "taking over" the connection, and will relinquish any
 * further processing until its callback (passed via the third parameter) is
 * called with the online parameter set to false.
 */
struct usb_ext_notification {
	int (*notify)(void *, int, void (*)(void *, int online), void *);
	void *ctxt;
};
#ifdef CONFIG_USB_BAM
bool msm_bam_lpm_ok(void);
void msm_bam_notify_lpm_resume(void);
void msm_bam_set_hsic_host_dev(struct device *dev);
void msm_bam_wait_for_hsic_prod_granted(void);
bool msm_bam_hsic_lpm_ok(void);
void msm_bam_hsic_notify_on_resume(void);
#else
static inline bool msm_bam_lpm_ok(void) { return true; }
static inline void msm_bam_notify_lpm_resume(void) {}
static inline void msm_bam_set_hsic_host_dev(struct device *dev) {}
static inline void msm_bam_wait_for_hsic_prod_granted(void) {}
static inline bool msm_bam_hsic_lpm_ok(void) { return true; }
static inline void msm_bam_hsic_notify_on_resume(void) {}
#endif
#ifdef CONFIG_USB_CI13XXX_MSM
void msm_hw_bam_disable(bool bam_disable);
#else
static inline void msm_hw_bam_disable(bool bam_disable)
{
}
#endif

#ifdef CONFIG_USB_DWC3_MSM
int msm_ep_config(struct usb_ep *ep);
int msm_ep_unconfig(struct usb_ep *ep);
void dwc3_tx_fifo_resize_request(struct usb_ep *ep, bool qdss_enable);
int msm_data_fifo_config(struct usb_ep *ep, u32 addr, u32 size,
	u8 dst_pipe_idx);

void msm_dwc3_restart_usb_session(struct usb_gadget *gadget);

int msm_register_usb_ext_notification(struct usb_ext_notification *info);
#else
static inline int msm_data_fifo_config(struct usb_ep *ep, u32 addr, u32 size,
	u8 dst_pipe_idx)
{
	return -ENODEV;
}

static inline int msm_ep_config(struct usb_ep *ep)
{
	return -ENODEV;
}

static inline int msm_ep_unconfig(struct usb_ep *ep)
{
	return -ENODEV;
}

static inline void dwc3_tx_fifo_resize_request(
					struct usb_ep *ep, bool qdss_enable)
{
	return;
}

static inline void msm_dwc3_restart_usb_session(struct usb_gadget *gadget)
{
	return;
}

static inline int msm_register_usb_ext_notification(
					struct usb_ext_notification *info)
{
	return -ENODEV;
}
#endif
#endif
