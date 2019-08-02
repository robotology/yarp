#include <stdbool.h> // C99+
#include <windows.h>

void yarp_logger_enable_vt_colors(void)
{
    static bool enabled = false;

    if (!enabled)
    {
        DWORD handleMode = 0;
        HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

        if (hStdout != INVALID_HANDLE_VALUE && GetConsoleMode(hStdout, &handleMode))
        {
            handleMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            SetConsoleMode(hStdout, handleMode);
        }

        enabled = true;
    }
}
