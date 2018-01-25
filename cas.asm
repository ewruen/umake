        .686                      ; create 32 bit code
        .mmx
        .xmm                     
        .model flat, stdcall      ; 32 bit memory model
        option casemap :none      ; case sensitive

.code 
align 16
@store32@8 proc syscall 
 option prologue:none, epilogue:none
 ;; int32_t fastcall store32 (void *p, int32_t val);
 ;; ecx <- p 
 ;; edx <- val 
 mov eax, edx
 lock xchg dword ptr[ecx], eax 
 ret  

@store32@8 endp 

align 16
@add32@8 proc syscall 
 option prologue:none, epilogue:none
 ;; int32_t fastcall add32 (void *p, int32_t val);
 ;; ecx <- p 
 ;; edx <- val 
 mov eax, edx
 lock xadd dword ptr[ecx], eax 
 ret  

@add32@8 endp 

align 16
@or32@8 proc syscall 
 option prologue:none, epilogue:none
 ;; int32_t fastcall or32 (void *p, int32_t val);
 ;; ecx <- p 
 ;; edx <- val 
 lock or dword ptr[ecx], edx 
 ret  

@or32@8 endp 

align 16
@add64@12 proc syscall
 option prologue:none, epilogue:none  
 ;; int64_t fastcall add64 (void *p, int64_t val);
 ;; ecx <- ptr
 ;; esp +4 <- val-lo 
 ;; esp +8 <- val-hi  
  push ebx ;; save ebx 
  push esi 
  push edi 
  push ebp

  mov esi, ecx
  mov edi, [esp+ 4+16] ;; add - lo

  xor eax, eax 
  mov ebp, [esp+ 8+16] ;; add - hi

align 16
@@:
  mov eax, [esi+0] ;; cmp - old - lo 
  mov edx, [esi+4] ;; cmp - old - hi
  mov ebx, edi     ;; add - lo cache 
  mov ecx, ebp     ;; add - hi cache 
  add ebx, eax     ;; add LODWORD
  adc ecx, edx     ;; add HIDWORD with carry 
  ;; cmpxchg8b http://www.felixcloutier.com/x86/CMPXCHG8B:CMPXCHG16B.html
  lock cmpxchg8b mmword ptr[esi] ;; CAS-write 
  jnz @B 

  pop ebp 
  pop edi 
  pop esi 
  pop ebx 

  ret 8
        
@add64@12 endp 

align 16
@casdp3@12 proc syscall
 option prologue:none, epilogue:none
 ;; kbool fastcall casdp3 (  void **p, 
 ;;                               void **val_gp, 
 ;;                               void **cmp_gp );      
 ;; ecx <- ptr 
 ;; edx <- val_gp
 ;; esp+4 <- cmp_gp 
 push ebx     
 push esi     
    
 mov ebx, [edx+0] 
 mov esi, ecx

 mov ecx, [edx+4]
 mov eax, [esp+4+8]

 mov edx, [eax+4] 
 mov eax, [eax]
 
 lock cmpxchg8b mmword ptr[esi]
 setz al 
 and eax, 1 

 pop esi 
 pop ebx 

 ret 4
        
@casdp3@12 endp 


end