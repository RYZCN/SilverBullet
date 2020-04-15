server: main.cpp ./threadpool/pool.h ./http/http_conn.cpp ./http/http_conn.h ./lock/lock.h 
	g++ -o server main.cpp ./threadpool/pool.h ./http/http_conn.cpp ./http/http_conn.h ./lock/lock.h -lpthread -std=c++11

clean:
	rm  -r server
