#include <signal.h>
#include <ucontext.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

extern int test_1();
extern int test_2(); 


namespace
{
    size_t match_div_instruction(const uint8_t *code)
    {        
        int i = ((code[0] & 0xF0) == 0x40) ? 1 : 0;

        uint8_t opcode = code[i++];
        if (opcode != 0xF6 && opcode != 0xF7) 
            return -1;      

        uint8_t modrm = code[i++];
        uint8_t reg   = (modrm >> 3) & 7; // reg часть modrm, которая дополняет opcode
        if (reg != 6 && reg != 7) // не div/idiv
            return -1;       

        uint8_t mod = (modrm >> 6) & 3;     // mod часть проверяет второй операнд в регистре или памяти          
        if (mod != 3) // mod == 3 - в регистре, иначе в памяти. В случае регистра команда закончена  
        { 
            uint8_t rm  = modrm & 7; //кодирует базовый регистр адресации
            if (rm == 4) // есть SIB байт
                i += 1;

            if (mod == 0 && rm == 5) // RIP + Disp32
                i += 4;   
            else if (mod == 1) 
                i += 1;         // disp8
            else if (mod == 2) 
                i += 4;         // disp32 
        }

        return i;
    }


    void handler(int sig, siginfo_t *info, void *ctx)
    {
        if(info->si_code == FPE_INTDIV)
        {
            ucontext_t *uc = (ucontext_t *)ctx;

            uint64_t rip = uc->uc_mcontext.gregs[REG_RIP];
            const uint8_t *code = (const uint8_t *)rip;

            size_t instr_len = match_div_instruction(code);

            if (instr_len > 0) 
            {
                uc->uc_mcontext.gregs[REG_RAX] = 0;
                uc->uc_mcontext.gregs[REG_RDX] = 0;
                uc->uc_mcontext.gregs[REG_RIP] += instr_len;
            }
        } 
    }

    void install_sigfpe_handler(void)
    {
        struct sigaction sa;
        memset(&sa, 0, sizeof(sa));
        sa.sa_sigaction = handler;
        sa.sa_flags = SA_SIGINFO;
        sigemptyset(&sa.sa_mask);

        if (sigaction(SIGFPE, &sa, NULL) != 0) {
            perror("sigaction");
            _exit(1);
        }
    }

}


int main(int, char**){
    install_sigfpe_handler();

    printf("=== Scenario 1 (volatile throughout) ===\n");
    test_1();

    printf("\n=== Scenario 2 (volatile read into local) ===\n");
    test_2();

    return 0;
}
