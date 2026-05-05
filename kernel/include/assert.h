#ifndef _ASSERT_H
#define _ASSERT_H
#define assert(x) ((void)0)
#ifndef __cplusplus
#ifndef static_assert
#define static_assert _Static_assert
#endif
#endif
#endif
