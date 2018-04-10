#ifndef HUFFMAN_CONSTANTS_H
#define HUFFMAN_CONSTANTS_H
	#ifdef OS_WINDOWS
	    //define pour windows
	    #define CLEAR "cls"
	    #define SYSREAD "pause"
	#else
	    //define pour linux
	    #define CLEAR "clear"
	    #define SYSREAD "read a"
	#endif
#endif //HUFFMAN_TOOLS_H
