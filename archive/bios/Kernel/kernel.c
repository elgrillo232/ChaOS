void main() {
    volatile unsigned char* video_memory = (volatile unsigned char*)0xb8000;
    video_memory[0] = 'X';
    video_memory[1] = 0x0F; /* white on black */

    for (;;) {
        __asm__ __volatile__("hlt");
    }
}