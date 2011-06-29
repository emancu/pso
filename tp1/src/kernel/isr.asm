[bits 32]

syscall_dir: resq 1

%include "interrupt.mac"

[EXTERN debug_kernelpanic]
[EXTERN error_num]

isr_define_ep isr_timerTick, isr_timerTick_c
isr_define_ep isr_keyboard, isr_keyboard_c
isr_define_page_handler isr_page_fault, isr_page_fault_c
isr_define_ep isr_fdd, isr_fdd_c, 38


isr_define_ep isr_serial, isr_serial_c

isr_dkp_e isr_0_DE, 0
isr_dkp_e isr_1_DB, 1
isr_dkp_e isr_2_NMI, 2
isr_dkp_e isr_3_BP, 3
isr_dkp_e isr_4_OF, 4
isr_dkp_e isr_5_BR, 5
isr_dkp_e isr_6_UD, 6
isr_dkp_e isr_7_NM, 7
isr_dkp_E isr_8_DF, 8
isr_dkp_e isr_9_CSO, 9
isr_dkp_E isr_A_TS, 10
isr_dkp_E isr_B_NP, 11
isr_dkp_E isr_C_SS, 12
isr_dkp_E isr_D_GP, 13
isr_dkp_E isr_E_PF, 14
isr_dkp_e isr_10_MF, 16
isr_dkp_E isr_11_AC, 17
isr_dkp_e isr_12_MC, 18
isr_dkp_e isr_13_XM, 19

[EXTERN syscall_list]
[GLOBAL isr_syscall]
isr_syscall:
  sti ;FIXME: Necesario??
  ;xchg bx, bx
  ;con esto se puede pasar hasta 4 parametros en las syscalls
  push edi
  push ebx
  push ecx
  push edx


  lea eax, [syscall_list+eax*4]
  mov eax, [eax]
  mov [syscall_dir], eax
  call [syscall_dir]

  pop edx
  pop ecx
  pop ebx
  pop edi
  iret
