#
# Makefile for the Linux kernel device drivers.
#
# 15 Sep 2000, Christoph Hellwig <hch@infradead.org>
# Rewritten to use lists instead of if-statements.
#

obj-y				+= irqchip/
obj-y				+= bus/

obj-$(CONFIG_ARM_AMBA)		+= amba/
# Many drivers currently depend on Sbus and/or PCI, sort them after the hosts
obj-$(CONFIG_SBUS)		+= sbus/
obj-$(CONFIG_PCI)		+= pci/
obj-$(CONFIG_PCCARD)		+= pcmcia/
obj-$(CONFIG_PNP)		+= pnp/
obj-$(CONFIG_ARM_CORENET)	+= bus/

# This MUST be before any drives that use it
obj-$(CONFIG_SLIMBUS)		+= slimbus/

obj-y				+= base/ block/ misc/ mfd/ nfc/
obj-$(CONFIG_DMA_SHARED_BUFFER) += dma-buf/
obj-y				+= macintosh/
obj-y				+= video/
obj-y				+= virtio/
obj-y				+= hv/

obj-$(CONFIG_PARPORT)		+= parport/
obj-y				+= char/
obj-$(CONFIG_GUD_SUPPORT)	+= gud/
obj-$(CONFIG_CORESIGHT)		+= coresight/
obj-$(CONFIG_BIF)		+= bif/
obj-$(CONFIG_SENSORS)		+= sensors/

# iommu/ after char for iommu_debugfs_init
obj-y				+= iommu/

obj-$(CONFIG_CONNECTOR)		+= connector/
obj-$(CONFIG_FB)		+= video/
obj-y				+= target/
obj-$(CONFIG_MTD)		+= mtd/
obj-$(CONFIG_SPI)		+= spi/
obj-$(CONFIG_SPMI)		+= spmi/
obj-y				+= hsi/
obj-y				+= net/
obj-$(CONFIG_ATM)		+= atm/
obj-$(CONFIG_FUSION)		+= message/
obj-$(CONFIG_FIREWIRE)		+= firewire/
obj-$(CONFIG_UIO)		+= uio/
obj-$(CONFIG_VFIO)		+= vfio/
obj-y				+= cdrom/
obj-$(CONFIG_IDE)		+= ide/
obj-$(CONFIG_SCSI)		+= scsi/
obj-$(CONFIG_ATA)		+= ata/
obj-$(CONFIG_INFINIBAND)	+= infiniband/
obj-$(CONFIG_SGI_SN)		+= sn/
obj-$(CONFIG_ADVANCE_DEBUG)	+= message/
obj-$(CONFIG_PARISC_CORE)	+= parisc/
obj-$(CONFIG_POWER_SUPPLY)	+= power/
obj-$(CONFIG_HWMON)		+= hwmon/
obj-$(CONFIG_THERMAL)		+= thermal/
obj-$(CONFIG_WATCHDOG)		+= watchdog/
obj-$(CONFIG_MD)		+= md/
obj-$(CONFIG_BT)		+= bluetooth/
obj-$(CONFIG_ISDN)		+= isdn/
obj-$(CONFIG_VME_BUS)		+= vme/
obj-$(CONFIG_IPACK_BUS)		+= ipack/
obj-$(CONFIG_MCA)		+= mca/
obj-$(CONFIG_EISA)		+= eisa/
obj-$(CONFIG_LGUEST_GUEST)	+= virtio/
obj-$(CONFIG_CPU_FREQ)		+= cpufreq/
obj-$(CONFIG_CPU_IDLE)		+= cpuidle/
obj-$(CONFIG_MMC)		+= mmc/
obj-$(CONFIG_MEMSTICK)		+= memstick/
obj-$(CONFIG_NEW_LEDS)		+= leds/
obj-$(CONFIG_INFINIBAND)	+= infiniband/
obj-$(CONFIG_SGI_IOC4)		+= sn/
obj-y				+= accessibility/
obj-$(CONFIG_ISDN)		+= isdn/
obj-$(CONFIG_EDAC)		+= edac/
obj-$(CONFIG_RTC_LIB)		+= rtc/
obj-y				+= i2c/
# obj-y				+= ppp/
obj-$(CONFIG_W1)		+= w1/

# obj-y				+= pwrseq/
obj-y				+= gpio/
obj-$(CONFIG_PCI)		+= pci/
obj-$(CONFIG_PARISC_CORE)	+= parisc/
obj-$(CONFIG_RAPIDIO)		+= rapidio/
obj-y				+= pinctrl/
obj-y				+= reset/
obj-y				+= phy/
obj-$(CONFIG_MCB)		+= mcb/
obj-$(CONFIG_RAS)		+= ras/
obj-$(CONFIG_THUNDERBOLT)	+= thunderbolt/
obj-$(CONFIG_FMC)		+= fmc/
obj-$(CONFIG_POWERCAP)		+= powercap/
obj-$(CONFIG_NTB)		+= ntb/
obj-$(CONFIG_IPACK_BUS)		+= ipack/
obj-$(CONFIG_VME_BUS)		+= vme/
obj-$(CONFIG_IIO)		+= iio/
obj-$(CONFIG_MEMORY)		+= memory/
obj-$(CONFIG_EXTCON)		+= extcon/
obj-$(CONFIG_PM_DEVFREQ)	+= devfreq/
obj-$(CONFIG_GENERIC_PHY)	+= phy/
obj-$(CONFIG_BCMA)		+= bcma/
obj-$(CONFIG_SSB)		+= ssb/
obj-$(CONFIG_SGI_SN)		+= sn/
obj-$(CONFIG_HYPERV)		+= hv/
obj-$(CONFIG_REMOTEPROC)	+= remoteproc/
obj-$(CONFIG_RPMSG)		+= rpmsg/
obj-$(CONFIG_VHOST_RING)	+= vhost/
obj-$(CONFIG_VHOST)		+= vhost/
obj-$(CONFIG_VIRTIO)		+= virtio/
obj-$(CONFIG_REGULATOR)		+= regulator/
obj-$(CONFIG_CLKDEV_LOOKUP)	+= clk/
obj-$(CONFIG_COMMON_CLK)	+= clk/
obj-$(CONFIG_HWSPINLOCK)	+= hwspinlock/
obj-$(CONFIG_IOMMU_API)		+= iommu/
obj-$(CONFIG_PWM)		+= pwm/
obj-y				+= soc/
