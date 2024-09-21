package main

import (
	"encoding/csv"
	"fmt"
	"net"
	"os"
	"strconv"
	"time"
)

func main() {
	if len(os.Args) < 2 {
		fmt.Println("missing parameter, filename")
		return
	}
	f, _ := os.Open(os.Args[1])
	defer f.Close()
	r := csv.NewReader(f)
	r.Comma = ':'
	records, err := r.ReadAll()
	if err != nil {
		panic(err)
	}

	sum := 0
	fmt.Printf("Time,ip,port,latency\n")
	for _, record := range records {
		ip := record[0]
		port, err := strconv.Atoi(record[1])
		if err != nil {
			panic(err)
		}
		timeout := 1 * time.Second

		startTime := time.Now()
		timer := time.NewTimer(timeout)
		conn, err := net.DialTimeout("tcp", fmt.Sprintf("%s:%d", ip, port), timeout)
		if !timer.Stop() {
			select {
			case <-timer.C:
				fmt.Fprintf(os.Stderr, "connection timed out")
			default:
			}
		}
		if err != nil {
			fmt.Fprintf(os.Stderr, "Failed to connect: %v\n", err)
			currentTimeUnix := time.Now().Unix()
			//endTime := time.Now()
			latency := "timeout"
			fmt.Printf("%d,%s,%d,%s\n", currentTimeUnix, ip, port, latency)
			return
		} else {
			currentTimeUnix := time.Now().Unix()
			endTime := time.Now()
			latency := endTime.Sub(startTime).Seconds() * 1000
			fmt.Printf("%d,%s,%d,%.2f\n", currentTimeUnix, ip, port, latency)
		}
		defer conn.Close()

		//endTime := time.Now()
		//latency := endTime.Sub(startTime).Seconds() * 1000

		//currentTimeUnix := time.Now().Unix()
		x, err := strconv.Atoi(record[1])
		if err != nil {
			panic(err)
		}
		sum += x
	}
	fmt.Println(sum)

}
