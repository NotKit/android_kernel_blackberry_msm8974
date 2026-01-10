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
 * File: rxtx.c
 *
 * Purpose: handle WMAC/802.3/802.11 rx & tx functions
 *
 * Author: Lyndon Chen
 *
 * Date: May 20, 2003
 *
 * Functions:
 *      vnt_generate_tx_parameter - Generate tx dma required parameter.
 *      vnt_get_duration_le - get tx data required duration
 *      vnt_get_rtscts_duration_le- get rtx/cts required duration
 *      vnt_get_rtscts_rsvtime_le- get rts/cts reserved time
 *      vnt_get_rsvtime- get frame reserved time
 *      vnt_fill_cts_head- fulfill CTS ctl header
 *
 * Revision History:
 *
 */

#include "device.h"
#include "rxtx.h"
#include "card.h"
#include "mac.h"
#include "rf.h"
#include "usbpipe.h"

static const u16 vnt_time_stampoff[2][MAX_RATE] = {
	{384, 288, 226, 209, 54, 43, 37, 31, 28, 25, 24, 23},/* Long Preamble */
	{384, 192, 130, 113, 54, 43, 37, 31, 28, 25, 24, 23},/* Short Preamble */
};

static const u16 vnt_fb_opt0[2][5] = {
	{RATE_12M, RATE_18M, RATE_24M, RATE_36M, RATE_48M}, /* fallback_rate0 */
	{RATE_12M, RATE_12M, RATE_18M, RATE_24M, RATE_36M}, /* fallback_rate1 */
};

static const u16 vnt_fb_opt1[2][5] = {
	{RATE_12M, RATE_18M, RATE_24M, RATE_24M, RATE_36M}, /* fallback_rate0 */
	{RATE_6M , RATE_6M,  RATE_12M, RATE_12M, RATE_18M}, /* fallback_rate1 */
};

#define RTSDUR_BB       0
#define RTSDUR_BA       1
#define RTSDUR_AA       2
#define CTSDUR_BA       3
#define RTSDUR_BA_F0    4
#define RTSDUR_AA_F0    5
#define RTSDUR_BA_F1    6
#define RTSDUR_AA_F1    7
#define CTSDUR_BA_F0    8
#define CTSDUR_BA_F1    9
#define DATADUR_B       10
#define DATADUR_A       11
#define DATADUR_A_F0    12
#define DATADUR_A_F1    13

static struct vnt_usb_send_context
	*vnt_get_free_context(struct vnt_private *priv)
{
	struct vnt_usb_send_context *context = NULL;
	int ii;

	dev_dbg(&priv->usb->dev, "%s\n", __func__);

	for (ii = 0; ii < priv->num_tx_context; ii++) {
		if (!priv->tx_context[ii])
			return NULL;

		context = priv->tx_context[ii];
		if (context->in_use == false) {
			context->in_use = true;
			memset(context->data, 0,
					MAX_TOTAL_SIZE_WITH_ALL_HEADERS);

			context->hdr = NULL;

			return context;
		}
	}

	if (ii == priv->num_tx_context)
		dev_dbg(&priv->usb->dev, "%s No Free Tx Context\n", __func__);

	return NULL;
}

static __le16 vnt_time_stamp_off(struct vnt_private *priv, u16 rate)
{
<<<<<<< HEAD
    PDWORD          pdwIV = (PDWORD) pbyIVHead;
    PDWORD          pdwExtIV = (PDWORD) ((PBYTE)pbyIVHead+4);
    WORD            wValue;
    PS802_11Header  pMACHeader = (PS802_11Header)pbyHdrBuf;
    DWORD           dwRevIVCounter;



    //Fill TXKEY
    if (pTransmitKey == NULL)
        return;

    dwRevIVCounter = cpu_to_le32(pDevice->dwIVCounter);
    *pdwIV = pDevice->dwIVCounter;
    pDevice->byKeyIndex = pTransmitKey->dwKeyIndex & 0xf;

    if (pTransmitKey->byCipherSuite == KEY_CTL_WEP) {
        if (pTransmitKey->uKeyLength == WLAN_WEP232_KEYLEN ){
            memcpy(pDevice->abyPRNG, (PBYTE)&(dwRevIVCounter), 3);
            memcpy(pDevice->abyPRNG+3, pTransmitKey->abyKey, pTransmitKey->uKeyLength);
        } else {
            memcpy(pbyBuf, (PBYTE)&(dwRevIVCounter), 3);
            memcpy(pbyBuf+3, pTransmitKey->abyKey, pTransmitKey->uKeyLength);
            if(pTransmitKey->uKeyLength == WLAN_WEP40_KEYLEN) {
                memcpy(pbyBuf+8, (PBYTE)&(dwRevIVCounter), 3);
                memcpy(pbyBuf+11, pTransmitKey->abyKey, pTransmitKey->uKeyLength);
            }
            memcpy(pDevice->abyPRNG, pbyBuf, 16);
        }
        // Append IV after Mac Header
        *pdwIV &= WEP_IV_MASK;//00000000 11111111 11111111 11111111
        *pdwIV |= (pDevice->byKeyIndex << 30);
        *pdwIV = cpu_to_le32(*pdwIV);
        pDevice->dwIVCounter++;
        if (pDevice->dwIVCounter > WEP_IV_MASK) {
            pDevice->dwIVCounter = 0;
        }
    } else if (pTransmitKey->byCipherSuite == KEY_CTL_TKIP) {
        pTransmitKey->wTSC15_0++;
        if (pTransmitKey->wTSC15_0 == 0) {
            pTransmitKey->dwTSC47_16++;
        }
        TKIPvMixKey(pTransmitKey->abyKey, pDevice->abyCurrentNetAddr,
                    pTransmitKey->wTSC15_0, pTransmitKey->dwTSC47_16, pDevice->abyPRNG);
        memcpy(pbyBuf, pDevice->abyPRNG, 16);
        // Make IV
        memcpy(pdwIV, pDevice->abyPRNG, 3);

        *(pbyIVHead+3) = (BYTE)(((pDevice->byKeyIndex << 6) & 0xc0) | 0x20); // 0x20 is ExtIV
        // Append IV&ExtIV after Mac Header
        *pdwExtIV = cpu_to_le32(pTransmitKey->dwTSC47_16);
	DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"vFillTxKey()---- pdwExtIV: %x\n",
		*pdwExtIV);

    } else if (pTransmitKey->byCipherSuite == KEY_CTL_CCMP) {
        pTransmitKey->wTSC15_0++;
        if (pTransmitKey->wTSC15_0 == 0) {
            pTransmitKey->dwTSC47_16++;
        }
        memcpy(pbyBuf, pTransmitKey->abyKey, 16);

        // Make IV
        *pdwIV = 0;
        *(pbyIVHead+3) = (BYTE)(((pDevice->byKeyIndex << 6) & 0xc0) | 0x20); // 0x20 is ExtIV
        *pdwIV |= cpu_to_le16((WORD)(pTransmitKey->wTSC15_0));
        //Append IV&ExtIV after Mac Header
        *pdwExtIV = cpu_to_le32(pTransmitKey->dwTSC47_16);

        //Fill MICHDR0
        *pMICHDR = 0x59;
        *((PBYTE)(pMICHDR+1)) = 0; // TxPriority
        memcpy(pMICHDR+2, &(pMACHeader->abyAddr2[0]), 6);
        *((PBYTE)(pMICHDR+8)) = HIBYTE(HIWORD(pTransmitKey->dwTSC47_16));
        *((PBYTE)(pMICHDR+9)) = LOBYTE(HIWORD(pTransmitKey->dwTSC47_16));
        *((PBYTE)(pMICHDR+10)) = HIBYTE(LOWORD(pTransmitKey->dwTSC47_16));
        *((PBYTE)(pMICHDR+11)) = LOBYTE(LOWORD(pTransmitKey->dwTSC47_16));
        *((PBYTE)(pMICHDR+12)) = HIBYTE(pTransmitKey->wTSC15_0);
        *((PBYTE)(pMICHDR+13)) = LOBYTE(pTransmitKey->wTSC15_0);
        *((PBYTE)(pMICHDR+14)) = HIBYTE(wPayloadLen);
        *((PBYTE)(pMICHDR+15)) = LOBYTE(wPayloadLen);

        //Fill MICHDR1
        *((PBYTE)(pMICHDR+16)) = 0; // HLEN[15:8]
        if (pDevice->bLongHeader) {
            *((PBYTE)(pMICHDR+17)) = 28; // HLEN[7:0]
        } else {
            *((PBYTE)(pMICHDR+17)) = 22; // HLEN[7:0]
        }
        wValue = cpu_to_le16(pMACHeader->wFrameCtl & 0xC78F);
        memcpy(pMICHDR+18, (PBYTE)&wValue, 2); // MSKFRACTL
        memcpy(pMICHDR+20, &(pMACHeader->abyAddr1[0]), 6);
        memcpy(pMICHDR+26, &(pMACHeader->abyAddr2[0]), 6);

        //Fill MICHDR2
        memcpy(pMICHDR+32, &(pMACHeader->abyAddr3[0]), 6);
        wValue = pMACHeader->wSeqCtl;
        wValue &= 0x000F;
        wValue = cpu_to_le16(wValue);
        memcpy(pMICHDR+38, (PBYTE)&wValue, 2); // MSKSEQCTL
        if (pDevice->bLongHeader) {
            memcpy(pMICHDR+40, &(pMACHeader->abyAddr4[0]), 6);
        }
    }
=======
	return cpu_to_le16(vnt_time_stampoff[priv->preamble_type % 2]
							[rate % MAX_RATE]);
>>>>>>> android-3.18
}

static u32 vnt_get_rsvtime(struct vnt_private *priv, u8 pkt_type,
	u32 frame_length, u16 rate, int need_ack)
{
	u32 data_time, ack_time;

	data_time = vnt_get_frame_time(priv->preamble_type, pkt_type,
							frame_length, rate);

	if (pkt_type == PK_TYPE_11B)
		ack_time = vnt_get_frame_time(priv->preamble_type, pkt_type,
					14, (u16)priv->top_cck_basic_rate);
	else
		ack_time = vnt_get_frame_time(priv->preamble_type, pkt_type,
					14, (u16)priv->top_ofdm_basic_rate);

	if (need_ack)
		return data_time + priv->sifs + ack_time;

	return data_time;
}

static __le16 vnt_rxtx_rsvtime_le16(struct vnt_private *priv, u8 pkt_type,
	u32 frame_length, u16 rate, int need_ack)
{
	return cpu_to_le16((u16)vnt_get_rsvtime(priv, pkt_type,
		frame_length, rate, need_ack));
}

static __le16 vnt_get_rtscts_rsvtime_le(struct vnt_private *priv,
	u8 rsv_type, u8 pkt_type, u32 frame_length, u16 current_rate)
{
	u32 rrv_time, rts_time, cts_time, ack_time, data_time;

	rrv_time = rts_time = cts_time = ack_time = data_time = 0;

	data_time = vnt_get_frame_time(priv->preamble_type, pkt_type,
						frame_length, current_rate);

	if (rsv_type == 0) {
		rts_time = vnt_get_frame_time(priv->preamble_type,
			pkt_type, 20, priv->top_cck_basic_rate);
		cts_time = ack_time = vnt_get_frame_time(priv->preamble_type,
			pkt_type, 14, priv->top_cck_basic_rate);
	} else if (rsv_type == 1) {
		rts_time = vnt_get_frame_time(priv->preamble_type,
			pkt_type, 20, priv->top_cck_basic_rate);
		cts_time = vnt_get_frame_time(priv->preamble_type, pkt_type,
			14, priv->top_cck_basic_rate);
		ack_time = vnt_get_frame_time(priv->preamble_type, pkt_type,
			14, priv->top_ofdm_basic_rate);
	} else if (rsv_type == 2) {
		rts_time = vnt_get_frame_time(priv->preamble_type, pkt_type,
			20, priv->top_ofdm_basic_rate);
		cts_time = ack_time = vnt_get_frame_time(priv->preamble_type,
			pkt_type, 14, priv->top_ofdm_basic_rate);
	} else if (rsv_type == 3) {
		cts_time = vnt_get_frame_time(priv->preamble_type, pkt_type,
			14, priv->top_cck_basic_rate);
		ack_time = vnt_get_frame_time(priv->preamble_type, pkt_type,
			14, priv->top_ofdm_basic_rate);

		rrv_time = cts_time + ack_time + data_time + 2 * priv->sifs;

		return cpu_to_le16((u16)rrv_time);
	}

	rrv_time = rts_time + cts_time + ack_time + data_time + 3 * priv->sifs;

	return cpu_to_le16((u16)rrv_time);
}

static __le16 vnt_get_duration_le(struct vnt_private *priv,
					u8 pkt_type, int need_ack)
{
	u32 ack_time = 0;

	if (need_ack) {
		if (pkt_type == PK_TYPE_11B)
			ack_time = vnt_get_frame_time(priv->preamble_type,
				pkt_type, 14, priv->top_cck_basic_rate);
		else
			ack_time = vnt_get_frame_time(priv->preamble_type,
				pkt_type, 14, priv->top_ofdm_basic_rate);

		return cpu_to_le16((u16)(priv->sifs + ack_time));
	}

	return 0;
}

static __le16 vnt_get_rtscts_duration_le(struct vnt_usb_send_context *context,
					 u8 dur_type, u8 pkt_type, u16 rate)
{
	struct vnt_private *priv = context->priv;
	u32 cts_time = 0, dur_time = 0;
	u32 frame_length = context->frame_len;
	u8 need_ack = context->need_ack;

	switch (dur_type) {
	case RTSDUR_BB:
	case RTSDUR_BA:
	case RTSDUR_BA_F0:
	case RTSDUR_BA_F1:
		cts_time = vnt_get_frame_time(priv->preamble_type,
				pkt_type, 14, priv->top_cck_basic_rate);
		dur_time = cts_time + 2 * priv->sifs +
			vnt_get_rsvtime(priv, pkt_type,
						frame_length, rate, need_ack);
		break;

	case RTSDUR_AA:
	case RTSDUR_AA_F0:
	case RTSDUR_AA_F1:
		cts_time = vnt_get_frame_time(priv->preamble_type,
				pkt_type, 14, priv->top_ofdm_basic_rate);
		dur_time = cts_time + 2 * priv->sifs +
			vnt_get_rsvtime(priv, pkt_type,
						frame_length, rate, need_ack);
		break;

	case CTSDUR_BA:
	case CTSDUR_BA_F0:
	case CTSDUR_BA_F1:
		dur_time = priv->sifs + vnt_get_rsvtime(priv,
				pkt_type, frame_length, rate, need_ack);
		break;

	default:
		break;
	}

	return cpu_to_le16((u16)dur_time);
}

static u16 vnt_mac_hdr_pos(struct vnt_usb_send_context *tx_context,
	struct ieee80211_hdr *hdr)
{
	u8 *head = tx_context->data + offsetof(struct vnt_tx_buffer, fifo_head);
	u8 *hdr_pos = (u8 *)hdr;

	tx_context->hdr = hdr;
	if (!tx_context->hdr)
		return 0;

	return (u16)(hdr_pos - head);
}

static u16 vnt_rxtx_datahead_g(struct vnt_usb_send_context *tx_context,
			       struct vnt_tx_datahead_g *buf)
{

	struct vnt_private *priv = tx_context->priv;
	struct ieee80211_hdr *hdr =
				(struct ieee80211_hdr *)tx_context->skb->data;
	u32 frame_len = tx_context->frame_len;
	u16 rate = tx_context->tx_rate;
	u8 need_ack = tx_context->need_ack;

	/* Get SignalField,ServiceField,Length */
	vnt_get_phy_field(priv, frame_len, rate, tx_context->pkt_type, &buf->a);
	vnt_get_phy_field(priv, frame_len, priv->top_cck_basic_rate,
							PK_TYPE_11B, &buf->b);

	/* Get Duration and TimeStamp */
	if (ieee80211_is_pspoll(hdr->frame_control)) {
		__le16 dur = cpu_to_le16(priv->current_aid | BIT(14) | BIT(15));

		buf->duration_a = dur;
		buf->duration_b = dur;
	} else {
		buf->duration_a = vnt_get_duration_le(priv,
						tx_context->pkt_type, need_ack);
		buf->duration_b = vnt_get_duration_le(priv,
							PK_TYPE_11B, need_ack);
	}

	buf->time_stamp_off_a = vnt_time_stamp_off(priv, rate);
	buf->time_stamp_off_b = vnt_time_stamp_off(priv,
					priv->top_cck_basic_rate);

	tx_context->tx_hdr_size = vnt_mac_hdr_pos(tx_context, &buf->hdr);

	return le16_to_cpu(buf->duration_a);
}

static u16 vnt_rxtx_datahead_g_fb(struct vnt_usb_send_context *tx_context,
				  struct vnt_tx_datahead_g_fb *buf)
{
	struct vnt_private *priv = tx_context->priv;
	u32 frame_len = tx_context->frame_len;
	u16 rate = tx_context->tx_rate;
	u8 need_ack = tx_context->need_ack;

	/* Get SignalField,ServiceField,Length */
	vnt_get_phy_field(priv, frame_len, rate, tx_context->pkt_type, &buf->a);

	vnt_get_phy_field(priv, frame_len, priv->top_cck_basic_rate,
						PK_TYPE_11B, &buf->b);

	/* Get Duration and TimeStamp */
	buf->duration_a = vnt_get_duration_le(priv, tx_context->pkt_type,
					      need_ack);
	buf->duration_b = vnt_get_duration_le(priv, PK_TYPE_11B, need_ack);

	buf->duration_a_f0 = vnt_get_duration_le(priv, tx_context->pkt_type,
						 need_ack);
	buf->duration_a_f1 = vnt_get_duration_le(priv, tx_context->pkt_type,
						 need_ack);

	buf->time_stamp_off_a = vnt_time_stamp_off(priv, rate);
	buf->time_stamp_off_b = vnt_time_stamp_off(priv,
						priv->top_cck_basic_rate);

	tx_context->tx_hdr_size = vnt_mac_hdr_pos(tx_context, &buf->hdr);

	return le16_to_cpu(buf->duration_a);
}

static u16 vnt_rxtx_datahead_a_fb(struct vnt_usb_send_context *tx_context,
				  struct vnt_tx_datahead_a_fb *buf)
{
	struct vnt_private *priv = tx_context->priv;
	u16 rate = tx_context->tx_rate;
	u8 pkt_type = tx_context->pkt_type;
	u8 need_ack = tx_context->need_ack;
	u32 frame_len = tx_context->frame_len;

	/* Get SignalField,ServiceField,Length */
	vnt_get_phy_field(priv, frame_len, rate, pkt_type, &buf->a);
	/* Get Duration and TimeStampOff */
	buf->duration = vnt_get_duration_le(priv, pkt_type, need_ack);

	buf->duration_f0 = vnt_get_duration_le(priv, pkt_type, need_ack);
	buf->duration_f1 = vnt_get_duration_le(priv, pkt_type, need_ack);

	buf->time_stamp_off = vnt_time_stamp_off(priv, rate);

	tx_context->tx_hdr_size = vnt_mac_hdr_pos(tx_context, &buf->hdr);

	return le16_to_cpu(buf->duration);
}

static u16 vnt_rxtx_datahead_ab(struct vnt_usb_send_context *tx_context,
				struct vnt_tx_datahead_ab *buf)
{
	struct vnt_private *priv = tx_context->priv;
	struct ieee80211_hdr *hdr =
				(struct ieee80211_hdr *)tx_context->skb->data;
	u32 frame_len = tx_context->frame_len;
	u16 rate = tx_context->tx_rate;
	u8 need_ack = tx_context->need_ack;

	/* Get SignalField,ServiceField,Length */
	vnt_get_phy_field(priv, frame_len, rate,
			  tx_context->pkt_type, &buf->ab);

	/* Get Duration and TimeStampOff */
	if (ieee80211_is_pspoll(hdr->frame_control)) {
		__le16 dur = cpu_to_le16(priv->current_aid | BIT(14) | BIT(15));

		buf->duration = dur;
	} else {
		buf->duration = vnt_get_duration_le(priv, tx_context->pkt_type,
						    need_ack);
	}

	buf->time_stamp_off = vnt_time_stamp_off(priv, rate);

	tx_context->tx_hdr_size = vnt_mac_hdr_pos(tx_context, &buf->hdr);

	return le16_to_cpu(buf->duration);
}

static int vnt_fill_ieee80211_rts(struct vnt_usb_send_context *tx_context,
	struct ieee80211_rts *rts, __le16 duration)
{
	struct ieee80211_hdr *hdr =
				(struct ieee80211_hdr *)tx_context->skb->data;

	rts->duration = duration;
	rts->frame_control =
		cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_RTS);

	memcpy(rts->ra, hdr->addr1, ETH_ALEN);
	memcpy(rts->ta, hdr->addr2, ETH_ALEN);

	return 0;
}

static u16 vnt_rxtx_rts_g_head(struct vnt_usb_send_context *tx_context,
			       struct vnt_rts_g *buf)
{
	struct vnt_private *priv = tx_context->priv;
	u16 rts_frame_len = 20;
	u16 current_rate = tx_context->tx_rate;

	vnt_get_phy_field(priv, rts_frame_len, priv->top_cck_basic_rate,
		PK_TYPE_11B, &buf->b);
	vnt_get_phy_field(priv, rts_frame_len, priv->top_ofdm_basic_rate,
			  tx_context->pkt_type, &buf->a);

	buf->duration_bb = vnt_get_rtscts_duration_le(tx_context, RTSDUR_BB,
						      PK_TYPE_11B,
						      priv->top_cck_basic_rate);
	buf->duration_aa = vnt_get_rtscts_duration_le(tx_context, RTSDUR_AA,
						      tx_context->pkt_type,
						      current_rate);
	buf->duration_ba = vnt_get_rtscts_duration_le(tx_context, RTSDUR_BA,
						      tx_context->pkt_type,
						      current_rate);

	vnt_fill_ieee80211_rts(tx_context, &buf->data, buf->duration_aa);

	return vnt_rxtx_datahead_g(tx_context, &buf->data_head);
}

static u16 vnt_rxtx_rts_g_fb_head(struct vnt_usb_send_context *tx_context,
				  struct vnt_rts_g_fb *buf)
{
<<<<<<< HEAD
    PSMgmtObject        pMgmt = &(pDevice->sMgmtObj);
    unsigned int cbFrameSize, cbFrameBodySize;
    PTX_BUFFER          pTxBufHead;
    unsigned int cb802_1_H_len;
    unsigned int cbIVlen = 0, cbICVlen = 0, cbMIClen = 0,
	    cbMACHdLen = 0, cbFCSlen = 4;
    unsigned int cbMICHDR = 0;
    BOOL                bNeedACK,bRTS;
    PBYTE               pbyType,pbyMacHdr,pbyIVHead,pbyPayloadHead,pbyTxBufferAddr;
    BYTE abySNAP_RFC1042[ETH_ALEN] = {0xAA, 0xAA, 0x03, 0x00, 0x00, 0x00};
    BYTE abySNAP_Bridgetunnel[ETH_ALEN] = {0xAA, 0xAA, 0x03, 0x00, 0x00, 0xF8};
    unsigned int uDuration;
    unsigned int cbHeaderLength = 0, uPadding = 0;
    void *pvRrvTime;
    PSMICHDRHead        pMICHDR;
    void *pvRTS;
    void *pvCTS;
    void *pvTxDataHd;
    BYTE                byFBOption = AUTO_FB_NONE,byFragType;
    WORD                wTxBufSize;
    DWORD               dwMICKey0,dwMICKey1,dwMIC_Priority,dwCRC;
    PDWORD              pdwMIC_L,pdwMIC_R;
    BOOL                bSoftWEP = FALSE;




    pvRrvTime = pMICHDR = pvRTS = pvCTS = pvTxDataHd = NULL;
    if ((bNeedEncryption) && (pTransmitKey != NULL))  {
        if (((PSKeyTable) (pTransmitKey->pvKeyTable))->bSoftWEP == TRUE) {
            // WEP 256
            bSoftWEP = TRUE;
        }
    }

    pTxBufHead = (PTX_BUFFER) usbPacketBuf;
    memset(pTxBufHead, 0, sizeof(TX_BUFFER));

    // Get pkt type
    if (ntohs(psEthHeader->wType) > ETH_DATA_LEN) {
        if (pDevice->dwDiagRefCount == 0) {
            cb802_1_H_len = 8;
        } else {
            cb802_1_H_len = 2;
        }
    } else {
        cb802_1_H_len = 0;
    }

    cbFrameBodySize = uSkbPacketLen - ETH_HLEN + cb802_1_H_len;

    //Set packet type
    pTxBufHead->wFIFOCtl |= (WORD)(byPktType<<8);

    if (pDevice->dwDiagRefCount != 0) {
        bNeedACK = FALSE;
        pTxBufHead->wFIFOCtl = pTxBufHead->wFIFOCtl & (~FIFOCTL_NEEDACK);
    } else { //if (pDevice->dwDiagRefCount != 0) {
	if ((pDevice->eOPMode == OP_MODE_ADHOC) ||
	    (pDevice->eOPMode == OP_MODE_AP)) {
		if (is_multicast_ether_addr(psEthHeader->abyDstAddr)) {
			bNeedACK = FALSE;
			pTxBufHead->wFIFOCtl =
				pTxBufHead->wFIFOCtl & (~FIFOCTL_NEEDACK);
		} else {
			bNeedACK = TRUE;
			pTxBufHead->wFIFOCtl |= FIFOCTL_NEEDACK;
		}
        }
        else {
            // MSDUs in Infra mode always need ACK
            bNeedACK = TRUE;
            pTxBufHead->wFIFOCtl |= FIFOCTL_NEEDACK;
        }
    } //if (pDevice->dwDiagRefCount != 0) {

    pTxBufHead->wTimeStamp = DEFAULT_MSDU_LIFETIME_RES_64us;

    //Set FIFOCTL_LHEAD
    if (pDevice->bLongHeader)
        pTxBufHead->wFIFOCtl |= FIFOCTL_LHEAD;

    if (pDevice->bSoftwareGenCrcErr) {
        pTxBufHead->wFIFOCtl |= FIFOCTL_CRCDIS; // set tx descriptors to NO hardware CRC
    }

    //Set FRAGCTL_MACHDCNT
    if (pDevice->bLongHeader) {
        cbMACHdLen = WLAN_HDR_ADDR3_LEN + 6;
    } else {
        cbMACHdLen = WLAN_HDR_ADDR3_LEN;
    }
    pTxBufHead->wFragCtl |= (WORD)(cbMACHdLen << 10);

    //Set FIFOCTL_GrpAckPolicy
    if (pDevice->bGrpAckPolicy == TRUE) {//0000 0100 0000 0000
        pTxBufHead->wFIFOCtl |=	FIFOCTL_GRPACK;
    }

    //Set Auto Fallback Ctl
    if (wCurrentRate >= RATE_18M) {
        if (pDevice->byAutoFBCtrl == AUTO_FB_0) {
            pTxBufHead->wFIFOCtl |= FIFOCTL_AUTO_FB_0;
            byFBOption = AUTO_FB_0;
        } else if (pDevice->byAutoFBCtrl == AUTO_FB_1) {
            pTxBufHead->wFIFOCtl |= FIFOCTL_AUTO_FB_1;
            byFBOption = AUTO_FB_1;
        }
    }

    if (bSoftWEP != TRUE) {
        if ((bNeedEncryption) && (pTransmitKey != NULL))  { //WEP enabled
            if (pTransmitKey->byCipherSuite == KEY_CTL_WEP) { //WEP40 or WEP104
                pTxBufHead->wFragCtl |= FRAGCTL_LEGACY;
            }
            if (pTransmitKey->byCipherSuite == KEY_CTL_TKIP) {
                DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"Tx Set wFragCtl == FRAGCTL_TKIP\n");
                pTxBufHead->wFragCtl |= FRAGCTL_TKIP;
            }
            else if (pTransmitKey->byCipherSuite == KEY_CTL_CCMP) { //CCMP
                pTxBufHead->wFragCtl |= FRAGCTL_AES;
            }
        }
    }


    if ((bNeedEncryption) && (pTransmitKey != NULL))  {
        if (pTransmitKey->byCipherSuite == KEY_CTL_WEP) {
            cbIVlen = 4;
            cbICVlen = 4;
        }
        else if (pTransmitKey->byCipherSuite == KEY_CTL_TKIP) {
            cbIVlen = 8;//IV+ExtIV
            cbMIClen = 8;
            cbICVlen = 4;
        }
        if (pTransmitKey->byCipherSuite == KEY_CTL_CCMP) {
            cbIVlen = 8;//RSN Header
            cbICVlen = 8;//MIC
            cbMICHDR = sizeof(SMICHDRHead);
        }
        if (bSoftWEP == FALSE) {
            //MAC Header should be padding 0 to DW alignment.
            uPadding = 4 - (cbMACHdLen%4);
            uPadding %= 4;
        }
    }

    cbFrameSize = cbMACHdLen + cbIVlen + (cbFrameBodySize + cbMIClen) + cbICVlen + cbFCSlen;

    if ( (bNeedACK == FALSE) ||(cbFrameSize < pDevice->wRTSThreshold) ) {
        bRTS = FALSE;
    } else {
        bRTS = TRUE;
        pTxBufHead->wFIFOCtl |= (FIFOCTL_RTS | FIFOCTL_LRETRY);
    }

    pbyTxBufferAddr = (PBYTE) &(pTxBufHead->adwTxKey[0]);
    wTxBufSize = sizeof(STxBufHead);
    if (byPktType == PK_TYPE_11GB || byPktType == PK_TYPE_11GA) {//802.11g packet
        if (byFBOption == AUTO_FB_NONE) {
            if (bRTS == TRUE) {//RTS_need
                pvRrvTime = (PSRrvTime_gRTS) (pbyTxBufferAddr + wTxBufSize);
                pMICHDR = (PSMICHDRHead) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_gRTS));
                pvRTS = (PSRTS_g) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_gRTS) + cbMICHDR);
                pvCTS = NULL;
                pvTxDataHd = (PSTxDataHead_g) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_gRTS) + cbMICHDR + sizeof(SRTS_g));
                cbHeaderLength = wTxBufSize + sizeof(SRrvTime_gRTS) + cbMICHDR + sizeof(SRTS_g) + sizeof(STxDataHead_g);
            }
            else { //RTS_needless
                pvRrvTime = (PSRrvTime_gCTS) (pbyTxBufferAddr + wTxBufSize);
                pMICHDR = (PSMICHDRHead) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_gCTS));
                pvRTS = NULL;
                pvCTS = (PSCTS) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_gCTS) + cbMICHDR);
                pvTxDataHd = (PSTxDataHead_g) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_gCTS) + cbMICHDR + sizeof(SCTS));
                cbHeaderLength = wTxBufSize + sizeof(SRrvTime_gCTS) + cbMICHDR + sizeof(SCTS) + sizeof(STxDataHead_g);
            }
        } else {
            // Auto Fall Back
            if (bRTS == TRUE) {//RTS_need
                pvRrvTime = (PSRrvTime_gRTS) (pbyTxBufferAddr + wTxBufSize);
                pMICHDR = (PSMICHDRHead) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_gRTS));
                pvRTS = (PSRTS_g_FB) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_gRTS) + cbMICHDR);
                pvCTS = NULL;
                pvTxDataHd = (PSTxDataHead_g_FB) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_gRTS) + cbMICHDR + sizeof(SRTS_g_FB));
                cbHeaderLength = wTxBufSize + sizeof(SRrvTime_gRTS) + cbMICHDR + sizeof(SRTS_g_FB) + sizeof(STxDataHead_g_FB);
            }
            else if (bRTS == FALSE) { //RTS_needless
                pvRrvTime = (PSRrvTime_gCTS) (pbyTxBufferAddr + wTxBufSize);
                pMICHDR = (PSMICHDRHead) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_gCTS));
                pvRTS = NULL;
                pvCTS = (PSCTS_FB) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_gCTS) + cbMICHDR);
                pvTxDataHd = (PSTxDataHead_g_FB) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_gCTS) + cbMICHDR + sizeof(SCTS_FB));
                cbHeaderLength = wTxBufSize + sizeof(SRrvTime_gCTS) + cbMICHDR + sizeof(SCTS_FB) + sizeof(STxDataHead_g_FB);
            }
        } // Auto Fall Back
    }
    else {//802.11a/b packet
        if (byFBOption == AUTO_FB_NONE) {
            if (bRTS == TRUE) {//RTS_need
                pvRrvTime = (PSRrvTime_ab) (pbyTxBufferAddr + wTxBufSize);
                pMICHDR = (PSMICHDRHead) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_ab));
                pvRTS = (PSRTS_ab) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_ab) + cbMICHDR);
                pvCTS = NULL;
                pvTxDataHd = (PSTxDataHead_ab) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_ab) + cbMICHDR + sizeof(SRTS_ab));
                cbHeaderLength = wTxBufSize + sizeof(PSRrvTime_ab) + cbMICHDR + sizeof(SRTS_ab) + sizeof(STxDataHead_ab);
            }
            else if (bRTS == FALSE) { //RTS_needless, no MICHDR
                pvRrvTime = (PSRrvTime_ab) (pbyTxBufferAddr + wTxBufSize);
                pMICHDR = (PSMICHDRHead) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_ab));
                pvRTS = NULL;
                pvCTS = NULL;
                pvTxDataHd = (PSTxDataHead_ab) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_ab) + cbMICHDR);
                cbHeaderLength = wTxBufSize + sizeof(SRrvTime_ab) + cbMICHDR + sizeof(STxDataHead_ab);
            }
        } else {
            // Auto Fall Back
            if (bRTS == TRUE) {//RTS_need
                pvRrvTime = (PSRrvTime_ab) (pbyTxBufferAddr + wTxBufSize);
                pMICHDR = (PSMICHDRHead) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_ab));
                pvRTS = (PSRTS_a_FB) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_ab) + cbMICHDR);
                pvCTS = NULL;
                pvTxDataHd = (PSTxDataHead_a_FB) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_ab) + cbMICHDR + sizeof(SRTS_a_FB));
                cbHeaderLength = wTxBufSize + sizeof(PSRrvTime_ab) + cbMICHDR + sizeof(SRTS_a_FB) + sizeof(STxDataHead_a_FB);
            }
            else if (bRTS == FALSE) { //RTS_needless
                pvRrvTime = (PSRrvTime_ab) (pbyTxBufferAddr + wTxBufSize);
                pMICHDR = (PSMICHDRHead) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_ab));
                pvRTS = NULL;
                pvCTS = NULL;
                pvTxDataHd = (PSTxDataHead_a_FB) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_ab) + cbMICHDR);
                cbHeaderLength = wTxBufSize + sizeof(SRrvTime_ab) + cbMICHDR + sizeof(STxDataHead_a_FB);
            }
        } // Auto Fall Back
    }

    pbyMacHdr = (PBYTE)(pbyTxBufferAddr + cbHeaderLength);
    pbyIVHead = (PBYTE)(pbyMacHdr + cbMACHdLen + uPadding);
    pbyPayloadHead = (PBYTE)(pbyMacHdr + cbMACHdLen + uPadding + cbIVlen);


    //=========================
    //    No Fragmentation
    //=========================
    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"No Fragmentation...\n");
    byFragType = FRAGCTL_NONFRAG;
    //uDMAIdx = TYPE_AC0DMA;
    //pTxBufHead = (PSTxBufHead) &(pTxBufHead->adwTxKey[0]);


    //Fill FIFO,RrvTime,RTS,and CTS
    s_vGenerateTxParameter(pDevice, byPktType, wCurrentRate,
			   (void *)pbyTxBufferAddr, pvRrvTime, pvRTS, pvCTS,
                               cbFrameSize, bNeedACK, uDMAIdx, psEthHeader);
    //Fill DataHead
    uDuration = s_uFillDataHead(pDevice, byPktType, wCurrentRate, pvTxDataHd, cbFrameSize, uDMAIdx, bNeedACK,
                                    0, 0, 1/*uMACfragNum*/, byFBOption);
    // Generate TX MAC Header
    s_vGenerateMACHeader(pDevice, pbyMacHdr, (WORD)uDuration, psEthHeader, bNeedEncryption,
                           byFragType, uDMAIdx, 0);

    if (bNeedEncryption == TRUE) {
        //Fill TXKEY
        s_vFillTxKey(pDevice, (PBYTE)(pTxBufHead->adwTxKey), pbyIVHead, pTransmitKey,
                         pbyMacHdr, (WORD)cbFrameBodySize, (PBYTE)pMICHDR);

        if (pDevice->bEnableHostWEP) {
            pMgmt->sNodeDBTable[uNodeIndex].dwTSC47_16 = pTransmitKey->dwTSC47_16;
            pMgmt->sNodeDBTable[uNodeIndex].wTSC15_0 = pTransmitKey->wTSC15_0;
        }
    }

    // 802.1H
    if (ntohs(psEthHeader->wType) > ETH_DATA_LEN) {
	if (pDevice->dwDiagRefCount == 0) {
		if ((psEthHeader->wType == cpu_to_be16(ETH_P_IPX)) ||
		    (psEthHeader->wType == cpu_to_le16(0xF380))) {
			memcpy((PBYTE) (pbyPayloadHead),
			       abySNAP_Bridgetunnel, 6);
            } else {
                memcpy((PBYTE) (pbyPayloadHead), &abySNAP_RFC1042[0], 6);
            }
            pbyType = (PBYTE) (pbyPayloadHead + 6);
            memcpy(pbyType, &(psEthHeader->wType), sizeof(WORD));
        } else {
            memcpy((PBYTE) (pbyPayloadHead), &(psEthHeader->wType), sizeof(WORD));

        }

    }


    if (pPacket != NULL) {
        // Copy the Packet into a tx Buffer
        memcpy((pbyPayloadHead + cb802_1_H_len),
                 (pPacket + ETH_HLEN),
                 uSkbPacketLen - ETH_HLEN
                 );

    } else {
        // while bRelayPacketSend psEthHeader is point to header+payload
        memcpy((pbyPayloadHead + cb802_1_H_len), ((PBYTE)psEthHeader) + ETH_HLEN, uSkbPacketLen - ETH_HLEN);
    }

    ASSERT(uLength == cbNdisBodySize);

    if ((bNeedEncryption == TRUE) && (pTransmitKey != NULL) && (pTransmitKey->byCipherSuite == KEY_CTL_TKIP)) {

        ///////////////////////////////////////////////////////////////////

        if (pDevice->sMgmtObj.eAuthenMode == WMAC_AUTH_WPANONE) {
            dwMICKey0 = *(PDWORD)(&pTransmitKey->abyKey[16]);
            dwMICKey1 = *(PDWORD)(&pTransmitKey->abyKey[20]);
        }
        else if ((pTransmitKey->dwKeyIndex & AUTHENTICATOR_KEY) != 0) {
            dwMICKey0 = *(PDWORD)(&pTransmitKey->abyKey[16]);
            dwMICKey1 = *(PDWORD)(&pTransmitKey->abyKey[20]);
        }
        else {
            dwMICKey0 = *(PDWORD)(&pTransmitKey->abyKey[24]);
            dwMICKey1 = *(PDWORD)(&pTransmitKey->abyKey[28]);
        }
        // DO Software Michael
        MIC_vInit(dwMICKey0, dwMICKey1);
        MIC_vAppend((PBYTE)&(psEthHeader->abyDstAddr[0]), 12);
        dwMIC_Priority = 0;
        MIC_vAppend((PBYTE)&dwMIC_Priority, 4);
	DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"MIC KEY: %X, %X\n",
		dwMICKey0, dwMICKey1);

        ///////////////////////////////////////////////////////////////////

        //DBG_PRN_GRP12(("Length:%d, %d\n", cbFrameBodySize, uFromHDtoPLDLength));
        //for (ii = 0; ii < cbFrameBodySize; ii++) {
        //    DBG_PRN_GRP12(("%02x ", *((PBYTE)((pbyPayloadHead + cb802_1_H_len) + ii))));
        //}
        //DBG_PRN_GRP12(("\n\n\n"));

        MIC_vAppend(pbyPayloadHead, cbFrameBodySize);

        pdwMIC_L = (PDWORD)(pbyPayloadHead + cbFrameBodySize);
        pdwMIC_R = (PDWORD)(pbyPayloadHead + cbFrameBodySize + 4);

        MIC_vGetMIC(pdwMIC_L, pdwMIC_R);
        MIC_vUnInit();

        if (pDevice->bTxMICFail == TRUE) {
            *pdwMIC_L = 0;
            *pdwMIC_R = 0;
            pDevice->bTxMICFail = FALSE;
        }
        //DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"uLength: %d, %d\n", uLength, cbFrameBodySize);
        //DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"cbReqCount:%d, %d, %d, %d\n", cbReqCount, cbHeaderLength, uPadding, cbIVlen);
        //DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"MIC:%lX, %lX\n", *pdwMIC_L, *pdwMIC_R);
    }


    if (bSoftWEP == TRUE) {

        s_vSWencryption(pDevice, pTransmitKey, (pbyPayloadHead), (WORD)(cbFrameBodySize + cbMIClen));

    } else if (  ((pDevice->eEncryptionStatus == Ndis802_11Encryption1Enabled) && (bNeedEncryption == TRUE))  ||
          ((pDevice->eEncryptionStatus == Ndis802_11Encryption2Enabled) && (bNeedEncryption == TRUE))   ||
          ((pDevice->eEncryptionStatus == Ndis802_11Encryption3Enabled) && (bNeedEncryption == TRUE))      ) {
        cbFrameSize -= cbICVlen;
    }

    if (pDevice->bSoftwareGenCrcErr == TRUE) {
	unsigned int cbLen;
        PDWORD pdwCRC;

        dwCRC = 0xFFFFFFFFL;
        cbLen = cbFrameSize - cbFCSlen;
        // calculate CRC, and wrtie CRC value to end of TD
        dwCRC = CRCdwGetCrc32Ex(pbyMacHdr, cbLen, dwCRC);
        pdwCRC = (PDWORD)(pbyMacHdr + cbLen);
        // finally, we must invert dwCRC to get the correct answer
        *pdwCRC = ~dwCRC;
        // Force Error
        *pdwCRC -= 1;
    } else {
        cbFrameSize -= cbFCSlen;
    }
=======
	struct vnt_private *priv = tx_context->priv;
	u16 current_rate = tx_context->tx_rate;
	u16 rts_frame_len = 20;

	vnt_get_phy_field(priv, rts_frame_len, priv->top_cck_basic_rate,
		PK_TYPE_11B, &buf->b);
	vnt_get_phy_field(priv, rts_frame_len, priv->top_ofdm_basic_rate,
			  tx_context->pkt_type, &buf->a);

	buf->duration_bb = vnt_get_rtscts_duration_le(tx_context, RTSDUR_BB,
						      PK_TYPE_11B,
						      priv->top_cck_basic_rate);
	buf->duration_aa = vnt_get_rtscts_duration_le(tx_context, RTSDUR_AA,
						      tx_context->pkt_type,
						      current_rate);
	buf->duration_ba = vnt_get_rtscts_duration_le(tx_context, RTSDUR_BA,
						      tx_context->pkt_type,
						      current_rate);

	buf->rts_duration_ba_f0 =
		vnt_get_rtscts_duration_le(tx_context, RTSDUR_BA_F0,
					   tx_context->pkt_type,
					   priv->tx_rate_fb0);
	buf->rts_duration_aa_f0 =
		vnt_get_rtscts_duration_le(tx_context, RTSDUR_AA_F0,
					   tx_context->pkt_type,
					   priv->tx_rate_fb0);
	buf->rts_duration_ba_f1 =
		vnt_get_rtscts_duration_le(tx_context, RTSDUR_BA_F1,
					   tx_context->pkt_type,
					   priv->tx_rate_fb1);
	buf->rts_duration_aa_f1 =
		vnt_get_rtscts_duration_le(tx_context, RTSDUR_AA_F1,
					   tx_context->pkt_type,
					   priv->tx_rate_fb1);

	vnt_fill_ieee80211_rts(tx_context, &buf->data, buf->duration_aa);

	return vnt_rxtx_datahead_g_fb(tx_context, &buf->data_head);
}
>>>>>>> android-3.18

static u16 vnt_rxtx_rts_ab_head(struct vnt_usb_send_context *tx_context,
				struct vnt_rts_ab *buf)
{
	struct vnt_private *priv = tx_context->priv;
	u16 current_rate = tx_context->tx_rate;
	u16 rts_frame_len = 20;

	vnt_get_phy_field(priv, rts_frame_len, priv->top_ofdm_basic_rate,
			  tx_context->pkt_type, &buf->ab);

	buf->duration = vnt_get_rtscts_duration_le(tx_context, RTSDUR_AA,
						   tx_context->pkt_type,
						   current_rate);

	vnt_fill_ieee80211_rts(tx_context, &buf->data, buf->duration);

	return vnt_rxtx_datahead_ab(tx_context, &buf->data_head);
}

static u16 vnt_rxtx_rts_a_fb_head(struct vnt_usb_send_context *tx_context,
				  struct vnt_rts_a_fb *buf)
{
	struct vnt_private *priv = tx_context->priv;
	u16 current_rate = tx_context->tx_rate;
	u16 rts_frame_len = 20;

	vnt_get_phy_field(priv, rts_frame_len,
		priv->top_ofdm_basic_rate, tx_context->pkt_type, &buf->a);

	buf->duration = vnt_get_rtscts_duration_le(tx_context, RTSDUR_AA,
						   tx_context->pkt_type,
						   current_rate);

	buf->rts_duration_f0 =
		vnt_get_rtscts_duration_le(tx_context, RTSDUR_AA_F0,
					   tx_context->pkt_type,
					   priv->tx_rate_fb0);

	buf->rts_duration_f1 =
		vnt_get_rtscts_duration_le(tx_context, RTSDUR_AA_F1,
					   tx_context->pkt_type,
					   priv->tx_rate_fb1);

	vnt_fill_ieee80211_rts(tx_context, &buf->data, buf->duration);

	return vnt_rxtx_datahead_a_fb(tx_context, &buf->data_head);
}

static u16 vnt_fill_cts_head(struct vnt_usb_send_context *tx_context,
	union vnt_tx_data_head *head)
{
	struct vnt_private *priv = tx_context->priv;
	u32 cts_frame_len = 14;
	u16 current_rate = tx_context->tx_rate;

	if (!head)
		return 0;

	if (tx_context->fb_option) {
		/* Auto Fall back */
		struct vnt_cts_fb *buf = &head->cts_g_fb;
		/* Get SignalField,ServiceField,Length */
		vnt_get_phy_field(priv, cts_frame_len,
			priv->top_cck_basic_rate, PK_TYPE_11B, &buf->b);
		buf->duration_ba =
			vnt_get_rtscts_duration_le(tx_context, CTSDUR_BA,
						   tx_context->pkt_type,
						   current_rate);
		/* Get CTSDuration_ba_f0 */
		buf->cts_duration_ba_f0 =
			vnt_get_rtscts_duration_le(tx_context, CTSDUR_BA_F0,
						   tx_context->pkt_type,
						   priv->tx_rate_fb0);
		/* Get CTSDuration_ba_f1 */
		buf->cts_duration_ba_f1 =
			vnt_get_rtscts_duration_le(tx_context, CTSDUR_BA_F1,
						   tx_context->pkt_type,
						   priv->tx_rate_fb1);
		/* Get CTS Frame body */
		buf->data.duration = buf->duration_ba;
		buf->data.frame_control =
			cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_CTS);

		memcpy(buf->data.ra, priv->current_net_addr, ETH_ALEN);

		return vnt_rxtx_datahead_g_fb(tx_context, &buf->data_head);
	} else {
		struct vnt_cts *buf = &head->cts_g;
		/* Get SignalField,ServiceField,Length */
		vnt_get_phy_field(priv, cts_frame_len,
			priv->top_cck_basic_rate, PK_TYPE_11B, &buf->b);
		/* Get CTSDuration_ba */
		buf->duration_ba =
			vnt_get_rtscts_duration_le(tx_context, CTSDUR_BA,
						   tx_context->pkt_type,
						   current_rate);
		/*Get CTS Frame body*/
		buf->data.duration = buf->duration_ba;
		buf->data.frame_control =
			cpu_to_le16(IEEE80211_FTYPE_CTL | IEEE80211_STYPE_CTS);

		memcpy(buf->data.ra, priv->current_net_addr, ETH_ALEN);

		return vnt_rxtx_datahead_g(tx_context, &buf->data_head);
	}

	return 0;
}

static u16 vnt_rxtx_rts(struct vnt_usb_send_context *tx_context,
			union vnt_tx_head *tx_head, bool need_mic)
{
	struct vnt_private *priv = tx_context->priv;
	struct vnt_rrv_time_rts *buf = &tx_head->tx_rts.rts;
	union vnt_tx_data_head *head = &tx_head->tx_rts.tx.head;
	u32 frame_len = tx_context->frame_len;
	u16 current_rate = tx_context->tx_rate;
	u8 need_ack = tx_context->need_ack;

	buf->rts_rrv_time_aa = vnt_get_rtscts_rsvtime_le(priv, 2,
			tx_context->pkt_type, frame_len, current_rate);
	buf->rts_rrv_time_ba = vnt_get_rtscts_rsvtime_le(priv, 1,
			tx_context->pkt_type, frame_len, current_rate);
	buf->rts_rrv_time_bb = vnt_get_rtscts_rsvtime_le(priv, 0,
			tx_context->pkt_type, frame_len, current_rate);

	buf->rrv_time_a = vnt_rxtx_rsvtime_le16(priv, tx_context->pkt_type,
						frame_len, current_rate,
						need_ack);
	buf->rrv_time_b = vnt_rxtx_rsvtime_le16(priv, PK_TYPE_11B, frame_len,
					priv->top_cck_basic_rate, need_ack);

	if (need_mic)
		head = &tx_head->tx_rts.tx.mic.head;

	if (tx_context->fb_option)
		return vnt_rxtx_rts_g_fb_head(tx_context, &head->rts_g_fb);

	return vnt_rxtx_rts_g_head(tx_context, &head->rts_g);
}

static u16 vnt_rxtx_cts(struct vnt_usb_send_context *tx_context,
			union vnt_tx_head *tx_head, bool need_mic)
{
	struct vnt_private *priv = tx_context->priv;
	struct vnt_rrv_time_cts *buf = &tx_head->tx_cts.cts;
	union vnt_tx_data_head *head = &tx_head->tx_cts.tx.head;
	u32 frame_len = tx_context->frame_len;
	u16 current_rate = tx_context->tx_rate;
	u8 need_ack = tx_context->need_ack;

	buf->rrv_time_a = vnt_rxtx_rsvtime_le16(priv, tx_context->pkt_type,
					frame_len, current_rate, need_ack);
	buf->rrv_time_b = vnt_rxtx_rsvtime_le16(priv, PK_TYPE_11B,
				frame_len, priv->top_cck_basic_rate, need_ack);

	buf->cts_rrv_time_ba = vnt_get_rtscts_rsvtime_le(priv, 3,
			tx_context->pkt_type, frame_len, current_rate);

	if (need_mic)
		head = &tx_head->tx_cts.tx.mic.head;

	/* Fill CTS */
	return vnt_fill_cts_head(tx_context, head);
}

static u16 vnt_rxtx_ab(struct vnt_usb_send_context *tx_context,
		       union vnt_tx_head *tx_head, bool need_rts, bool need_mic)
{
	struct vnt_private *priv = tx_context->priv;
	struct vnt_rrv_time_ab *buf = &tx_head->tx_ab.ab;
	union vnt_tx_data_head *head = &tx_head->tx_ab.tx.head;
	u32 frame_len = tx_context->frame_len;
	u16 current_rate = tx_context->tx_rate;
	u8 need_ack = tx_context->need_ack;

	buf->rrv_time = vnt_rxtx_rsvtime_le16(priv, tx_context->pkt_type,
			frame_len, current_rate, need_ack);

	if (need_mic)
		head = &tx_head->tx_ab.tx.mic.head;

	if (need_rts) {
		if (tx_context->pkt_type == PK_TYPE_11B)
			buf->rts_rrv_time = vnt_get_rtscts_rsvtime_le(priv, 0,
				tx_context->pkt_type, frame_len, current_rate);
		else /* PK_TYPE_11A */
			buf->rts_rrv_time = vnt_get_rtscts_rsvtime_le(priv, 2,
				tx_context->pkt_type, frame_len, current_rate);

		if (tx_context->fb_option &&
		    tx_context->pkt_type == PK_TYPE_11A)
			return vnt_rxtx_rts_a_fb_head(tx_context,
						      &head->rts_a_fb);

		return vnt_rxtx_rts_ab_head(tx_context, &head->rts_ab);
	}

	if (tx_context->pkt_type == PK_TYPE_11A)
		return vnt_rxtx_datahead_a_fb(tx_context,
					      &head->data_head_a_fb);

	return vnt_rxtx_datahead_ab(tx_context, &head->data_head_ab);
}

static u16 vnt_generate_tx_parameter(struct vnt_usb_send_context *tx_context,
	struct vnt_tx_buffer *tx_buffer,
	struct vnt_mic_hdr **mic_hdr, u32 need_mic,
	bool need_rts)
{

	if (tx_context->pkt_type == PK_TYPE_11GB ||
	    tx_context->pkt_type == PK_TYPE_11GA) {
		if (need_rts) {
			if (need_mic)
				*mic_hdr = &tx_buffer->
						tx_head.tx_rts.tx.mic.hdr;

			return vnt_rxtx_rts(tx_context, &tx_buffer->tx_head,
					    need_mic);
		}

		if (need_mic)
			*mic_hdr = &tx_buffer->tx_head.tx_cts.tx.mic.hdr;

		return vnt_rxtx_cts(tx_context, &tx_buffer->tx_head, need_mic);
	}

<<<<<<< HEAD
void
vDMA0_tx_80211(PSDevice  pDevice, struct sk_buff *skb) {

    PSMgmtObject    pMgmt = &(pDevice->sMgmtObj);
    BYTE            byPktType;
    PBYTE           pbyTxBufferAddr;
    void *pvRTS;
    void *pvCTS;
    void *pvTxDataHd;
    unsigned int            uDuration;
    unsigned int            cbReqCount;
    PS802_11Header  pMACHeader;
    unsigned int            cbHeaderSize;
    unsigned int            cbFrameBodySize;
    BOOL            bNeedACK;
    BOOL            bIsPSPOLL = FALSE;
    PSTxBufHead     pTxBufHead;
    unsigned int            cbFrameSize;
    unsigned int            cbIVlen = 0;
    unsigned int            cbICVlen = 0;
    unsigned int            cbMIClen = 0;
    unsigned int            cbFCSlen = 4;
    unsigned int            uPadding = 0;
    unsigned int            cbMICHDR = 0;
    unsigned int            uLength = 0;
    DWORD           dwMICKey0, dwMICKey1;
    DWORD           dwMIC_Priority;
    PDWORD          pdwMIC_L;
    PDWORD          pdwMIC_R;
    WORD            wTxBufSize;
    unsigned int            cbMacHdLen;
    SEthernetHeader sEthHeader;
    void *pvRrvTime;
    void *pMICHDR;
    WORD            wCurrentRate = RATE_1M;
    PUWLAN_80211HDR  p80211Header;
    unsigned int             uNodeIndex = 0;
    BOOL            bNodeExist = FALSE;
    SKeyItem        STempKey;
    PSKeyItem       pTransmitKey = NULL;
    PBYTE           pbyIVHead;
    PBYTE           pbyPayloadHead;
    PBYTE           pbyMacHdr;
    unsigned int            cbExtSuppRate = 0;
    PTX_BUFFER          pTX_Buffer;
    PUSB_SEND_CONTEXT   pContext;
//    PWLAN_IE        pItem;


    pvRrvTime = pMICHDR = pvRTS = pvCTS = pvTxDataHd = NULL;

    if(skb->len <= WLAN_HDR_ADDR3_LEN) {
       cbFrameBodySize = 0;
    }
    else {
       cbFrameBodySize = skb->len - WLAN_HDR_ADDR3_LEN;
    }
    p80211Header = (PUWLAN_80211HDR)skb->data;

    pContext = (PUSB_SEND_CONTEXT)s_vGetFreeContext(pDevice);

    if (NULL == pContext) {
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"DMA0 TX...NO CONTEXT!\n");
        dev_kfree_skb_irq(skb);
        return ;
    }

    pTX_Buffer = (PTX_BUFFER)(&pContext->Data[0]);
    pbyTxBufferAddr = (PBYTE)(&pTX_Buffer->adwTxKey[0]);
    pTxBufHead = (PSTxBufHead) pbyTxBufferAddr;
    wTxBufSize = sizeof(STxBufHead);
    memset(pTxBufHead, 0, wTxBufSize);

    if (pDevice->byBBType == BB_TYPE_11A) {
        wCurrentRate = RATE_6M;
        byPktType = PK_TYPE_11A;
    } else {
        wCurrentRate = RATE_1M;
        byPktType = PK_TYPE_11B;
    }

    // SetPower will cause error power TX state for OFDM Date packet in TX buffer.
    // 2004.11.11 Kyle -- Using OFDM power to tx MngPkt will decrease the connection capability.
    //                    And cmd timer will wait data pkt TX finish before scanning so it's OK
    //                    to set power here.
    if (pMgmt->eScanState != WMAC_NO_SCANNING) {
        RFbSetPower(pDevice, wCurrentRate, pDevice->byCurrentCh);
    } else {
        RFbSetPower(pDevice, wCurrentRate, pMgmt->uCurrChannel);
    }

    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"vDMA0_tx_80211: p80211Header->sA3.wFrameCtl = %x \n", p80211Header->sA3.wFrameCtl);

    //Set packet type
    if (byPktType == PK_TYPE_11A) {//0000 0000 0000 0000
        pTxBufHead->wFIFOCtl = 0;
    }
    else if (byPktType == PK_TYPE_11B) {//0000 0001 0000 0000
        pTxBufHead->wFIFOCtl |= FIFOCTL_11B;
    }
    else if (byPktType == PK_TYPE_11GB) {//0000 0010 0000 0000
        pTxBufHead->wFIFOCtl |= FIFOCTL_11GB;
    }
    else if (byPktType == PK_TYPE_11GA) {//0000 0011 0000 0000
        pTxBufHead->wFIFOCtl |= FIFOCTL_11GA;
    }

    pTxBufHead->wFIFOCtl |= FIFOCTL_TMOEN;
    pTxBufHead->wTimeStamp = cpu_to_le16(DEFAULT_MGN_LIFETIME_RES_64us);

    if (is_multicast_ether_addr(p80211Header->sA3.abyAddr1)) {
        bNeedACK = FALSE;
        if (pDevice->bEnableHostWEP) {
            uNodeIndex = 0;
            bNodeExist = TRUE;
        }
    }
    else {
        if (pDevice->bEnableHostWEP) {
            if (BSSbIsSTAInNodeDB(pDevice, (PBYTE)(p80211Header->sA3.abyAddr1), &uNodeIndex))
                bNodeExist = TRUE;
        }
        bNeedACK = TRUE;
        pTxBufHead->wFIFOCtl |= FIFOCTL_NEEDACK;
    };

    if ((pMgmt->eCurrMode == WMAC_MODE_ESS_AP) ||
        (pMgmt->eCurrMode == WMAC_MODE_IBSS_STA) ) {

        pTxBufHead->wFIFOCtl |= FIFOCTL_LRETRY;
        //Set Preamble type always long
        //pDevice->byPreambleType = PREAMBLE_LONG;

        // probe-response don't retry
        //if ((p80211Header->sA4.wFrameCtl & TYPE_SUBTYPE_MASK) == TYPE_MGMT_PROBE_RSP) {
        //     bNeedACK = FALSE;
        //     pTxBufHead->wFIFOCtl  &= (~FIFOCTL_NEEDACK);
        //}
    }

    pTxBufHead->wFIFOCtl |= (FIFOCTL_GENINT | FIFOCTL_ISDMA0);

    if ((p80211Header->sA4.wFrameCtl & TYPE_SUBTYPE_MASK) == TYPE_CTL_PSPOLL) {
        bIsPSPOLL = TRUE;
        cbMacHdLen = WLAN_HDR_ADDR2_LEN;
    } else {
        cbMacHdLen = WLAN_HDR_ADDR3_LEN;
    }

    // hostapd deamon ext support rate patch
    if (WLAN_GET_FC_FSTYPE(p80211Header->sA4.wFrameCtl) == WLAN_FSTYPE_ASSOCRESP) {

        if (((PWLAN_IE_SUPP_RATES)pMgmt->abyCurrSuppRates)->len != 0) {
            cbExtSuppRate += ((PWLAN_IE_SUPP_RATES)pMgmt->abyCurrSuppRates)->len + WLAN_IEHDR_LEN;
         }

        if (((PWLAN_IE_SUPP_RATES)pMgmt->abyCurrExtSuppRates)->len != 0) {
            cbExtSuppRate += ((PWLAN_IE_SUPP_RATES)pMgmt->abyCurrExtSuppRates)->len + WLAN_IEHDR_LEN;
         }

         if (cbExtSuppRate >0) {
            cbFrameBodySize = WLAN_ASSOCRESP_OFF_SUPP_RATES;
         }
    }


    //Set FRAGCTL_MACHDCNT
    pTxBufHead->wFragCtl |= cpu_to_le16((WORD)cbMacHdLen << 10);

    // Notes:
    // Although spec says MMPDU can be fragmented; In most case,
    // no one will send a MMPDU under fragmentation. With RTS may occur.
    pDevice->bAES = FALSE;  //Set FRAGCTL_WEPTYP


    if (WLAN_GET_FC_ISWEP(p80211Header->sA4.wFrameCtl) != 0) {
        if (pDevice->eEncryptionStatus == Ndis802_11Encryption1Enabled) {
            cbIVlen = 4;
            cbICVlen = 4;
    	    pTxBufHead->wFragCtl |= FRAGCTL_LEGACY;
        }
        else if (pDevice->eEncryptionStatus == Ndis802_11Encryption2Enabled) {
            cbIVlen = 8;//IV+ExtIV
            cbMIClen = 8;
            cbICVlen = 4;
    	    pTxBufHead->wFragCtl |= FRAGCTL_TKIP;
    	    //We need to get seed here for filling TxKey entry.
            //TKIPvMixKey(pTransmitKey->abyKey, pDevice->abyCurrentNetAddr,
            //            pTransmitKey->wTSC15_0, pTransmitKey->dwTSC47_16, pDevice->abyPRNG);
        }
        else if (pDevice->eEncryptionStatus == Ndis802_11Encryption3Enabled) {
            cbIVlen = 8;//RSN Header
            cbICVlen = 8;//MIC
            cbMICHDR = sizeof(SMICHDRHead);
            pTxBufHead->wFragCtl |= FRAGCTL_AES;
            pDevice->bAES = TRUE;
        }
        //MAC Header should be padding 0 to DW alignment.
        uPadding = 4 - (cbMacHdLen%4);
        uPadding %= 4;
    }

    cbFrameSize = cbMacHdLen + cbFrameBodySize + cbIVlen + cbMIClen + cbICVlen + cbFCSlen + cbExtSuppRate;

    //Set FIFOCTL_GrpAckPolicy
    if (pDevice->bGrpAckPolicy == TRUE) {//0000 0100 0000 0000
        pTxBufHead->wFIFOCtl |=	FIFOCTL_GRPACK;
    }
    //the rest of pTxBufHead->wFragCtl:FragTyp will be set later in s_vFillFragParameter()


    if (byPktType == PK_TYPE_11GB || byPktType == PK_TYPE_11GA) {//802.11g packet

        pvRrvTime = (PSRrvTime_gCTS) (pbyTxBufferAddr + wTxBufSize);
        pMICHDR = (PSMICHDRHead) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_gCTS));
        pvRTS = NULL;
        pvCTS = (PSCTS) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_gCTS) + cbMICHDR);
        pvTxDataHd = (PSTxDataHead_g) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_gCTS) + cbMICHDR + sizeof(SCTS));
        cbHeaderSize = wTxBufSize + sizeof(SRrvTime_gCTS) + cbMICHDR + sizeof(SCTS) + sizeof(STxDataHead_g);

    }
    else {//802.11a/b packet

        pvRrvTime = (PSRrvTime_ab) (pbyTxBufferAddr + wTxBufSize);
        pMICHDR = (PSMICHDRHead) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_ab));
        pvRTS = NULL;
        pvCTS = NULL;
        pvTxDataHd = (PSTxDataHead_ab) (pbyTxBufferAddr + wTxBufSize + sizeof(SRrvTime_ab) + cbMICHDR);
        cbHeaderSize = wTxBufSize + sizeof(SRrvTime_ab) + cbMICHDR + sizeof(STxDataHead_ab);
    }
    memset((void *)(pbyTxBufferAddr + wTxBufSize), 0,
	   (cbHeaderSize - wTxBufSize));
    memcpy(&(sEthHeader.abyDstAddr[0]),
	   &(p80211Header->sA3.abyAddr1[0]),
	   ETH_ALEN);
    memcpy(&(sEthHeader.abySrcAddr[0]),
	   &(p80211Header->sA3.abyAddr2[0]),
	   ETH_ALEN);
    //=========================
    //    No Fragmentation
    //=========================
    pTxBufHead->wFragCtl |= (WORD)FRAGCTL_NONFRAG;


    //Fill FIFO,RrvTime,RTS,and CTS
    s_vGenerateTxParameter(pDevice, byPktType, wCurrentRate, pbyTxBufferAddr, pvRrvTime, pvRTS, pvCTS,
                           cbFrameSize, bNeedACK, TYPE_TXDMA0, &sEthHeader);

    //Fill DataHead
    uDuration = s_uFillDataHead(pDevice, byPktType, wCurrentRate, pvTxDataHd, cbFrameSize, TYPE_TXDMA0, bNeedACK,
                                0, 0, 1, AUTO_FB_NONE);

    pMACHeader = (PS802_11Header) (pbyTxBufferAddr + cbHeaderSize);

    cbReqCount = cbHeaderSize + cbMacHdLen + uPadding + cbIVlen + (cbFrameBodySize + cbMIClen) + cbExtSuppRate;

    pbyMacHdr = (PBYTE)(pbyTxBufferAddr + cbHeaderSize);
    pbyPayloadHead = (PBYTE)(pbyMacHdr + cbMacHdLen + uPadding + cbIVlen);
    pbyIVHead = (PBYTE)(pbyMacHdr + cbMacHdLen + uPadding);

    // Copy the Packet into a tx Buffer
    memcpy(pbyMacHdr, skb->data, cbMacHdLen);

    // version set to 0, patch for hostapd deamon
    pMACHeader->wFrameCtl &= cpu_to_le16(0xfffc);
    memcpy(pbyPayloadHead, (skb->data + cbMacHdLen), cbFrameBodySize);

    // replace support rate, patch for hostapd deamon( only support 11M)
    if (WLAN_GET_FC_FSTYPE(p80211Header->sA4.wFrameCtl) == WLAN_FSTYPE_ASSOCRESP) {
        if (cbExtSuppRate != 0) {
            if (((PWLAN_IE_SUPP_RATES)pMgmt->abyCurrSuppRates)->len != 0)
                memcpy((pbyPayloadHead + cbFrameBodySize),
                        pMgmt->abyCurrSuppRates,
                        ((PWLAN_IE_SUPP_RATES)pMgmt->abyCurrSuppRates)->len + WLAN_IEHDR_LEN
                       );
             if (((PWLAN_IE_SUPP_RATES)pMgmt->abyCurrExtSuppRates)->len != 0)
                memcpy((pbyPayloadHead + cbFrameBodySize) + ((PWLAN_IE_SUPP_RATES)pMgmt->abyCurrSuppRates)->len + WLAN_IEHDR_LEN,
                        pMgmt->abyCurrExtSuppRates,
                        ((PWLAN_IE_SUPP_RATES)pMgmt->abyCurrExtSuppRates)->len + WLAN_IEHDR_LEN
                       );
         }
    }

    // Set wep
    if (WLAN_GET_FC_ISWEP(p80211Header->sA4.wFrameCtl) != 0) {

        if (pDevice->bEnableHostWEP) {
            pTransmitKey = &STempKey;
            pTransmitKey->byCipherSuite = pMgmt->sNodeDBTable[uNodeIndex].byCipherSuite;
            pTransmitKey->dwKeyIndex = pMgmt->sNodeDBTable[uNodeIndex].dwKeyIndex;
            pTransmitKey->uKeyLength = pMgmt->sNodeDBTable[uNodeIndex].uWepKeyLength;
            pTransmitKey->dwTSC47_16 = pMgmt->sNodeDBTable[uNodeIndex].dwTSC47_16;
            pTransmitKey->wTSC15_0 = pMgmt->sNodeDBTable[uNodeIndex].wTSC15_0;
            memcpy(pTransmitKey->abyKey,
                &pMgmt->sNodeDBTable[uNodeIndex].abyWepKey[0],
                pTransmitKey->uKeyLength
                );
        }

        if ((pTransmitKey != NULL) && (pTransmitKey->byCipherSuite == KEY_CTL_TKIP)) {

            dwMICKey0 = *(PDWORD)(&pTransmitKey->abyKey[16]);
            dwMICKey1 = *(PDWORD)(&pTransmitKey->abyKey[20]);

            // DO Software Michael
            MIC_vInit(dwMICKey0, dwMICKey1);
            MIC_vAppend((PBYTE)&(sEthHeader.abyDstAddr[0]), 12);
            dwMIC_Priority = 0;
            MIC_vAppend((PBYTE)&dwMIC_Priority, 4);
		DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"DMA0_tx_8021:MIC KEY:"\
			" %X, %X\n", dwMICKey0, dwMICKey1);

            uLength = cbHeaderSize + cbMacHdLen + uPadding + cbIVlen;

            MIC_vAppend((pbyTxBufferAddr + uLength), cbFrameBodySize);

            pdwMIC_L = (PDWORD)(pbyTxBufferAddr + uLength + cbFrameBodySize);
            pdwMIC_R = (PDWORD)(pbyTxBufferAddr + uLength + cbFrameBodySize + 4);

            MIC_vGetMIC(pdwMIC_L, pdwMIC_R);
            MIC_vUnInit();

            if (pDevice->bTxMICFail == TRUE) {
                *pdwMIC_L = 0;
                *pdwMIC_R = 0;
                pDevice->bTxMICFail = FALSE;
            }

            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"uLength: %d, %d\n", uLength, cbFrameBodySize);
            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"cbReqCount:%d, %d, %d, %d\n", cbReqCount, cbHeaderSize, uPadding, cbIVlen);
		DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"MIC:%x, %x\n",
			*pdwMIC_L, *pdwMIC_R);

        }

        s_vFillTxKey(pDevice, (PBYTE)(pTxBufHead->adwTxKey), pbyIVHead, pTransmitKey,
                     pbyMacHdr, (WORD)cbFrameBodySize, (PBYTE)pMICHDR);

        if (pDevice->bEnableHostWEP) {
            pMgmt->sNodeDBTable[uNodeIndex].dwTSC47_16 = pTransmitKey->dwTSC47_16;
            pMgmt->sNodeDBTable[uNodeIndex].wTSC15_0 = pTransmitKey->wTSC15_0;
        }

        if ((pDevice->byLocalID <= REV_ID_VT3253_A1)) {
            s_vSWencryption(pDevice, pTransmitKey, pbyPayloadHead, (WORD)(cbFrameBodySize + cbMIClen));
        }
    }

    pMACHeader->wSeqCtl = cpu_to_le16(pDevice->wSeqCounter << 4);
    pDevice->wSeqCounter++ ;
    if (pDevice->wSeqCounter > 0x0fff)
        pDevice->wSeqCounter = 0;


    if (bIsPSPOLL) {
        // The MAC will automatically replace the Duration-field of MAC header by Duration-field
        // of  FIFO control header.
        // This will cause AID-field of PS-POLL packet be incorrect (Because PS-POLL's AID field is
        // in the same place of other packet's Duration-field).
        // And it will cause Cisco-AP to issue Disassociation-packet
        if (byPktType == PK_TYPE_11GB || byPktType == PK_TYPE_11GA) {
            ((PSTxDataHead_g)pvTxDataHd)->wDuration_a = cpu_to_le16(p80211Header->sA2.wDurationID);
            ((PSTxDataHead_g)pvTxDataHd)->wDuration_b = cpu_to_le16(p80211Header->sA2.wDurationID);
        } else {
            ((PSTxDataHead_ab)pvTxDataHd)->wDuration = cpu_to_le16(p80211Header->sA2.wDurationID);
        }
    }

    pTX_Buffer->wTxByteCount = cpu_to_le16((WORD)(cbReqCount));
    pTX_Buffer->byPKTNO = (BYTE) (((wCurrentRate<<4) &0x00F0) | ((pDevice->wSeqCounter - 1) & 0x000F));
    pTX_Buffer->byType = 0x00;

    pContext->pPacket = skb;
    pContext->Type = CONTEXT_MGMT_PACKET;
    pContext->uBufLen = (WORD)cbReqCount + 4;  //USB header

    if (WLAN_GET_FC_TODS(pMACHeader->wFrameCtl) == 0) {
        s_vSaveTxPktInfo(pDevice, (BYTE) (pTX_Buffer->byPKTNO & 0x0F), &(pMACHeader->abyAddr1[0]),(WORD)cbFrameSize,pTX_Buffer->wFIFOCtl);
    }
    else {
        s_vSaveTxPktInfo(pDevice, (BYTE) (pTX_Buffer->byPKTNO & 0x0F), &(pMACHeader->abyAddr3[0]),(WORD)cbFrameSize,pTX_Buffer->wFIFOCtl);
    }
    PIPEnsSendBulkOut(pDevice,pContext);
    return ;
=======
	if (need_mic)
		*mic_hdr = &tx_buffer->tx_head.tx_ab.tx.mic.hdr;
>>>>>>> android-3.18

	return vnt_rxtx_ab(tx_context, &tx_buffer->tx_head, need_rts, need_mic);
}

static void vnt_fill_txkey(struct vnt_usb_send_context *tx_context,
	u8 *key_buffer, struct ieee80211_key_conf *tx_key, struct sk_buff *skb,
	u16 payload_len, struct vnt_mic_hdr *mic_hdr)
{
	struct ieee80211_hdr *hdr = tx_context->hdr;
	struct ieee80211_key_seq seq;
	u8 *iv = ((u8 *)hdr + ieee80211_get_hdrlen_from_skb(skb));

	/* strip header and icv len from payload */
	payload_len -= ieee80211_get_hdrlen_from_skb(skb);
	payload_len -= tx_key->icv_len;

	switch (tx_key->cipher) {
	case WLAN_CIPHER_SUITE_WEP40:
	case WLAN_CIPHER_SUITE_WEP104:
		memcpy(key_buffer, iv, 3);
		memcpy(key_buffer + 3, tx_key->key, tx_key->keylen);

		if (tx_key->keylen == WLAN_KEY_LEN_WEP40) {
			memcpy(key_buffer + 8, iv, 3);
			memcpy(key_buffer + 11,
					tx_key->key, WLAN_KEY_LEN_WEP40);
		}

		break;
	case WLAN_CIPHER_SUITE_TKIP:
		ieee80211_get_tkip_p2k(tx_key, skb, key_buffer);

		break;
	case WLAN_CIPHER_SUITE_CCMP:

		if (!mic_hdr)
			return;

		mic_hdr->id = 0x59;
		mic_hdr->payload_len = cpu_to_be16(payload_len);
		memcpy(mic_hdr->mic_addr2, hdr->addr2, ETH_ALEN);

		ieee80211_get_key_tx_seq(tx_key, &seq);

		memcpy(mic_hdr->ccmp_pn, seq.ccmp.pn, IEEE80211_CCMP_PN_LEN);

		if (ieee80211_has_a4(hdr->frame_control))
			mic_hdr->hlen = cpu_to_be16(28);
		else
			mic_hdr->hlen = cpu_to_be16(22);

		memcpy(mic_hdr->addr1, hdr->addr1, ETH_ALEN);
		memcpy(mic_hdr->addr2, hdr->addr2, ETH_ALEN);
		memcpy(mic_hdr->addr3, hdr->addr3, ETH_ALEN);

		mic_hdr->frame_control = cpu_to_le16(
			le16_to_cpu(hdr->frame_control) & 0xc78f);
		mic_hdr->seq_ctrl = cpu_to_le16(
				le16_to_cpu(hdr->seq_ctrl) & 0xf);

		if (ieee80211_has_a4(hdr->frame_control))
			memcpy(mic_hdr->addr4, hdr->addr4, ETH_ALEN);


		memcpy(key_buffer, tx_key->key, WLAN_KEY_LEN_CCMP);

		break;
	default:
		break;
	}

<<<<<<< HEAD
int nsDMA_tx_packet(PSDevice pDevice, unsigned int uDMAIdx, struct sk_buff *skb)
{
    PSMgmtObject    pMgmt = &(pDevice->sMgmtObj);
    unsigned int BytesToWrite = 0, uHeaderLen = 0;
    unsigned int            uNodeIndex = 0;
    BYTE            byMask[8] = {1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80};
    WORD            wAID;
    BYTE            byPktType;
    BOOL            bNeedEncryption = FALSE;
    PSKeyItem       pTransmitKey = NULL;
    SKeyItem        STempKey;
    unsigned int            ii;
    BOOL            bTKIP_UseGTK = FALSE;
    BOOL            bNeedDeAuth = FALSE;
    PBYTE           pbyBSSID;
    BOOL            bNodeExist = FALSE;
    PUSB_SEND_CONTEXT pContext;
    BOOL            fConvertedPacket;
    PTX_BUFFER      pTX_Buffer;
    unsigned int            status;
    WORD            wKeepRate = pDevice->wCurrentRate;
    struct net_device_stats* pStats = &pDevice->stats;
     BOOL            bTxeapol_key = FALSE;


    if (pMgmt->eCurrMode == WMAC_MODE_ESS_AP) {

        if (pDevice->uAssocCount == 0) {
            dev_kfree_skb_irq(skb);
            return 0;
        }

	if (is_multicast_ether_addr((PBYTE)(skb->data))) {
            uNodeIndex = 0;
            bNodeExist = TRUE;
            if (pMgmt->sNodeDBTable[0].bPSEnable) {

                skb_queue_tail(&(pMgmt->sNodeDBTable[0].sTxPSQueue), skb);
                pMgmt->sNodeDBTable[0].wEnQueueCnt++;
                // set tx map
                pMgmt->abyPSTxMap[0] |= byMask[0];
                return 0;
            }
            // muticast/broadcast data rate

            if (pDevice->byBBType != BB_TYPE_11A)
                pDevice->wCurrentRate = RATE_2M;
            else
                pDevice->wCurrentRate = RATE_24M;
            // long preamble type
            pDevice->byPreambleType = PREAMBLE_SHORT;

        }else {

            if (BSSbIsSTAInNodeDB(pDevice, (PBYTE)(skb->data), &uNodeIndex)) {

                if (pMgmt->sNodeDBTable[uNodeIndex].bPSEnable) {

                    skb_queue_tail(&pMgmt->sNodeDBTable[uNodeIndex].sTxPSQueue, skb);

                    pMgmt->sNodeDBTable[uNodeIndex].wEnQueueCnt++;
                    // set tx map
                    wAID = pMgmt->sNodeDBTable[uNodeIndex].wAID;
                    pMgmt->abyPSTxMap[wAID >> 3] |=  byMask[wAID & 7];
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO "Set:pMgmt->abyPSTxMap[%d]= %d\n",
                             (wAID >> 3), pMgmt->abyPSTxMap[wAID >> 3]);

                    return 0;
                }
                // AP rate decided from node
                pDevice->wCurrentRate = pMgmt->sNodeDBTable[uNodeIndex].wTxDataRate;
                // tx preamble decided from node

                if (pMgmt->sNodeDBTable[uNodeIndex].bShortPreamble) {
                    pDevice->byPreambleType = pDevice->byShortPreamble;

                }else {
                    pDevice->byPreambleType = PREAMBLE_LONG;
                }
                bNodeExist = TRUE;
            }
        }

        if (bNodeExist == FALSE) {
            DBG_PRT(MSG_LEVEL_DEBUG, KERN_DEBUG"Unknown STA not found in node DB \n");
            dev_kfree_skb_irq(skb);
            return 0;
        }
    }

    pContext = (PUSB_SEND_CONTEXT)s_vGetFreeContext(pDevice);

    if (pContext == NULL) {
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_DEBUG" pContext == NULL\n");
        dev_kfree_skb_irq(skb);
        return STATUS_RESOURCES;
    }

    memcpy(pDevice->sTxEthHeader.abyDstAddr, (PBYTE)(skb->data), ETH_HLEN);

//mike add:station mode check eapol-key challenge--->
{
    BYTE  Protocol_Version;    //802.1x Authentication
    BYTE  Packet_Type;           //802.1x Authentication
    BYTE  Descriptor_type;
    WORD Key_info;

    Protocol_Version = skb->data[ETH_HLEN];
    Packet_Type = skb->data[ETH_HLEN+1];
    Descriptor_type = skb->data[ETH_HLEN+1+1+2];
    Key_info = (skb->data[ETH_HLEN+1+1+2+1] << 8)|(skb->data[ETH_HLEN+1+1+2+2]);
	if (pDevice->sTxEthHeader.wType == cpu_to_be16(ETH_P_PAE)) {
		/* 802.1x OR eapol-key challenge frame transfer */
		if (((Protocol_Version == 1) || (Protocol_Version == 2)) &&
			(Packet_Type == 3)) {
                        bTxeapol_key = TRUE;
                       if(!(Key_info & BIT3) &&  //WPA or RSN group-key challenge
			   (Key_info & BIT8) && (Key_info & BIT9)) {    //send 2/2 key
			  if(Descriptor_type==254) {
                               pDevice->fWPA_Authened = TRUE;
			     PRINT_K("WPA ");
			  }
			  else {
                               pDevice->fWPA_Authened = TRUE;
			     PRINT_K("WPA2(re-keying) ");
			  }
			  PRINT_K("Authentication completed!!\n");
                        }
		    else if((Key_info & BIT3) && (Descriptor_type==2) &&  //RSN pairse-key challenge
			       (Key_info & BIT8) && (Key_info & BIT9)) {
			  pDevice->fWPA_Authened = TRUE;
                            PRINT_K("WPA2 Authentication completed!!\n");
		     }
             }
   }
}
//mike add:station mode check eapol-key challenge<---

    if (pDevice->bEncryptionEnable == TRUE) {
        bNeedEncryption = TRUE;
        // get Transmit key
        do {
            if ((pMgmt->eCurrMode == WMAC_MODE_ESS_STA) &&
                (pMgmt->eCurrState == WMAC_STATE_ASSOC)) {
                pbyBSSID = pDevice->abyBSSID;
                // get pairwise key
                if (KeybGetTransmitKey(&(pDevice->sKey), pbyBSSID, PAIRWISE_KEY, &pTransmitKey) == FALSE) {
                    // get group key
                    if(KeybGetTransmitKey(&(pDevice->sKey), pbyBSSID, GROUP_KEY, &pTransmitKey) == TRUE) {
                        bTKIP_UseGTK = TRUE;
                        DBG_PRT(MSG_LEVEL_DEBUG, KERN_DEBUG"Get GTK.\n");
                        break;
                    }
                } else {
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_DEBUG"Get PTK.\n");
                    break;
                }
            }else if (pMgmt->eCurrMode == WMAC_MODE_IBSS_STA) {

                pbyBSSID = pDevice->sTxEthHeader.abyDstAddr;  //TO_DS = 0 and FROM_DS = 0 --> 802.11 MAC Address1
                DBG_PRT(MSG_LEVEL_DEBUG, KERN_DEBUG"IBSS Serach Key: \n");
                for (ii = 0; ii< 6; ii++)
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_DEBUG"%x \n", *(pbyBSSID+ii));
                DBG_PRT(MSG_LEVEL_DEBUG, KERN_DEBUG"\n");

                // get pairwise key
                if(KeybGetTransmitKey(&(pDevice->sKey), pbyBSSID, PAIRWISE_KEY, &pTransmitKey) == TRUE)
                    break;
            }
            // get group key
            pbyBSSID = pDevice->abyBroadcastAddr;
            if(KeybGetTransmitKey(&(pDevice->sKey), pbyBSSID, GROUP_KEY, &pTransmitKey) == FALSE) {
                pTransmitKey = NULL;
                if (pMgmt->eCurrMode == WMAC_MODE_IBSS_STA) {
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_DEBUG"IBSS and KEY is NULL. [%d]\n", pMgmt->eCurrMode);
                }
                else
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_DEBUG"NOT IBSS and KEY is NULL. [%d]\n", pMgmt->eCurrMode);
            } else {
                bTKIP_UseGTK = TRUE;
                DBG_PRT(MSG_LEVEL_DEBUG, KERN_DEBUG"Get GTK.\n");
            }
        } while(FALSE);
    }

    if (pDevice->bEnableHostWEP) {
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_DEBUG"acdma0: STA index %d\n", uNodeIndex);
        if (pDevice->bEncryptionEnable == TRUE) {
            pTransmitKey = &STempKey;
            pTransmitKey->byCipherSuite = pMgmt->sNodeDBTable[uNodeIndex].byCipherSuite;
            pTransmitKey->dwKeyIndex = pMgmt->sNodeDBTable[uNodeIndex].dwKeyIndex;
            pTransmitKey->uKeyLength = pMgmt->sNodeDBTable[uNodeIndex].uWepKeyLength;
            pTransmitKey->dwTSC47_16 = pMgmt->sNodeDBTable[uNodeIndex].dwTSC47_16;
            pTransmitKey->wTSC15_0 = pMgmt->sNodeDBTable[uNodeIndex].wTSC15_0;
            memcpy(pTransmitKey->abyKey,
                &pMgmt->sNodeDBTable[uNodeIndex].abyWepKey[0],
                pTransmitKey->uKeyLength
                );
         }
    }

    byPktType = (BYTE)pDevice->byPacketType;

    if (pDevice->bFixRate) {
        if (pDevice->byBBType == BB_TYPE_11B) {
            if (pDevice->uConnectionRate >= RATE_11M) {
                pDevice->wCurrentRate = RATE_11M;
            } else {
                pDevice->wCurrentRate = (WORD)pDevice->uConnectionRate;
            }
        } else {
            if ((pDevice->byBBType == BB_TYPE_11A) &&
                (pDevice->uConnectionRate <= RATE_6M)) {
                pDevice->wCurrentRate = RATE_6M;
            } else {
                if (pDevice->uConnectionRate >= RATE_54M)
                    pDevice->wCurrentRate = RATE_54M;
                else
                    pDevice->wCurrentRate = (WORD)pDevice->uConnectionRate;
            }
        }
    }
    else {
        if (pDevice->eOPMode == OP_MODE_ADHOC) {
            // Adhoc Tx rate decided from node DB
	    if (is_multicast_ether_addr(pDevice->sTxEthHeader.abyDstAddr)) {
                // Multicast use highest data rate
                pDevice->wCurrentRate = pMgmt->sNodeDBTable[0].wTxDataRate;
                // preamble type
                pDevice->byPreambleType = pDevice->byShortPreamble;
            }
            else {
                if(BSSbIsSTAInNodeDB(pDevice, &(pDevice->sTxEthHeader.abyDstAddr[0]), &uNodeIndex)) {
                    pDevice->wCurrentRate = pMgmt->sNodeDBTable[uNodeIndex].wTxDataRate;
                    if (pMgmt->sNodeDBTable[uNodeIndex].bShortPreamble) {
                        pDevice->byPreambleType = pDevice->byShortPreamble;

                    }
                    else {
                        pDevice->byPreambleType = PREAMBLE_LONG;
                    }
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"Found Node Index is [%d]  Tx Data Rate:[%d]\n",uNodeIndex, pDevice->wCurrentRate);
                }
                else {
                    if (pDevice->byBBType != BB_TYPE_11A)
                       pDevice->wCurrentRate = RATE_2M;
                    else
                       pDevice->wCurrentRate = RATE_24M; // refer to vMgrCreateOwnIBSS()'s
                                                         // abyCurrExtSuppRates[]
                    pDevice->byPreambleType = PREAMBLE_SHORT;
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"Not Found Node use highest basic Rate.....\n");
                }
            }
        }
        if (pDevice->eOPMode == OP_MODE_INFRASTRUCTURE) {
            // Infra STA rate decided from AP Node, index = 0
            pDevice->wCurrentRate = pMgmt->sNodeDBTable[0].wTxDataRate;
        }
    }

	if (pDevice->sTxEthHeader.wType == cpu_to_be16(ETH_P_PAE)) {
		if (pDevice->byBBType != BB_TYPE_11A) {
			pDevice->wCurrentRate = RATE_1M;
			pDevice->byACKRate = RATE_1M;
			pDevice->byTopCCKBasicRate = RATE_1M;
			pDevice->byTopOFDMBasicRate = RATE_6M;
		} else {
			pDevice->wCurrentRate = RATE_6M;
			pDevice->byACKRate = RATE_6M;
			pDevice->byTopCCKBasicRate = RATE_1M;
			pDevice->byTopOFDMBasicRate = RATE_6M;
		}
	}

    DBG_PRT(MSG_LEVEL_DEBUG,
	    KERN_INFO "dma_tx: pDevice->wCurrentRate = %d\n",
	    pDevice->wCurrentRate);

    if (wKeepRate != pDevice->wCurrentRate) {
	bScheduleCommand((void *) pDevice, WLAN_CMD_SETPOWER, NULL);
    }

    if (pDevice->wCurrentRate <= RATE_11M) {
        byPktType = PK_TYPE_11B;
    }

    if (bNeedEncryption == TRUE) {
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"ntohs Pkt Type=%04x\n", ntohs(pDevice->sTxEthHeader.wType));
	if ((pDevice->sTxEthHeader.wType) == cpu_to_be16(ETH_P_PAE)) {
		bNeedEncryption = FALSE;
            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"Pkt Type=%04x\n", (pDevice->sTxEthHeader.wType));
            if ((pMgmt->eCurrMode == WMAC_MODE_ESS_STA) && (pMgmt->eCurrState == WMAC_STATE_ASSOC)) {
                if (pTransmitKey == NULL) {
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"Don't Find TX KEY\n");
                }
                else {
                    if (bTKIP_UseGTK == TRUE) {
                        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"error: KEY is GTK!!~~\n");
                    }
                    else {
			DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"Find PTK [%X]\n",
				pTransmitKey->dwKeyIndex);
                        bNeedEncryption = TRUE;
                    }
                }
            }

            if (pDevice->byCntMeasure == 2) {
                bNeedDeAuth = TRUE;
                pDevice->s802_11Counter.TKIPCounterMeasuresInvoked++;
            }

            if (pDevice->bEnableHostWEP) {
                if ((uNodeIndex != 0) &&
                    (pMgmt->sNodeDBTable[uNodeIndex].dwKeyIndex & PAIRWISE_KEY)) {
			DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"Find PTK [%X]\n",
				pTransmitKey->dwKeyIndex);
                    bNeedEncryption = TRUE;
                 }
             }
        }
        else {

            if (pTransmitKey == NULL) {
                DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"return no tx key\n");
                dev_kfree_skb_irq(skb);
                pStats->tx_dropped++;
                return STATUS_FAILURE;
            }
        }
    }

    fConvertedPacket = s_bPacketToWirelessUsb(pDevice, byPktType,
                        (PBYTE)(&pContext->Data[0]), bNeedEncryption,
                        skb->len, uDMAIdx, &pDevice->sTxEthHeader,
                        (PBYTE)skb->data, pTransmitKey, uNodeIndex,
                        pDevice->wCurrentRate,
                        &uHeaderLen, &BytesToWrite
                       );

    if (fConvertedPacket == FALSE) {
        pContext->bBoolInUse = FALSE;
        dev_kfree_skb_irq(skb);
        return STATUS_FAILURE;
    }

    if ( pDevice->bEnablePSMode == TRUE ) {
        if ( !pDevice->bPSModeTxBurst ) {
		bScheduleCommand((void *) pDevice,
				 WLAN_CMD_MAC_DISPOWERSAVING,
				 NULL);
            pDevice->bPSModeTxBurst = TRUE;
        }
    }

    pTX_Buffer = (PTX_BUFFER)&(pContext->Data[0]);
    pTX_Buffer->byPKTNO = (BYTE) (((pDevice->wCurrentRate<<4) &0x00F0) | ((pDevice->wSeqCounter - 1) & 0x000F));
    pTX_Buffer->wTxByteCount = (WORD)BytesToWrite;

    pContext->pPacket = skb;
    pContext->Type = CONTEXT_DATA_PACKET;
    pContext->uBufLen = (WORD)BytesToWrite + 4 ; //USB header

    s_vSaveTxPktInfo(pDevice, (BYTE) (pTX_Buffer->byPKTNO & 0x0F), &(pContext->sEthHeader.abyDstAddr[0]),(WORD) (BytesToWrite-uHeaderLen),pTX_Buffer->wFIFOCtl);

    status = PIPEnsSendBulkOut(pDevice,pContext);

    if (bNeedDeAuth == TRUE) {
        WORD wReason = WLAN_MGMT_REASON_MIC_FAILURE;

	bScheduleCommand((void *) pDevice, WLAN_CMD_DEAUTH, (PBYTE) &wReason);
    }

  if(status!=STATUS_PENDING) {
     pContext->bBoolInUse = FALSE;
    dev_kfree_skb_irq(skb);
    return STATUS_FAILURE;
  }
  else
    return 0;
=======
}
>>>>>>> android-3.18

int vnt_tx_packet(struct vnt_private *priv, struct sk_buff *skb)
{
	struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
	struct ieee80211_tx_rate *tx_rate = &info->control.rates[0];
	struct ieee80211_rate *rate;
	struct ieee80211_key_conf *tx_key;
	struct ieee80211_hdr *hdr;
	struct vnt_mic_hdr *mic_hdr = NULL;
	struct vnt_tx_buffer *tx_buffer;
	struct vnt_tx_fifo_head *tx_buffer_head;
	struct vnt_usb_send_context *tx_context;
	unsigned long flags;
	u16 tx_bytes, tx_header_size, tx_body_size, current_rate, duration_id;
	u8 pkt_type, fb_option = AUTO_FB_NONE;
	bool need_rts = false, is_pspoll = false;
	bool need_mic = false;

	hdr = (struct ieee80211_hdr *)(skb->data);

	rate = ieee80211_get_tx_rate(priv->hw, info);

	current_rate = rate->hw_value;
	if (priv->current_rate != current_rate &&
			!(priv->hw->conf.flags & IEEE80211_CONF_OFFCHANNEL)) {
		priv->current_rate = current_rate;
		vnt_schedule_command(priv, WLAN_CMD_SETPOWER);
	}

	if (current_rate > RATE_11M) {
		if (info->band == IEEE80211_BAND_5GHZ) {
			pkt_type = PK_TYPE_11A;
		} else {
			if (tx_rate->flags & IEEE80211_TX_RC_USE_CTS_PROTECT) {
				if (priv->basic_rates & VNT_B_RATES)
					pkt_type = PK_TYPE_11GB;
				else
					pkt_type = PK_TYPE_11GA;
			} else {
				pkt_type = PK_TYPE_11A;
			}
		}
	} else {
		pkt_type = PK_TYPE_11B;
	}

	spin_lock_irqsave(&priv->lock, flags);

	tx_context = vnt_get_free_context(priv);
	if (!tx_context) {
		dev_dbg(&priv->usb->dev, "%s No free context\n", __func__);
		spin_unlock_irqrestore(&priv->lock, flags);
		return -ENOMEM;
	}

	tx_context->skb = skb;
	tx_context->pkt_type = pkt_type;
	tx_context->need_ack = false;
	tx_context->frame_len = skb->len + 4;
	tx_context->tx_rate = current_rate;

	spin_unlock_irqrestore(&priv->lock, flags);

	tx_buffer = (struct vnt_tx_buffer *)tx_context->data;
	tx_buffer_head = &tx_buffer->fifo_head;
	tx_body_size = skb->len;

	/*Set fifo controls */
	if (pkt_type == PK_TYPE_11A)
		tx_buffer_head->fifo_ctl = 0;
	else if (pkt_type == PK_TYPE_11B)
		tx_buffer_head->fifo_ctl = cpu_to_le16(FIFOCTL_11B);
	else if (pkt_type == PK_TYPE_11GB)
		tx_buffer_head->fifo_ctl = cpu_to_le16(FIFOCTL_11GB);
	else if (pkt_type == PK_TYPE_11GA)
		tx_buffer_head->fifo_ctl = cpu_to_le16(FIFOCTL_11GA);

	if (!ieee80211_is_data(hdr->frame_control)) {
		tx_buffer_head->fifo_ctl |= cpu_to_le16(FIFOCTL_GENINT |
							FIFOCTL_ISDMA0);
		tx_buffer_head->fifo_ctl |= cpu_to_le16(FIFOCTL_TMOEN);

		tx_buffer_head->time_stamp =
			cpu_to_le16(DEFAULT_MGN_LIFETIME_RES_64us);
	} else {
		tx_buffer_head->time_stamp =
			cpu_to_le16(DEFAULT_MSDU_LIFETIME_RES_64us);
	}

	if (!(info->flags & IEEE80211_TX_CTL_NO_ACK)) {
		tx_buffer_head->fifo_ctl |= cpu_to_le16(FIFOCTL_NEEDACK);
		tx_context->need_ack = true;
	}

	if (ieee80211_has_retry(hdr->frame_control))
		tx_buffer_head->fifo_ctl |= cpu_to_le16(FIFOCTL_LRETRY);

	if (tx_rate->flags & IEEE80211_TX_RC_USE_SHORT_PREAMBLE)
		priv->preamble_type = PREAMBLE_SHORT;
	else
		priv->preamble_type = PREAMBLE_LONG;

	if (tx_rate->flags & IEEE80211_TX_RC_USE_RTS_CTS) {
		need_rts = true;
		tx_buffer_head->fifo_ctl |= cpu_to_le16(FIFOCTL_RTS);
	}

	if (ieee80211_has_a4(hdr->frame_control))
		tx_buffer_head->fifo_ctl |= cpu_to_le16(FIFOCTL_LHEAD);

	if (info->flags & IEEE80211_TX_CTL_NO_PS_BUFFER)
		is_pspoll = true;

	tx_buffer_head->frag_ctl =
			cpu_to_le16(ieee80211_get_hdrlen_from_skb(skb) << 10);

	if (info->control.hw_key) {
		tx_key = info->control.hw_key;
		switch (info->control.hw_key->cipher) {
		case WLAN_CIPHER_SUITE_WEP40:
		case WLAN_CIPHER_SUITE_WEP104:
			tx_buffer_head->frag_ctl |= cpu_to_le16(FRAGCTL_LEGACY);
			break;
		case WLAN_CIPHER_SUITE_TKIP:
			tx_buffer_head->frag_ctl |= cpu_to_le16(FRAGCTL_TKIP);
			break;
		case WLAN_CIPHER_SUITE_CCMP:
			tx_buffer_head->frag_ctl |= cpu_to_le16(FRAGCTL_AES);
			need_mic = true;
		default:
			break;
		}
		tx_context->frame_len += tx_key->icv_len;
	}

	tx_buffer_head->current_rate = cpu_to_le16(current_rate);

	/* legacy rates TODO use ieee80211_tx_rate */
	if (current_rate >= RATE_18M && ieee80211_is_data(hdr->frame_control)) {
		if (priv->auto_fb_ctrl == AUTO_FB_0) {
			tx_buffer_head->fifo_ctl |=
						cpu_to_le16(FIFOCTL_AUTO_FB_0);

			priv->tx_rate_fb0 =
				vnt_fb_opt0[FB_RATE0][current_rate - RATE_18M];
			priv->tx_rate_fb1 =
				vnt_fb_opt0[FB_RATE1][current_rate - RATE_18M];

			fb_option = AUTO_FB_0;
		} else if (priv->auto_fb_ctrl == AUTO_FB_1) {
			tx_buffer_head->fifo_ctl |=
						cpu_to_le16(FIFOCTL_AUTO_FB_1);

			priv->tx_rate_fb0 =
				vnt_fb_opt1[FB_RATE0][current_rate - RATE_18M];
			priv->tx_rate_fb1 =
				vnt_fb_opt1[FB_RATE1][current_rate - RATE_18M];

			fb_option = AUTO_FB_1;
		}
	}

	tx_context->fb_option = fb_option;

	duration_id = vnt_generate_tx_parameter(tx_context, tx_buffer, &mic_hdr,
						need_mic, need_rts);

	tx_header_size = tx_context->tx_hdr_size;
	if (!tx_header_size) {
		tx_context->in_use = false;
		return -ENOMEM;
	}

	tx_buffer_head->frag_ctl |= cpu_to_le16(FRAGCTL_NONFRAG);

	tx_bytes = tx_header_size + tx_body_size;

	memcpy(tx_context->hdr, skb->data, tx_body_size);

	hdr->duration_id = cpu_to_le16(duration_id);

	if (info->control.hw_key) {
		tx_key = info->control.hw_key;
		if (tx_key->keylen > 0)
			vnt_fill_txkey(tx_context, tx_buffer_head->tx_key,
				tx_key, skb, tx_body_size, mic_hdr);
	}

	priv->seq_counter = (le16_to_cpu(hdr->seq_ctrl) &
						IEEE80211_SCTL_SEQ) >> 4;

	tx_buffer->tx_byte_count = cpu_to_le16(tx_bytes);
	tx_buffer->pkt_no = tx_context->pkt_no;
	tx_buffer->type = 0x00;

	tx_bytes += 4;

	tx_context->type = CONTEXT_DATA_PACKET;
	tx_context->buf_len = tx_bytes;

	spin_lock_irqsave(&priv->lock, flags);

	if (vnt_tx_context(priv, tx_context) != STATUS_PENDING) {
		spin_unlock_irqrestore(&priv->lock, flags);
		return -EIO;
	}

	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

static int vnt_beacon_xmit(struct vnt_private *priv,
	struct sk_buff *skb)
{
	struct vnt_beacon_buffer *beacon_buffer;
	struct vnt_tx_short_buf_head *short_head;
	struct ieee80211_tx_info *info;
	struct vnt_usb_send_context *context;
	struct ieee80211_mgmt *mgmt_hdr;
	unsigned long flags;
	u32 frame_size = skb->len + 4;
	u16 current_rate, count;

	spin_lock_irqsave(&priv->lock, flags);

	context = vnt_get_free_context(priv);
	if (!context) {
		dev_dbg(&priv->usb->dev, "%s No free context!\n", __func__);
		spin_unlock_irqrestore(&priv->lock, flags);
		return -ENOMEM;
	}

	context->skb = skb;

	spin_unlock_irqrestore(&priv->lock, flags);

	beacon_buffer = (struct vnt_beacon_buffer *)&context->data[0];
	short_head = &beacon_buffer->short_head;

	if (priv->bb_type == BB_TYPE_11A) {
		current_rate = RATE_6M;

		/* Get SignalField,ServiceField,Length */
		vnt_get_phy_field(priv, frame_size, current_rate,
			PK_TYPE_11A, &short_head->ab);

		/* Get Duration and TimeStampOff */
		short_head->duration = vnt_get_duration_le(priv,
							PK_TYPE_11A, false);
		short_head->time_stamp_off =
				vnt_time_stamp_off(priv, current_rate);
	} else {
		current_rate = RATE_1M;
		short_head->fifo_ctl |= cpu_to_le16(FIFOCTL_11B);

		/* Get SignalField,ServiceField,Length */
		vnt_get_phy_field(priv, frame_size, current_rate,
					PK_TYPE_11B, &short_head->ab);

		/* Get Duration and TimeStampOff */
		short_head->duration = vnt_get_duration_le(priv,
						PK_TYPE_11B, false);
		short_head->time_stamp_off =
			vnt_time_stamp_off(priv, current_rate);
	}

	/* Generate Beacon Header */
	mgmt_hdr = &beacon_buffer->mgmt_hdr;
	memcpy(mgmt_hdr, skb->data, skb->len);

	/* time stamp always 0 */
	mgmt_hdr->u.beacon.timestamp = 0;

	info = IEEE80211_SKB_CB(skb);
	if (info->flags & IEEE80211_TX_CTL_ASSIGN_SEQ) {
		struct ieee80211_hdr *hdr = (struct ieee80211_hdr *)mgmt_hdr;

		hdr->duration_id = 0;
		hdr->seq_ctrl = cpu_to_le16(priv->seq_counter << 4);
	}

	priv->seq_counter++;
	if (priv->seq_counter > 0x0fff)
		priv->seq_counter = 0;

	count = sizeof(struct vnt_tx_short_buf_head) + skb->len;

	beacon_buffer->tx_byte_count = cpu_to_le16(count);
	beacon_buffer->pkt_no = context->pkt_no;
	beacon_buffer->type = 0x01;

	context->type = CONTEXT_BEACON_PACKET;
	context->buf_len = count + 4; /* USB header */

	spin_lock_irqsave(&priv->lock, flags);

	if (vnt_tx_context(priv, context) != STATUS_PENDING)
		ieee80211_free_txskb(priv->hw, context->skb);

	spin_unlock_irqrestore(&priv->lock, flags);

	return 0;
}

int vnt_beacon_make(struct vnt_private *priv, struct ieee80211_vif *vif)
{
	struct sk_buff *beacon;

	beacon = ieee80211_beacon_get(priv->hw, vif);
	if (!beacon)
		return -ENOMEM;

	if (vnt_beacon_xmit(priv, beacon)) {
		ieee80211_free_txskb(priv->hw, beacon);
		return -ENODEV;
	}

	return 0;
}

int vnt_beacon_enable(struct vnt_private *priv, struct ieee80211_vif *vif,
	struct ieee80211_bss_conf *conf)
{
	vnt_mac_reg_bits_off(priv, MAC_REG_TCR, TCR_AUTOBCNTX);

	vnt_mac_reg_bits_off(priv, MAC_REG_TFTCTL, TFTCTL_TSFCNTREN);

	vnt_mac_set_beacon_interval(priv, conf->beacon_int);

	vnt_clear_current_tsf(priv);

	vnt_mac_reg_bits_on(priv, MAC_REG_TFTCTL, TFTCTL_TSFCNTREN);

	vnt_reset_next_tbtt(priv, conf->beacon_int);

	return vnt_beacon_make(priv, vif);
}
