package main

import "core:bytes"
import "core:fmt"
import "core:net"
import "core:os"
import "core:strings"
import "core:time"

print_usage :: proc() {
	fmt.println("Usage: %s filename\n", os.args[0])
}

get_unix_time :: proc() {
	unix_time :: proc "contextless" (t: Time) -> i64 {
	}

}

main :: proc() {
}
