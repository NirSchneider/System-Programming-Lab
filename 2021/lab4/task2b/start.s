section	.rodata                  
    format_string: db "%s", 10, 0 ; format string
section .data
    msg: db 'Hello, infected File', 0Ah
    length: equ $ - msg
    fd: dd 0
    lengthFile: dd 0,0
section .text
    global _start
    global system_call
    global code_start
    global code_end
    global infector
    extern main

_start:
    pop    dword ecx              
    mov    esi,esp                                    
    mov     eax,ecx               
    shl     eax,2               
    add     eax,esi              
    add     eax,4                
    push    dword eax            
    push    dword esi            
    push    dword ecx            

    call    main                 
    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop

system_call:
    push    ebp                  
    mov     ebp, esp
    sub     esp, 4               
    pushad                       

    mov     eax, [ebp+8]         
    mov     ebx, [ebp+12]        
    mov     ecx, [ebp+16]        
    mov     edx, [ebp+20]        
    int     0x80                 
    mov     [ebp-4], eax         
    popad                        
    mov     eax, [ebp-4]         
    add     esp, 4               
    pop     ebp                  
    ret                          


code_start:
    infection:
        push    ebp                
        mov     ebp, esp
        sub     esp, 4 
        pushad                     
        mov     eax, 4             
        mov     ebx, 1             
        mov     ecx, msg           
        mov     edx, length        
        int     0x80               
        mov     [ebp-4], eax         
        mov     [ebp-4], eax          
        popad                         
        mov     eax, [ebp-4]          
        add     esp, 4                
        pop     ebp                   
        ret                           
code_end:
    
    infector:
        push    ebp                   
        mov     ebp, esp
        sub     esp, 4 
        pushad  
        mov eax, code_end
       sub eax, code_start
        mov [lengthFile],eax
        mov     eax, 5
        mov     ebx, [ebp+8] 
        mov     ecx, 1
        mov     edx, 0x644
        int     0x80 ; OPEN FILE
        mov     [fd] , eax
        mov     eax, 19
        mov     ebx, [fd]
        mov     ecx, 0 
        mov     edx, 2
        int     0x80 ; LSEEK
        mov     eax, 4  
        mov     ebx, [fd]
        mov     ecx, code_end
        mov     edx, [lengthFile]
        int     0x80 ; Write
        mov     ebx, [fd]
        mov     eax, 6
        int     0x80 ;; close file    
        mov     [ebp-4], eax         
        popad                        
        mov     eax, [ebp-4]          
        add     esp, 4               
        pop     ebp                  
        ret                         
