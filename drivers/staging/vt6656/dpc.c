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
 * File: dpc.c
 *
 * Purpose: handle dpc rx functions
 *
 * Author: Lyndon Chen
 *
 * Date: May 20, 2003
 *
 * Functions:
 *
 * Revision History:
 *
 */

#include "dpc.h"
#include "device.h"
#include "mac.h"
#include "baseband.h"
#include "rf.h"

int vnt_rx_data(struct vnt_private *priv, struct vnt_rcb *ptr_rcb,
	unsigned long bytes_received)
{
<<<<<<< HEAD
    PBYTE           pbyRxBuffer;
    unsigned int            cbHeaderSize = 0;
    PWORD           pwType;
    PS802_11Header  pMACHeader;
    int             ii;


    pMACHeader = (PS802_11Header) (pbyRxBufferAddr + cbHeaderSize);

    s_vGetDASA((PBYTE)pMACHeader, &cbHeaderSize, &pDevice->sRxEthHeader);

    if (bIsWEP) {
        if (bExtIV) {
            // strip IV&ExtIV , add 8 byte
            cbHeaderSize += (WLAN_HDR_ADDR3_LEN + 8);
        } else {
            // strip IV , add 4 byte
            cbHeaderSize += (WLAN_HDR_ADDR3_LEN + 4);
        }
    }
    else {
        cbHeaderSize += WLAN_HDR_ADDR3_LEN;
    };

    pbyRxBuffer = (PBYTE) (pbyRxBufferAddr + cbHeaderSize);
    if (!compare_ether_addr(pbyRxBuffer, &pDevice->abySNAP_Bridgetunnel[0])) {
        cbHeaderSize += 6;
    } else if (!compare_ether_addr(pbyRxBuffer, &pDevice->abySNAP_RFC1042[0])) {
        cbHeaderSize += 6;
        pwType = (PWORD) (pbyRxBufferAddr + cbHeaderSize);
	if ((*pwType == cpu_to_be16(ETH_P_IPX)) ||
	    (*pwType == cpu_to_le16(0xF380))) {
		cbHeaderSize -= 8;
            pwType = (PWORD) (pbyRxBufferAddr + cbHeaderSize);
            if (bIsWEP) {
                if (bExtIV) {
                    *pwType = htons(cbPacketSize - WLAN_HDR_ADDR3_LEN - 8);    // 8 is IV&ExtIV
                } else {
                    *pwType = htons(cbPacketSize - WLAN_HDR_ADDR3_LEN - 4);    // 4 is IV
                }
            }
            else {
                *pwType = htons(cbPacketSize - WLAN_HDR_ADDR3_LEN);
            }
        }
    }
    else {
        cbHeaderSize -= 2;
        pwType = (PWORD) (pbyRxBufferAddr + cbHeaderSize);
        if (bIsWEP) {
            if (bExtIV) {
                *pwType = htons(cbPacketSize - WLAN_HDR_ADDR3_LEN - 8);    // 8 is IV&ExtIV
            } else {
                *pwType = htons(cbPacketSize - WLAN_HDR_ADDR3_LEN - 4);    // 4 is IV
            }
        }
        else {
            *pwType = htons(cbPacketSize - WLAN_HDR_ADDR3_LEN);
        }
    }

    cbHeaderSize -= (ETH_ALEN * 2);
    pbyRxBuffer = (PBYTE) (pbyRxBufferAddr + cbHeaderSize);
    for (ii = 0; ii < ETH_ALEN; ii++)
        *pbyRxBuffer++ = pDevice->sRxEthHeader.abyDstAddr[ii];
    for (ii = 0; ii < ETH_ALEN; ii++)
        *pbyRxBuffer++ = pDevice->sRxEthHeader.abySrcAddr[ii];

    *pcbHeadSize = cbHeaderSize;
}




static BYTE s_byGetRateIdx(BYTE byRate)
{
    BYTE    byRateIdx;

    for (byRateIdx = 0; byRateIdx <MAX_RATE ; byRateIdx++) {
        if (acbyRxRate[byRateIdx%MAX_RATE] == byRate)
            return byRateIdx;
    }
    return 0;
}


static
void
s_vGetDASA (
      PBYTE pbyRxBufferAddr,
     unsigned int *pcbHeaderSize,
     PSEthernetHeader psEthHeader
    )
{
	unsigned int            cbHeaderSize = 0;
	PS802_11Header  pMACHeader;
	int             ii;

	pMACHeader = (PS802_11Header) (pbyRxBufferAddr + cbHeaderSize);

	if ((pMACHeader->wFrameCtl & FC_TODS) == 0) {
		if (pMACHeader->wFrameCtl & FC_FROMDS) {
			for (ii = 0; ii < ETH_ALEN; ii++) {
				psEthHeader->abyDstAddr[ii] =
					pMACHeader->abyAddr1[ii];
				psEthHeader->abySrcAddr[ii] =
					pMACHeader->abyAddr3[ii];
			}
		} else {
			/* IBSS mode */
			for (ii = 0; ii < ETH_ALEN; ii++) {
				psEthHeader->abyDstAddr[ii] =
					pMACHeader->abyAddr1[ii];
				psEthHeader->abySrcAddr[ii] =
					pMACHeader->abyAddr2[ii];
			}
=======
	struct ieee80211_hw *hw = priv->hw;
	struct ieee80211_supported_band *sband;
	struct sk_buff *skb;
	struct ieee80211_rx_status rx_status = { 0 };
	struct ieee80211_hdr *hdr;
	__le16 fc;
	u8 *rsr, *new_rsr, *rssi, *frame;
	__le64 *tsf_time;
	u32 frame_size;
	int ii, r;
	u8 *rx_sts, *rx_rate, *sq, *sq_3;
	u32 wbk_status;
	u8 *skb_data;
	u16 *pay_load_len;
	u16 pay_load_with_padding;
	u8 rate_idx = 0;
	u8 rate[MAX_RATE] = {2, 4, 11, 22, 12, 18, 24, 36, 48, 72, 96, 108};
	long rx_dbm;

	skb = ptr_rcb->skb;

	/* [31:16]RcvByteCount ( not include 4-byte Status ) */
	wbk_status = *((u32 *)(skb->data));
	frame_size = wbk_status >> 16;
	frame_size += 4;

	if (bytes_received != frame_size) {
		dev_dbg(&priv->usb->dev, "------- WRONG Length 1\n");
		return false;
	}

	if ((bytes_received > 2372) || (bytes_received <= 40)) {
		/* Frame Size error drop this packet.*/
		dev_dbg(&priv->usb->dev, "------ WRONG Length 2\n");
		return false;
	}

	skb_data = (u8 *)skb->data;

	rx_sts = skb_data+4;
	rx_rate = skb_data+5;

	/* real Frame Size = USBframe_size -4WbkStatus - 4RxStatus */
	/* -8TSF - 4RSR - 4SQ3 - ?Padding */

	/* if SQ3 the range is 24~27, if no SQ3 the range is 20~23 */

	pay_load_len = (u16 *) (skb_data + 6);

	/*Fix hardware bug => PLCP_Length error */
	if (((bytes_received - (*pay_load_len)) > 27) ||
		((bytes_received - (*pay_load_len)) < 24) ||
			(bytes_received < (*pay_load_len))) {
		dev_dbg(&priv->usb->dev, "Wrong PLCP Length %x\n",
							*pay_load_len);
		return false;
	}

	sband = hw->wiphy->bands[hw->conf.chandef.chan->band];

	for (r = RATE_1M; r < MAX_RATE; r++) {
		if (*rx_rate == rate[r])
			break;
	}

	priv->rx_rate = r;

	for (ii = 0; ii < sband->n_bitrates; ii++) {
		if (sband->bitrates[ii].hw_value == r) {
			rate_idx = ii;
				break;
>>>>>>> android-3.18
		}
	}

	if (ii == sband->n_bitrates) {
		dev_dbg(&priv->usb->dev, "Wrong RxRate %x\n", *rx_rate);
		return false;
	}

	pay_load_with_padding = ((*pay_load_len / 4) +
		((*pay_load_len % 4) ? 1 : 0)) * 4;

	tsf_time = (__le64 *)(skb_data + 8 + pay_load_with_padding);

	priv->tsf_time = le64_to_cpu(*tsf_time);

	if (priv->bb_type == BB_TYPE_11G) {
		sq_3 = skb_data + 8 + pay_load_with_padding + 12;
		sq = sq_3;
	} else {
		sq = skb_data + 8 + pay_load_with_padding + 8;
		sq_3 = sq;
	}

	new_rsr = skb_data + 8 + pay_load_with_padding + 9;
	rssi = skb_data + 8 + pay_load_with_padding + 10;

	rsr = skb_data + 8 + pay_load_with_padding + 11;
	if (*rsr & (RSR_IVLDTYP | RSR_IVLDLEN))
		return false;

	frame_size = *pay_load_len;

	vnt_rf_rssi_to_dbm(priv, *rssi, &rx_dbm);

	priv->bb_pre_ed_rssi = (u8)-rx_dbm + 1;
	priv->current_rssi = priv->bb_pre_ed_rssi;

	frame = skb_data + 8;

	skb_pull(skb, 8);
	skb_trim(skb, frame_size);

	rx_status.mactime = priv->tsf_time;
	rx_status.band = hw->conf.chandef.chan->band;
	rx_status.signal = rx_dbm;
	rx_status.flag = 0;
	rx_status.freq = hw->conf.chandef.chan->center_freq;

	if (!(*rsr & RSR_CRCOK))
		rx_status.flag |= RX_FLAG_FAILED_FCS_CRC;

	hdr = (struct ieee80211_hdr *)(skb->data);
	fc = hdr->frame_control;

	rx_status.rate_idx = rate_idx;

	if (ieee80211_has_protected(fc)) {
		if (priv->local_id > REV_ID_VT3253_A1) {
			rx_status.flag |= RX_FLAG_DECRYPTED;

			/* Drop packet */
			if (!(*new_rsr & NEWRSR_DECRYPTOK)) {
				dev_kfree_skb(skb);
				return true;
			}
		}
	}

	memcpy(IEEE80211_SKB_RXCB(skb), &rx_status, sizeof(rx_status));

	ieee80211_rx_irqsafe(priv->hw, skb);

	return true;
}
<<<<<<< HEAD

static BOOL s_bHandleRxEncryption (
     PSDevice     pDevice,
     PBYTE        pbyFrame,
     unsigned int         FrameSize,
     PBYTE        pbyRsr,
     PBYTE       pbyNewRsr,
     PSKeyItem   * pKeyOut,
    int *       pbExtIV,
     PWORD       pwRxTSC15_0,
     PDWORD      pdwRxTSC47_16
    )
{
    unsigned int            PayloadLen = FrameSize;
    PBYTE           pbyIV;
    BYTE            byKeyIdx;
    PSKeyItem       pKey = NULL;
    BYTE            byDecMode = KEY_CTL_WEP;
    PSMgmtObject    pMgmt = &(pDevice->sMgmtObj);


    *pwRxTSC15_0 = 0;
    *pdwRxTSC47_16 = 0;

    pbyIV = pbyFrame + WLAN_HDR_ADDR3_LEN;
    if ( WLAN_GET_FC_TODS(*(PWORD)pbyFrame) &&
         WLAN_GET_FC_FROMDS(*(PWORD)pbyFrame) ) {
         pbyIV += 6;             // 6 is 802.11 address4
         PayloadLen -= 6;
    }
    byKeyIdx = (*(pbyIV+3) & 0xc0);
    byKeyIdx >>= 6;
    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"\nKeyIdx: %d\n", byKeyIdx);

    if ((pMgmt->eAuthenMode == WMAC_AUTH_WPA) ||
        (pMgmt->eAuthenMode == WMAC_AUTH_WPAPSK) ||
        (pMgmt->eAuthenMode == WMAC_AUTH_WPANONE) ||
        (pMgmt->eAuthenMode == WMAC_AUTH_WPA2) ||
        (pMgmt->eAuthenMode == WMAC_AUTH_WPA2PSK)) {
        if (((*pbyRsr & (RSR_ADDRBROAD | RSR_ADDRMULTI)) == 0) &&
            (pMgmt->byCSSPK != KEY_CTL_NONE)) {
            // unicast pkt use pairwise key
            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"unicast pkt\n");
            if (KeybGetKey(&(pDevice->sKey), pDevice->abyBSSID, 0xFFFFFFFF, &pKey) == TRUE) {
                if (pMgmt->byCSSPK == KEY_CTL_TKIP)
                    byDecMode = KEY_CTL_TKIP;
                else if (pMgmt->byCSSPK == KEY_CTL_CCMP)
                    byDecMode = KEY_CTL_CCMP;
            }
            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"unicast pkt: %d, %p\n", byDecMode, pKey);
        } else {
            // use group key
            KeybGetKey(&(pDevice->sKey), pDevice->abyBSSID, byKeyIdx, &pKey);
            if (pMgmt->byCSSGK == KEY_CTL_TKIP)
                byDecMode = KEY_CTL_TKIP;
            else if (pMgmt->byCSSGK == KEY_CTL_CCMP)
                byDecMode = KEY_CTL_CCMP;
            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"group pkt: %d, %d, %p\n", byKeyIdx, byDecMode, pKey);
        }
    }
    // our WEP only support Default Key
    if (pKey == NULL) {
        // use default group key
        KeybGetKey(&(pDevice->sKey), pDevice->abyBroadcastAddr, byKeyIdx, &pKey);
        if (pMgmt->byCSSGK == KEY_CTL_TKIP)
            byDecMode = KEY_CTL_TKIP;
        else if (pMgmt->byCSSGK == KEY_CTL_CCMP)
            byDecMode = KEY_CTL_CCMP;
    }
    *pKeyOut = pKey;

    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"AES:%d %d %d\n", pMgmt->byCSSPK, pMgmt->byCSSGK, byDecMode);

    if (pKey == NULL) {
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey == NULL\n");
        if (byDecMode == KEY_CTL_WEP) {
//            pDevice->s802_11Counter.WEPUndecryptableCount.QuadPart++;
        } else if (pDevice->bLinkPass == TRUE) {
//            pDevice->s802_11Counter.DecryptFailureCount.QuadPart++;
        }
        return FALSE;
    }
    if (byDecMode != pKey->byCipherSuite) {
        if (byDecMode == KEY_CTL_WEP) {
//            pDevice->s802_11Counter.WEPUndecryptableCount.QuadPart++;
        } else if (pDevice->bLinkPass == TRUE) {
//            pDevice->s802_11Counter.DecryptFailureCount.QuadPart++;
        }
        *pKeyOut = NULL;
        return FALSE;
    }
    if (byDecMode == KEY_CTL_WEP) {
        // handle WEP
        if ((pDevice->byLocalID <= REV_ID_VT3253_A1) ||
            (((PSKeyTable)(pKey->pvKeyTable))->bSoftWEP == TRUE)) {
            // Software WEP
            // 1. 3253A
            // 2. WEP 256

            PayloadLen -= (WLAN_HDR_ADDR3_LEN + 4 + 4); // 24 is 802.11 header,4 is IV, 4 is crc
            memcpy(pDevice->abyPRNG, pbyIV, 3);
            memcpy(pDevice->abyPRNG + 3, pKey->abyKey, pKey->uKeyLength);
            rc4_init(&pDevice->SBox, pDevice->abyPRNG, pKey->uKeyLength + 3);
            rc4_encrypt(&pDevice->SBox, pbyIV+4, pbyIV+4, PayloadLen);

            if (ETHbIsBufferCrc32Ok(pbyIV+4, PayloadLen)) {
                *pbyNewRsr |= NEWRSR_DECRYPTOK;
            }
        }
    } else if ((byDecMode == KEY_CTL_TKIP) ||
               (byDecMode == KEY_CTL_CCMP)) {
        // TKIP/AES

        PayloadLen -= (WLAN_HDR_ADDR3_LEN + 8 + 4); // 24 is 802.11 header, 8 is IV&ExtIV, 4 is crc
        *pdwRxTSC47_16 = cpu_to_le32(*(PDWORD)(pbyIV + 4));
	DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"ExtIV: %x\n", *pdwRxTSC47_16);
        if (byDecMode == KEY_CTL_TKIP) {
            *pwRxTSC15_0 = cpu_to_le16(MAKEWORD(*(pbyIV+2), *pbyIV));
        } else {
            *pwRxTSC15_0 = cpu_to_le16(*(PWORD)pbyIV);
        }
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"TSC0_15: %x\n", *pwRxTSC15_0);

        if ((byDecMode == KEY_CTL_TKIP) &&
            (pDevice->byLocalID <= REV_ID_VT3253_A1)) {
            // Software TKIP
            // 1. 3253 A
            PS802_11Header  pMACHeader = (PS802_11Header) (pbyFrame);
            TKIPvMixKey(pKey->abyKey, pMACHeader->abyAddr2, *pwRxTSC15_0, *pdwRxTSC47_16, pDevice->abyPRNG);
            rc4_init(&pDevice->SBox, pDevice->abyPRNG, TKIP_KEY_LEN);
            rc4_encrypt(&pDevice->SBox, pbyIV+8, pbyIV+8, PayloadLen);
            if (ETHbIsBufferCrc32Ok(pbyIV+8, PayloadLen)) {
                *pbyNewRsr |= NEWRSR_DECRYPTOK;
                DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"ICV OK!\n");
            } else {
                DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"ICV FAIL!!!\n");
                DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"PayloadLen = %d\n", PayloadLen);
            }
        }
    }// end of TKIP/AES

    if ((*(pbyIV+3) & 0x20) != 0)
        *pbExtIV = TRUE;
    return TRUE;
}


static BOOL s_bHostWepRxEncryption (
     PSDevice     pDevice,
     PBYTE        pbyFrame,
     unsigned int         FrameSize,
     PBYTE        pbyRsr,
     BOOL         bOnFly,
     PSKeyItem    pKey,
     PBYTE       pbyNewRsr,
    int *       pbExtIV,
     PWORD       pwRxTSC15_0,
     PDWORD      pdwRxTSC47_16
    )
{
    PSMgmtObject    pMgmt = &(pDevice->sMgmtObj);
    unsigned int            PayloadLen = FrameSize;
    PBYTE           pbyIV;
    BYTE            byKeyIdx;
    BYTE            byDecMode = KEY_CTL_WEP;
    PS802_11Header  pMACHeader;



    *pwRxTSC15_0 = 0;
    *pdwRxTSC47_16 = 0;

    pbyIV = pbyFrame + WLAN_HDR_ADDR3_LEN;
    if ( WLAN_GET_FC_TODS(*(PWORD)pbyFrame) &&
         WLAN_GET_FC_FROMDS(*(PWORD)pbyFrame) ) {
         pbyIV += 6;             // 6 is 802.11 address4
         PayloadLen -= 6;
    }
    byKeyIdx = (*(pbyIV+3) & 0xc0);
    byKeyIdx >>= 6;
    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"\nKeyIdx: %d\n", byKeyIdx);


    if (pMgmt->byCSSGK == KEY_CTL_TKIP)
        byDecMode = KEY_CTL_TKIP;
    else if (pMgmt->byCSSGK == KEY_CTL_CCMP)
        byDecMode = KEY_CTL_CCMP;

    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"AES:%d %d %d\n", pMgmt->byCSSPK, pMgmt->byCSSGK, byDecMode);

    if (byDecMode != pKey->byCipherSuite) {
        if (byDecMode == KEY_CTL_WEP) {
//            pDevice->s802_11Counter.WEPUndecryptableCount.QuadPart++;
        } else if (pDevice->bLinkPass == TRUE) {
//            pDevice->s802_11Counter.DecryptFailureCount.QuadPart++;
        }
        return FALSE;
    }

    if (byDecMode == KEY_CTL_WEP) {
        // handle WEP
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"byDecMode == KEY_CTL_WEP \n");
        if ((pDevice->byLocalID <= REV_ID_VT3253_A1) ||
            (((PSKeyTable)(pKey->pvKeyTable))->bSoftWEP == TRUE) ||
            (bOnFly == FALSE)) {
            // Software WEP
            // 1. 3253A
            // 2. WEP 256
            // 3. NotOnFly

            PayloadLen -= (WLAN_HDR_ADDR3_LEN + 4 + 4); // 24 is 802.11 header,4 is IV, 4 is crc
            memcpy(pDevice->abyPRNG, pbyIV, 3);
            memcpy(pDevice->abyPRNG + 3, pKey->abyKey, pKey->uKeyLength);
            rc4_init(&pDevice->SBox, pDevice->abyPRNG, pKey->uKeyLength + 3);
            rc4_encrypt(&pDevice->SBox, pbyIV+4, pbyIV+4, PayloadLen);

            if (ETHbIsBufferCrc32Ok(pbyIV+4, PayloadLen)) {
                *pbyNewRsr |= NEWRSR_DECRYPTOK;
            }
        }
    } else if ((byDecMode == KEY_CTL_TKIP) ||
               (byDecMode == KEY_CTL_CCMP)) {
        // TKIP/AES

        PayloadLen -= (WLAN_HDR_ADDR3_LEN + 8 + 4); // 24 is 802.11 header, 8 is IV&ExtIV, 4 is crc
        *pdwRxTSC47_16 = cpu_to_le32(*(PDWORD)(pbyIV + 4));
	DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"ExtIV: %x\n", *pdwRxTSC47_16);

        if (byDecMode == KEY_CTL_TKIP) {
            *pwRxTSC15_0 = cpu_to_le16(MAKEWORD(*(pbyIV+2), *pbyIV));
        } else {
            *pwRxTSC15_0 = cpu_to_le16(*(PWORD)pbyIV);
        }
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"TSC0_15: %x\n", *pwRxTSC15_0);

        if (byDecMode == KEY_CTL_TKIP) {

            if ((pDevice->byLocalID <= REV_ID_VT3253_A1) || (bOnFly == FALSE)) {
                // Software TKIP
                // 1. 3253 A
                // 2. NotOnFly
                DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"soft KEY_CTL_TKIP \n");
                pMACHeader = (PS802_11Header) (pbyFrame);
                TKIPvMixKey(pKey->abyKey, pMACHeader->abyAddr2, *pwRxTSC15_0, *pdwRxTSC47_16, pDevice->abyPRNG);
                rc4_init(&pDevice->SBox, pDevice->abyPRNG, TKIP_KEY_LEN);
                rc4_encrypt(&pDevice->SBox, pbyIV+8, pbyIV+8, PayloadLen);
                if (ETHbIsBufferCrc32Ok(pbyIV+8, PayloadLen)) {
                    *pbyNewRsr |= NEWRSR_DECRYPTOK;
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"ICV OK!\n");
                } else {
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"ICV FAIL!!!\n");
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"PayloadLen = %d\n", PayloadLen);
                }
            }
        }

        if (byDecMode == KEY_CTL_CCMP) {
            if (bOnFly == FALSE) {
                // Software CCMP
                // NotOnFly
                DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"soft KEY_CTL_CCMP\n");
                if (AESbGenCCMP(pKey->abyKey, pbyFrame, FrameSize)) {
                    *pbyNewRsr |= NEWRSR_DECRYPTOK;
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"CCMP MIC compare OK!\n");
                } else {
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"CCMP MIC fail!\n");
                }
            }
        }

    }// end of TKIP/AES

    if ((*(pbyIV+3) & 0x20) != 0)
        *pbExtIV = TRUE;
    return TRUE;
}



static BOOL s_bAPModeRxData (
     PSDevice pDevice,
     struct sk_buff *skb,
     unsigned int     FrameSize,
     unsigned int     cbHeaderOffset,
     signed int      iSANodeIndex,
     signed int      iDANodeIndex
    )

{
    PSMgmtObject        pMgmt = &(pDevice->sMgmtObj);
    BOOL                bRelayAndForward = FALSE;
    BOOL                bRelayOnly = FALSE;
    BYTE                byMask[8] = {1, 2, 4, 8, 0x10, 0x20, 0x40, 0x80};
    WORD                wAID;


    struct sk_buff* skbcpy = NULL;

    if (FrameSize > CB_MAX_BUF_SIZE)
        return FALSE;
    // check DA
    if (is_multicast_ether_addr((PBYTE)(skb->data+cbHeaderOffset))) {
       if (pMgmt->sNodeDBTable[0].bPSEnable) {

           skbcpy = dev_alloc_skb((int)pDevice->rx_buf_sz);

        // if any node in PS mode, buffer packet until DTIM.
           if (skbcpy == NULL) {
               DBG_PRT(MSG_LEVEL_NOTICE, KERN_INFO "relay multicast no skb available \n");
           }
           else {
               skbcpy->dev = pDevice->dev;
               skbcpy->len = FrameSize;
               memcpy(skbcpy->data, skb->data+cbHeaderOffset, FrameSize);
               skb_queue_tail(&(pMgmt->sNodeDBTable[0].sTxPSQueue), skbcpy);
               pMgmt->sNodeDBTable[0].wEnQueueCnt++;
               // set tx map
               pMgmt->abyPSTxMap[0] |= byMask[0];
           }
       }
       else {
           bRelayAndForward = TRUE;
       }
    }
    else {
        // check if relay
        if (BSSbIsSTAInNodeDB(pDevice, (PBYTE)(skb->data+cbHeaderOffset), &iDANodeIndex)) {
            if (pMgmt->sNodeDBTable[iDANodeIndex].eNodeState >= NODE_ASSOC) {
                if (pMgmt->sNodeDBTable[iDANodeIndex].bPSEnable) {
                    // queue this skb until next PS tx, and then release.

	                skb->data += cbHeaderOffset;
	                skb->tail += cbHeaderOffset;
                    skb_put(skb, FrameSize);
                    skb_queue_tail(&pMgmt->sNodeDBTable[iDANodeIndex].sTxPSQueue, skb);

                    pMgmt->sNodeDBTable[iDANodeIndex].wEnQueueCnt++;
                    wAID = pMgmt->sNodeDBTable[iDANodeIndex].wAID;
                    pMgmt->abyPSTxMap[wAID >> 3] |=  byMask[wAID & 7];
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO "relay: index= %d, pMgmt->abyPSTxMap[%d]= %d\n",
                               iDANodeIndex, (wAID >> 3), pMgmt->abyPSTxMap[wAID >> 3]);
                    return TRUE;
                }
                else {
                    bRelayOnly = TRUE;
                }
            }
        }
    }

    if (bRelayOnly || bRelayAndForward) {
        // relay this packet right now
        if (bRelayAndForward)
            iDANodeIndex = 0;

        if ((pDevice->uAssocCount > 1) && (iDANodeIndex >= 0)) {
		bRelayPacketSend(pDevice, (PBYTE) (skb->data + cbHeaderOffset),
				 FrameSize, (unsigned int) iDANodeIndex);
        }

        if (bRelayOnly)
            return FALSE;
    }
    // none associate, don't forward
    if (pDevice->uAssocCount == 0)
        return FALSE;

    return TRUE;
}




void RXvWorkItem(void *Context)
{
    PSDevice pDevice = (PSDevice) Context;
    int ntStatus;
    PRCB            pRCB=NULL;

    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"---->Rx Polling Thread\n");
    spin_lock_irq(&pDevice->lock);

    while ((pDevice->Flags & fMP_POST_READS) &&
            MP_IS_READY(pDevice) &&
            (pDevice->NumRecvFreeList != 0) ) {
        pRCB = pDevice->FirstRecvFreeList;
        pDevice->NumRecvFreeList--;
        ASSERT(pRCB);// cannot be NULL
        DequeueRCB(pDevice->FirstRecvFreeList, pDevice->LastRecvFreeList);
        ntStatus = PIPEnsBulkInUsbRead(pDevice, pRCB);
    }
    pDevice->bIsRxWorkItemQueued = FALSE;
    spin_unlock_irq(&pDevice->lock);

}


void
RXvFreeRCB(
     PRCB pRCB,
     BOOL bReAllocSkb
    )
{
    PSDevice pDevice = (PSDevice)pRCB->pDevice;


    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"---->RXvFreeRCB\n");

    ASSERT(!pRCB->Ref);     // should be 0
    ASSERT(pRCB->pDevice);  // shouldn't be NULL

    if (bReAllocSkb == TRUE) {
        pRCB->skb = dev_alloc_skb((int)pDevice->rx_buf_sz);
        // todo error handling
        if (pRCB->skb == NULL) {
            DBG_PRT(MSG_LEVEL_ERR,KERN_ERR" Failed to re-alloc rx skb\n");
        }else {
            pRCB->skb->dev = pDevice->dev;
        }
    }
    //
    // Insert the RCB back in the Recv free list
    //
    EnqueueRCB(pDevice->FirstRecvFreeList, pDevice->LastRecvFreeList, pRCB);
    pDevice->NumRecvFreeList++;


    if ((pDevice->Flags & fMP_POST_READS) && MP_IS_READY(pDevice) &&
        (pDevice->bIsRxWorkItemQueued == FALSE) ) {

        pDevice->bIsRxWorkItemQueued = TRUE;
        tasklet_schedule(&pDevice->ReadWorkItem);
    }
    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"<----RXFreeRCB %d %d\n",pDevice->NumRecvFreeList, pDevice->NumRecvMngList);
}


void RXvMngWorkItem(void *Context)
{
    PSDevice pDevice = (PSDevice) Context;
    PRCB            pRCB=NULL;
    PSRxMgmtPacket  pRxPacket;
    BOOL            bReAllocSkb = FALSE;

    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"---->Rx Mng Thread\n");

    spin_lock_irq(&pDevice->lock);
    while (pDevice->NumRecvMngList!=0)
    {
        pRCB = pDevice->FirstRecvMngList;
        pDevice->NumRecvMngList--;
        DequeueRCB(pDevice->FirstRecvMngList, pDevice->LastRecvMngList);
        if(!pRCB){
            break;
        }
        ASSERT(pRCB);// cannot be NULL
        pRxPacket = &(pRCB->sMngPacket);
	vMgrRxManagePacket((void *) pDevice, &(pDevice->sMgmtObj), pRxPacket);
        pRCB->Ref--;
        if(pRCB->Ref == 0) {
            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"RxvFreeMng %d %d\n",pDevice->NumRecvFreeList, pDevice->NumRecvMngList);
            RXvFreeRCB(pRCB, bReAllocSkb);
        } else {
            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"Rx Mng Only we have the right to free RCB\n");
        }
    }

	pDevice->bIsRxMngWorkItemQueued = FALSE;
	spin_unlock_irq(&pDevice->lock);

}


=======
>>>>>>> android-3.18
