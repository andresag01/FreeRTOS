#ifndef __ASSERT_H__
#define __ASSERT_H__

#define assert( cond )				\
	do								\
	{								\
		if( !( cond ) )				\
		{							\
			__asm volatile(			\
				"ldc r0, 99		\n"	\
				"halt r0		\n"	\
			);						\
		}							\
	}								\
	while ( 0 )

#endif /* __ASSERT_H__ */
