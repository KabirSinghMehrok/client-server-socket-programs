compile:
	gcc -w -lpthread thread_server.c -o thread_server
	gcc -lpthread thread_client.c -o thread_client

run_server:
	./thread_server

run_client:
	./thread_client

clean:
	rm -f thread_out.txt
	rm -f thread_server
	rm -f thread_client