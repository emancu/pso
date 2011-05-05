;
; Tasks.asm, truco para incluir las tareas en el kernel.bin
;


%macro include_task 2 ; task , taskfile
global task_%1_pso
task_%1_pso:
incbin %2
global task_%1_pso_end
task_%1_pso_end:
%endmacro

include_task task1, "tasks/task1.pso"

include_task task2, "tasks/task2.pso"
