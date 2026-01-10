/*
 * comedi/drivers/ni_labpc.c
 * Driver for National Instruments Lab-PC series boards and compatibles
 * Copyright (C) 2001-2003 Frank Mori Hess <fmhess@users.sourceforge.net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

/*
 * Driver: ni_labpc
 * Description: National Instruments Lab-PC (& compatibles)
 * Devices: (National Instruments) Lab-PC-1200 [lab-pc-1200]
 *	    (National Instruments) Lab-PC-1200AI [lab-pc-1200ai]
 *	    (National Instruments) Lab-PC+ [lab-pc+]
 * Author: Frank Mori Hess <fmhess@users.sourceforge.net>
 * Status: works
 *
 * Configuration options - ISA boards:
 *   [0] - I/O port base address
 *   [1] - IRQ (optional, required for timed or externally triggered
 *		conversions)
 *   [2] - DMA channel (optional)
 *
 * Tested with lab-pc-1200.  For the older Lab-PC+, not all input
 * ranges and analog references will work, the available ranges/arefs
 * will depend on how you have configured the jumpers on your board
 * (see your owner's manual).
 *
 * Kernel-level ISA plug-and-play support for the lab-pc-1200 boards
 * has not yet been added to the driver, mainly due to the fact that
 * I don't know the device id numbers. If you have one of these boards,
 * please file a bug report at http://comedi.org/ so I can get the
 * necessary information from you.
 *
 * The 1200 series boards have onboard calibration dacs for correcting
 * analog input/output offsets and gains. The proper settings for these
 * caldacs are stored on the board's eeprom. To read the caldac values
 * from the eeprom and store them into a file that can be then be used
 * by comedilib, use the comedi_calibrate program.
 *
 * The Lab-pc+ has quirky chanlist requirements when scanning multiple
 * channels. Multiple channel scan sequence must start at highest channel,
 * then decrement down to channel 0. The rest of the cards can scan down
 * like lab-pc+ or scan up from channel zero. Chanlists consisting of all
 * one channel are also legal, and allow you to pace conversions in bursts.
 *
 * NI manuals:
 * 341309a (labpc-1200 register manual)
 * 320502b (lab-pc+)
 */

#include <linux/module.h>

#include "../comedidev.h"

#include "ni_labpc.h"
#include "ni_labpc_isadma.h"

static const struct labpc_boardinfo labpc_boards[] = {
	{
		.name			= "lab-pc-1200",
		.ai_speed		= 10000,
		.ai_scan_up		= 1,
		.has_ao			= 1,
		.is_labpc1200		= 1,
	}, {
		.name			= "lab-pc-1200ai",
		.ai_speed		= 10000,
		.ai_scan_up		= 1,
		.is_labpc1200		= 1,
	}, {
		.name			= "lab-pc+",
		.ai_speed		= 12000,
		.has_ao			= 1,
	},
};

static int labpc_attach(struct comedi_device *dev, struct comedi_devconfig *it)
{
<<<<<<< HEAD
	unsigned long iobase = 0;
	unsigned int irq = 0;
	unsigned int dma_chan = 0;
#ifdef CONFIG_COMEDI_PCI_DRIVERS
	int retval;
#endif

	/* allocate and initialize dev->private */
	if (alloc_private(dev, sizeof(struct labpc_private)) < 0)
		return -ENOMEM;

	/* get base address, irq etc. based on bustype */
	switch (thisboard->bustype) {
	case isa_bustype:
#ifdef CONFIG_ISA_DMA_API
		iobase = it->options[0];
		irq = it->options[1];
		dma_chan = it->options[2];
#else
		printk(KERN_ERR " this driver has not been built with ISA DMA "
								"support.\n");
		return -EINVAL;
#endif
		break;
	case pci_bustype:
#ifdef CONFIG_COMEDI_PCI_DRIVERS
		retval = labpc_find_device(dev, it->options[0], it->options[1]);
		if (retval < 0)
			return retval;
		retval = mite_setup(devpriv->mite);
		if (retval < 0)
			return retval;
		iobase = (unsigned long)devpriv->mite->daq_io_addr;
		irq = mite_irq(devpriv->mite);
#else
		printk(KERN_ERR " this driver has not been built with PCI "
								"support.\n");
		return -EINVAL;
#endif
		break;
	case pcmcia_bustype:
		printk
		    (" this driver does not support pcmcia cards, use ni_labpc_cs.o\n");
		return -EINVAL;
		break;
	default:
		printk(KERN_ERR "bug! couldn't determine board type\n");
		return -EINVAL;
		break;
	}

	return labpc_common_attach(dev, iobase, irq, dma_chan);
}

/* adapted from ni_pcimio for finding mite based boards (pc-1200) */
#ifdef CONFIG_COMEDI_PCI_DRIVERS
static int labpc_find_device(struct comedi_device *dev, int bus, int slot)
{
	struct mite_struct *mite;
	int i;
	for (mite = mite_devices; mite; mite = mite->next) {
		if (mite->used)
			continue;
/* if bus/slot are specified then make sure we have the right bus/slot */
		if (bus || slot) {
			if (bus != mite->pcidev->bus->number
			    || slot != PCI_SLOT(mite->pcidev->devfn))
				continue;
		}
		for (i = 0; i < driver_labpc.num_names; i++) {
			if (labpc_boards[i].bustype != pci_bustype)
				continue;
			if (mite_device_id(mite) == labpc_boards[i].device_id) {
				devpriv->mite = mite;
/* fixup board pointer, in case we were using the dummy "ni_labpc" entry */
				dev->board_ptr = &labpc_boards[i];
				return 0;
			}
		}
	}
	printk(KERN_ERR "no device found\n");
	mite_list_devices();
	return -EIO;
}
#endif

int labpc_common_detach(struct comedi_device *dev)
{
	printk(KERN_ERR "comedi%d: ni_labpc: detach\n", dev->minor);

	if (dev->subdevices)
		subdev_8255_cleanup(dev, dev->subdevices + 2);

#ifdef CONFIG_ISA_DMA_API
	/* only free stuff if it has been allocated by _attach */
	kfree(devpriv->dma_buffer);
	if (devpriv->dma_chan)
		free_dma(devpriv->dma_chan);
#endif
	if (dev->irq)
		free_irq(dev->irq, dev);
	if (thisboard->bustype == isa_bustype && dev->iobase)
		release_region(dev->iobase, LABPC_SIZE);
#ifdef CONFIG_COMEDI_PCI_DRIVERS
	if (devpriv->mite)
		mite_unsetup(devpriv->mite);
#endif

	return 0;
};
EXPORT_SYMBOL_GPL(labpc_common_detach);

static void labpc_clear_adc_fifo(const struct comedi_device *dev)
{
	devpriv->write_byte(0x1, dev->iobase + ADC_CLEAR_REG);
	devpriv->read_byte(dev->iobase + ADC_FIFO_REG);
	devpriv->read_byte(dev->iobase + ADC_FIFO_REG);
}

static int labpc_cancel(struct comedi_device *dev, struct comedi_subdevice *s)
{
	unsigned long flags;

	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->command2_bits &= ~SWTRIG_BIT & ~HWTRIG_BIT & ~PRETRIG_BIT;
	devpriv->write_byte(devpriv->command2_bits, dev->iobase + COMMAND2_REG);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	devpriv->command3_bits = 0;
	devpriv->write_byte(devpriv->command3_bits, dev->iobase + COMMAND3_REG);

	return 0;
}

static enum scan_mode labpc_ai_scan_mode(const struct comedi_cmd *cmd)
{
	if (cmd->chanlist_len == 1)
		return MODE_SINGLE_CHAN;

	/* chanlist may be NULL during cmdtest. */
	if (cmd->chanlist == NULL)
		return MODE_MULT_CHAN_UP;

	if (CR_CHAN(cmd->chanlist[0]) == CR_CHAN(cmd->chanlist[1]))
		return MODE_SINGLE_CHAN_INTERVAL;

	if (CR_CHAN(cmd->chanlist[0]) < CR_CHAN(cmd->chanlist[1]))
		return MODE_MULT_CHAN_UP;

	if (CR_CHAN(cmd->chanlist[0]) > CR_CHAN(cmd->chanlist[1]))
		return MODE_MULT_CHAN_DOWN;

	printk(KERN_ERR "ni_labpc: bug! this should never happen\n");

	return 0;
}

static int labpc_ai_chanlist_invalid(const struct comedi_device *dev,
				     const struct comedi_cmd *cmd)
{
	int mode, channel, range, aref, i;

	if (cmd->chanlist == NULL)
		return 0;

	mode = labpc_ai_scan_mode(cmd);

	if (mode == MODE_SINGLE_CHAN)
		return 0;

	if (mode == MODE_SINGLE_CHAN_INTERVAL) {
		if (cmd->chanlist_len > 0xff) {
			comedi_error(dev,
				     "ni_labpc: chanlist too long for single channel interval mode\n");
			return 1;
		}
	}

	channel = CR_CHAN(cmd->chanlist[0]);
	range = CR_RANGE(cmd->chanlist[0]);
	aref = CR_AREF(cmd->chanlist[0]);

	for (i = 0; i < cmd->chanlist_len; i++) {

		switch (mode) {
		case MODE_SINGLE_CHAN_INTERVAL:
			if (CR_CHAN(cmd->chanlist[i]) != channel) {
				comedi_error(dev,
					     "channel scanning order specified in chanlist is not supported by hardware.\n");
				return 1;
			}
			break;
		case MODE_MULT_CHAN_UP:
			if (CR_CHAN(cmd->chanlist[i]) != i) {
				comedi_error(dev,
					     "channel scanning order specified in chanlist is not supported by hardware.\n");
				return 1;
			}
			break;
		case MODE_MULT_CHAN_DOWN:
			if (CR_CHAN(cmd->chanlist[i]) !=
			    cmd->chanlist_len - i - 1) {
				comedi_error(dev,
					     "channel scanning order specified in chanlist is not supported by hardware.\n");
				return 1;
			}
			break;
		default:
			printk(KERN_ERR "ni_labpc: bug! in chanlist check\n");
			return 1;
			break;
		}

		if (CR_RANGE(cmd->chanlist[i]) != range) {
			comedi_error(dev,
				     "entries in chanlist must all have the same range\n");
			return 1;
		}

		if (CR_AREF(cmd->chanlist[i]) != aref) {
			comedi_error(dev,
				     "entries in chanlist must all have the same reference\n");
			return 1;
		}
	}

	return 0;
}

static int labpc_use_continuous_mode(const struct comedi_cmd *cmd)
{
	if (labpc_ai_scan_mode(cmd) == MODE_SINGLE_CHAN)
		return 1;

	if (cmd->scan_begin_src == TRIG_FOLLOW)
		return 1;

	return 0;
}

static unsigned int labpc_ai_convert_period(const struct comedi_cmd *cmd)
{
	if (cmd->convert_src != TRIG_TIMER)
		return 0;

	if (labpc_ai_scan_mode(cmd) == MODE_SINGLE_CHAN &&
	    cmd->scan_begin_src == TRIG_TIMER)
		return cmd->scan_begin_arg;

	return cmd->convert_arg;
}

static void labpc_set_ai_convert_period(struct comedi_cmd *cmd, unsigned int ns)
{
	if (cmd->convert_src != TRIG_TIMER)
		return;

	if (labpc_ai_scan_mode(cmd) == MODE_SINGLE_CHAN &&
	    cmd->scan_begin_src == TRIG_TIMER) {
		cmd->scan_begin_arg = ns;
		if (cmd->convert_arg > cmd->scan_begin_arg)
			cmd->convert_arg = cmd->scan_begin_arg;
	} else
		cmd->convert_arg = ns;
}

static unsigned int labpc_ai_scan_period(const struct comedi_cmd *cmd)
{
	if (cmd->scan_begin_src != TRIG_TIMER)
		return 0;

	if (labpc_ai_scan_mode(cmd) == MODE_SINGLE_CHAN &&
	    cmd->convert_src == TRIG_TIMER)
		return 0;

	return cmd->scan_begin_arg;
}

static void labpc_set_ai_scan_period(struct comedi_cmd *cmd, unsigned int ns)
{
	if (cmd->scan_begin_src != TRIG_TIMER)
		return;

	if (labpc_ai_scan_mode(cmd) == MODE_SINGLE_CHAN &&
	    cmd->convert_src == TRIG_TIMER)
		return;

	cmd->scan_begin_arg = ns;
}

static int labpc_ai_cmdtest(struct comedi_device *dev,
			    struct comedi_subdevice *s, struct comedi_cmd *cmd)
{
	int err = 0;
	int tmp, tmp2;
	int stop_mask;

	/* step 1: make sure trigger sources are trivially valid */

	tmp = cmd->start_src;
	cmd->start_src &= TRIG_NOW | TRIG_EXT;
	if (!cmd->start_src || tmp != cmd->start_src)
		err++;

	tmp = cmd->scan_begin_src;
	cmd->scan_begin_src &= TRIG_TIMER | TRIG_FOLLOW | TRIG_EXT;
	if (!cmd->scan_begin_src || tmp != cmd->scan_begin_src)
		err++;

	tmp = cmd->convert_src;
	cmd->convert_src &= TRIG_TIMER | TRIG_EXT;
	if (!cmd->convert_src || tmp != cmd->convert_src)
		err++;

	tmp = cmd->scan_end_src;
	cmd->scan_end_src &= TRIG_COUNT;
	if (!cmd->scan_end_src || tmp != cmd->scan_end_src)
		err++;

	tmp = cmd->stop_src;
	stop_mask = TRIG_COUNT | TRIG_NONE;
	if (thisboard->register_layout == labpc_1200_layout)
		stop_mask |= TRIG_EXT;
	cmd->stop_src &= stop_mask;
	if (!cmd->stop_src || tmp != cmd->stop_src)
		err++;

	if (err)
		return 1;

	/* step 2: make sure trigger sources are unique and mutually compatible */

	if (cmd->start_src != TRIG_NOW && cmd->start_src != TRIG_EXT)
		err++;
	if (cmd->scan_begin_src != TRIG_TIMER &&
	    cmd->scan_begin_src != TRIG_FOLLOW &&
	    cmd->scan_begin_src != TRIG_EXT)
		err++;
	if (cmd->convert_src != TRIG_TIMER && cmd->convert_src != TRIG_EXT)
		err++;
	if (cmd->stop_src != TRIG_COUNT &&
	    cmd->stop_src != TRIG_EXT && cmd->stop_src != TRIG_NONE)
		err++;

	/* can't have external stop and start triggers at once */
	if (cmd->start_src == TRIG_EXT && cmd->stop_src == TRIG_EXT)
		err++;

	if (err)
		return 2;

	/* step 3: make sure arguments are trivially compatible */

	if (cmd->start_arg == TRIG_NOW && cmd->start_arg != 0) {
		cmd->start_arg = 0;
		err++;
	}

	if (!cmd->chanlist_len)
		err++;

	if (cmd->scan_end_arg != cmd->chanlist_len) {
		cmd->scan_end_arg = cmd->chanlist_len;
		err++;
	}

	if (cmd->convert_src == TRIG_TIMER) {
		if (cmd->convert_arg < thisboard->ai_speed) {
			cmd->convert_arg = thisboard->ai_speed;
			err++;
		}
	}
	/* make sure scan timing is not too fast */
	if (cmd->scan_begin_src == TRIG_TIMER) {
		if (cmd->convert_src == TRIG_TIMER &&
		    cmd->scan_begin_arg <
		    cmd->convert_arg * cmd->chanlist_len) {
			cmd->scan_begin_arg =
			    cmd->convert_arg * cmd->chanlist_len;
			err++;
		}
		if (cmd->scan_begin_arg <
		    thisboard->ai_speed * cmd->chanlist_len) {
			cmd->scan_begin_arg =
			    thisboard->ai_speed * cmd->chanlist_len;
			err++;
		}
	}
	/* stop source */
	switch (cmd->stop_src) {
	case TRIG_COUNT:
		if (!cmd->stop_arg) {
			cmd->stop_arg = 1;
			err++;
		}
		break;
	case TRIG_NONE:
		if (cmd->stop_arg != 0) {
			cmd->stop_arg = 0;
			err++;
		}
		break;
		/*
		 * TRIG_EXT doesn't care since it doesn't
		 * trigger off a numbered channel
		 */
	default:
		break;
	}

	if (err)
		return 3;

	/* step 4: fix up any arguments */

	tmp = cmd->convert_arg;
	tmp2 = cmd->scan_begin_arg;
	labpc_adc_timing(dev, cmd);
	if (tmp != cmd->convert_arg || tmp2 != cmd->scan_begin_arg)
		err++;

	if (err)
		return 4;

	if (labpc_ai_chanlist_invalid(dev, cmd))
		return 5;

	return 0;
}

static int labpc_ai_cmd(struct comedi_device *dev, struct comedi_subdevice *s)
{
	int channel, range, aref;
#ifdef CONFIG_ISA_DMA_API
	unsigned long irq_flags;
#endif
	int ret;
	struct comedi_async *async = s->async;
	struct comedi_cmd *cmd = &async->cmd;
	enum transfer_type xfer;
	unsigned long flags;

	if (!dev->irq) {
		comedi_error(dev, "no irq assigned, cannot perform command");
		return -1;
	}

	range = CR_RANGE(cmd->chanlist[0]);
	aref = CR_AREF(cmd->chanlist[0]);

	/* make sure board is disabled before setting up acquisition */
	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->command2_bits &= ~SWTRIG_BIT & ~HWTRIG_BIT & ~PRETRIG_BIT;
	devpriv->write_byte(devpriv->command2_bits, dev->iobase + COMMAND2_REG);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	devpriv->command3_bits = 0;
	devpriv->write_byte(devpriv->command3_bits, dev->iobase + COMMAND3_REG);

	/*  initialize software conversion count */
	if (cmd->stop_src == TRIG_COUNT)
		devpriv->count = cmd->stop_arg * cmd->chanlist_len;

	/*  setup hardware conversion counter */
	if (cmd->stop_src == TRIG_EXT) {
		/*
		 * load counter a1 with count of 3
		 * (pc+ manual says this is minimum allowed) using mode 0
		 */
		ret = labpc_counter_load(dev, dev->iobase + COUNTER_A_BASE_REG,
					 1, 3, 0);
		if (ret < 0) {
			comedi_error(dev, "error loading counter a1");
			return -1;
		}
	} else			/*
				 * otherwise, just put a1 in mode 0
				 * with no count to set its output low
				 */
		devpriv->write_byte(INIT_A1_BITS,
				    dev->iobase + COUNTER_A_CONTROL_REG);

#ifdef CONFIG_ISA_DMA_API
	/*  figure out what method we will use to transfer data */
	if (devpriv->dma_chan &&	/*  need a dma channel allocated */
		/*
		 * dma unsafe at RT priority,
		 * and too much setup time for TRIG_WAKE_EOS for
		 */
	    (cmd->flags & (TRIG_WAKE_EOS | TRIG_RT)) == 0 &&
	    /*  only available on the isa boards */
	    thisboard->bustype == isa_bustype) {
		xfer = isa_dma_transfer;
		/* pc-plus has no fifo-half full interrupt */
	} else
#endif
	if (thisboard->register_layout == labpc_1200_layout &&
		   /*  wake-end-of-scan should interrupt on fifo not empty */
		   (cmd->flags & TRIG_WAKE_EOS) == 0 &&
		   /*  make sure we are taking more than just a few points */
		   (cmd->stop_src != TRIG_COUNT || devpriv->count > 256)) {
		xfer = fifo_half_full_transfer;
	} else
		xfer = fifo_not_empty_transfer;
	devpriv->current_transfer = xfer;

	/*  setup command6 register for 1200 boards */
	if (thisboard->register_layout == labpc_1200_layout) {
		/*  reference inputs to ground or common? */
		if (aref != AREF_GROUND)
			devpriv->command6_bits |= ADC_COMMON_BIT;
		else
			devpriv->command6_bits &= ~ADC_COMMON_BIT;
		/*  bipolar or unipolar range? */
		if (thisboard->ai_range_is_unipolar[range])
			devpriv->command6_bits |= ADC_UNIP_BIT;
		else
			devpriv->command6_bits &= ~ADC_UNIP_BIT;
		/*  interrupt on fifo half full? */
		if (xfer == fifo_half_full_transfer)
			devpriv->command6_bits |= ADC_FHF_INTR_EN_BIT;
		else
			devpriv->command6_bits &= ~ADC_FHF_INTR_EN_BIT;
		/*  enable interrupt on counter a1 terminal count? */
		if (cmd->stop_src == TRIG_EXT)
			devpriv->command6_bits |= A1_INTR_EN_BIT;
		else
			devpriv->command6_bits &= ~A1_INTR_EN_BIT;
		/*  are we scanning up or down through channels? */
		if (labpc_ai_scan_mode(cmd) == MODE_MULT_CHAN_UP)
			devpriv->command6_bits |= ADC_SCAN_UP_BIT;
		else
			devpriv->command6_bits &= ~ADC_SCAN_UP_BIT;
		/*  write to register */
		devpriv->write_byte(devpriv->command6_bits,
				    dev->iobase + COMMAND6_REG);
	}

	/* setup channel list, etc (command1 register) */
	devpriv->command1_bits = 0;
	if (labpc_ai_scan_mode(cmd) == MODE_MULT_CHAN_UP)
		channel = CR_CHAN(cmd->chanlist[cmd->chanlist_len - 1]);
	else
		channel = CR_CHAN(cmd->chanlist[0]);
	/* munge channel bits for differential / scan disabled mode */
	if ((labpc_ai_scan_mode(cmd) == MODE_SINGLE_CHAN ||
	     labpc_ai_scan_mode(cmd) == MODE_SINGLE_CHAN_INTERVAL) &&
	    aref == AREF_DIFF)
		channel *= 2;
	devpriv->command1_bits |= ADC_CHAN_BITS(channel);
	devpriv->command1_bits |= thisboard->ai_range_code[range];
	devpriv->write_byte(devpriv->command1_bits, dev->iobase + COMMAND1_REG);
	/* manual says to set scan enable bit on second pass */
	if (labpc_ai_scan_mode(cmd) == MODE_MULT_CHAN_UP ||
	    labpc_ai_scan_mode(cmd) == MODE_MULT_CHAN_DOWN) {
		devpriv->command1_bits |= ADC_SCAN_EN_BIT;
		/* need a brief delay before enabling scan, or scan
		 * list will get screwed when you switch
		 * between scan up to scan down mode - dunno why */
		udelay(1);
		devpriv->write_byte(devpriv->command1_bits,
				    dev->iobase + COMMAND1_REG);
	}

	devpriv->write_byte(cmd->chanlist_len,
			    dev->iobase + INTERVAL_COUNT_REG);
	/*  load count */
	devpriv->write_byte(INTERVAL_LOAD_BITS,
			    dev->iobase + INTERVAL_LOAD_REG);

	if (cmd->convert_src == TRIG_TIMER || cmd->scan_begin_src == TRIG_TIMER) {
		/*  set up pacing */
		labpc_adc_timing(dev, cmd);
		/*  load counter b0 in mode 3 */
		ret = labpc_counter_load(dev, dev->iobase + COUNTER_B_BASE_REG,
					 0, devpriv->divisor_b0, 3);
		if (ret < 0) {
			comedi_error(dev, "error loading counter b0");
			return -1;
		}
	}
	/*  set up conversion pacing */
	if (labpc_ai_convert_period(cmd)) {
		/*  load counter a0 in mode 2 */
		ret = labpc_counter_load(dev, dev->iobase + COUNTER_A_BASE_REG,
					 0, devpriv->divisor_a0, 2);
		if (ret < 0) {
			comedi_error(dev, "error loading counter a0");
			return -1;
		}
	} else
		devpriv->write_byte(INIT_A0_BITS,
				    dev->iobase + COUNTER_A_CONTROL_REG);

	/*  set up scan pacing */
	if (labpc_ai_scan_period(cmd)) {
		/*  load counter b1 in mode 2 */
		ret = labpc_counter_load(dev, dev->iobase + COUNTER_B_BASE_REG,
					 1, devpriv->divisor_b1, 2);
		if (ret < 0) {
			comedi_error(dev, "error loading counter b1");
			return -1;
		}
	}

	labpc_clear_adc_fifo(dev);

#ifdef CONFIG_ISA_DMA_API
	/*  set up dma transfer */
	if (xfer == isa_dma_transfer) {
		irq_flags = claim_dma_lock();
		disable_dma(devpriv->dma_chan);
		/* clear flip-flop to make sure 2-byte registers for
		 * count and address get set correctly */
		clear_dma_ff(devpriv->dma_chan);
		set_dma_addr(devpriv->dma_chan,
			     virt_to_bus(devpriv->dma_buffer));
		/*  set appropriate size of transfer */
		devpriv->dma_transfer_size = labpc_suggest_transfer_size(*cmd);
		if (cmd->stop_src == TRIG_COUNT &&
		    devpriv->count * sample_size < devpriv->dma_transfer_size) {
			devpriv->dma_transfer_size =
			    devpriv->count * sample_size;
		}
		set_dma_count(devpriv->dma_chan, devpriv->dma_transfer_size);
		enable_dma(devpriv->dma_chan);
		release_dma_lock(irq_flags);
		/*  enable board's dma */
		devpriv->command3_bits |= DMA_EN_BIT | DMATC_INTR_EN_BIT;
	} else
		devpriv->command3_bits &= ~DMA_EN_BIT & ~DMATC_INTR_EN_BIT;
#endif

	/*  enable error interrupts */
	devpriv->command3_bits |= ERR_INTR_EN_BIT;
	/*  enable fifo not empty interrupt? */
	if (xfer == fifo_not_empty_transfer)
		devpriv->command3_bits |= ADC_FNE_INTR_EN_BIT;
	else
		devpriv->command3_bits &= ~ADC_FNE_INTR_EN_BIT;
	devpriv->write_byte(devpriv->command3_bits, dev->iobase + COMMAND3_REG);

	/*  setup any external triggering/pacing (command4 register) */
	devpriv->command4_bits = 0;
	if (cmd->convert_src != TRIG_EXT)
		devpriv->command4_bits |= EXT_CONVERT_DISABLE_BIT;
	/* XXX should discard first scan when using interval scanning
	 * since manual says it is not synced with scan clock */
	if (labpc_use_continuous_mode(cmd) == 0) {
		devpriv->command4_bits |= INTERVAL_SCAN_EN_BIT;
		if (cmd->scan_begin_src == TRIG_EXT)
			devpriv->command4_bits |= EXT_SCAN_EN_BIT;
	}
	/*  single-ended/differential */
	if (aref == AREF_DIFF)
		devpriv->command4_bits |= ADC_DIFF_BIT;
	devpriv->write_byte(devpriv->command4_bits, dev->iobase + COMMAND4_REG);

	/*  startup acquisition */

	/*  command2 reg */
	/*  use 2 cascaded counters for pacing */
	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->command2_bits |= CASCADE_BIT;
	switch (cmd->start_src) {
	case TRIG_EXT:
		devpriv->command2_bits |= HWTRIG_BIT;
		devpriv->command2_bits &= ~PRETRIG_BIT & ~SWTRIG_BIT;
		break;
	case TRIG_NOW:
		devpriv->command2_bits |= SWTRIG_BIT;
		devpriv->command2_bits &= ~PRETRIG_BIT & ~HWTRIG_BIT;
		break;
	default:
		comedi_error(dev, "bug with start_src");
		return -1;
		break;
	}
	switch (cmd->stop_src) {
	case TRIG_EXT:
		devpriv->command2_bits |= HWTRIG_BIT | PRETRIG_BIT;
		break;
	case TRIG_COUNT:
	case TRIG_NONE:
		break;
	default:
		comedi_error(dev, "bug with stop_src");
		return -1;
	}
	devpriv->write_byte(devpriv->command2_bits, dev->iobase + COMMAND2_REG);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	return 0;
}

/* interrupt service routine */
static irqreturn_t labpc_interrupt(int irq, void *d)
{
	struct comedi_device *dev = d;
	struct comedi_subdevice *s = dev->read_subdev;
	struct comedi_async *async;
	struct comedi_cmd *cmd;

	if (dev->attached == 0) {
		comedi_error(dev, "premature interrupt");
		return IRQ_HANDLED;
	}

	async = s->async;
	cmd = &async->cmd;
	async->events = 0;

	/* read board status */
	devpriv->status1_bits = devpriv->read_byte(dev->iobase + STATUS1_REG);
	if (thisboard->register_layout == labpc_1200_layout)
		devpriv->status2_bits =
		    devpriv->read_byte(dev->iobase + STATUS2_REG);

	if ((devpriv->status1_bits & (DMATC_BIT | TIMER_BIT | OVERFLOW_BIT |
				      OVERRUN_BIT | DATA_AVAIL_BIT)) == 0
	    && (devpriv->status2_bits & A1_TC_BIT) == 0
	    && (devpriv->status2_bits & FNHF_BIT)) {
		return IRQ_NONE;
	}

	if (devpriv->status1_bits & OVERRUN_BIT) {
		/* clear error interrupt */
		devpriv->write_byte(0x1, dev->iobase + ADC_CLEAR_REG);
		async->events |= COMEDI_CB_ERROR | COMEDI_CB_EOA;
		comedi_event(dev, s);
		comedi_error(dev, "overrun");
		return IRQ_HANDLED;
	}

#ifdef CONFIG_ISA_DMA_API
	if (devpriv->current_transfer == isa_dma_transfer) {
		/*
		 * if a dma terminal count of external stop trigger
		 * has occurred
		 */
		if (devpriv->status1_bits & DMATC_BIT ||
		    (thisboard->register_layout == labpc_1200_layout
		     && devpriv->status2_bits & A1_TC_BIT)) {
			handle_isa_dma(dev);
		}
	} else
#endif
		labpc_drain_fifo(dev);

	if (devpriv->status1_bits & TIMER_BIT) {
		comedi_error(dev, "handled timer interrupt?");
		/*  clear it */
		devpriv->write_byte(0x1, dev->iobase + TIMER_CLEAR_REG);
	}

	if (devpriv->status1_bits & OVERFLOW_BIT) {
		/*  clear error interrupt */
		devpriv->write_byte(0x1, dev->iobase + ADC_CLEAR_REG);
		async->events |= COMEDI_CB_ERROR | COMEDI_CB_EOA;
		comedi_event(dev, s);
		comedi_error(dev, "overflow");
		return IRQ_HANDLED;
	}
	/*  handle external stop trigger */
	if (cmd->stop_src == TRIG_EXT) {
		if (devpriv->status2_bits & A1_TC_BIT) {
			labpc_drain_dregs(dev);
			labpc_cancel(dev, s);
			async->events |= COMEDI_CB_EOA;
		}
	}

	/* TRIG_COUNT end of acquisition */
	if (cmd->stop_src == TRIG_COUNT) {
		if (devpriv->count == 0) {
			labpc_cancel(dev, s);
			async->events |= COMEDI_CB_EOA;
		}
	}

	comedi_event(dev, s);
	return IRQ_HANDLED;
}

/* read all available samples from ai fifo */
static int labpc_drain_fifo(struct comedi_device *dev)
{
	unsigned int lsb, msb;
	short data;
	struct comedi_async *async = dev->read_subdev->async;
	const int timeout = 10000;
	unsigned int i;

	devpriv->status1_bits = devpriv->read_byte(dev->iobase + STATUS1_REG);

	for (i = 0; (devpriv->status1_bits & DATA_AVAIL_BIT) && i < timeout;
	     i++) {
		/*  quit if we have all the data we want */
		if (async->cmd.stop_src == TRIG_COUNT) {
			if (devpriv->count == 0)
				break;
			devpriv->count--;
		}
		lsb = devpriv->read_byte(dev->iobase + ADC_FIFO_REG);
		msb = devpriv->read_byte(dev->iobase + ADC_FIFO_REG);
		data = (msb << 8) | lsb;
		cfc_write_to_buffer(dev->read_subdev, data);
		devpriv->status1_bits =
		    devpriv->read_byte(dev->iobase + STATUS1_REG);
	}
	if (i == timeout) {
		comedi_error(dev, "ai timeout, fifo never empties");
		async->events |= COMEDI_CB_ERROR | COMEDI_CB_EOA;
		return -1;
	}

	return 0;
}

#ifdef CONFIG_ISA_DMA_API
static void labpc_drain_dma(struct comedi_device *dev)
{
	struct comedi_subdevice *s = dev->read_subdev;
	struct comedi_async *async = s->async;
	int status;
	unsigned long flags;
	unsigned int max_points, num_points, residue, leftover;
	int i;

	status = devpriv->status1_bits;

	flags = claim_dma_lock();
	disable_dma(devpriv->dma_chan);
	/* clear flip-flop to make sure 2-byte registers for
	 * count and address get set correctly */
	clear_dma_ff(devpriv->dma_chan);

	/*  figure out how many points to read */
	max_points = devpriv->dma_transfer_size / sample_size;
	/* residue is the number of points left to be done on the dma
	 * transfer.  It should always be zero at this point unless
	 * the stop_src is set to external triggering.
	 */
	residue = get_dma_residue(devpriv->dma_chan) / sample_size;
	num_points = max_points - residue;
	if (devpriv->count < num_points && async->cmd.stop_src == TRIG_COUNT)
		num_points = devpriv->count;

	/*  figure out how many points will be stored next time */
	leftover = 0;
	if (async->cmd.stop_src != TRIG_COUNT) {
		leftover = devpriv->dma_transfer_size / sample_size;
	} else if (devpriv->count > num_points) {
		leftover = devpriv->count - num_points;
		if (leftover > max_points)
			leftover = max_points;
	}

	/* write data to comedi buffer */
	for (i = 0; i < num_points; i++)
		cfc_write_to_buffer(s, devpriv->dma_buffer[i]);

	if (async->cmd.stop_src == TRIG_COUNT)
		devpriv->count -= num_points;

	/*  set address and count for next transfer */
	set_dma_addr(devpriv->dma_chan, virt_to_bus(devpriv->dma_buffer));
	set_dma_count(devpriv->dma_chan, leftover * sample_size);
	release_dma_lock(flags);

	async->events |= COMEDI_CB_BLOCK;
}

static void handle_isa_dma(struct comedi_device *dev)
{
	labpc_drain_dma(dev);

	enable_dma(devpriv->dma_chan);

	/*  clear dma tc interrupt */
	devpriv->write_byte(0x1, dev->iobase + DMATC_CLEAR_REG);
}
#endif

/* makes sure all data acquired by board is transferred to comedi (used
 * when acquisition is terminated by stop_src == TRIG_EXT). */
static void labpc_drain_dregs(struct comedi_device *dev)
{
#ifdef CONFIG_ISA_DMA_API
	if (devpriv->current_transfer == isa_dma_transfer)
		labpc_drain_dma(dev);
#endif

	labpc_drain_fifo(dev);
}

static int labpc_ai_rinsn(struct comedi_device *dev, struct comedi_subdevice *s,
			  struct comedi_insn *insn, unsigned int *data)
{
	int i, n;
	int chan, range;
	int lsb, msb;
	int timeout = 1000;
	unsigned long flags;

	/*  disable timed conversions */
	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->command2_bits &= ~SWTRIG_BIT & ~HWTRIG_BIT & ~PRETRIG_BIT;
	devpriv->write_byte(devpriv->command2_bits, dev->iobase + COMMAND2_REG);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	/*  disable interrupt generation and dma */
	devpriv->command3_bits = 0;
	devpriv->write_byte(devpriv->command3_bits, dev->iobase + COMMAND3_REG);

	/* set gain and channel */
	devpriv->command1_bits = 0;
	chan = CR_CHAN(insn->chanspec);
	range = CR_RANGE(insn->chanspec);
	devpriv->command1_bits |= thisboard->ai_range_code[range];
	/* munge channel bits for differential/scan disabled mode */
	if (CR_AREF(insn->chanspec) == AREF_DIFF)
		chan *= 2;
	devpriv->command1_bits |= ADC_CHAN_BITS(chan);
	devpriv->write_byte(devpriv->command1_bits, dev->iobase + COMMAND1_REG);

	/* setup command6 register for 1200 boards */
	if (thisboard->register_layout == labpc_1200_layout) {
		/*  reference inputs to ground or common? */
		if (CR_AREF(insn->chanspec) != AREF_GROUND)
			devpriv->command6_bits |= ADC_COMMON_BIT;
		else
			devpriv->command6_bits &= ~ADC_COMMON_BIT;
		/* bipolar or unipolar range? */
		if (thisboard->ai_range_is_unipolar[range])
			devpriv->command6_bits |= ADC_UNIP_BIT;
		else
			devpriv->command6_bits &= ~ADC_UNIP_BIT;
		/* don't interrupt on fifo half full */
		devpriv->command6_bits &= ~ADC_FHF_INTR_EN_BIT;
		/* don't enable interrupt on counter a1 terminal count? */
		devpriv->command6_bits &= ~A1_INTR_EN_BIT;
		/* write to register */
		devpriv->write_byte(devpriv->command6_bits,
				    dev->iobase + COMMAND6_REG);
	}
	/* setup command4 register */
	devpriv->command4_bits = 0;
	devpriv->command4_bits |= EXT_CONVERT_DISABLE_BIT;
	/* single-ended/differential */
	if (CR_AREF(insn->chanspec) == AREF_DIFF)
		devpriv->command4_bits |= ADC_DIFF_BIT;
	devpriv->write_byte(devpriv->command4_bits, dev->iobase + COMMAND4_REG);

	/*
	 * initialize pacer counter output to make sure it doesn't
	 * cause any problems
	 */
	devpriv->write_byte(INIT_A0_BITS, dev->iobase + COUNTER_A_CONTROL_REG);

	labpc_clear_adc_fifo(dev);

	for (n = 0; n < insn->n; n++) {
		/* trigger conversion */
		devpriv->write_byte(0x1, dev->iobase + ADC_CONVERT_REG);

		for (i = 0; i < timeout; i++) {
			if (devpriv->read_byte(dev->iobase +
					       STATUS1_REG) & DATA_AVAIL_BIT)
				break;
			udelay(1);
		}
		if (i == timeout) {
			comedi_error(dev, "timeout");
			return -ETIME;
		}
		lsb = devpriv->read_byte(dev->iobase + ADC_FIFO_REG);
		msb = devpriv->read_byte(dev->iobase + ADC_FIFO_REG);
		data[n] = (msb << 8) | lsb;
	}

	return n;
}

/* analog output insn */
static int labpc_ao_winsn(struct comedi_device *dev, struct comedi_subdevice *s,
			  struct comedi_insn *insn, unsigned int *data)
{
	int channel, range;
	unsigned long flags;
	int lsb, msb;

	channel = CR_CHAN(insn->chanspec);

	/* turn off pacing of analog output channel */
	/* note: hardware bug in daqcard-1200 means pacing cannot
	 * be independently enabled/disabled for its the two channels */
	spin_lock_irqsave(&dev->spinlock, flags);
	devpriv->command2_bits &= ~DAC_PACED_BIT(channel);
	devpriv->write_byte(devpriv->command2_bits, dev->iobase + COMMAND2_REG);
	spin_unlock_irqrestore(&dev->spinlock, flags);

	/* set range */
	if (thisboard->register_layout == labpc_1200_layout) {
		range = CR_RANGE(insn->chanspec);
		if (range & AO_RANGE_IS_UNIPOLAR)
			devpriv->command6_bits |= DAC_UNIP_BIT(channel);
		else
			devpriv->command6_bits &= ~DAC_UNIP_BIT(channel);
		/*  write to register */
		devpriv->write_byte(devpriv->command6_bits,
				    dev->iobase + COMMAND6_REG);
	}
	/* send data */
	lsb = data[0] & 0xff;
	msb = (data[0] >> 8) & 0xff;
	devpriv->write_byte(lsb, dev->iobase + DAC_LSB_REG(channel));
	devpriv->write_byte(msb, dev->iobase + DAC_MSB_REG(channel));

	/* remember value for readback */
	devpriv->ao_value[channel] = data[0];

	return 1;
}

/* analog output readback insn */
static int labpc_ao_rinsn(struct comedi_device *dev, struct comedi_subdevice *s,
			  struct comedi_insn *insn, unsigned int *data)
{
	data[0] = devpriv->ao_value[CR_CHAN(insn->chanspec)];

	return 1;
}

static int labpc_calib_read_insn(struct comedi_device *dev,
				 struct comedi_subdevice *s,
				 struct comedi_insn *insn, unsigned int *data)
{
	data[0] = devpriv->caldac[CR_CHAN(insn->chanspec)];

	return 1;
}

static int labpc_calib_write_insn(struct comedi_device *dev,
				  struct comedi_subdevice *s,
				  struct comedi_insn *insn, unsigned int *data)
{
	int channel = CR_CHAN(insn->chanspec);

	write_caldac(dev, channel, data[0]);
	return 1;
}

static int labpc_eeprom_read_insn(struct comedi_device *dev,
				  struct comedi_subdevice *s,
				  struct comedi_insn *insn, unsigned int *data)
{
	data[0] = devpriv->eeprom_data[CR_CHAN(insn->chanspec)];

	return 1;
}

static int labpc_eeprom_write_insn(struct comedi_device *dev,
				   struct comedi_subdevice *s,
				   struct comedi_insn *insn, unsigned int *data)
{
	int channel = CR_CHAN(insn->chanspec);
=======
	struct labpc_private *devpriv;
	unsigned int irq = it->options[1];
	unsigned int dma_chan = it->options[2];
>>>>>>> android-3.18
	int ret;

	devpriv = comedi_alloc_devpriv(dev, sizeof(*devpriv));
	if (!devpriv)
		return -ENOMEM;

	ret = comedi_request_region(dev, it->options[0], 0x20);
	if (ret)
		return ret;

	ret = labpc_common_attach(dev, irq, 0);
	if (ret)
		return ret;

	if (dev->irq)
		labpc_init_dma_chan(dev, dma_chan);

	return 0;
}

static void labpc_detach(struct comedi_device *dev)
{
	struct labpc_private *devpriv = dev->private;

	if (devpriv)
		labpc_free_dma_chan(dev);

	comedi_legacy_detach(dev);
}

static struct comedi_driver labpc_driver = {
	.driver_name	= "ni_labpc",
	.module		= THIS_MODULE,
	.attach		= labpc_attach,
	.detach		= labpc_detach,
	.num_names	= ARRAY_SIZE(labpc_boards),
	.board_name	= &labpc_boards[0].name,
	.offset		= sizeof(struct labpc_boardinfo),
};
module_comedi_driver(labpc_driver);

MODULE_AUTHOR("Comedi http://www.comedi.org");
MODULE_DESCRIPTION("Comedi driver for NI Lab-PC ISA boards");
MODULE_LICENSE("GPL");
