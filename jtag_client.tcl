puts "======================================================================="
puts "==================== vJTAG Interface (CC + Stream) ===================="
puts "================================================================tvSh==="

# Global Settings
set port_stream 2540
set port_cc     2541
set device_lock_time 1000

puts "\nGlobal Settings:"
puts "-----------------------------------------------"
puts -nonewline "\tPort Streaming:       "
puts $port_stream
puts -nonewline "\tPort Control Const:   "
puts $port_cc

# Set Command line mode
global pgm_mode

if { $argc > 0 } {	
	switch [lindex $argv] {
		"1" {
			set pgm_mode 1
			puts "\tMode: control constants to TCP server"
		}
		"2" {
			set pgm_mode 2
			puts "\tMode: bash streaming"
		}
		"3" {
			set pgm_mode 3
			puts "\tMode: streaming to TCP server"
		}
		"4" {
			set pgm_mode 4
			puts "\tMode: streaming + control constants to TCP server"
		}
		default {
			puts "\tMode: bash control constants"
			set pgm_mode 0
		}
	}
} else {
	puts "\n+---------------------------------------------+"
	puts   "|             The mode is not set             |"
	puts   "+---------------------------------------------+"
	exit
}

##############################################################################################
##################################### Additional proc ########################################
##############################################################################################


# Sleep 
proc sleep {time} {
	after $time set end 1
	vwait end
}

proc hex2dec {largeHex} {
    set res 0
    foreach hexDigit [split $largeHex {}] {
        set new 0x$hexDigit
        set res [expr {16*$res + $new}]
    }
    return $res
}

proc hex2bin {hex} {
  binary scan [binary format H* $hex] B* bin
  return $bin
}


##############################################################################################
################################### Basic vJTAG Interface ####################################
##############################################################################################
global open_port_set
# flag_read_write: 0 - read; 1 - write
global flag_read_write

# Definition $usbblaster_name
puts "\nProgramming Hardwares:"
puts "-----------------------------------------------"
foreach hardware_name [get_hardware_names] {
	puts "\t- $hardware_name"
	if { [string match "USB-Blaster*" $hardware_name] } {
		set usbblaster_name $hardware_name
	}
}
puts "\t...\n\tConnecting to $usbblaster_name."

# Definition $test_device
puts "\nDevices on the JTAG chain:"
puts "-----------------------------------------------"
foreach device_name [get_device_names -hardware_name $usbblaster_name] {
	puts "\t- $device_name"
	if { [string match "@1*" $device_name] } {
		set test_device $device_name
	}
}
puts "\t...\n\tSelect device: $test_device."


# Open device 
proc OpenPort {} {
	puts "\nOpen device:"
	puts "-----------------------------------------------"
	global usbblaster_name
	global test_device
	global device_lock_time
	open_device -hardware_name $usbblaster_name -device_name $test_device
	device_lock -timeout $device_lock_time
	device_ir_shift -ir_value 6 -no_captured_ir_value
	puts "\tIDCODE: 0x[device_dr_shift -length 32 -value_in_hex]"
	device_unlock
}

# Close device.  Just used if communication error occurs
proc ClosePort {} {
	puts "\nClose device"
	catch {device_unlock}
	catch {close_device}
}

# Data write & transmission 
proc TransDataW {} {

	global device_lock_time

	puts "\nData transmission ("
	puts "-----------------------------------------------"
	
	# Gets IR DR Data
	# IR
	puts -nonewline "IR_DATA: 0x"
	flush stdout
	set ir_data [gets stdin]
	set length_ir [string length $ir_data]
	if {$length_ir == 0 || $length_ir > 4} {
		puts "ERROR: IR_DATA == 0, IR_DATA > 4"
		return 0
	}
	set ir_data [hex2dec $ir_data]
	# DR
	puts -nonewline "DR_DATA: 0x"
	flush stdout
	set dr_data [gets stdin]
	set dr_data [string trim $dr_data]
	set dr_data [string toupper $dr_data]
	set length_dr [string length $dr_data]
	if {$length_dr > 16} {
		puts "ERROR: DR_DATA == 0, DR_DATA > 16"
		return 0
	}
	while {$length_dr < 16 } {
		append dr_data_long "0"
		incr length_dr
	}
	append dr_data_long $dr_data
	
	# Transmission
	device_lock -timeout $device_lock_time
	
	device_virtual_ir_shift -instance_index 0 -ir_value $ir_data -no_captured_ir_value
	set tdi [device_virtual_dr_shift -dr_value $dr_data_long -instance_index 0  -length 64 -value_in_hex]
	
	device_virtual_ir_shift -instance_index 0 -ir_value 0 -no_captured_ir_value
	
	puts "IR: $ir_data; DR: $dr_data_long"
	puts "OUTPUT: $tdi"
	
	device_unlock
	
}

# Data read & transmission
proc TransData {line} {

	global flag_read_write
	global device_lock_time

	puts "\nData transmission"
	puts "-----------------------------------------------"
	
	# Analysis of data
	set line [string trim $line]
	set line [string toupper $line]
	
	set data_array [split $line]
	set ir_data [lindex $data_array 0]
	set dr_data [lindex $data_array 1]

	# IR
	set length_ir [string length $ir_data]
	if {$length_ir == 0 || $length_ir > 4} {
		puts "ERROR: IR_DATA == 0, IR_DATA > 4"
		return 0
	}
	set ir_data_long ""
	while {$length_ir < 4 } {
		append ir_data_long "0"
		incr length_ir
	}
	append ir_data_long $ir_data
	set ir_data_long [hex2bin $ir_data_long]
	set flag_read_write [string index $ir_data_long 1]
	
	set ir_data [hex2dec $ir_data]

	# DR
	set length_dr [string length $dr_data]
	if {$length_dr > 16} {
		puts "ERROR: DR_DATA == 0, DR_DATA > 16"
		return 0
	}
	set dr_data_long ""
	while {$length_dr < 16 } {
		append dr_data_long "0"
		incr length_dr
	}
	append dr_data_long $dr_data
		
	
	# Transmission
	device_lock -timeout $device_lock_time
	
	device_virtual_ir_shift -instance_index 0 -ir_value $ir_data -no_captured_ir_value
	set tdi [device_virtual_dr_shift -dr_value $dr_data_long -instance_index 0  -length 64 -value_in_hex]
	
	device_virtual_ir_shift -instance_index 0 -ir_value 0 -no_captured_ir_value
		
		
	set ir_data_hex [format %x $ir_data]
	
	set lengthIR [string length $ir_data_hex]
	set ir_and_data ""
	
	while {$lengthIR < 4 } {
		append ir_and_data "0"
		incr lengthIR
	}
	
	append ir_and_data "$ir_data_hex"
	append ir_and_data " $tdi"
	
	puts "IR: $ir_data"
	puts "DR: $dr_data_long"
	puts "OUTPUT: $ir_and_data"
	
	device_unlock
		
	if {$flag_read_write == 0} {
		# return $tdi
		return $ir_and_data
	} else {
		return 0
	}
	
}


##############################################################################################
####################################### Start program ########################################
##############################################################################################

# Mode: bash control constants
if { $pgm_mode == 0 } {
	OpenPort
	# Run TransData always
	while {true} {
		TransDataW
		sleep 1000
	}

# Mode: control constants to TCP server
} elseif { $pgm_mode == 1 } {
	set serv_cc [socket localhost $port_cc]
	fconfigure $serv_cc -buffering line
	
	OpenPort
	
	while {true} {
	
		if {[catch {gets $serv_cc line}]} {
			close $serv_cc
			puts "Close $serv_cc"

		} else {
			if {$line == ""} {
				puts "TCP/IP ==> NULL"
				return
			}
			set returnTransData [TransData $line]
			if {$returnTransData != 0} {
				puts $serv_cc $returnTransData
				puts "TCP/IP <== $returnTransData"
			}
		}

		# sleep 3000
	}
	
# Mode: bash streaming
} elseif { $pgm_mode == 2 } {
	OpenPort

	puts "\nData receiving"
	puts "-----------------------------------------------"
	while {true} {
		device_lock -timeout 1
		set datain [device_virtual_dr_shift -instance_index 1  -length 64 -value_in_hex]
		puts "[device_virtual_ir_shift -instance_index 1 -ir_value 3]:$datain"
		device_unlock
		sleep 500
	}

# Mode: streaming to TCP server
} elseif { $pgm_mode == 3 } {
	set serv_stm [socket localhost $port_stream]
	fconfigure $serv_stm -buffering line

	OpenPort
	
	while {true} {
		device_lock -timeout 1
		set datain [device_virtual_dr_shift -instance_index 1  -length 64 -value_in_hex]
		puts $serv_stm "[device_virtual_ir_shift -instance_index 1 -ir_value 3]:$datain"
		device_unlock
	}

# Mode: streaming + control constants to TCP server
} elseif { $pgm_mode == 4 } {
	set serv_stm [socket localhost $port_stream]
	fconfigure $serv_stm -buffering line
	
	set serv_cc [socket localhost $port_cc]
	fconfigure $serv_cc -buffering line -blocking False 

	OpenPort
	
	while {true} {
	
		device_lock -timeout 1
		set datain [device_virtual_dr_shift -instance_index 1  -length 64 -value_in_hex]
		puts $serv_stm "[device_virtual_ir_shift -instance_index 1 -ir_value 3]:$datain"
		device_unlock
		
		set line [read $serv_cc 23]
		if { $line != "" } {
			if {$line == ""} {
				puts "TCP/IP ==> NULL"
				return
			}
			set returnTransData [TransData $line]
			if {$returnTransData != 0} {
				puts $serv_cc $returnTransData
				puts "TCP/IP <== $returnTransData"
			}
		}
		
	}
}

ClosePort
exit






