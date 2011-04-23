Programacion de Sistemas Operativos
===================================

Integrantes
-----------

 * Alejandro Mataloni
 * Emiliano  Mancuso
 * Martin     Miguel


Punteo para el desarrollo
-------------------------

###mm_init

* **inicializar estructuras**: la memoria solo cuenta con dos estructuras de manejo de *page frames* libres. Una maneja los page frames del kernel (solo los ultimos 3mb del espacio de kernel) y los del usuario (el resto de la memoria). Para el kernel nos alcanza con un arreglo de bits con suficientes bits para las paginas en 3 mb. Para el usuario necesitamos bits para frames para un total de 4gb - 4mb. No obstante, cuantas paginas de estas son realmente usables depende del total de memoria fisica. La inicializacion de estructuras debe conocer esto y guadar la informacion en un valor que limite hasta que bit del arreglo debe leerse. Un bit en 0 indica que la pagina esta libre, el bit 1 significa que esta ocupada.

* **detectar memoria total**: esto se realiza con una llamada al bios INT 0x15, EAX = 0xE801. (Ver detalles: http://wiki.osdev.org/Detecting_Memory_%28x86%29#BIOS_Function:_INT_0x15.2C_AX_.3D_0xE801)

* **reservar paginas**: mm_mem_alloc y mm_mem_kalloc buscan un bit libre (0) en el arreglo correspondiente y devuelve la direccion fisica representada por ese page frame. (Deberia mapearlo en alguna estructura de paginas?)

* **liberar paginas**: mm_mem_free solo pone en 0 la entrada correspondiente a la pagina recibida.

* **syscall palloc**: busca un bit en 0 en la estructura de page frames de usuario (puede fallar si no hay), luego usa el cr3 para seguir el arbol de paginas y mapearla en la primera posicion disponible. Llena los atributos de la manera standard (read/write, excecutable, usuario, presente). Puede fallar si no hay mas paginas virtuales para mapear o si el kernel no tiene mas page frames libres y se necesita uno nuevo para una nueva tabla de paginas. Si todo esto funciona pone en 1 el bit en la estrucutra de page frames de usuario y devuelve la direccion virtual a la que fue mapeada la pagina.

