compile:
	gcc poll_server.c -o poll_server
	gcc -lpthread poll_client.c -o poll_client

run_server:
	./poll_server

run_client:
	./poll_client

clean:
	rm -f poll_out.txt
	rm -f poll_server
	rm -f poll_client