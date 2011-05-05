; Task Switch

;; Funcion para utilizar en la creacion de tareas
GLOBAL task_ret
task_ret:
    ret


;; Funcion del loader encargada de hacer el switch.
;; void loader_switchto(uint_32 pid);

extern cur_pid
extern task_table
GLOBAL loader_switchto
loader_switchto:
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
