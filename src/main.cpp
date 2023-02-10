#include "../includes/my.hpp"
#include "../includes/memory.hpp"
#include "../includes/image.hpp"
#include "../includes/registry.hpp"

void disable_input_buffering()
{
    tcgetattr(STDIN_FILENO, &original_tio);
    struct termios new_tio = original_tio;
    new_tio.c_lflag &= ~ICANON & ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_input_buffering()
{
    tcsetattr(STDIN_FILENO, TCSANOW, &original_tio);
}

void handle_interrupt(int signal)
{
    restore_input_buffering();
    printf("\n");
    exit(-2);
}

int main(int argc, const char *argv[])
{
    Memory *m_memory = new Memory();
    Image *image = new Image(m_memory, argv[1]);
    Registry *m_registry = new Registry();

    if (argc < 2)
    {
        /* show usage string */
        printf("lc3 [image-file1] ...\n");
        exit(2);
    }

    // for (int j = 1; j < argc; ++j)
    // {
    if (!image->read())
    {
        printf("failed to load image: %s\n", argv[1]);
        exit(1);
    }
    // }
    signal(SIGINT, handle_interrupt);
    disable_input_buffering();

    /* since exactly one condition flag should be set at any given time, set the Z flag */
    //    reg[R_COND] = FL_ZRO;
    m_registry->set(static_cast<uint16_t>(Registry::R_COND), static_cast<uint16_t>(Registry::FL_ZRO));

    /* set the PC to starting position */
    /* 0x3000 is the default */
    enum
    {
        PC_START = 0x3000
    };
    //    reg[R_PC] = PC_START;
    m_registry->set(Registry::R_PC, PC_START);

    bool running = true;
    while (running)
    {
        /* FETCH */
        uint16_t pc = m_registry->get(static_cast<uint16_t>(Registry::R_PC));
        m_registry->set(static_cast<uint16_t>(Registry::R_PC), pc + 1);
        uint16_t instr = m_memory->read(pc);
        printf("PC: %d\n", pc);
        uint16_t op = instr >> 12;

        // printf("%d\n", instr);
        switch (op)
        {
        case Registry::OP_ADD:
        {
            /* destination register (DR) */
            uint16_t r0 = (instr >> 9) & 0x7;
            /* first operand (SR1) */
            uint16_t r1 = (instr >> 6) & 0x7;
            /* whether we are in immediate mode */
            uint16_t imm_flag = (instr >> 5) & 0x1;

            if (imm_flag)
            {
                uint16_t imm5 = m_registry->sign_extend(instr & 0x1F, 5);
                m_registry->set(r0, m_registry->get(r1) + imm5);
            }
            else
            {
                uint16_t r2 = instr & 0x7;

                m_registry->set(r0, m_registry->get(r1) + m_registry->get(r2));
            }

            m_registry->update_flags(r0);
        }
        break;
        case Registry::OP_AND:
        {
            uint16_t r0 = (instr >> 9) & 0x7;
            uint16_t r1 = (instr >> 6) & 0x7;
            uint16_t imm_flag = (instr >> 5) & 0x1;

            if (imm_flag)
            {
                uint16_t imm5 = m_registry->sign_extend(instr & 0x1F, 5);

                m_registry->set(r0, m_registry->get(r1) & imm5);
            }
            else
            {
                uint16_t r2 = instr & 0x7;
                // reg[r0] = reg[r1] & reg[r2];
                m_registry->set(r0, m_registry->get(r1) & m_registry->get(r2));
            }
            m_registry->update_flags(r0);
        }
        break;
        case Registry::OP_NOT:
        {
            uint16_t r0 = (instr >> 9) & 0x7;
            uint16_t r1 = (instr >> 6) & 0x7;

            m_registry->set(r0, ~m_registry->get(r1));
            m_registry->update_flags(r0);
        }
        break;
            /* TODO: check if there is a bug here */

        case Registry::OP_BR:
        {
            uint16_t pc_offset = m_registry->sign_extend(static_cast<int>(instr & 0x1FF), 9);
            uint16_t cond_flag = (instr >> 9) & 0x7;

            if (cond_flag & m_registry->get(static_cast<uint16_t>(Registry::R_COND)))
            {
                // printf("\n\n\npc reg\n\n: %d", m_registry->get(static_cast<uint16_t>(Registry::R_PC)));
                m_registry->set(static_cast<uint16_t>(Registry::R_PC), m_registry->get(static_cast<uint16_t>(Registry::R_PC)) + pc_offset);
                // printf("pc reg saved: %d\n\n", m_registry->get(static_cast<uint16_t>(Registry::R_PC)));
                // printf("pc offset: %d\n\n", pc_offset);
            }
        }
        break;

            /* TODO: check if there is a bug here */

        case Registry::OP_JMP:
        {
            //     /* Also handles RET */
            //     uint16_t r1 = (instr >> 6) & 0x7;

            //     m_registry->set(static_cast<uint16_t>(Registry::R_PC), m_registry->get(r1));
        }
        break;

            /* TODO: check if there is a bug here */

        case Registry::OP_JSR:
        {
            //     uint16_t long_flag = (instr >> 11) & 1;

            //     m_registry->set(static_cast<uint16_t>(Registry::R_R7), m_registry->get(static_cast<uint16_t>(Registry::R_PC)));
            //     if (long_flag)
            //     {
            //         uint16_t long_pc_offset = m_registry->sign_extend(instr & 0x7FF, 11);

            //         m_registry->set(static_cast<uint16_t>(Registry::R_PC), m_registry->get(static_cast<uint16_t>(Registry::R_PC)) + long_pc_offset); /* JSR */
            //     }
            //     else
            //     {
            //         uint16_t r1 = (instr >> 6) & 0x7;

            //         m_registry->set(static_cast<uint16_t>(Registry::R_PC), m_registry->get(r1)); /* JSRR */
            //     }
        }
        break;
        case Registry::OP_LD:
        {
            uint16_t r0 = (instr >> 9) & 0x7;
            uint16_t pc_offset = m_registry->sign_extend(instr & 0x1FF, 9);

            m_registry->set(r0, m_memory->read(m_registry->get(static_cast<uint16_t>(Registry::R_PC)) + pc_offset));
            m_registry->update_flags(r0);
        }
        break;
        case Registry::OP_LDI:
        {
            /* destination register (DR) */
            uint16_t r0 = (instr >> 9) & 0x7;
            /* PCoffset 9*/
            uint16_t pc_offset = m_registry->sign_extend(instr & 0x1FF, 9);

            /* add pc_offset to the current PC, look at that memory location to get the final address */
            m_registry->set(r0, m_memory->read(m_memory->read(m_registry->get(static_cast<uint16_t>(Registry::R_PC)) + pc_offset)));
            m_registry->update_flags(r0);
        }
        break;
        case Registry::OP_LDR:
        {
            uint16_t r0 = (instr >> 9) & 0x7;
            uint16_t r1 = (instr >> 6) & 0x7;
            uint16_t offset = m_registry->sign_extend(instr & 0x3F, 6);

            m_registry->set(r0, m_memory->read(m_registry->get(r1) + offset));
            m_registry->update_flags(r0);
        }
        break;
        case Registry::OP_LEA:
        {
            uint16_t r0 = (instr >> 9) & 0x7;
            uint16_t pc_offset = m_registry->sign_extend(instr & 0x1FF, 9);

            m_registry->set(r0, m_memory->read(m_registry->get(Registry::R_PC) + pc_offset));
            m_registry->update_flags(r0);
        }
        break;
        case Registry::OP_ST:
        {
            uint16_t r0 = (instr >> 9) & 0x7;
            uint16_t pc_offset = m_registry->sign_extend(instr & 0x1FF, 9);

            m_memory->write(m_registry->get(static_cast<uint16_t>(Registry::R_PC)) + pc_offset, m_registry->get(r0));
        }
        break;
        case Registry::OP_STI:
        {
            uint16_t r0 = (instr >> 9) & 0x7;
            uint16_t pc_offset = m_registry->sign_extend(instr & 0x1FF, 9);

            m_memory->write(m_memory->read(m_registry->get(static_cast<uint16_t>(Registry::R_PC)) + pc_offset), m_registry->get(r0));
        }
        break;
        case Registry::OP_STR:
        {
            uint16_t r0 = (instr >> 9) & 0x7;
            uint16_t r1 = (instr >> 6) & 0x7;
            uint16_t offset = m_registry->sign_extend(instr & 0x3F, 6);

            m_memory->write(m_registry->get(r1) + offset, m_registry->get(r0));
        }
        break;
        case Registry::OP_TRAP:
            m_registry->set(static_cast<uint16_t>(Registry::R_R7), m_registry->get(static_cast<uint16_t>(Registry::R_PC)));
            switch (instr & 0xFF)
            {
            case Registry::TRAP_GETC:
                /* read a single ASCII char */
                m_registry->set(static_cast<uint16_t>(Registry::R_R0), static_cast<uint16_t>(getchar()));
                m_registry->update_flags(static_cast<uint16_t>(Registry::R_R0));
                break;
            case Registry::TRAP_OUT:
                putc(static_cast<char>(m_registry->get(static_cast<uint16_t>(Registry::R_R0))), stdout);
                fflush(stdout);
                break;
            case Registry::TRAP_PUTS:
            {
                /* one char per word */
                uint16_t *c = m_memory->get() + m_registry->get(static_cast<uint16_t>(Registry::R_R0));
                while (*c)
                {
                    putc(static_cast<char>(*c), stdout);
                    ++c;
                }
                fflush(stdout);
            }
            break;
            case Registry::TRAP_IN:
            {
                printf("Enter a character: ");
                char c = getchar();
                putc(c, stdout);
                fflush(stdout);
                m_registry->set(m_registry->get(static_cast<uint16_t>(Registry::R_R0)), static_cast<uint16_t>(c));

                m_registry->update_flags(static_cast<uint16_t>(Registry::R_R0));
            }
            break;
            case Registry::TRAP_PUTSP:
            {
                /* one char per byte (two bytes per word)
                   here we need to swap back to
                   big endian format */
                uint16_t *c = m_memory->get() + m_registry->get(static_cast<uint16_t>(Registry::R_R0));
                while (*c)
                {
                    char char1 = (*c) & 0xFF;
                    putc(char1, stdout);
                    char char2 = (*c) >> 8;
                    if (char2)
                        putc(char2, stdout);
                    ++c;
                }
                fflush(stdout);
            }
            break;
            case Registry::TRAP_HALT:
                puts("HALT");
                fflush(stdout);
                running = 0;
                break;
            }
            break;
        case Registry::OP_RES:
            abort();
            break;
        case Registry::OP_RTI:
            abort();
            break;
        default:
            abort();
            break;
        }
    }
    restore_input_buffering();
}
