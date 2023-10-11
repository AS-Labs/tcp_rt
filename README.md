# tcp_rt

## This is a simple program, where we get a server ip and port to measure the response time.

output should be a timestamp with the response time (timestamp,tcp response time)

- Will be using the below:
	- first arg as target servers IP
	- second arg as the target port
	- create socket variable to hold the fd number given from the socket syscall
	- create a struct for sockaddr_in with a variable name for the server IP
