// SPDX-License-Identifier: BSD-2-Clause-Patent
#include "shim.h"

EFI_STATUS
tdx_log_event_raw(EFI_PHYSICAL_ADDRESS buf, UINTN size, UINT8 pcr,
                  const CHAR8 *log, UINTN logsize, UINT32 type, CHAR8 *hash)
{
	EFI_STATUS efi_status;
	EFI_TD_EVENT *event;
	efi_td_protocol_t *tdx;
	EFI_TD_MR_INDEX mr;

	efi_status = LibLocateProtocol(&EFI_TD_PROTOCOL_GUID, (VOID **)&tdx);
	if (EFI_ERROR(efi_status) || tdx == NULL)
		return EFI_SUCCESS;

	efi_status = tdx->map_pcr_to_mr_index(tdx, pcr, &mr);
	if (EFI_ERROR(efi_status))
		return EFI_NOT_FOUND;

	UINTN event_size = sizeof(*event) - sizeof(event->Event) + logsize;

	event = AllocatePool(event_size);
	if (!event) {
		perror(L"Unable to allocate event structure\n");
		return EFI_OUT_OF_RESOURCES;
	}

	event->Header.HeaderSize = sizeof(EFI_TD_EVENT_HEADER);
	event->Header.HeaderVersion = 1;
	event->Header.MrIndex = mr;
	event->Header.EventType = type;
	event->Size = event_size;
	CopyMem(event->Event, (VOID *)log, logsize);
	if (hash) {
		efi_status = tdx->hash_log_extend_event(tdx, PE_COFF_IMAGE, buf,
		                                        (UINT64)size, event);
	}

	if (!hash || EFI_ERROR(efi_status)) {
		efi_status = tdx->hash_log_extend_event(tdx, 0, buf,
		                                        (UINT64)size, event);
	}
	FreePool(event);
	return efi_status;
}