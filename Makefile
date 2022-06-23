all:
	g++ -g DirCompare.c nsp_log.c md5.cpp -o DirCompare -Wattributes -lpthread
	g++ -g DirCompare.c nsp_log.c md5.cpp -o DirCompare_dbg -D NSP_LOG_TERM -D NSP_LOG_DEBUG -Wattributes -lpthread
