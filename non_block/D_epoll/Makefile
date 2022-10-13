compile:
	gcc epoll_server.c -o epoll_server
	gcc -lpthread epoll_client.c -o epoll_client

run_server:
	./epoll_server

run_client:
	./epoll_client

clean:
	rm -f epoll_out.txt
	rm -f epoll_server
	rm -f epoll_client