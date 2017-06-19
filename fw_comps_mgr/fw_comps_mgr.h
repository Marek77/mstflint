/*
 * Copyright (C) Jan 2013 Mellanox Technologies Ltd. All rights reserved.
 *
 * This software is available to you under a choice of one of two
 * licenses.  You may choose to be licensed under the terms of the GNU
 * General Public License (GPL) Version 2, available from the file
 * COPYING in the main directory of this source tree, or the
 * OpenIB.org BSD license below:
 *
 *     Redistribution and use in source and binary forms, with or
 *     without modification, are permitted provided that the following
 *     conditions are met:
 *
 *      - Redistributions of source code must retain the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer.
 *
 *      - Redistributions in binary form must reproduce the above
 *        copyright notice, this list of conditions and the following
 *        disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */
/*
 * fw_comps_mgr.h
 *
 *  Created on: Jul 31, 2016
 *      Author: adham
 */

#ifndef USER_MLXFWOPS_LIB_FW_COMPS_MGR_H_
#define USER_MLXFWOPS_LIB_FW_COMPS_MGR_H_

#include "mtcr.h"
#include <vector>
#include "reg_access/reg_access.h"

#include "mlxfwops/uefi_c/mft_uefi_common.h"

#define PSID_LEN 16
#define MAX_ROM_NUM 4

typedef struct reg_access_hca_mcqs_reg comp_status_st;
typedef struct reg_access_hca_mcqi_reg comp_info_st;
typedef struct reg_access_hca_mcc_reg fsm_control_st;
typedef struct reg_access_hca_mcqi_cap comp_cap_st;

typedef struct reg_access_hca_mcqi_version component_version_st;

typedef int (*ProgressFunc) (int completion);

struct uid_entry {
     u_int8_t num_allocated;
     u_int64_t uid;
};

typedef struct {
    u_int8_t type;
    u_int8_t arch;
    u_int32_t version;
} mgirRomInfo;

typedef struct {
    u_int8_t secure_fw;
    u_int8_t signed_fw;
    u_int8_t debug_fw;
    u_int8_t dev_fw;
} security_fw_t;

typedef struct {
    component_version_st pending_fw_version;
    component_version_st running_fw_version;
    security_fw_t        security_type;
    char        psid[PSID_LEN + 1];
    uid_entry   base_guid;
    uid_entry   base_mac;
    uid_entry   base_guid_orig;
    uid_entry   base_mac_orig;
    u_int16_t   dev_id;
    u_int16_t   hw_dev_id;
    u_int16_t   rev_id;
    u_int8_t    signed_fw;
    u_int8_t    pending_fw_valid;
    mgirRomInfo roms[MAX_ROM_NUM];
    int         nRoms;
} fwInfoT;

typedef struct {
    comp_status_st comp_status;
    comp_cap_st    comp_cap;
    int            valid;
} comp_query_st;

typedef enum {
    MCDA_READ_COMP=0x0,
    MCDA_WRITE_COMP
} access_type_t;

typedef enum {
    COMPINFO_CAPABILITIES,
    COMPINFO_VERSIONS,
    COMPINFO_PUBLIC_KEYS,
    COMPINFO_FORBIDDEN_VERSION,
    COMPINFO_ACTIVATION_METHOD=0x5,
} comp_info_t;


class FwComponent {
public:

    typedef enum {
        COMPID_BOOT_IMG        = 0x1,
        COMPID_RUNTIME_IMG     = 0x2,
        COMPID_USER_NVCONFIG   = 0x3,
        COMPID_OEM_NVCONFIG    = 0x4,
        COMPID_MLNX_NVCONFIG   = 0x5,
        COMPID_CS_TOKEN        = 0x6,
        COMPID_DBG_TOKEN       = 0x7,
        COMPID_DEV_INFO        = 0x8,
        COMPID_UNKNOWN         = 0xff,
    } comps_ids_t;

    FwComponent() : _size(0), _type(COMPID_UNKNOWN), _componentIndex(0xffffffff), _initialized(false) {};
    FwComponent(comps_ids_t compId) : _size(0), _type(compId), _componentIndex(0xffffffff), _initialized(false) {};
    ~FwComponent() {};

    bool        init(const std::vector<u_int8_t>&  buff,
                     u_int32_t   size,
                     comps_ids_t type,
                     u_int32_t   idx=0xffffffff);
    std::vector<u_int8_t>&   getData() { return _data;} ;
    u_int32_t                getSize() { return _size;} ;
    comps_ids_t              getType() { return _type;} ;

    void                    setData(const std::vector<u_int8_t>&  buff) {_data = buff;} ;
    void                    setSize(u_int32_t size) {_size = size;} ;
    void                    setType(comps_ids_t compId) { _type = compId; };

    static const char*       getCompIdStr(comps_ids_t compId);

private:
    std::vector<u_int8_t>   _data;
    u_int32_t               _size;
    comps_ids_t             _type;
    u_int32_t               _componentIndex;
    bool                    _initialized;

};

typedef enum {
    FWCOMPS_SUCCESS       = 0x0,
    FWCOMPS_INFO_TYPE_NOT_SUPPORTED,
    FWCOMPS_COMP_NOT_SUPPORTED,
    FWCOMPS_REG_FAILED,
    FWCOMPS_READ_COMP_FAILED,
    FWCOMPS_QUERY_FAILED,
    FWCOMPS_MEM_ALLOC_FAILED,
    FWCOMPS_DOWNLOAD_FAILED,
    FWCOMPS_MCC_TOUT,
    FWCOMPS_ABORTED,
    FWCOMPS_READ_COMP_NOT_SUPPORTED,
    FWCOMPS_BAD_PARAM,
    FWCOMPS_GENERAL_ERR,
    FWCOMPS_CR_ERR,
    FWCOMPS_READ_OUTSIDE_IMAGE_RANGE,
    FWCOMPS_UNSUPPORTED_DEVICE,

    //MCC Return codes
    FWCOMPS_MCC_ERR_CODES = 0x100              ,
    FWCOMPS_MCC_ERR_ERROR                      ,
    FWCOMPS_MCC_ERR_REJECTED_DIGEST_ERR        ,
    FWCOMPS_MCC_ERR_REJECTED_NOT_APPLICABLE    ,
    FWCOMPS_MCC_ERR_REJECTED_UNKNOWN_KEY       ,
    FWCOMPS_MCC_ERR_REJECTED_AUTH_FAILED       ,
    FWCOMPS_MCC_ERR_REJECTED_UNSIGNED          ,
    FWCOMPS_MCC_ERR_REJECTED_KEY_NOT_APPLICABLE,
    FWCOMPS_MCC_ERR_REJECTED_BAD_FORMAT        ,
    FWCOMPS_MCC_ERR_BLOCKED_PENDING_RESET      ,
    FWCOMPS_MCC_UNEXPECTED_STATE               ,
    FWCOMPS_MCC_REJECTED_NOT_A_SECURED_FW       ,
    FWCOMPS_MCC_REJECTED_MFG_BASE_MAC_NOT_LISTED,
    FWCOMPS_MCC_REJECTED_NO_DEBUG_TOKEN         ,
    FWCOMPS_MCC_REJECTED_VERSION_NUM_MISMATCH   ,
    FWCOMPS_MCC_REJECTED_USER_TIMESTAMP_MISMATCH,
    FWCOMPS_MCC_REJECTED_FORBIDDEN_VERSION      ,
    FWCOMPS_MCC_FLASH_ERASE_ERROR               ,

    // errors regarding REG_ACCESS
    FWCOMPS_REG_ACCESS_OK = 0,
    FWCOMPS_REG_ACCESS_BAD_STATUS_ERR = 0x200,
    FWCOMPS_REG_ACCESS_BAD_METHOD,
    FWCOMPS_REG_ACCESS_NOT_SUPPORTED,
    FWCOMPS_REG_ACCESS_DEV_BUSY,
    FWCOMPS_REG_ACCESS_VER_NOT_SUPP,
    FWCOMPS_REG_ACCESS_UNKNOWN_TLV,
    FWCOMPS_REG_ACCESS_REG_NOT_SUPP,
    FWCOMPS_REG_ACCESS_CLASS_NOT_SUPP,
    FWCOMPS_REG_ACCESS_METHOD_NOT_SUPP,
    FWCOMPS_REG_ACCESS_BAD_PARAM,
    FWCOMPS_REG_ACCESS_RES_NOT_AVLBL,
    FWCOMPS_REG_ACCESS_MSG_RECPT_ACK,
    FWCOMPS_REG_ACCESS_UNKNOWN_ERR,
    FWCOMPS_REG_ACCESS_SIZE_EXCCEEDS_LIMIT,
    FWCOMPS_REG_ACCESS_CONF_CORRUPT,
    FWCOMPS_REG_ACCESS_LEN_TOO_SMALL,
    FWCOMPS_REG_ACCESS_BAD_CONFIG,
    FWCOMPS_REG_ACCESS_ERASE_EXEEDED,
    FWCOMPS_REG_ACCESS_INTERNAL_ERROR,

} fw_comps_error_t;


typedef enum {
    FSM_QUERY = 0,
    FSM_CMD_LOCK_UPDATE_HANDLE     = 0x1,
    FSM_CMD_RELEASE_UPDATE_HANDLE  = 0x2,
    FSM_CMD_UPDATE_COMPONENT       = 0x3,
    FSM_CMD_VERIFY_COMPONENT       = 0x4,
    FSM_CMD_ACTIVATE_COMPONENET    = 0x5,
    FSM_CMD_ACTIVATE_ALL           = 0x6,
    FSM_CMD_READ_COMPONENT         = 0x7,
    FSM_CMD_CANCEL                 = 0x8,
    FSM_CMD_CHECK_UPDATE_HANDLE    = 0x9,
    FSM_CMD_FORCE_HANDLE_RELEASE   = 0xA,
    FSM_CMD_READ_PENDING_COMPONENT = 0xB,
    FSM_CMD_UNDEFINED              = 0xFF,
} fsm_command_t;


class FwCompsMgr {
public:

    FwCompsMgr(const char* devname);
    FwCompsMgr(mfile* mf);
    FwCompsMgr(uefi_Dev_t *uefi_dev, uefi_dev_extra_t* uefi_extra);

    virtual ~FwCompsMgr();

    u_int32_t        getFwSupport();
    mfile*           getMfileObj() {return _mf;};

    bool             burnComponents (std::vector<FwComponent>& comps, ProgressFunc func=(ProgressFunc)NULL);
    bool             getFwComponents(std::vector<FwComponent>& comps, bool readEn=false);

    bool             readComponent(FwComponent::comps_ids_t compType,
                                    FwComponent& fwComp,
                                    bool readPending=false);

    bool             getComponentVersion(FwComponent::comps_ids_t compType,
                                         bool                     pending,
                                         component_version_st*    cmpVer);

    bool             queryFwInfo(fwInfoT* query);

    bool             forceRelease();
    fw_comps_error_t getLastError() { return _lastError;};
    const char*      getLastErrMsg();

    bool             readBlockFromComponent(FwComponent::comps_ids_t compId,
                                            u_int32_t offset,
                                            u_int32_t size,
                                            std::vector<u_int8_t>& data);

private:

    typedef enum {
        FSMST_IDLE           = 0x0,
        FSMST_LOCKED         = 0x1,
        FSMST_INITIALIZE     = 0x2,
        FSMST_DOWNLOAD       = 0x3,
        FSMST_VERIFY         = 0x4,
        FSMST_APPLY          = 0x5,
        FSMST_ACTIVATE       = 0x6,
        FSMST_UPLOAD         = 0x7,
        FSMST_UPLOAD_PENDING = 0x8,
        FSMST_NA             = 0xFF,
    } fsm_state_t;

    enum {
        MCC_ERRCODE_OK = 0x0,
        MCC_ERRCODE_ERROR,
        MCC_ERRCODE_REJECTED_DIGEST_ERR,
        MCC_ERRCODE_REJECTED_NOT_APPLICABLE,
        MCC_ERRCODE_REJECTED_UNKNOWN_KEY,
        MCC_ERRCODE_REJECTED_AUTH_FAILED,
        MCC_ERRCODE_REJECTED_UNSIGNED,
        MCC_ERRCODE_REJECTED_KEY_NOT_APPLICABLE,
        MCC_ERRCODE_REJECTED_BAD_FORMAT,
        MCC_ERRCODE_BLOCKED_PENDING_RESET,
        MCC_ERRCODE_REJECTED_NOT_A_SECURED_FW,
        MCC_ERRCODE_REJECTED_MFG_BASE_MAC_NOT_LISTED,
        MCC_ERRCODE_REJECTED_NO_DEBUG_TOKEN,
        MCC_ERRCODE_REJECTED_VERSION_NUM_MISMATCH,
        MCC_ERRCODE_REJECTED_USER_TIMESTAMP_MISMATCH,
        MCC_ERRCODE_REJECTED_FORBIDDEN_VERSION,
        MCC_ERRCODE_FLASH_ERASE_ERROR,
    };

    void          initialize(mfile* mf);

    void          generateHandle();

    fsm_command_t getStateMachine();

    bool          updateStateMachine(fsm_command_t newStatus);

    bool          accessComponent(u_int32_t   offset,
                            u_int32_t   size,
                            u_int32_t   data[],
                            access_type_t access,
                            ProgressFunc func=(ProgressFunc)NULL);

    bool           queryComponentStaus(u_int32_t componentIndex,
                            comp_status_st* query);

    bool           controlFsm(fsm_command_t command,
                            fsm_state_t   expStatus=FSMST_NA,
                            u_int32_t     size=0,
                            fsm_state_t   currState=FSMST_NA);

    bool           queryComponentInfo(u_int32_t componentIndex,
                            u_int8_t   readPending,
                            u_int32_t  infoType,
                            u_int32_t  dataSize,
                            u_int32_t* data);

    bool           runMCQI(u_int32_t componentIndex,
                            u_int8_t   readPending,
                            u_int32_t  infoType,
                            u_int32_t  dataSize,
                            u_int32_t  offset,
                            u_int32_t* data);

    bool          runNVDA(std::vector<u_int8_t>& buff,
                            u_int16_t len,
                            u_int32_t tlvType,
                            reg_access_method_t method,
                            bool queryDefault);

    bool           readComponentInfo(FwComponent::comps_ids_t compType,
                            comp_info_t infoType,
                            std::vector<u_int32_t>& retData,
                            bool readPending=false);

    void           getInfoAsVersion(std::vector<u_int32_t>& infoData,
                            component_version_st* cmpVer);

    reg_access_status_t getGI(mfile* mf, struct tools_open_mgir* gi);
    fw_comps_error_t regErrTrans(reg_access_status_t err);
    fw_comps_error_t mccErrTrans(u_int8_t err);

    bool           extractMacsGuids(fwInfoT* fwQuery);
    void           extractRomInfo(tools_open_mgir* mgir, fwInfoT* fwQuery);
    bool           refreshComponentsStatus();
    static void    deal_with_signal();

    std::vector<comp_query_st> _compsQueryMap;

    bool                _refreshed;
    bool                _clearSetEnv;
    bool                _openedMfile;
    comp_query_st*      _currCompQuery;
    comp_info_st        _currCompInfo;
    u_int32_t           _updateHandle;
    fsm_control_st      _lastFsmCtrl;
    u_int32_t           _componentIndex;
    fw_comps_error_t    _lastError;
    reg_access_status_t _lastRegAccessStatus;
    u_int32_t           _hwDevId;
    mfile* _mf;
};

#endif /* USER_MLXFWOPS_LIB_FW_COMPS_MGR_H_ */