/*
 * Copyright (c) 1996, 2003 VIA Networking Technologies, Inc.
 * All rights reserved.
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
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *
 * File: mac.c
 *
 * Purpose:  MAC routines
 *
 * Author: Tevin Chen
 *
 * Date: May 21, 1996
 *
 * Functions:
 *
 * Revision History:
 */

#include "desc.h"
#include "mac.h"
#include "usbpipe.h"

/*
 * Description:
 *      Write MAC Multicast Address Mask
 *
 * Parameters:
 *  In:
 *	mc_filter (mac filter)
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
void vnt_mac_set_filter(struct vnt_private *priv, u64 mc_filter)
{
	__le64 le_mc = cpu_to_le64(mc_filter);

	vnt_control_out(priv, MESSAGE_TYPE_WRITE, MAC_REG_MAR0,
		MESSAGE_REQUEST_MACREG, sizeof(le_mc), (u8 *)&le_mc);
}

/*
 * Description:
 *      Shut Down MAC
 *
 * Parameters:
 *  In:
 *  Out:
 *      none
 *
 *
 */
void vnt_mac_shutdown(struct vnt_private *priv)
{
	vnt_control_out(priv, MESSAGE_TYPE_MACSHUTDOWN, 0, 0, 0, NULL);
}

void vnt_mac_set_bb_type(struct vnt_private *priv, u8 type)
{
	u8 data[2];

	data[0] = type;
	data[1] = EnCFG_BBType_MASK;

	vnt_control_out(priv, MESSAGE_TYPE_WRITE_MASK, MAC_REG_ENCFG0,
		MESSAGE_REQUEST_MACREG,	ARRAY_SIZE(data), data);
}

/*
 * Description:
 *      Disable the Key Entry by MISCFIFO
 *
 * Parameters:
 *  In:
 *      dwIoBase        - Base Address for MAC
 *
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
void vnt_mac_disable_keyentry(struct vnt_private *priv, u8 entry_idx)
{
	vnt_control_out(priv, MESSAGE_TYPE_CLRKEYENTRY, 0, 0,
		sizeof(entry_idx), &entry_idx);
}

/*
 * Description:
 *      Set the Key by MISCFIFO
 *
 * Parameters:
 *  In:
 *      dwIoBase        - Base Address for MAC
 *
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
void vnt_mac_set_keyentry(struct vnt_private *priv, u16 key_ctl, u32 entry_idx,
	u32 key_idx, u8 *addr, u8 *key)
{
<<<<<<< HEAD
PBYTE           pbyKey;
WORD            wOffset;
DWORD           dwData1,dwData2;
int             ii;
BYTE            pbyData[24];

    if ( pDevice->byLocalID <= MAC_REVISION_A1 ) {
        if ( pDevice->sMgmtObj.byCSSPK == KEY_CTL_CCMP )
            return;
    }

    wOffset = MISCFIFO_KEYETRY0;
    wOffset += (uEntryIdx * MISCFIFO_KEYENTRYSIZE);

    dwData1 = 0;
    dwData1 |= wKeyCtl;
    dwData1 <<= 16;
    dwData1 |= MAKEWORD(*(pbyAddr+4), *(pbyAddr+5));

	DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"1. wOffset: %d, Data: %X,"\
		" KeyCtl:%X\n", wOffset, dwData1, wKeyCtl);

    //VNSvOutPortW(dwIoBase + MAC_REG_MISCFFNDEX, wOffset);
    //VNSvOutPortD(dwIoBase + MAC_REG_MISCFFDATA, dwData);
    //VNSvOutPortW(dwIoBase + MAC_REG_MISCFFCTL, MISCFFCTL_WRITE);

    //wOffset++;

    dwData2 = 0;
    dwData2 |= *(pbyAddr+3);
    dwData2 <<= 8;
    dwData2 |= *(pbyAddr+2);
    dwData2 <<= 8;
    dwData2 |= *(pbyAddr+1);
    dwData2 <<= 8;
    dwData2 |= *(pbyAddr+0);

	DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"2. wOffset: %d, Data: %X\n",
		wOffset, dwData2);

    //VNSvOutPortW(dwIoBase + MAC_REG_MISCFFNDEX, wOffset);
    //VNSvOutPortD(dwIoBase + MAC_REG_MISCFFDATA, dwData);
    //VNSvOutPortW(dwIoBase + MAC_REG_MISCFFCTL, MISCFFCTL_WRITE);

    //wOffset++;

    //wOffset += (uKeyIdx * 4);
/*    for (ii=0;ii<4;ii++) {
        // alway push 128 bits
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"3.(%d) wOffset: %d, Data: %lX\n", ii, wOffset+ii, *pdwKey);
        VNSvOutPortW(dwIoBase + MAC_REG_MISCFFNDEX, wOffset+ii);
        VNSvOutPortD(dwIoBase + MAC_REG_MISCFFDATA, *pdwKey++);
        VNSvOutPortW(dwIoBase + MAC_REG_MISCFFCTL, MISCFFCTL_WRITE);
    }
*/
    pbyKey = (PBYTE)pdwKey;

    pbyData[0] = (BYTE)dwData1;
    pbyData[1] = (BYTE)(dwData1>>8);
    pbyData[2] = (BYTE)(dwData1>>16);
    pbyData[3] = (BYTE)(dwData1>>24);
    pbyData[4] = (BYTE)dwData2;
    pbyData[5] = (BYTE)(dwData2>>8);
    pbyData[6] = (BYTE)(dwData2>>16);
    pbyData[7] = (BYTE)(dwData2>>24);
    for (ii = 8; ii < 24; ii++)
	pbyData[ii] = *pbyKey++;

    CONTROLnsRequestOut(pDevice,
                        MESSAGE_TYPE_SETKEY,
                        wOffset,
                        (WORD)uKeyIdx,
                        24,
                        pbyData
                        );
=======
	struct vnt_mac_set_key set_key;
	u16 offset;
>>>>>>> android-3.18

	offset = MISCFIFO_KEYETRY0;
	offset += (entry_idx * MISCFIFO_KEYENTRYSIZE);

	set_key.u.write.key_ctl = cpu_to_le16(key_ctl);
	memcpy(set_key.u.write.addr, addr, ETH_ALEN);

	/* swap over swap[0] and swap[1] to get correct write order */
	swap(set_key.u.swap[0], set_key.u.swap[1]);

	memcpy(set_key.key, key, WLAN_KEY_LEN_CCMP);

	dev_dbg(&priv->usb->dev, "offset %d key ctl %d set key %24ph\n",
				offset, key_ctl, (u8 *)&set_key);

	vnt_control_out(priv, MESSAGE_TYPE_SETKEY, offset,
		(u16)key_idx, sizeof(struct vnt_mac_set_key), (u8 *)&set_key);
}

void vnt_mac_reg_bits_off(struct vnt_private *priv, u8 reg_ofs, u8 bits)
{
	u8 data[2];

	data[0] = 0;
	data[1] = bits;

	vnt_control_out(priv, MESSAGE_TYPE_WRITE_MASK,
		reg_ofs, MESSAGE_REQUEST_MACREG, ARRAY_SIZE(data), data);
}

void vnt_mac_reg_bits_on(struct vnt_private *priv, u8 reg_ofs, u8 bits)
{
	u8 data[2];

	data[0] = bits;
	data[1] = bits;

	vnt_control_out(priv, MESSAGE_TYPE_WRITE_MASK,
		reg_ofs, MESSAGE_REQUEST_MACREG, ARRAY_SIZE(data), data);
}

void vnt_mac_write_word(struct vnt_private *priv, u8 reg_ofs, u16 word)
{
	u8 data[2];

	data[0] = (u8)(word & 0xff);
	data[1] = (u8)(word >> 8);

	vnt_control_out(priv, MESSAGE_TYPE_WRITE,
		reg_ofs, MESSAGE_REQUEST_MACREG, ARRAY_SIZE(data), data);
}

void vnt_mac_set_bssid_addr(struct vnt_private *priv, u8 *addr)
{
	vnt_control_out(priv, MESSAGE_TYPE_WRITE, MAC_REG_BSSID0,
		MESSAGE_REQUEST_MACREG, ETH_ALEN, addr);
}

void vnt_mac_enable_protect_mode(struct vnt_private *priv)
{
	u8 data[2];

	data[0] = EnCFG_ProtectMd;
	data[1] = EnCFG_ProtectMd;

	vnt_control_out(priv, MESSAGE_TYPE_WRITE_MASK,
		MAC_REG_ENCFG0, MESSAGE_REQUEST_MACREG, ARRAY_SIZE(data), data);
}

void vnt_mac_disable_protect_mode(struct vnt_private *priv)
{
	u8 data[2];

	data[0] = 0;
	data[1] = EnCFG_ProtectMd;

	vnt_control_out(priv, MESSAGE_TYPE_WRITE_MASK,
		MAC_REG_ENCFG0, MESSAGE_REQUEST_MACREG, ARRAY_SIZE(data), data);
}

void vnt_mac_enable_barker_preamble_mode(struct vnt_private *priv)
{
	u8 data[2];

	data[0] = EnCFG_BarkerPream;
	data[1] = EnCFG_BarkerPream;

	vnt_control_out(priv, MESSAGE_TYPE_WRITE_MASK,
		MAC_REG_ENCFG2, MESSAGE_REQUEST_MACREG, ARRAY_SIZE(data), data);
}

void vnt_mac_disable_barker_preamble_mode(struct vnt_private *priv)
{
	u8 data[2];

	data[0] = 0;
	data[1] = EnCFG_BarkerPream;

	vnt_control_out(priv, MESSAGE_TYPE_WRITE_MASK,
		MAC_REG_ENCFG2, MESSAGE_REQUEST_MACREG, ARRAY_SIZE(data), data);
}

void vnt_mac_set_beacon_interval(struct vnt_private *priv, u16 interval)
{
	u8 data[2];

	data[0] = (u8)(interval & 0xff);
	data[1] = (u8)(interval >> 8);

	vnt_control_out(priv, MESSAGE_TYPE_WRITE,
		MAC_REG_BI, MESSAGE_REQUEST_MACREG, ARRAY_SIZE(data), data);
}

void vnt_mac_set_led(struct vnt_private *priv, u8 state, u8 led)
{
	u8 data[2];

	data[0] = led;
	data[1] = state;

	vnt_control_out(priv, MESSAGE_TYPE_WRITE_MASK, MAC_REG_PAPEDELAY,
			MESSAGE_REQUEST_MACREG, ARRAY_SIZE(data), data);
}
