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
 * File: key.c
 *
 * Purpose: Implement functions for 802.11i Key management
 *
 * Author: Jerry Chen
 *
 * Date: May 29, 2003
 *
 * Functions:
 *
 * Revision History:
 *
 */

#include "mac.h"
<<<<<<< HEAD
#include "rndis.h"
#include "control.h"

/*---------------------  Static Definitions -------------------------*/

/*---------------------  Static Classes  ----------------------------*/

/*---------------------  Static Variables  --------------------------*/
static int          msglevel                =MSG_LEVEL_INFO;
//static int          msglevel                =MSG_LEVEL_DEBUG;
/*---------------------  Static Functions  --------------------------*/

/*---------------------  Export Variables  --------------------------*/

/*---------------------  Static Definitions -------------------------*/

/*---------------------  Static Classes  ----------------------------*/

/*---------------------  Static Variables  --------------------------*/

/*---------------------  Static Functions  --------------------------*/
static void s_vCheckKeyTableValid(void *pDeviceHandler,
				  PSKeyManagement pTable)
{
    PSDevice    pDevice = (PSDevice) pDeviceHandler;
    int         i;
    WORD        wLength = 0;
    BYTE        pbyData[MAX_KEY_TABLE];

    for (i=0;i<MAX_KEY_TABLE;i++) {
        if ((pTable->KeyTable[i].bInUse == TRUE) &&
            (pTable->KeyTable[i].PairwiseKey.bKeyValid == FALSE) &&
            (pTable->KeyTable[i].GroupKey[0].bKeyValid == FALSE) &&
            (pTable->KeyTable[i].GroupKey[1].bKeyValid == FALSE) &&
            (pTable->KeyTable[i].GroupKey[2].bKeyValid == FALSE) &&
            (pTable->KeyTable[i].GroupKey[3].bKeyValid == FALSE)
            ) {

            pTable->KeyTable[i].bInUse = FALSE;
            pTable->KeyTable[i].wKeyCtl = 0;
            pTable->KeyTable[i].bSoftWEP = FALSE;
            pbyData[wLength++] = (BYTE) i;
            //MACvDisableKeyEntry(pDevice, i);
        }
    }
    if ( wLength != 0 ) {
        CONTROLnsRequestOut(pDevice,
                            MESSAGE_TYPE_CLRKEYENTRY,
                            0,
                            0,
                            wLength,
                            pbyData
                            );
    }

}


/*---------------------  Export Functions  --------------------------*/


/*
 * Description: Init Key management table
 *
 * Parameters:
 *  In:
 *      pTable          - Pointer to Key table
 *  Out:
 *      none
 *
 * Return Value: none
 *
 */
void KeyvInitTable(void *pDeviceHandler, PSKeyManagement pTable)
{
    PSDevice    pDevice = (PSDevice) pDeviceHandler;
    int i;
    int jj;
    BYTE       pbyData[MAX_KEY_TABLE+1];

    spin_lock_irq(&pDevice->lock);
    for (i=0;i<MAX_KEY_TABLE;i++) {
        pTable->KeyTable[i].bInUse = FALSE;
        pTable->KeyTable[i].PairwiseKey.bKeyValid = FALSE;
	pTable->KeyTable[i].PairwiseKey.pvKeyTable =
	  (void *)&pTable->KeyTable[i];
        for (jj=0; jj < MAX_GROUP_KEY; jj++) {
            pTable->KeyTable[i].GroupKey[jj].bKeyValid = FALSE;
	    pTable->KeyTable[i].GroupKey[jj].pvKeyTable =
	      (void *) &(pTable->KeyTable[i]);
        }
        pTable->KeyTable[i].wKeyCtl = 0;
        pTable->KeyTable[i].dwGTKeyIndex = 0;
        pTable->KeyTable[i].bSoftWEP = FALSE;
        pbyData[i] = (BYTE) i;
    }
    pbyData[i] = (BYTE) i;
    CONTROLnsRequestOut(pDevice,
                        MESSAGE_TYPE_CLRKEYENTRY,
                        0,
                        0,
                        11,
                        pbyData
                        );

    spin_unlock_irq(&pDevice->lock);

    return;
}


/*
 * Description: Get Key from table
 *
 * Parameters:
 *  In:
 *      pTable          - Pointer to Key table
 *      pbyBSSID        - BSSID of Key
 *      dwKeyIndex      - Key Index (0xFFFFFFFF means pairwise key)
 *  Out:
 *      pKey            - Key return
 *
 * Return Value: TRUE if found otherwise FALSE
 *
 */
BOOL KeybGetKey(PSKeyManagement pTable, PBYTE pbyBSSID, DWORD dwKeyIndex,
		PSKeyItem *pKey)
{
    int i;

    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"KeybGetKey() \n");

    *pKey = NULL;
    for (i=0;i<MAX_KEY_TABLE;i++) {
        if ((pTable->KeyTable[i].bInUse == TRUE) &&
	    !compare_ether_addr(pTable->KeyTable[i].abyBSSID, pbyBSSID)) {
            if (dwKeyIndex == 0xFFFFFFFF) {
                if (pTable->KeyTable[i].PairwiseKey.bKeyValid == TRUE) {
                    *pKey = &(pTable->KeyTable[i].PairwiseKey);
                    return (TRUE);
                }
                else {
                    return (FALSE);
                }
            } else if (dwKeyIndex < MAX_GROUP_KEY) {
                if (pTable->KeyTable[i].GroupKey[dwKeyIndex].bKeyValid == TRUE) {
                    *pKey = &(pTable->KeyTable[i].GroupKey[dwKeyIndex]);
                    return (TRUE);
                }
                else {
                    return (FALSE);
                }
            }
            else {
                return (FALSE);
            }
        }
    }
    return (FALSE);
}


/*
 * Description: Set Key to table
 *
 * Parameters:
 *  In:
 *      pTable          - Pointer to Key table
 *      pbyBSSID        - BSSID of Key
 *      dwKeyIndex      - Key index (reference to NDIS DDK)
 *      uKeyLength      - Key length
 *      KeyRSC          - Key RSC
 *      pbyKey          - Pointer to key
 *  Out:
 *      none
 *
 * Return Value: TRUE if success otherwise FALSE
 *
 */
BOOL KeybSetKey(
    void *pDeviceHandler,
    PSKeyManagement pTable,
    PBYTE           pbyBSSID,
    DWORD           dwKeyIndex,
	u32 uKeyLength,
    PQWORD          pKeyRSC,
    PBYTE           pbyKey,
    BYTE            byKeyDecMode
    )
{
    PSDevice    pDevice = (PSDevice) pDeviceHandler;
    int         i,j;
    unsigned int        ii;
    PSKeyItem   pKey;
    unsigned int        uKeyIdx;

	DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO
		"Enter KeybSetKey: %X\n", dwKeyIndex);

    j = (MAX_KEY_TABLE-1);
    for (i=0;i<(MAX_KEY_TABLE-1);i++) {
        if ((pTable->KeyTable[i].bInUse == FALSE) &&
            (j == (MAX_KEY_TABLE-1))) {
            // found empty table
            j = i;
        }
        if ((pTable->KeyTable[i].bInUse == TRUE) &&
	    !compare_ether_addr(pTable->KeyTable[i].abyBSSID, pbyBSSID)) {
            // found table already exist
            if ((dwKeyIndex & PAIRWISE_KEY) != 0) {
                // Pairwise key
                pKey = &(pTable->KeyTable[i].PairwiseKey);
                pTable->KeyTable[i].wKeyCtl &= 0xFFF0;          // clear pairwise key control filed
                pTable->KeyTable[i].wKeyCtl |= byKeyDecMode;
                uKeyIdx = 4;                                    // use HW key entry 4 for pairwise key
            } else {
                // Group key
                if ((dwKeyIndex & 0x000000FF) >= MAX_GROUP_KEY)
                    return (FALSE);
                pKey = &(pTable->KeyTable[i].GroupKey[dwKeyIndex & 0x000000FF]);
                if ((dwKeyIndex & TRANSMIT_KEY) != 0)  {
                    // Group transmit key
                    pTable->KeyTable[i].dwGTKeyIndex = dwKeyIndex;
			DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO
				"Group transmit key(R)[%X]: %d\n",
					pTable->KeyTable[i].dwGTKeyIndex, i);
                }
                pTable->KeyTable[i].wKeyCtl &= 0xFF0F;          // clear group key control filed
                pTable->KeyTable[i].wKeyCtl |= (byKeyDecMode << 4);
                pTable->KeyTable[i].wKeyCtl |= 0x0040;          // use group key for group address
                uKeyIdx = (dwKeyIndex & 0x000000FF);
            }
            pTable->KeyTable[i].wKeyCtl |= 0x8000;              // enable on-fly

            pKey->bKeyValid = TRUE;
            pKey->uKeyLength = uKeyLength;
            pKey->dwKeyIndex = dwKeyIndex;
            pKey->byCipherSuite = byKeyDecMode;
            memcpy(pKey->abyKey, pbyKey, uKeyLength);
            if (byKeyDecMode == KEY_CTL_WEP) {
                if (uKeyLength == WLAN_WEP40_KEYLEN)
                    pKey->abyKey[15] &= 0x7F;
                if (uKeyLength == WLAN_WEP104_KEYLEN)
                    pKey->abyKey[15] |= 0x80;
            }
            MACvSetKeyEntry(pDevice, pTable->KeyTable[i].wKeyCtl, i, uKeyIdx, pbyBSSID, (PDWORD)pKey->abyKey);

            if ((dwKeyIndex & USE_KEYRSC) == 0) {
                // RSC set by NIC
		    memset(&(pKey->KeyRSC), 0, sizeof(QWORD));
            }
            else {
                memcpy(&(pKey->KeyRSC), pKeyRSC,  sizeof(QWORD));
            }
            pKey->dwTSC47_16 = 0;
            pKey->wTSC15_0 = 0;

            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"KeybSetKey(R): \n");
            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->bKeyValid: %d\n ", pKey->bKeyValid);
            //DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->uKeyLength: %d\n ", pKey->uKeyLength);
            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->abyKey: ");
            for (ii = 0; ii < pKey->uKeyLength; ii++) {
                DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO "%02x ", pKey->abyKey[ii]);
            }
            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"\n");

		DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->dwTSC47_16: %x\n ",
			pKey->dwTSC47_16);
		DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->wTSC15_0: %x\n ",
			pKey->wTSC15_0);
		DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->dwKeyIndex: %x\n ",
			pKey->dwKeyIndex);

            return (TRUE);
        }
    }
    if (j < (MAX_KEY_TABLE-1)) {
	memcpy(pTable->KeyTable[j].abyBSSID, pbyBSSID, ETH_ALEN);
        pTable->KeyTable[j].bInUse = TRUE;
        if ((dwKeyIndex & PAIRWISE_KEY) != 0)  {
            // Pairwise key
            pKey = &(pTable->KeyTable[j].PairwiseKey);
            pTable->KeyTable[j].wKeyCtl &= 0xFFF0;          // clear pairwise key control filed
            pTable->KeyTable[j].wKeyCtl |= byKeyDecMode;
            uKeyIdx = 4;                                    // use HW key entry 4 for pairwise key
        } else {
            // Group key
            if ((dwKeyIndex & 0x000000FF) >= MAX_GROUP_KEY)
                return (FALSE);
            pKey = &(pTable->KeyTable[j].GroupKey[dwKeyIndex & 0x000000FF]);
            if ((dwKeyIndex & TRANSMIT_KEY) != 0)  {
                // Group transmit key
                pTable->KeyTable[j].dwGTKeyIndex = dwKeyIndex;
		DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO
			"Group transmit key(N)[%X]: %d\n",
				pTable->KeyTable[j].dwGTKeyIndex, j);
            }
            pTable->KeyTable[j].wKeyCtl &= 0xFF0F;          // clear group key control filed
            pTable->KeyTable[j].wKeyCtl |= (byKeyDecMode << 4);
            pTable->KeyTable[j].wKeyCtl |= 0x0040;          // use group key for group address
            uKeyIdx = (dwKeyIndex & 0x000000FF);
        }
        pTable->KeyTable[j].wKeyCtl |= 0x8000;              // enable on-fly

        pKey->bKeyValid = TRUE;
        pKey->uKeyLength = uKeyLength;
        pKey->dwKeyIndex = dwKeyIndex;
        pKey->byCipherSuite = byKeyDecMode;
        memcpy(pKey->abyKey, pbyKey, uKeyLength);
        if (byKeyDecMode == KEY_CTL_WEP) {
            if (uKeyLength == WLAN_WEP40_KEYLEN)
                pKey->abyKey[15] &= 0x7F;
            if (uKeyLength == WLAN_WEP104_KEYLEN)
                pKey->abyKey[15] |= 0x80;
        }
        MACvSetKeyEntry(pDevice, pTable->KeyTable[j].wKeyCtl, j, uKeyIdx, pbyBSSID, (PDWORD)pKey->abyKey);

        if ((dwKeyIndex & USE_KEYRSC) == 0) {
            // RSC set by NIC
		memset(&(pKey->KeyRSC), 0, sizeof(QWORD));
        }
        else {
            memcpy(&(pKey->KeyRSC), pKeyRSC,  sizeof(QWORD));
        }
        pKey->dwTSC47_16 = 0;
        pKey->wTSC15_0 = 0;

        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"KeybSetKey(N): \n");
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->bKeyValid: %d\n ", pKey->bKeyValid);
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->uKeyLength: %d\n ", (int)pKey->uKeyLength);
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->abyKey: ");
        for (ii = 0; ii < pKey->uKeyLength; ii++) {
            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO "%02x ", pKey->abyKey[ii]);
        }
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"\n");

	DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->dwTSC47_16: %x\n ",
		pKey->dwTSC47_16);
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->wTSC15_0: %x\n ", pKey->wTSC15_0);
	DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->dwKeyIndex: %x\n ",
		pKey->dwKeyIndex);

        return (TRUE);
    }
    return (FALSE);
}


/*
 * Description: Remove Key from table
 *
 * Parameters:
 *  In:
 *      pTable          - Pointer to Key table
 *      pbyBSSID        - BSSID of Key
 *      dwKeyIndex      - Key Index (reference to NDIS DDK)
 *  Out:
 *      none
 *
 * Return Value: TRUE if success otherwise FALSE
 *
 */
BOOL KeybRemoveKey(
    void *pDeviceHandler,
    PSKeyManagement pTable,
    PBYTE           pbyBSSID,
    DWORD           dwKeyIndex
    )
{
    PSDevice    pDevice = (PSDevice) pDeviceHandler;
    int     i;
    BOOL    bReturnValue = FALSE;

    if (is_broadcast_ether_addr(pbyBSSID)) {
        // dealte all key
        if ((dwKeyIndex & PAIRWISE_KEY) != 0) {
            for (i=0;i<MAX_KEY_TABLE;i++) {
                pTable->KeyTable[i].PairwiseKey.bKeyValid = FALSE;
            }
            bReturnValue =  TRUE;
        }
        else if ((dwKeyIndex & 0x000000FF) < MAX_GROUP_KEY) {
            for (i=0;i<MAX_KEY_TABLE;i++) {
                pTable->KeyTable[i].GroupKey[dwKeyIndex & 0x000000FF].bKeyValid = FALSE;
                if ((dwKeyIndex & 0x7FFFFFFF) == (pTable->KeyTable[i].dwGTKeyIndex & 0x7FFFFFFF)) {
                    // remove Group transmit key
                    pTable->KeyTable[i].dwGTKeyIndex = 0;
                }
            }
            bReturnValue = TRUE;
        }
        else {
            bReturnValue = FALSE;
        }

    } else {
        for (i=0;i<MAX_KEY_TABLE;i++) {
            if ( (pTable->KeyTable[i].bInUse == TRUE) &&
		 !compare_ether_addr(pTable->KeyTable[i].abyBSSID, pbyBSSID)) {

                if ((dwKeyIndex & PAIRWISE_KEY) != 0) {
                    pTable->KeyTable[i].PairwiseKey.bKeyValid = FALSE;
                    bReturnValue = TRUE;
                    break;
                }
                else if ((dwKeyIndex & 0x000000FF) < MAX_GROUP_KEY) {
                    pTable->KeyTable[i].GroupKey[dwKeyIndex & 0x000000FF].bKeyValid = FALSE;
                    if ((dwKeyIndex & 0x7FFFFFFF) == (pTable->KeyTable[i].dwGTKeyIndex & 0x7FFFFFFF)) {
                        // remove Group transmit key
                        pTable->KeyTable[i].dwGTKeyIndex = 0;
                    }
                    bReturnValue = TRUE;
                    break;
                }
                else {
                    bReturnValue = FALSE;
                    break;
                }
            } //pTable->KeyTable[i].bInUse == TRUE
        }  //for
        bReturnValue = TRUE;
    }

    s_vCheckKeyTableValid(pDevice,pTable);
    return bReturnValue;


}


/*
 * Description: Remove Key from table
 *
 * Parameters:
 *  In:
 *      pTable          - Pointer to Key table
 *      pbyBSSID        - BSSID of Key
 *  Out:
 *      none
 *
 * Return Value: TRUE if success otherwise FALSE
 *
 */
BOOL KeybRemoveAllKey(
    void *pDeviceHandler,
    PSKeyManagement pTable,
    PBYTE           pbyBSSID
    )
{
    PSDevice    pDevice = (PSDevice) pDeviceHandler;
    int  i,u;

    for (i=0;i<MAX_KEY_TABLE;i++) {
        if ((pTable->KeyTable[i].bInUse == TRUE) &&
	    !compare_ether_addr(pTable->KeyTable[i].abyBSSID, pbyBSSID)) {
            pTable->KeyTable[i].PairwiseKey.bKeyValid = FALSE;
	    for (u = 0; u < MAX_GROUP_KEY; u++)
		pTable->KeyTable[i].GroupKey[u].bKeyValid = FALSE;

            pTable->KeyTable[i].dwGTKeyIndex = 0;
            s_vCheckKeyTableValid(pDevice, pTable);
            return (TRUE);
        }
    }
    return (FALSE);
}

/*
 * Description: Remove WEP Key from table
 *
 * Parameters:
 *  In:
 *      pTable          - Pointer to Key table
 *  Out:
 *      none
 *
 * Return Value: TRUE if success otherwise FALSE
 *
 */
void KeyvRemoveWEPKey(
    void *pDeviceHandler,
    PSKeyManagement pTable,
    DWORD           dwKeyIndex
    )
{
    PSDevice    pDevice = (PSDevice) pDeviceHandler;

   if ((dwKeyIndex & 0x000000FF) < MAX_GROUP_KEY) {
        if (pTable->KeyTable[MAX_KEY_TABLE-1].bInUse == TRUE) {
            if (pTable->KeyTable[MAX_KEY_TABLE-1].GroupKey[dwKeyIndex & 0x000000FF].byCipherSuite == KEY_CTL_WEP) {
                pTable->KeyTable[MAX_KEY_TABLE-1].GroupKey[dwKeyIndex & 0x000000FF].bKeyValid = FALSE;
                if ((dwKeyIndex & 0x7FFFFFFF) == (pTable->KeyTable[MAX_KEY_TABLE-1].dwGTKeyIndex & 0x7FFFFFFF)) {
                    // remove Group transmit key
                    pTable->KeyTable[MAX_KEY_TABLE-1].dwGTKeyIndex = 0;
                }
            }
        }
        s_vCheckKeyTableValid(pDevice, pTable);
    }
    return;
}

void KeyvRemoveAllWEPKey(void *pDeviceHandler, PSKeyManagement pTable)
{
	PSDevice pDevice = (PSDevice) pDeviceHandler;
	int i;

	for (i = 0; i < MAX_GROUP_KEY; i++)
		KeyvRemoveWEPKey(pDevice, pTable, i);
}
=======
#include "key.h"
#include "usbpipe.h"
>>>>>>> android-3.18

int vnt_key_init_table(struct vnt_private *priv)
{
	u8 i;
	u8 data[MAX_KEY_TABLE];

	for (i = 0; i < MAX_KEY_TABLE; i++)
		data[i] = i;

<<<<<<< HEAD

                    return (TRUE);
                }
                else {
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"PairwiseKey.bKeyValid == FALSE\n");
                    return (FALSE);
                }
            } // End of Type == PAIRWISE
            else {
                if (pTable->KeyTable[i].dwGTKeyIndex == 0) {
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"ERROR: dwGTKeyIndex == 0 !!!\n");
                    return FALSE;
                }
                if (pTable->KeyTable[i].GroupKey[(pTable->KeyTable[i].dwGTKeyIndex&0x000000FF)].bKeyValid == TRUE) {
                    *pKey = &(pTable->KeyTable[i].GroupKey[(pTable->KeyTable[i].dwGTKeyIndex&0x000000FF)]);

                        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"KeybGetTransmitKey:");
                        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"GROUP_KEY: KeyTable.abyBSSID\n");
                        for (ii = 0; ii < 6; ii++) {
                            DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"%x ", pTable->KeyTable[i].abyBSSID[ii]);
                        }
                        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"\n");
			DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"dwGTKeyIndex: %X\n",
				pTable->KeyTable[i].dwGTKeyIndex);

                    return (TRUE);
                }
                else {
                    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"GroupKey.bKeyValid == FALSE\n");
                    return (FALSE);
                }
            } // End of Type = GROUP
        } // BSSID match
    }
    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"ERROR: NO Match BSSID !!! ");
    for (ii = 0; ii < 6; ii++) {
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"%02x ", *(pbyBSSID+ii));
    }
    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"\n");
    return (FALSE);
=======
	return vnt_control_out(priv, MESSAGE_TYPE_CLRKEYENTRY,
			0, 0, ARRAY_SIZE(data), data);
>>>>>>> android-3.18
}

static int vnt_set_keymode(struct ieee80211_hw *hw, u8 *mac_addr,
	struct ieee80211_key_conf *key, u32 key_type, u32 mode,
	bool onfly_latch)
{
	struct vnt_private *priv = hw->priv;
	u8 broadcast[6] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
	u16 key_mode = 0;
	u32 entry = 0;
	u8 *bssid;
	u8 key_inx = key->keyidx;
	u8 i;

	if (mac_addr)
		bssid = mac_addr;
	else
		bssid = &broadcast[0];

	if (key_type != VNT_KEY_DEFAULTKEY) {
		for (i = 0; i < (MAX_KEY_TABLE - 1); i++) {
			if (!test_bit(i, &priv->key_entry_inuse)) {
				set_bit(i, &priv->key_entry_inuse);

				key->hw_key_idx = i;
				entry = key->hw_key_idx;
				break;
			}
		}
	}

	switch (key_type) {
		/* fallthrough */
	case VNT_KEY_DEFAULTKEY:
		/* default key last entry */
		entry = MAX_KEY_TABLE - 1;
		key->hw_key_idx = entry;
	case VNT_KEY_ALLGROUP:
		key_mode |= VNT_KEY_ALLGROUP;
		if (onfly_latch)
			key_mode |= VNT_KEY_ONFLY_ALL;
	case VNT_KEY_GROUP_ADDRESS:
		key_mode |= mode;
	case VNT_KEY_GROUP:
		key_mode |= (mode << 4);
		key_mode |= VNT_KEY_GROUP;
		break;
	case  VNT_KEY_PAIRWISE:
		key_mode |= mode;
		key_inx = 4;
		/* Don't save entry for pairwise key for station mode */
		if (priv->op_mode == NL80211_IFTYPE_STATION)
			clear_bit(entry, &priv->key_entry_inuse);
		break;
	default:
		return -EINVAL;
	}

	if (onfly_latch)
		key_mode |= VNT_KEY_ONFLY;

	if (mode == KEY_CTL_WEP) {
		if (key->keylen == WLAN_KEY_LEN_WEP40)
			key->key[15] &= 0x7f;
		if (key->keylen == WLAN_KEY_LEN_WEP104)
			key->key[15] |= 0x80;
	}

	vnt_mac_set_keyentry(priv, key_mode, entry, key_inx, bssid, key->key);

	return 0;
}

<<<<<<< HEAD
/*
 * Description: Set Key to table
 *
 * Parameters:
 *  In:
 *      pTable          - Pointer to Key table
 *      dwKeyIndex      - Key index (reference to NDIS DDK)
 *      uKeyLength      - Key length
 *      KeyRSC          - Key RSC
 *      pbyKey          - Pointer to key
 *  Out:
 *      none
 *
 * Return Value: TRUE if success otherwise FALSE
 *
 */
BOOL KeybSetDefaultKey(
    void *pDeviceHandler,
    PSKeyManagement pTable,
    DWORD           dwKeyIndex,
	u32 uKeyLength,
    PQWORD          pKeyRSC,
    PBYTE           pbyKey,
    BYTE            byKeyDecMode
    )
=======
int vnt_set_keys(struct ieee80211_hw *hw, struct ieee80211_sta *sta,
	struct ieee80211_vif *vif, struct ieee80211_key_conf *key)
>>>>>>> android-3.18
{
	struct ieee80211_bss_conf *conf = &vif->bss_conf;
	struct vnt_private *priv = hw->priv;
	u8 *mac_addr = NULL;
	u8 key_dec_mode = 0;
	int ret = 0, u;

	if (sta)
		mac_addr = &sta->addr[0];

<<<<<<< HEAD
    // Group key
    pKey = &(pTable->KeyTable[MAX_KEY_TABLE-1].GroupKey[dwKeyIndex & 0x000000FF]);
    if ((dwKeyIndex & TRANSMIT_KEY) != 0)  {
        // Group transmit key
        pTable->KeyTable[MAX_KEY_TABLE-1].dwGTKeyIndex = dwKeyIndex;
		DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO
		"Group transmit key(R)[%X]: %d\n",
		pTable->KeyTable[MAX_KEY_TABLE-1].dwGTKeyIndex,
		MAX_KEY_TABLE-1);

    }
    pTable->KeyTable[MAX_KEY_TABLE-1].wKeyCtl &= 0x7F00;          // clear all key control filed
    pTable->KeyTable[MAX_KEY_TABLE-1].wKeyCtl |= (byKeyDecMode << 4);
    pTable->KeyTable[MAX_KEY_TABLE-1].wKeyCtl |= (byKeyDecMode);
    pTable->KeyTable[MAX_KEY_TABLE-1].wKeyCtl |= 0x0044;          // use group key for all address
    uKeyIdx = (dwKeyIndex & 0x000000FF);

    if ((uKeyLength == WLAN_WEP232_KEYLEN) &&
        (byKeyDecMode == KEY_CTL_WEP)) {
        pTable->KeyTable[MAX_KEY_TABLE-1].wKeyCtl |= 0x4000;              // disable on-fly disable address match
        pTable->KeyTable[MAX_KEY_TABLE-1].bSoftWEP = TRUE;
    } else {
        if (pTable->KeyTable[MAX_KEY_TABLE-1].bSoftWEP == FALSE)
            pTable->KeyTable[MAX_KEY_TABLE-1].wKeyCtl |= 0xC000;          // enable on-fly disable address match
    }

    pKey->bKeyValid = TRUE;
    pKey->uKeyLength = uKeyLength;
    pKey->dwKeyIndex = dwKeyIndex;
    pKey->byCipherSuite = byKeyDecMode;
    memcpy(pKey->abyKey, pbyKey, uKeyLength);
    if (byKeyDecMode == KEY_CTL_WEP) {
        if (uKeyLength == WLAN_WEP40_KEYLEN)
            pKey->abyKey[15] &= 0x7F;
        if (uKeyLength == WLAN_WEP104_KEYLEN)
            pKey->abyKey[15] |= 0x80;
    }

    MACvSetKeyEntry(pDevice, pTable->KeyTable[MAX_KEY_TABLE-1].wKeyCtl, MAX_KEY_TABLE-1, uKeyIdx, pTable->KeyTable[MAX_KEY_TABLE-1].abyBSSID, (PDWORD) pKey->abyKey);

    if ((dwKeyIndex & USE_KEYRSC) == 0) {
        // RSC set by NIC
	    memset(&(pKey->KeyRSC), 0, sizeof(QWORD));
    } else {
        memcpy(&(pKey->KeyRSC), pKeyRSC,  sizeof(QWORD));
    }
    pKey->dwTSC47_16 = 0;
    pKey->wTSC15_0 = 0;


    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"KeybSetKey(R): \n");
    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->bKeyValid: %d\n", pKey->bKeyValid);
    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->uKeyLength: %d\n", (int)pKey->uKeyLength);
    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->abyKey: \n");
    for (ii = 0; ii < pKey->uKeyLength; ii++) {
        DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"%x", pKey->abyKey[ii]);
    }
    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"\n");

	DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->dwTSC47_16: %x\n",
		pKey->dwTSC47_16);
    DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->wTSC15_0: %x\n", pKey->wTSC15_0);
	DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"pKey->dwKeyIndex: %x\n",
		pKey->dwKeyIndex);

    return (TRUE);
}


/*
 * Description: Set Key to table
 *
 * Parameters:
 *  In:
 *      pTable          - Pointer to Key table
 *      dwKeyIndex      - Key index (reference to NDIS DDK)
 *      uKeyLength      - Key length
 *      KeyRSC          - Key RSC
 *      pbyKey          - Pointer to key
 *  Out:
 *      none
 *
 * Return Value: TRUE if success otherwise FALSE
 *
 */
BOOL KeybSetAllGroupKey(
    void *pDeviceHandler,
    PSKeyManagement pTable,
    DWORD           dwKeyIndex,
	u32 uKeyLength,
    PQWORD          pKeyRSC,
    PBYTE           pbyKey,
    BYTE            byKeyDecMode
    )
{
    PSDevice    pDevice = (PSDevice) pDeviceHandler;
    int         i;
    unsigned int        ii;
    PSKeyItem   pKey;
    unsigned int        uKeyIdx;

	DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO"Enter KeybSetAllGroupKey: %X\n",
		dwKeyIndex);
=======
	switch (key->cipher) {
	case 0:
		for (u = 0 ; u < MAX_KEY_TABLE; u++)
			vnt_mac_disable_keyentry(priv, u);
		return ret;

	case WLAN_CIPHER_SUITE_WEP40:
	case WLAN_CIPHER_SUITE_WEP104:
		for (u = 0; u < MAX_KEY_TABLE; u++)
			vnt_mac_disable_keyentry(priv, u);
>>>>>>> android-3.18

		vnt_set_keymode(hw, mac_addr, key, VNT_KEY_DEFAULTKEY,
			KEY_CTL_WEP, true);

		key->flags |= IEEE80211_KEY_FLAG_GENERATE_IV;

<<<<<<< HEAD
    for (i=0; i < MAX_KEY_TABLE-1; i++) {
        if (pTable->KeyTable[i].bInUse == TRUE) {
            // found table already exist
            // Group key
            pKey = &(pTable->KeyTable[i].GroupKey[dwKeyIndex & 0x000000FF]);
            if ((dwKeyIndex & TRANSMIT_KEY) != 0)  {
                // Group transmit key
                pTable->KeyTable[i].dwGTKeyIndex = dwKeyIndex;
		DBG_PRT(MSG_LEVEL_DEBUG, KERN_INFO
			"Group transmit key(R)[%X]: %d\n",
			pTable->KeyTable[i].dwGTKeyIndex, i);
=======
		return ret;
	case WLAN_CIPHER_SUITE_TKIP:
		key->flags |= IEEE80211_KEY_FLAG_GENERATE_MMIC;
		key->flags |= IEEE80211_KEY_FLAG_GENERATE_IV;
>>>>>>> android-3.18

		key_dec_mode = KEY_CTL_TKIP;

		break;
	case WLAN_CIPHER_SUITE_CCMP:
		if (priv->local_id <= MAC_REVISION_A1)
			return -EINVAL;

		key_dec_mode = KEY_CTL_CCMP;

		key->flags |= IEEE80211_KEY_FLAG_GENERATE_IV;
	}


	if (key->flags & IEEE80211_KEY_FLAG_PAIRWISE) {
		vnt_set_keymode(hw, mac_addr, key, VNT_KEY_PAIRWISE,
					key_dec_mode, true);
	} else {
		vnt_set_keymode(hw, mac_addr, key, VNT_KEY_DEFAULTKEY,
						key_dec_mode, true);

		vnt_set_keymode(hw, (u8 *)conf->bssid, key,
			VNT_KEY_GROUP_ADDRESS, key_dec_mode, true);
	}

	return 0;
}
