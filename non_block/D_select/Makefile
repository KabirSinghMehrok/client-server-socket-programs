compile:
	gcc select_server.c -o select_server
	gcc -lpthread select_client.c -o select_client

run_server:
	./select_server

run_client:
	./select_client

clean:
	rm -f select_out.txt
	rm -f select_server
	rm -f select_client