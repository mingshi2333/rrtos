extern "C" {

void *__dso_handle = 0;

void __cxa_pure_virtual(void)
{
    for (;;) {
    }
}

int __cxa_atexit(void (*destructor)(void *), void *object, void *dso_handle)
{
    (void)destructor;
    (void)object;
    (void)dso_handle;
    return 0;
}

}
