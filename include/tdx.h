// SPDX-License-Identifier: BSD-2-Clause-Patent

#ifndef SHIM_TDX_H
#define SHIM_TDX_H

typedef struct tdEFI_TD_VERSION {
  uint8_t Major;
  uint8_t Minor;
} __attribute__ ((packed)) EFI_TD_VERSION;

typedef uint32_t EFI_TD_EVENT_LOG_BITMAP;
typedef uint32_t EFI_TD_EVENT_LOG_FORMAT;
typedef uint32_t EFI_TD_EVENT_ALGORITHM_BITMAP;
typedef uint32_t EFI_TD_MR_INDEX;

typedef struct tdEFI_TD_EVENT_HEADER {
  //
  // Size of the event header itself (sizeof(EFI_TD_EVENT_HEADER)).
  //
  uint32_t HeaderSize;
  //
  // Header version. For this version of this specification, the value shall be 1.
  //
  uint16_t HeaderVersion;
  //
  // Index of the MR that shall be extended.
  //
  EFI_TD_MR_INDEX MrIndex;
  //
  // Type of the event that shall be extended (and optionally logged).
  //
  uint32_t EventType;
} __attribute__ ((packed)) EFI_TD_EVENT_HEADER;

typedef struct tdEFI_TD_EVENT {
  //
  // Total size of the event including the Size component, the header and the Event data.
  //
  uint32_t Size;
  EFI_TD_EVENT_HEADER Header;
  uint8_t Event[1];
} __attribute__ ((packed)) EFI_TD_EVENT;

typedef struct tdEFI_TD_BOOT_SERVICE_CAPABILITY {
  //
  // Allocated size of the structure
  //
  uint8_t Size;
  //
  // Version of the EFI_TD_BOOT_SERVICE_CAPABILITY structure itself.
  // For this version of the protocol, the Major version shall be set to 1
  // and the Minor version shall be set to 1.
  //
  EFI_TD_VERSION StructureVersion;
  //
  // Version of the EFI TD protocol.
  // For this version of the protocol, the Major version shall be set to 1
  // and the Minor version shall be set to 1.
  //
  EFI_TD_VERSION ProtocolVersion;
  //
  // Supported hash algorithms
  //
  EFI_TD_EVENT_ALGORITHM_BITMAP HashAlgorithmBitmap;
  //
  // Bitmap of supported event log formats
  //
  EFI_TD_EVENT_LOG_BITMAP SupportedEventLogs;
  //
  // False = TD not present
  //
  BOOLEAN TdPresentFlag;
} EFI_TD_BOOT_SERVICE_CAPABILITY;

struct efi_td_protocol
{
  EFI_STATUS (EFIAPI *get_capability) (struct efi_td_protocol *this,
              EFI_TD_BOOT_SERVICE_CAPABILITY *ProtocolCapability);
  EFI_STATUS (EFIAPI *get_event_log) (struct efi_td_protocol *this,
				      EFI_TD_EVENT_LOG_FORMAT EventLogFormat,
				      EFI_PHYSICAL_ADDRESS *EventLogLocation,
				      EFI_PHYSICAL_ADDRESS *EventLogLastEntry,
				      BOOLEAN *EventLogTruncated);
  EFI_STATUS (EFIAPI *hash_log_extend_event) (struct efi_td_protocol *this,
					    uint64_t Flags,
					    EFI_PHYSICAL_ADDRESS DataToHash,
					    uint64_t DataToHashLen,
					    EFI_TD_EVENT *EfiTdEvent);
  EFI_STATUS (EFIAPI *map_pcr_to_mr_index) (struct efi_td_protocol *this,
              uint32_t PcrIndex,
              EFI_TD_MR_INDEX *MrIndex);
};

typedef struct efi_td_protocol efi_td_protocol_t;

EFI_STATUS tdx_log_event_raw(EFI_PHYSICAL_ADDRESS buf, UINTN size,
			     UINT8 pcr, const CHAR8 *log, UINTN logsize,
			     UINT32 type, CHAR8 *hash);

#endif /* SHIM_TDX_H */
// vim:fenc=utf-8:tw=75
