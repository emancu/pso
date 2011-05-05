; Task Switch


GLOBAL task_ret
task_ret:
;xchg bx,bx
    ret



extern cur_pid
extern task_table
GLOBAL loader_switchto
loader_switchto:
    ;xchg bx,bx
    push ebp
    mov ebp, esp


    push ebx
    push esi
    push edi

    mov esi, [ebp+8]; traemos el pid a switchear


    ;mov ecx, [cur_pid]
    mov eax, [cur_pid]
    mov edx, 16 ; sizeof(*task_table)
    mul edx ;
    lea eax, [task_table+eax+12] ; current esp

    mov [eax], esp;guardamos el curren esp en la tarea vieja

    mov edx, 16 ; sizeof(*task_table)
    mov eax, esi
    mul edx ;
    lea eax, [task_table+eax+8] ; new cr3
    mov eax, [eax]
    mov cr3, eax

    ;xchg bx,bx
    mov edx, 16 ; sizeof(*task_table)
    mov eax, esi
    mul edx ;
    lea eax, [task_table+eax+12] ; new ESP

    mov esp, [eax]
    mov [cur_pid], esi

    pop edi
    pop esi
    pop ebx

    pop ebp
    ret
