#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <naeem/scard.h>
#include <naeem/log.h>


NAEEM_result
NAEEM_scard_open_context (NAEEM_scard_context_ptr scard_context_ptr) {
  NAEEM_uint32 result = SCardEstablishContext(SCARD_SCOPE_USER, NULL, NULL, scard_context_ptr);
  if (result != SCARD_S_SUCCESS) {
    printf ("INTERNAL SCARD ERROR: 0x%x\n", result);
    return NAEEM_RESULT_SCARD_CONTEXT_NOT_ESTABLISHED;
  }
  return NAEEM_RESULT_SUCCESS;
}


NAEEM_result
NAEEM_scard_close_context (NAEEM_scard_context scard_context) {
  SCardReleaseContext(scard_context);
  return NAEEM_RESULT_SUCCESS;
}


NAEEM_result
NAEEM_scard_enumerate_smart_cards (NAEEM_scard_context scard_context,
                                   NAEEM_string_ptr_ptr scard_names,
                                   NAEEM_uint32_ptr number_of_scards) {
  unsigned char sc_names[256];
  DWORD num_sc_names = 256;
  NAEEM_uint32 result = SCardListReaders(scard_context, NULL, sc_names, &num_sc_names);
  if (result != SCARD_S_SUCCESS) {
    printf ("INTERNAL SCARD ERROR: 0x%x\n", result);
    return NAEEM_RESULT_SCARD_LIST_NOT_OBTAINED;
  }
  if (num_sc_names == 0) {
    return NAEEM_RESULT_NO_SCARD_IS_FOUND;
  }  
  *scard_names = (NAEEM_string_ptr)malloc(10 * sizeof(NAEEM_string));
  char *p = sc_names;
  NAEEM_uint32 i = 0;
  while (*p != 0 && i < num_sc_names) {
    (*scard_names)[i] = (unsigned char*)malloc(128 * sizeof(unsigned char));
    strcpy((*scard_names)[i], p);
    p += strlen(p) + 1;
    i++;
  }
  *number_of_scards = i;
  return NAEEM_RESULT_SUCCESS;
}


NAEEM_result
NAEEM_scard_connect_card (NAEEM_scard_context scard_context,
                          NAEEM_string scard_name,
                          NAEEM_scard_handle_ptr scard_handle_ptr) {
  DWORD dwProtocol;
  NAEEM_uint32 result = SCardConnect(scard_context, scard_name, SCARD_SHARE_EXCLUSIVE, 
                                     SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1, scard_handle_ptr, &dwProtocol);
  if (result != SCARD_S_SUCCESS) {
    printf ("INTERNAL SCARD ERROR: 0x%x\n", result);
    return NAEEM_RESULT_SCARD_CAN_NOT_BE_CONNECTED;
  }
  return NAEEM_RESULT_SUCCESS;
}

NAEEM_result
NAEEM_scard_disconnect_card (NAEEM_scard_handle scard_handle) {
  NAEEM_uint32 result = SCardDisconnect(scard_handle, SCARD_UNPOWER_CARD);  
  if (result != SCARD_S_SUCCESS) {
    printf ("INTERNAL SCARD ERROR: 0x%x\n", result);
    return NAEEM_RESULT_SCARD_CAN_NOT_BE_DISCONNECTED;
  }
  return NAEEM_RESULT_SUCCESS;
}


NAEEM_result
NAEEM_scard_transmit (NAEEM_scard_handle scard_handle,
                      NAEEM_data send_buffer,
                      NAEEM_uint32 send_buffer_length,
                      NAEEM_data recv_buffer,
                      NAEEM_uint32_ptr recv_buffer_length) {
  SCARD_IO_REQUEST send_io = {SCARD_PROTOCOL_T1, sizeof(SCARD_IO_REQUEST)};
  SCARD_IO_REQUEST recv_io = {SCARD_PROTOCOL_T1, sizeof(SCARD_IO_REQUEST)};
  DWORD buffer_length = *recv_buffer_length;
  NAEEM_uint32 result = SCardTransmit(scard_handle, &send_io, send_buffer, send_buffer_length, 
                                                    &recv_io, recv_buffer, &buffer_length);
  if (result != SCARD_S_SUCCESS) {
    printf ("INTERNAL SCARD ERROR: 0x%x\n", result);
    return NAEEM_RESULT_SCARD_TRANSMIT_ERROR;
  }
  *recv_buffer_length = buffer_length;
  if (recv_buffer[buffer_length - 2] == 0x90 && recv_buffer[buffer_length - 1] == 0x00) {
    return NAEEM_RESULT_APDU_STATUS_90_00_SUCCESS;
  }
  NAEEM_char error[128];
  sprintf(error, "Transmit error: 0x%02x 0x%02x.", (NAEEM_uint32)recv_buffer[buffer_length - 2], 
                                            (NAEEM_uint32)recv_buffer[buffer_length - 1]);
  NAEEM_log_error("epass-reader", error);
  if (recv_buffer[buffer_length - 2] == 0x63 && recv_buffer[buffer_length - 1] == 0x00) {
    return NAEEM_RESULT_APDU_STATUS_63_00_VERIFICATION_FAILED;
  }
  if (recv_buffer[buffer_length - 2] == 0x69 && recv_buffer[buffer_length - 1] == 0x82) {
    return NAEEM_RESULT_APDU_STATUS_69_82_SECURITY_CONDITION_NOT_SATISFIED;
  }
  if (recv_buffer[buffer_length - 2] == 0x6a && recv_buffer[buffer_length - 1] == 0x82) {
    return NAEEM_RESULT_APDU_STATUS_6A_82_FILE_NOT_FOUND;
  }
  if (recv_buffer[buffer_length - 2] == 0x67 && recv_buffer[buffer_length - 1] == 0x00) {
    return NAEEM_RESULT_APDU_STATUS_67_00_WRONG_LENGTH;
  }
  return NAEEM_RESULT_SCARD_TRANSMIT_ERROR;
}