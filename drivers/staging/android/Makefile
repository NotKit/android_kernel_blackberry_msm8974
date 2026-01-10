ccflags-y += -I$(src)			# needed for trace events

<<<<<<< HEAD
obj-$(CONFIG_ANDROID_BINDER_IPC)	+= binder.o
=======
obj-y					+= ion/

obj-$(CONFIG_ANDROID_BINDER_IPC)	+= binder.o binder_alloc.o
obj-$(CONFIG_ANDROID_BINDER_IPC_SELFTEST) += binder_alloc_selftest.o
>>>>>>> android-3.18
obj-$(CONFIG_ASHMEM)			+= ashmem.o
obj-$(CONFIG_ANDROID_TIMED_OUTPUT)	+= timed_output.o
obj-$(CONFIG_ANDROID_TIMED_GPIO)	+= timed_gpio.o
obj-$(CONFIG_ANDROID_LOW_MEMORY_KILLER)	+= lowmemorykiller.o
<<<<<<< HEAD
obj-$(CONFIG_ANDROID_SWITCH)		+= switch/
obj-$(CONFIG_ANDROID_INTF_ALARM_DEV)	+= alarm-dev.o

CFLAGS_REMOVE_trace_persistent.o = -pg
=======
obj-$(CONFIG_SYNC)			+= sync.o sync_debug.o
obj-$(CONFIG_SW_SYNC)			+= sw_sync.o
obj-$(CONFIG_ANDROID_VSOC)		+= vsoc.o
>>>>>>> android-3.18
