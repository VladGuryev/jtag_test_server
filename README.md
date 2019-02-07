# jtag_test_server
The repo contains the class that provides connection to the quartus JTAG server for the fpga
The class provide the functionality to read and write information from registers of DE0-nano Altera board
which is programmed with special driver soft to control step engine. The test server connects to the jtag client that is launched by
the QProcess. QProcess starts the quartus_stp file with TCL script that controls the low level data flow between board and PC.
