package main

import "core:fmt"
import "core:net"
import "core:os"
import "core:strconv"
import "core:time"

main :: proc() {
	if os.args.count < 2 {
		fmt.println("missing parameter, filename")
		return
	}

	file_name := os.args[1]
	f, err := os.open(file_name)
	defer os.close(f)

	if err != nil {
		fmt.printf("Failed to open file: %v\n", err)
		return
	}

	records, err := parse_csv(f)
	if err != nil {
		fmt.printf("Failed to read CSV: %v\n", err)
		return
	}

	sum := 0
	fmt.printf("Time,ip,port,latency\n")

	for record in records {
		ip := record[0]
		port, err := strconv.atoi(record[1])
		if err != nil {
			fmt.printf("Failed to parse port: %v\n", err)
			continue
		}

		timeout := time.second

		start_time := time.now()
		conn, err := net.dial_tcp(ip, port, timeout)

		if err != nil {
			fmt.printf("Failed to connect: %v\n", err)
			current_time_unix := time.now().unix()
			fmt.printf("%d,%s,%d,timeout\n", current_time_unix, ip, port)
			return
		} else {
			defer conn.close()

			current_time_unix := time.now().unix()
			end_time := time.now()
			latency := (end_time - start_time).seconds() * 1000.0
			fmt.printf("%d,%s,%d,%.2f\n", current_time_unix, ip, port, latency)
		}

		port_value, err := strconv.atoi(record[1])
		if err != nil {
			fmt.printf("Failed to parse value: %v\n", err)
			continue
		}
		sum += port_value
	}
	fmt.println(sum)
}

parse_csv :: proc(f: ^os.File) -> ([][]string, os.Error) {
	r := csv.Reader{}
	r.delimiter = ':'
	return r.read_all(f)
}
