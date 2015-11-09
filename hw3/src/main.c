#include "includes/screen.h"
#include "includes/desc_tables.h"
#include "includes/pic.h"
#include "includes/serial_port.h"
#include "includes/task.h"

char buf;
int is_ready_to_write = 0;

void read(void){
    char ch;
    while(1){
        buf = read_serial();
        is_ready_to_write = 1;
    }
}

void write(void){
    while(1){
        while(!is_ready_to_write);
        printf("%c", buf);
        is_ready_to_write = 0;
    }
}

void first_thread(void){
    int counter = 0;
    
    // asm volatile ("int $0x20");
    while(1){
        counter++;
        // asm volatile ("sti");
        if(counter % 10000000 == 0)
        {
            printf("First thread message: %d\n",counter);
            asm volatile ("int $0x20");
        }
    }
}

void second_thread(void){
    int counter = 0;
    while(1){
        counter++;
        // asm volatile ("int $0x20");
        if(counter % 10000000 == 0)
        {
            counter++;
            // asm volatile ("int $0x20");
            printf("Second thread message: %d\n",counter);
            asm volatile ("int $0x20");
        }
    }
}

void cmain(void)
{
    cls();
    
    init_tables();
    init_timer();
    // init_serial();

    add_task(first_thread);
    add_task(second_thread);

    asm volatile ("sti");

    int counter = 0;
    while(1)
    {
        counter++;
        
        if(counter % 10000000 == 0)
        {
            printf("Main thread message: %d\n",counter);
            // asm volatile ("int $0x20");
        }
    }
}
