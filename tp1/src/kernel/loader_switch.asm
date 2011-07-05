; Task Switch

;; Funcion para utilizar en la creacion de tareas
GLOBAL task_ret
task_ret:
;xchg bx, bx
    mov eax, 0x23
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    iret

;; Funcion para utilizar en la creacion de tareas
GLOBAL fork_ret
fork_ret:
;xchg bx, bx
    mov eax, 0x23
    mov ds, ax
    mov es, ax
    mov gs, ax
    mov fs, ax
    mov eax, 0x0
    iret

;; Funcion del loader encargada de hacer el switch.
;; void loader_switchto(uint_32 pid);

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

    mov esi, [ebp+8]  ; traemos el pid que recibimos como parametro
    mov ebx, 16       ; sizeof(*task_table)

    mov eax, [cur_pid]
    mul ebx                       ; en eax calculamos el desplazamiento en el array
    lea eax, [task_table+eax+12]  ; current esp

    mov [eax], esp                ;guardamos el curren esp en la tarea vieja

    mov eax, esi
    mul ebx
    lea eax, [task_table+eax+8]   ; new cr3
    mov eax, [eax]
    mov cr3, eax

    mov eax, esi
    mul ebx
    lea eax, [task_table+eax+12]  ; new ESP

    mov esp, [eax]
    mov [cur_pid], esi

    pop edi
    pop esi
    pop ebx

    pop ebp
    ret



extern mm_dir_free
GLOBAL loader_switchto_exit
loader_switchto_exit:
    ;xchg bx,bx
    push ebp
    mov ebp, esp

    push ebx
    push esi
    push edi

    mov esi, [ebp+8]  ; traemos el pid que recibimos como parametro
    mov ebx, 16       ; sizeof(*task_table)
    mov edi, [ebp+12] ; cr3 a borrar

    mov eax, [cur_pid]
    mul ebx                       ; en eax calculamos el desplazamiento en el array
    lea eax, [task_table+eax+12]  ; current esp

    mov [eax], esp                ;guardamos el curren esp en la tarea vieja

    mov eax, esi
    mul ebx
    lea eax, [task_table+eax+8]   ; new cr3
    mov eax, [eax]
    mov cr3, eax

    mov eax, esi
    mul ebx
    lea eax, [task_table+eax+12]  ; new ESP

    mov esp, [eax]        ; Cambiamos e pila
    xchg [cur_pid], esi

    push edi
    call mm_dir_free
    add esp, 4
    
    mov ebx, 16
    mov eax, esi
    mul ebx
    mov DWORD [task_table+eax+8], 0   ; old cr3 := 0

    pop edi
    pop esi
    pop ebx

    pop ebp
    ret
