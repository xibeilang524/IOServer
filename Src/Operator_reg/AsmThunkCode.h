// AsmThunkCode.h 用于扰乱汇编代码用的花指令宏

#ifndef __ASM_THUNK_CODE_H__
#define __ASM_THUNK_CODE_H__

#ifndef _USE_THUNK_CODE // 不使用花指令

#define __THUNK_CODE_0 _asm nop
#define __THUNK_CODE_1 _asm nop
#define __THUNK_CODE_2 _asm nop
#define __THUNK_CODE_3 _asm nop
#define __THUNK_CODE_4 _asm nop
#define __THUNK_CODE_5 _asm nop
#define __THUNK_CODE_6 _asm nop
#define __THUNK_CODE_7 _asm nop
#define __THUNK_CODE_8 _asm nop
#define __THUNK_CODE_9 _asm nop

#else

#define __THUNK_CODE_0 _asm \
{\
	_asm jz  __label_thunk_code0\
	_asm jnz __label_thunk_code0\
	_asm _emit 0afh\
	_asm _emit 063h\
	_asm _emit 0e8h\
	_asm _emit 0eah\
} \
__label_thunk_code0:

#define __THUNK_CODE_1 _asm \
{\
	_asm jz  __label_thunk_code1\
	_asm jnz __label_thunk_code1\
	_asm _emit 045h\
	_asm _emit 09fh\
	_asm _emit 038h\
	_asm _emit 038h\
} \
__label_thunk_code1:

#define __THUNK_CODE_2 _asm \
{\
	_asm jz  __label_thunk_code2\
	_asm jnz __label_thunk_code2\
	_asm _emit 076h\
	_asm _emit 08ah\
	_asm _emit 0cdh\
	_asm _emit 04bh\
} \
__label_thunk_code2:

#define __THUNK_CODE_3 _asm \
{\
	_asm jz  __label_thunk_code3\
	_asm jnz __label_thunk_code3\
	_asm _emit 00ah\
	_asm _emit 0a4h\
	_asm _emit 0bch\
	_asm _emit 0efh\
} \
__label_thunk_code3:

#define __THUNK_CODE_4 _asm \
{\
	_asm jz  __label_thunk_code4\
	_asm jnz __label_thunk_code4\
	_asm _emit 0a6h\
	_asm _emit 0ddh\
	_asm _emit 08ch\
	_asm _emit 01fh\
} \
__label_thunk_code4:

#define __THUNK_CODE_5 _asm \
{\
	_asm jz  __label_thunk_code5\
	_asm jnz __label_thunk_code5\
	_asm _emit 0aah\
	_asm _emit 079h\
	_asm _emit 0f3h\
	_asm _emit 03fh\
} \
__label_thunk_code5:

#define __THUNK_CODE_6 _asm \
{\
	_asm jz  __label_thunk_code6\
	_asm jnz __label_thunk_code6\
	_asm _emit 0dah\
	_asm _emit 0fah\
	_asm _emit 0b7h\
	_asm _emit 09ah\
} \
__label_thunk_code6:

#define __THUNK_CODE_7 _asm \
{\
	_asm jz  __label_thunk_code7\
	_asm jnz __label_thunk_code7\
	_asm _emit 0ffh\
	_asm _emit 0c6h\
	_asm _emit 0a8h\
	_asm _emit 0d5h\
} \
__label_thunk_code7:

#define __THUNK_CODE_8 _asm \
{\
	_asm jz  __label_thunk_code8\
	_asm jnz __label_thunk_code8\
	_asm _emit 0abh\
	_asm _emit 0ceh\
	_asm _emit 0efh\
	_asm _emit 0aeh\
} \
__label_thunk_code8:

#define __THUNK_CODE_9 _asm \
{\
	_asm jz  __label_thunk_code9\
	_asm jnz __label_thunk_code9\
	_asm _emit 06ah\
	_asm _emit 0beh\
	_asm _emit 043h\
	_asm _emit 02ch\
} \
__label_thunk_code9:

#endif

#endif // end of __ASM_THUNK_CODE_H__