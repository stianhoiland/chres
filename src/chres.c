#include <stddef.h> /* size_t */
#include <wchar.h> /* wchar_t */

#define lenof(a) (int)(sizeof(a) / sizeof(*(a)))
#define s(s) (char *)s, lenof(s)-1

typedef unsigned short ushort; /* shorter short */

enum {
  CDS_UPDATEREGISTRY = 0x00000001,
  DM_PELSWIDTH = 0x00080000,
  DM_PELSHEIGHT = 0x00100000
};

#define W32 __attribute__((dllimport, stdcall))
W32 wchar_t *GetCommandLineW();
W32 wchar_t **CommandLineToArgvW(wchar_t *, int *);
W32 size_t GetStdHandle(int);
W32 int WriteFile(size_t, char *, int, int *, size_t);
W32 void ExitProcess(int) __attribute((noreturn));
W32 int ChangeDisplaySettingsW(void *, int);

static void Print(int fd, char *data, int len)
{
  WriteFile(GetStdHandle(-10 - fd), data, len, &(int){0}, 0);
}

static int ParseUnsigned(wchar_t **str)
{
  wchar_t *c = *str;
  while ((*c < L'0' || L'9' < *c) && *c != L'\0') {
    c++;
  }
  int result = 0;
  while (L'0' <= *c && *c <= L'9') {
    result = result * 10 + (int)(*c - L'0');
    c++;
  }
  *str = c;
  return result;
}

void __stdcall mainCRTStartup()
{
  int argc = 0;
  wchar_t **argv = CommandLineToArgvW(GetCommandLineW(), &argc);

  if (argc != 2) {
    Print(2, s("Example usage: chdir 1920x1080@144\n"));
    ExitProcess(1);
  }

  wchar_t *c = argv[1];
  int w = ParseUnsigned(&c);
  int h = ParseUnsigned(&c);

  struct { /* DEVMODEW */
       char _0x00[68];
     ushort dmSize;
       char _0x46[1];
        int dmFields;
       char _0x4B[96];
        int dmPelsWidth, dmPelsHeight;
       char _0xB3[40];
  } desiredMode = {0};
  desiredMode.dmSize = sizeof desiredMode;
  desiredMode.dmPelsWidth = w;
  desiredMode.dmPelsHeight = h;
  desiredMode.dmFields = DM_PELSHEIGHT | DM_PELSWIDTH;

  int error = ChangeDisplaySettingsW(&desiredMode, CDS_UPDATEREGISTRY);
  if (error) {
    Print(2, s("Failed to change resolution"));
  }

  ExitProcess(error); /* 'error' will be 0 if no error */
}
